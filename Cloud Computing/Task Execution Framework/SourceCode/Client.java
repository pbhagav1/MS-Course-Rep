import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.CommandLine;

/**
 * 
 */

/**
 * @author prasanth
 *
 */
public class Client {

	/**
	 * 
	 */
	
	public static final Options opts = new Options();
	public static String stringOption;
	public static String queueName;
	public static int numOfWorkers;
	public static String workloadFile;
	
	public static void createOptions(){
		opts.addOption("s", true, "Option to run using LOCAL QUEUE or SQS QUEUE");
		opts.addOption("t", true, "Option to specify the number of Workers");
		opts.addOption("w", true, "Workload file which has the commands to be executed by workers");
	}
	
	public static void fetchOptions(String[] args) throws ParseException{
		
		CommandLineParser parser = new DefaultParser();
		CommandLine cmd = parser.parse(opts, args);
		
		if (cmd.hasOption("s")){
			stringOption = cmd.getOptionValue("s");
			if (stringOption.equals("LOCAL")){
				queueName = "LOCAL";
				System.out.println("Client: The Code will run using LOCAL QUEUE");
			}else{
				queueName = stringOption;
				System.out.println("Client: The Code will run using SQS QUEUE");
			}
		}

		if (cmd.hasOption("t")){
			numOfWorkers = Integer.parseInt(cmd.getOptionValue("t"));
			System.out.println("Client: The Number of Workers will be : "+ numOfWorkers);
		}
		
		if (cmd.hasOption("w")){
			workloadFile = cmd.getOptionValue("w");
			System.out.println("Client: The Workload filename is : "+ workloadFile);
		}

	}
	
	public static void processLocalQueue(){
		LocalQueueProcessing localClient = new LocalQueueProcessing(numOfWorkers, workloadFile);
		localClient.startProcess();
	}
	
	public static void processSQSQueue(){
		RemoteQueueProcessing remoteClient = new RemoteQueueProcessing(queueName, workloadFile);
		remoteClient.startProcess();
	}
	/**
	 * @param args
	 * @throws ParseException 
	 */
	public static void main(String[] args) throws ParseException {
		// TODO Auto-generated method stub
		createOptions();
		fetchOptions(args);
		if(stringOption.equals("LOCAL")){
			processLocalQueue();
		}else{
			processSQSQueue();
		}
	}
}
