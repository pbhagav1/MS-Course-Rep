import java.util.ArrayList;
import java.util.List;

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
import com.amazonaws.services.ec2.AmazonEC2;
import com.amazonaws.services.ec2.AmazonEC2Client;
import com.amazonaws.services.ec2.model.DescribeSpotInstanceRequestsRequest;
import com.amazonaws.services.ec2.model.DescribeSpotInstanceRequestsResult;
import com.amazonaws.services.ec2.model.LaunchSpecification;
import com.amazonaws.services.ec2.model.RequestSpotInstancesRequest;
import com.amazonaws.services.ec2.model.RequestSpotInstancesResult;
import com.amazonaws.services.ec2.model.SpotInstanceRequest;
import com.amazonaws.services.sqs.AmazonSQS;

public class Worker {

	private static final long SLEEP_CYCLE = 60000;
	
	public static final Options opts = new Options();
	public static String stringOption;
	public static String queueName;
	public static int numOfWorkers;

	private static AWSCredentials credentials;
    private static Region reg;
    private static AmazonEC2 ec2;
    private static ArrayList<String> instanceIds;
    private static ArrayList<String> spotInstanceRequestIds;

    
	public static void createOptions(){
		opts.addOption("s", true, "Option to run using LOCAL QUEUE or SQS QUEUE");
		opts.addOption("t", true, "Option to specify the number of Workers");
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

		if (cmd.hasOption("t")){
			numOfWorkers = Integer.parseInt(cmd.getOptionValue("t"));
			System.out.println("Client: The Number of Workers will be : "+ numOfWorkers);
		}
	}
	
	public static void startWorkers(){
		try {
            credentials = new ProfileCredentialsProvider("default").getCredentials();
        } catch (Exception e) {
            throw new AmazonClientException("Cannot load the credentials from the credential profiles file", e);
        }
		
		ec2 = new AmazonEC2Client(credentials);
		reg = Region.getRegion(Regions.US_EAST_1);
        ec2.setRegion(reg);
        
        
        
        RequestSpotInstancesRequest requestRequest = new RequestSpotInstancesRequest();
        requestRequest.setSpotPrice("0.03");
        requestRequest.setInstanceCount(Integer.valueOf(numOfWorkers));

        LaunchSpecification launchSpecification = new LaunchSpecification();
        launchSpecification.setImageId("ami-8c1fece5");
        launchSpecification.setInstanceType("t2.micro");
        launchSpecification.setUserData("#!/bin/bash\n java RemoteWorker -s " + queueName);

        ArrayList<String> securityGroups = new ArrayList<String>();
        securityGroups.add("GettingStartedGroup");
        launchSpecification.setSecurityGroups(securityGroups);

        requestRequest.setLaunchSpecification(launchSpecification);

        RequestSpotInstancesResult requestResult = ec2.requestSpotInstances(requestRequest);
        List<SpotInstanceRequest> requestResponses = requestResult.getSpotInstanceRequests();

        spotInstanceRequestIds = new ArrayList<String>();

        for (SpotInstanceRequest requestResponse : requestResponses) {
            System.out.println("Created Spot Request: "+requestResponse.getSpotInstanceRequestId());
            spotInstanceRequestIds.add(requestResponse.getSpotInstanceRequestId());
        }

        /*do
        {
            try{
            	Thread.sleep(SLEEP_CYCLE);
            }
            catch(InterruptedException ex){
            	System.out.println(ex.getMessage());
            }
        } while (areAnyOpen());*/
        
        
        
	}
	
    public static boolean areAnyOpen() {
        DescribeSpotInstanceRequestsRequest describeRequest = new DescribeSpotInstanceRequestsRequest();
        describeRequest.setSpotInstanceRequestIds(spotInstanceRequestIds);

        instanceIds = new ArrayList<String>();

        try
        {
            DescribeSpotInstanceRequestsResult describeResult = ec2.describeSpotInstanceRequests(describeRequest);
            List<SpotInstanceRequest> describeResponses = describeResult.getSpotInstanceRequests();

            for (SpotInstanceRequest describeResponse : describeResponses) {
                //System.out.println(" " +describeResponse.getSpotInstanceRequestId() +
                //                   " is in the "+describeResponse.getState() + " state.");

                if (describeResponse.getState().equals("open")) {
                    return true;
                }

                instanceIds.add(describeResponse.getInstanceId());
            }
        } catch (AmazonServiceException e) {
            System.out.println("Error when calling describeSpotInstances");
            System.out.println("Caught Exception: " + e.getMessage());
            System.out.println("Reponse Status Code: " + e.getStatusCode());
            System.out.println("Error Code: " + e.getErrorCode());
            System.out.println("Request ID: " + e.getRequestId());
            return true;
        }
        return false;
    }	
	
	/**
	 * @param args
	 * @throws ParseException 
	 */
	public static void main(String[] args) throws ParseException {
		// TODO Auto-generated method stub
		createOptions();
		fetchOptions(args);
		if(!stringOption.equals("LOCAL")){
			startWorkers();
		}
	}

}
