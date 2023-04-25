#include <stdio.h>
#include <omp.h>

static long num_steps = 1000000000;
long double step;
#define NUM_THREADS 2

int main()
{
	int i;
	long double x, pi, sum[NUM_THREADS];
	// scanf("%ld", &num_steps);

	double start_time, end_time;
	start_time = omp_get_wtime();

	step = 1.0 / (long double)num_steps;
	omp_set_num_threads(NUM_THREADS);  //���� 2 �߳�
	#pragma omp parallel private(i, x)  //������ʼ��ÿ���߳�(0 �� 1)����ִ�иô���
	{
		int id;
		id = omp_get_thread_num();
		for (i = id, sum[id] = 0.0; i < num_steps; i = i + NUM_THREADS) {
			x = (i + 0.5) * step;
			sum[id] += 4.0 / (1.0 + x * x);
		}
	}
	for (i = 0, pi = 0.0; i < NUM_THREADS; i++)  pi += sum[i] * step;

	end_time = omp_get_wtime();
	printf("Time: %f seconds\n", end_time - start_time);

	printf("%.20Lf\n", pi);
	return 0;
}//��2���̲߳μӼ��㣬�����߳�0���е�����0,2,4,...�߳�1���е�����1,3,5,....
