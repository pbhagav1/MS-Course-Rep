import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.Map.Entry;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.AWSCredentials;
import com.amazonaws.auth.profile.ProfileCredentialsProvider;
import com.amazonaws.regions.Region;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDBClient;
import com.amazonaws.services.dynamodbv2.document.Item;
import com.amazonaws.services.dynamodbv2.document.Table;
import com.amazonaws.services.dynamodbv2.model.AttributeValue;
import com.amazonaws.services.dynamodbv2.model.GetItemRequest;
import com.amazonaws.services.dynamodbv2.model.GetItemResult;
import com.amazonaws.services.dynamodbv2.model.PutItemRequest;
import com.amazonaws.services.dynamodbv2.model.PutItemResult;
import com.amazonaws.services.sqs.AmazonSQS;
import com.amazonaws.services.sqs.AmazonSQSClient;
import com.amazonaws.services.sqs.model.Message;
import com.amazonaws.services.sqs.model.ReceiveMessageRequest;

public class RemoteWorker {

	public static final Options opts = new Options();
	public static String stringOption;
	public static String queueName;
	public static int numOfWorkers;

	private static AWSCredentials credentials;
    private static AmazonSQS sqs;
    private static String sqsQueueURL;
	private static Region reg;
	private static AmazonDynamoDBClient dynamoDB;

	
	public static void createOptions(){
		opts.addOption("s", true, "Option to run using LOCAL QUEUE or SQS QUEUE");
	}
	
	public static void fetchOptions(String[] args) throws ParseException{
		
		CommandLineParser parser = new DefaultParser();
		CommandLine cmd = parser.parse(opts, args);
		
		if (cmd.hasOption("s")){
			stringOption = cmd.getOptionValue("s");
			if (stringOption.equals("LOCAL")){
				queueName = "LOCAL";
				System.out.println("Worker: Not expecting LOCAL keyword");
			}else{
				queueName = stringOption;
				System.out.println("Client: The Code will run using SQS QUEUE" + queueName);
			}
		}
	}

	public static void getSQSQueueAndDynamoDBTable(){
		try {
            credentials = new ProfileCredentialsProvider("default").getCredentials();
        } catch (Exception e) {
            throw new AmazonClientException("Cannot load the credentials from the credential profiles file", e);
        }
		
		sqs = new AmazonSQSClient(credentials);
        dynamoDB = new AmazonDynamoDBClient(credentials);
		reg = Region.getRegion(Regions.US_EAST_1);
        sqs.setRegion(reg);
        dynamoDB.setRegion(reg);
        
        try {
        	sqsQueueURL = sqs.getQueueUrl(queueName).getQueueUrl();
        } catch (AmazonServiceException ase) {
            System.out.println("Caught an AmazonServiceException");
            System.out.println("Error Message:    " + ase.getMessage());
            System.out.println("HTTP Status Code: " + ase.getStatusCode());
            System.out.println("AWS Error Code:   " + ase.getErrorCode());
            System.out.println("Error Type:       " + ase.getErrorType());
            System.out.println("Request ID:       " + ase.getRequestId());
        } catch (AmazonClientException ace) {
            System.out.println("Caught an AmazonClientException");
            System.out.println("Error Message: " + ace.getMessage());
        }
	}
	
	public static void main(String[] args) throws ParseException {
		createOptions();
		fetchOptions(args);
		getSQSQueueAndDynamoDBTable();
		processMessage();
	}
	
	public static boolean checkTaskStatus(int taskNum){
		Map<String, AttributeValue> key = new HashMap<String, AttributeValue>();
		AttributeValue val = new AttributeValue();
		val.withN(Integer.toString(taskNum));
		key.put("TaskNum", val);
        GetItemResult getItemResult = dynamoDB.getItem("TaskStatus", key);
        if (getItemResult.toString() == null)
        	return false;
        else
        	return true;
	}
	
	public static void updateTaskStatus(int taskNum){
		Map<String, AttributeValue> key = new HashMap<String, AttributeValue>();
		AttributeValue val = new AttributeValue();
		val.withN(Integer.toString(taskNum));
		key.put("TaskNum", val);
		PutItemResult putItemResult = dynamoDB.putItem("TaskStatus", key);
	}

	public static void processMessage(){
		//read from queue and execute the task
		
		String inpTask = null;
		String nextTask = null;
		String resp = null;
		int taskNum = 0;
		boolean taskCompleted = false;
		
		try{
			do{
				ReceiveMessageRequest receiveMessageRequest = new ReceiveMessageRequest(sqsQueueURL);
	            List<Message> messages = sqs.receiveMessage(receiveMessageRequest).getMessages();
	            for (Message message : messages) {
	                inpTask = message.getBody();
	                if (inpTask != null){
	                	StringTokenizer st = new StringTokenizer(inpTask);
	                	while(st.hasMoreTokens()){
	                		switch(nextTask = st.nextToken()){
	                			case "TaskNum":
	                				taskNum = Integer.parseInt(st.nextToken());
	                				break;
	                			case "sleep":
	                				int sleepTime = Integer.parseInt(st.nextToken());
	                				taskCompleted = checkTaskStatus(taskNum);
	                				if (!taskCompleted){
	                					Thread.sleep((long) sleepTime);
	                					updateTaskStatus(taskNum);
	                				}
	                				//resp = "TaskNum " + taskNum + " completed by Worker " + workerId;
	                				//rspQueue.add(resp);
	                				break;
	                			default:
	                				System.out.println("LocalWorker: Unhandled command " + nextTask);
	                		}
	                	}
	                }
	            }
			}while(inpTask != null);
		}
		catch(Exception ex){
			System.out.println(ex.getMessage());
		}
	}
	
}
