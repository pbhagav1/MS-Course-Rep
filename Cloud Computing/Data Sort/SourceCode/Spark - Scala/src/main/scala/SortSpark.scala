import java.io.BufferedInputStream
import java.io.File
import java.io.FileInputStream
import java.io.InputStream
import java.lang.Object

import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf

import org.apache.hadoop.conf._
import org.apache.hadoop.fs._
import org.apache.hadoop.io.NullWritable
import org.apache.hadoop.mapred.lib.MultipleTextOutputFormat

object SortSpark{

    def main(args: Array[String]) {
        val sparkConfig = new SparkConf().setAppName("Sort Spark")
        val numOfPartitions = 4
        val sc = new SparkContext(sparkConfig)
        val conf = new Configuration()
        val fileSystem = FileSystem.get(conf)
        val inpPath = new Path(args(0))
        RemoteIterator<LocatedFileStatus> ritr = fs.listFiles(inpPath, false);
        while(ritr.hasnext()){
            val inputData = sc.textFile(new Path(ritr.next().toString()))
            val keyValuePair = inputData.map(line => createKeyValueTuple(line))
            val sortedKeyValue = keyValuePair.repartitionAndSortWithinPartitions(new SortPartitioner(numOfPartitions))
            //sorted.KeyValue.saveAsHadoopFile(args(1), CompositeKey.class, NullWritable.class, OutputFileLocator.class)
            sorted.KeyValue.saveAsHadoopFile(args(1), classOf[CompositeKey], classOf[None], classOf[OutputFileLocator])
        }
    }
  
    def createKeyValueTuple(line: String): (CompositeKey, None) = {
        (CompositeKey(line.substring(0, 9), line.substring(12, 44), line.substring(46)), None())
    }
  
    case class CompositeKey(primaryKey: String, secondaryKey: String, recordValue: String)
  
    class SortPartitioner(partitions: Int) extends Partitioner{
        require(partitions >= 0, s"Number of partitions ($partitions) cannot be negative.")
      
        override def numPartitions: Int = partitions
      
        override def getPartition(key: Any): Int = {
            val k = key.asInstanceOf[CompositeKey]
            (k.primaryKey.hashcode() & Integer.MAX_VALUE) % numPartitions
        }
    }
  
    object CompositeKey{
        implicit def orderingByPrimaryKeySecondaryKey[A <: CompositeKey] : Ordering[A] = {
            Ordering.by(ck => (ck.primaryKey, ck.secondaryKey))    
        }
    }
    
    class OutputFileLocator extends MultipleTextOutputFormat[Any, Any] {
        override def generateFileNameForKeyValue(key: Any, value: Any, name: String): String = 
            name
        /*{
            val k = key.asInstanceOf[CompositeKey]
            "part-r-000" + ((k.primaryKey.hashcode() & Integer.MAX_VALUE) % numPartitions).toString()
        }*/
    }
}
