
all: benchmark_cpu_run benchmark_mem_run benchmark_dsk_run

benchmark_cpu_run: Benchmark_CPU.c
	gcc -o benchmark_cpu_run Benchmark_CPU.c -pthread -Ofast

benchmark_mem_run: Benchmark_Memory.c
	gcc -o benchmark_mem_run Benchmark_Memory.c -pthread -Ofast

benchmark_dsk_run: Benchmark_Disk.c
	gcc -o benchmark_dsk_run Benchmark_Disk.c -pthread -Ofast

run_all: benchmark_cpu_run benchmark_dsk_run benchmark_mem_run
	./benchmark_cpu_run
	./benchmark_dsk_run
	./benchmark_mem_run

run_cpu: benchmark_cpu_run
	./benchmark_cpu_run

run_mem: benchmark_mem_run
	./benchmark_mem_run

run_dsk: benchmark_dsk_run
	./benchmark_dsk_run

clean:
	rm -f benchmark_cpu_run benchmark_mem_run benchmark_dsk_run

clean_data:
	rm -f Benchmark_Disk_Seq_1B.txt
	rm -f Benchmark_Disk_Seq_1KB.txt
	rm -f Benchmark_Disk_Seq_1MB.txt
	rm -f Benchmark_Disk_Rnd_1B.txt
	rm -f Benchmark_Disk_Rnd_1KB.txt
	rm -f Benchmark_Disk_Rnd_1MB.txt
