all: sort_hadoop sort_hadoop_jar

sort_hadoop: SortHadoopv3.java
	javac SortHadoopv3.java

sort_hadoop_jar: SortHadoopv3.class
	jar -cvf SH.jar SortHadoo*.class

clean:
	rm -f SH.jar
	rm -f SortHadoo*.class
