import java.io.BufferedReader;
import java.io.FileReader;
import java.util.StringTokenizer;
import java.util.concurrent.*;

public class LocalQueueProcessing{

	private int numOfWorkers;
	private String workloadFile;
	//private long timeout = 5;
	//private TimeUnit unit = TimeUnit.SECONDS;
	
	public LocalQueueProcessing(int numOfWorkers, String workloadFile) {
		this.numOfWorkers = numOfWorkers;
		this.workloadFile = workloadFile;
	}

	public void startProcess() {
		
		//System.out.println("Local Queue Processing: Starts");
		BlockingQueue<String> reqQueue = new LinkedBlockingQueue<String>();
		BlockingQueue<String> rspQueue = new LinkedBlockingQueue<String>();
		
		LocalClient locClient = new LocalClient(reqQueue, rspQueue, workloadFile);
		locClient.start();
		//System.out.println("LocalQueueProcessing: Client Started");

		try{
			Thread.sleep(500);
		}
		catch(InterruptedException ie){
			System.out.println(ie.getMessage());
		}
		
		for (int i=0; i < numOfWorkers; i++){
			//start the worker threads
			LocalWorker locWorker = new LocalWorker(i, reqQueue, rspQueue);
			locWorker.start();
			//System.out.println("LocalQueueProcessing: Worker " + i + " Started");
		}
		
		/*try{
			String resp = null;
			do{
				//synchronized(rspQueue){
					resp = rspQueue.poll(timeout, unit);
					//resp = rspQueue.poll();
				//}
				System.out.println("Response Queue: " + resp);
			}while (resp != null);
		}
		catch(Exception ex){
			System.out.println(ex.getMessage());
		}*/
	}

}


//class LocalClient extends Thread implements Runnable{
class LocalClient extends Thread {
	private BlockingQueue<String> reqQueue;
	private BlockingQueue<String> rspQueue;
	private String workloadFile;
	
	private long timeout = 5;
	private TimeUnit unit = TimeUnit.SECONDS;
	
	public LocalClient(BlockingQueue<String> req, BlockingQueue<String> rsp, String file){
		this.reqQueue = req;
		this.rspQueue = rsp;
		this.workloadFile = file;
	}
	
	@Override
	public void run(){
		//read workloadFile and write data to queue
		//boolean elementAdd;
		
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
								System.out.println("LocalClient: Unhandled command while reading file" + nextTask);
						}
					}
					//synchronized(reqQueue){
						//elementAdd = reqQueue.add(task);
					reqQueue.add(task);
					//}
					/*if (elementAdd){
						System.out.println("LocalClient: Added " + task);
					}*/
				}
			}while(inpTask != null);
		}
		catch(Exception ex){
			System.out.println(ex.getMessage());
		}
		
		try{
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
		}
		
	}
}

//class LocalWorker extends Thread implements Runnable{
class LocalWorker extends Thread {
	private int workerId;
	private BlockingQueue<String> reqQueue;
	private BlockingQueue<String> rspQueue;
	
	public LocalWorker(int id, BlockingQueue<String> req, BlockingQueue<String> rsp){
		this.workerId = id;
		this.reqQueue = req;
		this.rspQueue = rsp;
	}
	
	@Override
	public void run(){
		//read from queue and execute the task
		//boolean elementAdd;
		
		String inpTask = null;
		String nextTask = null;
		String resp = null;
		int taskNum = 0;
		
		//long timeout = 2;
		//TimeUnit unit = TimeUnit.SECONDS;
		
		try{
			do{
				synchronized(reqQueue){
					//inpTask = reqQueue.poll(timeout, unit);
					inpTask = reqQueue.poll();
				}
				if (inpTask != null){
					//System.out.println("Worker " + workerId + ": Fetched " + inpTask);
					StringTokenizer st = new StringTokenizer(inpTask);
					while(st.hasMoreTokens()){
						switch(nextTask = st.nextToken()){
							case "TaskNum":
								taskNum = Integer.parseInt(st.nextToken());
								break;
							case "sleep":
								//System.out.println("Worker " + workerId + ": Fetched " + inpTask);
								int sleepTime = Integer.parseInt(st.nextToken());
								Thread.sleep((long) sleepTime);
								resp = "TaskNum " + taskNum + " completed by Worker " + workerId;
								//synchronized(rspQueue){
									//elementAdd = rspQueue.add(resp);
								rspQueue.add(resp);
								//}
								//System.out.println(resp);
								break;
							default:
								System.out.println("LocalWorker: Unhandled command " + nextTask);
						}
					}
				}/*else{
					System.out.println("Worker " + workerId + " polled but could not retrieve data");
				}*/
			}while(inpTask != null);
		}
		catch(Exception ex){
			System.out.println(ex.getMessage());
		}
	}
}