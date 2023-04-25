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
#pragma omp parallel private(i, x, sum) //���Ӿ��ʾx,sum��������ÿ���߳���˽�е�
	{
		int id;
		id = omp_get_thread_num();
		for (i = id, sum = 0.0; i < num_steps; i = i + NUM_THREADS) {
			x = (i + 0.5) * step;
			sum += 4.0 / (1.0 + x * x);
		}
#pragma omp critical  //ָ���������ͬһʱ��ֻ����һ���߳̽���ִ��
		pi += sum * step;
	}
	end_time = omp_get_wtime();
	printf("Time: %f seconds\n", end_time - start_time);
	printf("%.10Lf\n", pi);
}	//��2���̲߳μӼ��㣬�����߳�0���е�����0,2,4,...�߳�1���е�����1,3,5,....����ָ��Ϊcritical�Ĵ����	���ڱ�0�߳�ִ��ʱ��1�̵߳�ִ��Ҳ����ô���Σ�������������֪��0�߳��˳��ٽ�����
