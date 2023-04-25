#include <stdio.h>
#include <omp.h>

static long num_steps = 1000000000;
long double step;
#define NUM_THREADS 2

int main()
{
	int i;
	long double pi = 0.0;
	long double sum = 0.0;
	long double x = 0.0;
	// scanf("%ld", &num_steps);
	double start_time, end_time;
	start_time = omp_get_wtime();
	step = 1.0 / (long double)num_steps;
	omp_set_num_threads(NUM_THREADS);  //设置2线程
#pragma omp parallel private(i, x, sum) //该子句表示x,sum变量对于每个线程是私有的
	{
		int id;
		id = omp_get_thread_num();
		for (i = id, sum = 0.0; i < num_steps; i = i + NUM_THREADS) {
			x = (i + 0.5) * step;
			sum += 4.0 / (1.0 + x * x);
		}
#pragma omp critical  //指定代码段在同一时刻只能由一个线程进行执行
		pi += sum * step;
	}
	end_time = omp_get_wtime();
	printf("Time: %f seconds\n", end_time - start_time);
	printf("%.10Lf\n", pi);
}	//共2个线程参加计算，其中线程0进行迭代步0,2,4,...线程1进行迭代步1,3,5,....当被指定为critical的代码段	正在被0线程执行时，1线程的执行也到达该代码段，则它将被阻塞知道0线程退出临界区。
