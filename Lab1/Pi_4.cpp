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
	omp_set_num_threads(NUM_THREADS);  //����2�߳�
#pragma omp parallel for reduction(+:sum) private(i, x) //ÿ���̱߳���һ��˽�п���sum��xΪ�߳�˽�У������߳�������sum����+��Լ��������sum��ȫ��ֵ
	for (i = 1; i <= num_steps; i++) {
		x = (i - 0.5) * step;
		sum += 4.0 / (1.0 + x * x);
	}
	pi = sum * step;
	end_time = omp_get_wtime();
	printf("Time: %f seconds\n", end_time - start_time);
	printf("%.20Lf\n", pi);
}	//��2���̲߳μӼ��㣬�����߳�0���е�����0~49999���߳�1���е�����50000~99999.
