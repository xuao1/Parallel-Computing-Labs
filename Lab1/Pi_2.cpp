#include <stdio.h>
#include <omp.h>
#include <time.h>
static long num_steps = 1000000000;
long double step;
#define NUM_THREADS 2

int main()
{
	int i;
	long double x, pi, sum[NUM_THREADS];
	// scanf("%ld", &num_steps);
	clock_t start = clock();
	step = 1.0 / (long double)num_steps;
	omp_set_num_threads(NUM_THREADS);  //����2�߳�

#pragma omp parallel private(i, x) //������ʼ��ÿ���߳�(0��1)����ִ�иô���
	{
		int id;
		id = omp_get_thread_num();
		sum[id] = 0;

#pragma omp for  //δָ��chunk������ƽ����������̣߳�0��1������������
		for (i = 0; i < num_steps; i++) {
			x = (i + 0.5) * step;
			sum[id] += 4.0 / (1.0 + x * x);
		}
	}

	for (i = 0, pi = 0.0; i < NUM_THREADS; i++)  pi += sum[i] * step;
	clock_t end = clock();
	double duration = (double)(end - start) / CLOCKS_PER_SEC;
	printf("Time: %lf s\n", duration);
	printf("%.20Lf\n", pi);
	return 0;
}
