import java.io.IOException;
import java.io.DataInput;
import java.io.DataOutput;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.WritableUtils;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.io.WritableComparator;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Partitioner;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class SortHadoopv3 {

    public static class CompositeKeyWritable implements Writable, WritableComparable<CompositeKeyWritable>{
        private String primaryKey;
        private String secondaryKey;
        private String recordValue;
        
        public CompositeKeyWritable(){
        }
        
        public CompositeKeyWritable(String primaryKey, String secondaryKey, String recordValue){
            this.primaryKey = primaryKey;
            this.secondaryKey = secondaryKey;
            this.recordValue = recordValue;
        }
        
        public void readFields(DataInput dataInput) throws IOException {
    		primaryKey = WritableUtils.readString(dataInput);
    		secondaryKey = WritableUtils.readString(dataInput);
    		recordValue = WritableUtils.readString(dataInput);
    	}

    	public void write(DataOutput dataOutput) throws IOException {
    		WritableUtils.writeString(dataOutput, primaryKey);
    		WritableUtils.writeString(dataOutput, secondaryKey);
    		WritableUtils.writeString(dataOutput, recordValue);
    	}
    	
    	public int compareTo(CompositeKeyWritable objKeyPair) {
    		int result = primaryKey.compareTo(objKeyPair.primaryKey);
    		if (0 == result) {
    			result = secondaryKey.compareTo(objKeyPair.secondaryKey);
    		}
    		return result;
    	}
    	
    	public String getPrimaryKey(){
    	    return primaryKey;
    	}
    	
    	public String getSecondaryKey(){
    	    return secondaryKey;
    	}
    	
    	public String getRecordValue(){
    	    return recordValue;
    	}
    }

    public static class CompositeKeyPartitioner extends Partitioner<CompositeKeyWritable, NullWritable>{
        public int getPartition(CompositeKeyWritable key, NullWritable value, int numReduceTasks) {
    		return ((key.getPrimaryKey().hashCode() & Integer.MAX_VALUE) % numReduceTasks);
    	}
    }
    
    /*public static class CompositeKeySortComparator extends WritableComparator{
        protected CompositeKeySortComparator(){
            super(CompositeKeyWritable.class, true);
        }
        
        public int compare(WritableComparable w1, WritableComparable w2) {
    		CompositeKeyWritable key1 = (CompositeKeyWritable) w1;
	    	CompositeKeyWritable key2 = (CompositeKeyWritable) w2;

	    	int cmpResult = key1.getprimaryKey().compareTo(key2.getprimaryKey());
	    	if (cmpResult == 0){
	    		cmpResult = key1.getSecondaryKey().compareTo(key2.getSecondaryKey());
    		}
    		return cmpResult;
    	}
    }*/
    
    public static class CompositeKeyGroupingComparator extends WritableComparator{
        protected CompositeKeyGroupingComparator(){
            super(CompositeKeyWritable.class, true);
        }
        
        public int compare(WritableComparable w1, WritableComparable w2) {
	    	CompositeKeyWritable key1 = (CompositeKeyWritable) w1;
	    	CompositeKeyWritable key2 = (CompositeKeyWritable) w2;
	    	return key1.getPrimaryKey().compareTo(key2.getPrimaryKey());
    	}
    }

    public static class RecordMapper
        extends Mapper<Object, Text, CompositeKeyWritable, NullWritable>{
            public void map(Object key, Text value, Context context) throws IOException, InterruptedException{
                String record = value.toString();
                String primaryKey = record.substring(0, 10);
                String secondaryKey = record.substring(12, 44);
                String recordValue = record.substring(46);
                context.write(new CompositeKeyWritable(primaryKey, secondaryKey, recordValue), NullWritable.get());
            }
        }
    
    public static class RecordReducer
        extends Reducer<CompositeKeyWritable, NullWritable, Text, NullWritable>{
            public void reduce(CompositeKeyWritable key, Iterable<NullWritable> values, Context context) 
                throws IOException, InterruptedException{
                    for (NullWritable value: values){
                        String recordStr = key.getPrimaryKey() + "  " + key.getSecondaryKey() + "  " + key.getRecordValue();
                        Text recordText = new Text(recordStr);
                        context.write(recordText, NullWritable.get());
                    }
            }
        }
        
    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        Job job = Job.getInstance(conf, "Tera Sort");
        job.setJarByClass(SortHadoopv3.class);
        job.setMapperClass(RecordMapper.class);
        job.setMapOutputKeyClass(CompositeKeyWritable.class);
		job.setMapOutputValueClass(NullWritable.class);
		job.setPartitionerClass(CompositeKeyPartitioner.class);
		//job.setSortComparatorClass(SecondarySortBasicCompKeySortComparator.class);
		job.setGroupingComparatorClass(CompositeKeyGroupingComparator.class);
        job.setNumReduceTasks(32);
        job.setReducerClass(RecordReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(NullWritable.class);
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}
