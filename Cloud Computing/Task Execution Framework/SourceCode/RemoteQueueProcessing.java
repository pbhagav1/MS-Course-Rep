import java.io.BufferedReader;
import java.io.FileReader;
import java.util.StringTokenizer;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.AWSCredentials;
import com.amazonaws.auth.profile.ProfileCredentialsProvider;
import com.amazonaws.regions.Region;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.sqs.AmazonSQS;
import com.amazonaws.services.sqs.AmazonSQSClient;
import com.amazonaws.services.sqs.model.CreateQueueRequest;
import com.amazonaws.services.sqs.model.SendMessageRequest;

public class RemoteQueueProcessing{
	private String queueName;
	private String sqsQueueURL;
	private String workloadFile;
	private AWSCredentials credentials;
    private AmazonSQS sqs;
    private Region reg;

	public RemoteQueueProcessing(String queueName, String workloadFile) {
		// TODO Auto-generated constructor stub
		this.queueName = queueName;
		this.workloadFile = workloadFile;
	}

	public void getSQSQueue(){
		try {
            credentials = new ProfileCredentialsProvider("default").getCredentials();
        } catch (Exception e) {
            throw new AmazonClientException("Cannot load the credentials from the credential profiles file", e);
        }
		
		sqs = new AmazonSQSClient(credentials);
		reg = Region.getRegion(Regions.US_EAST_1);
        sqs.setRegion(reg);
        
        try {
            // Create a queue
            //CreateQueueRequest createQueueRequest = new CreateQueueRequest(queueName);
            //sqsQueueURL = sqs.createQueue(createQueueRequest).getQueueUrl();
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
	
	public void startProcess() {
		// TODO Auto-generated method stub
		getSQSQueue();
		RemoteClient remClient = new RemoteClient(sqs, sqsQueueURL, workloadFile);
		remClient.start();
	}

}


class RemoteClient extends Thread {
	private String sqsQueueURL;
	private String workloadFile;
    private AmazonSQS sqs;
    
    public RemoteClient(AmazonSQS sqs, String queueUrl, String workload){
    	this.sqs = sqs;
    	this.sqsQueueURL = queueUrl;
    	this.workloadFile = workload;
    }
    
    @Override
    public void run(){
		String inpTask = null;
		String nextTask = null;
		String task = null;
		int taskId = 0;
		try{
			BufferedReader input = new BufferedReader(new FileReader(workloadFile));
			do{
				inpTask = input.readLine();
				if (inpTask != null){
					task = null;
					taskId++;
					StringTokenizer st = new StringTokenizer(inpTask);
					while(st.hasMoreTokens()){
						switch(nextTask = st.nextToken()){
							case "sleep":
								int sleepTime = Integer.parseInt(st.nextToken());
								task = "TaskNum " + taskId + " sleep " + sleepTime;
								break;
							default:
								System.out.println("RemoteClient: Unhandled command while reading file" + nextTask);
						}
					}
					//reqQueue.add(task);
					sqs.sendMessage(new SendMessageRequest(sqsQueueURL, task));
				}
			}while(inpTask != null);
		}
		catch (AmazonServiceException ase) {
            System.out.println("Caught an AmazonServiceException");
            System.out.println("Error Message:    " + ase.getMessage());
            System.out.println("HTTP Status Code: " + ase.getStatusCode());
            System.out.println("AWS Error Code:   " + ase.getErrorCode());
            System.out.println("Error Type:       " + ase.getErrorType());
            System.out.println("Request ID:       " + ase.getRequestId());
        }
		catch(Exception ex){
			System.out.println(ex.getMessage());
		}
		
		/*try{
			String resp = null;
			do{
				//synchronized(rspQueue){
					resp = rspQueue.poll(timeout, unit);
					//resp = rspQueue.poll();
				//}
				//System.out.println("Response Queue: " + resp);
			}while (resp != null);
		}
		catch(Exception ex){
			System.out.println(ex.getMessage());
		}*/
    }
}