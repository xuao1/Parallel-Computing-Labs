#include <stdio.h>
#include <omp.h>
#include <algorithm>
#include <time.h>

int N = 27;
int NUM_THREADS = 3;

int main()
{
	srand(time(NULL));
	printf("please input the number of numbers: \n");
	scanf("%d", &N);
	int* A = (int*)malloc(N * sizeof(int));
	for (int i = 0; i < N; i++) {
		A[i] = rand() % 100;
	}
	printf("please input the number of threads: \n");
	scanf("%d", &NUM_THREADS);
	printf("number of threads: %d\n", NUM_THREADS);
	printf("before sorting: \n");
	for (int i = 0; i < N; i++)
		printf("%d ", A[i]);
	printf("\n");

	double start_time, end_time;
	start_time = omp_get_wtime();

	int* samples = (int*)malloc(NUM_THREADS * NUM_THREADS * sizeof(int));
	int* pivots = (int*)malloc(NUM_THREADS * sizeof(int));
	int parts[NUM_THREADS][NUM_THREADS][N] = { 0 }; // 第i个线程，发往来自各个线程的有序数组
	int count[NUM_THREADS][NUM_THREADS] = { 0 };// 第i个线程，发往各个线程的数据数目
	// 以上两个数组，除去第一维，即为某个线程局部数据划分出的数组，一个存数据，一个存个数

	omp_set_num_threads(NUM_THREADS);
#pragma omp parallel
	{
		int id = omp_get_thread_num();
		// step 1. 均匀划分
		int pstart = id * N / NUM_THREADS;
		int pend = (id + 1) * N / NUM_THREADS;
		// step 2. 局部排序
		std::sort(A + pstart, A + pend);
		// step 3. 选取样本
		int step = N / (NUM_THREADS * NUM_THREADS);
		for (int j = 0; j < NUM_THREADS; j++) {
			samples[id * NUM_THREADS + j] = A[pstart + j * step];
		}
	}
	// step 4. 采样排序
	std::sort(samples, samples + NUM_THREADS * NUM_THREADS);
	// step 5. 选择主元
	for (int i = 0; i < NUM_THREADS - 1; i++) {
		pivots[i] = samples[(i + 1) * NUM_THREADS];
	}
	// step 6. 主元划分 & step 7. 全局交换	
#pragma omp parallel
	{
		int id = omp_get_thread_num();
		int pstart = id * N / NUM_THREADS;
		int pend = (id + 1) * N / NUM_THREADS;
		int index = 0; // 主元下标
		int j = pstart;
		while (j < pend && index < NUM_THREADS - 1) {
			if (A[j] < pivots[index]) {
				parts[id][index][count[id][index]] = A[j];
				count[id][index]++; // <= pivot[index] 的有 count[i][index] 个
				j++;
			}
			else {
				index++;
			}
		}
		// 大于最后一个主元的部分：此时 index = NUM_THREADS - 1
		while (j < pend) {
			parts[id][index][count[id][index]] = A[j];
			count[id][index]++;
			j++;
		}
	}
	// 至此，每个线程 pi 只需要处理 parts[][i][] 
	// step 8. 归并排序
	int result[NUM_THREADS][N];
	int result_count[NUM_THREADS];
#pragma omp parallel
	{
		int id = omp_get_thread_num();
		// int result = (int*)malloc(N * sizeof(int));
		int total_count = 0;
		int* tmp = (int*)malloc(N * sizeof(int));
		int tmp_count = 0;
		for (int index = 0; index < NUM_THREADS; index++) {
			// Merge: result[] 和 parts[index][id][]
			int i = 0, j = 0, k = 0;
			int A_count = total_count;
			int B_count = count[index][id];
			tmp_count = A_count + B_count;
			while (i < A_count && j < B_count) {
				if (result[id][i] <= parts[index][id][j]) tmp[k++] = result[id][i++];
				else tmp[k++] = parts[index][id][j++];
			}
			while (i < A_count) tmp[k++] = result[id][i++];
			while (j < B_count) tmp[k++] = parts[index][id][j++];
			total_count = tmp_count;
			for (int l = 0; l < tmp_count; l++) {
				result[id][l] = tmp[l];
			}
		}
		result_count[id] = total_count;
	}
	int k = 0;
	for (int i = 0; i < NUM_THREADS; i++) {
		for (int j = 0; j < result_count[i]; j++) {
			A[k++] = result[i][j];
		}
	}

	end_time = omp_get_wtime();
	printf("Time: %f seconds\n", end_time - start_time);

	printf("after sorting: \n");
	for (int i = 0; i < N; i++)
		printf("%d ", A[i]);
	printf("\n");
	return 0;
}
