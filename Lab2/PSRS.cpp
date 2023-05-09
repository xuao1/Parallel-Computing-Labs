#include <stdio.h>
#include <algorithm>
#include <mpi.h>
#include <time.h>
int N = 27;
int NUM_THREADS = 3;

int main(int argc, char* argv[])
{
    srand(time(NULL));
    ptintf("please input the number of numbers: \n");
    scanf("%d", &N);
    int* A = (int*)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        A[i] = rand % 100;
    }
    ptintf("please input the number of threads: \n");
    scanf("%d", &NUM_THREADS);
    int* global_samples = (int*)malloc(NUM_THREADS * NUM_THREADS * sizeof(int));
    MPI_Init(&argc, &argv);
    int thread_num = NUM_THREADS;
    int id;
    MPI_Comm_size(MPI_COMM_WORLD, &thread_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    // step 1. 均匀划分
    int pstart = id * N / NUM_THREADS;
    int pend = (id + 1) * N / NUM_THREADS;
    // step 2. 局部排序
    std::sort(A + pstart, A + pend);
    // step 3. 选取样本
    int step = N / (NUM_THREADS * NUM_THREADS);
    int* samples = (int*)malloc(NUM_THREADS * sizeof(int));
    for (int j = 0; j < NUM_THREADS; j++) {
        samples[j] = A[pstart + j * step];
    }
    // 全局 samples 只需要线程 0 获取即可，它从这些数据中选择主元
    // 再将主元广播到各个线程
    int* global_samples = (int*)malloc(NUM_THREADS * NUM_THREADS * sizeof(int));
    int* pivots = (int*)malloc(NUM_THREADS * sizeof(int));
    MPI_Gather(samples, NUM_THREADS, MPI_INT, global_samples, NUM_THREADS, MPI_INT, 0, MPI_COMM_WORLD);
    if (id == 0) {
        // step 4. 采样排序
        std::sort(global_samples, global_samples + NUM_THREADS * NUM_THREADS);
        // step 5. 选择主元
        for (int i = 0; i < NUM_THREADS - 1; i++) {
            pivots[i] = global_samples[(i + 1) * NUM_THREADS];
        }
    }
    MPI_Bcast(pivots, NUM_THREADS - 1, MPI_INT, 0, MPI_COMM_WORLD);
    // step 6. 主元划分
    int index = 0;
    int j = pstart;
    int local_parts[NUM_THREADS][N] = { 0 };
    int local_count[NUM_THREADS] = { 0 };
    while (j < pend && index < NUM_THREADS - 1) {
        if (A[j] <= pivots[index]) {
            local_parts[index][local_count[index]] = A[j];
            local_count[index]++; // <= pivot[index] 的有 count[index] 个
            j++;
        }
        else index++;
    }
    // 大于最后一个主元的部分：此时 index = NUM_THREADS - 1
    while (j < pend) {
        local_parts[index][local_count[index]] = A[j];
        local_count[index]++;
        j++;
    }
    // step 7. 全局交换
    // 每个线程 p_id 需要处理 NUM_THREADS 个 local_parts[id][] 
    int* send_buf = (int*)malloc(N * NUM_THREADS * sizeof(int));
    int* recv_buf = (int*)malloc(N * NUM_THREADS * sizeof(int));
    int* send_counts = (int*)malloc(NUM_THREADS * sizeof(int));
    int* recv_counts = (int*)malloc(NUM_THREADS * sizeof(int));
    int* send_displs = (int*)malloc(NUM_THREADS * sizeof(int));
    int* recv_displs = (int*)malloc(NUM_THREADS * sizeof(int));
    for (int i = 0; i < NUM_THREADS; i++) {
        send_counts[i] = local_count[i];
    }
    MPI_Alltoall(send_counts, 1, MPI_INT, recv_counts, 1, MPI_INT, MPI_COMM_WORLD);
    send_displs[0] = recv_displs[0] = 0;
    for (int i = 1; i < NUM_THREADS; i++) {
        send_displs[i] = send_displs[i - 1] + send_counts[i - 1];
        recv_displs[i] = recv_displs[i - 1] + recv_counts[i - 1];
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        for (int j = 0; j < send_counts[i]; j++) {
            send_buf[send_displs[i] + j] = local_parts[i][j];
        }
    }
    MPI_Alltoallv(send_buf, send_counts, send_displs, MPI_INT, recv_buf, recv_counts, recv_displs, MPI_INT, MPI_COMM_WORLD);
    //printf("id = %d\n", id);
    //for (int i = 0; i < recv_displs[NUM_THREADS - 1] + recv_counts[NUM_THREADS - 1]; i++) {
    //    printf("%d  ", recv_buf[i]);
    //}
    //printf("\n");
    //std::sort(recv_buf, recv_buf + N * NUM_THREADS);
    //std::sort(recv_buf, recv_buf + recv_displs[NUM_THREADS - 1] + recv_counts[NUM_THREADS - 1]); 

    // 至此，完成全局交换
    // 每个线程有一个一维数组 recv_buf,按 recv_couts 分段有序（本质上是因为来源于全局交换）
    // step 8. 归并排序
    // 将 recv_buf 分段 merge，最终写入到 A 数组
    int result[N] = { 0 };
    int tmp[N] = { 0 };
    int result_count = 0, tmp_count = 0;
    for (int index = 0; index < NUM_THREADS; index++) {
        // Merge: result[] 和 parts[index][id][]
        int i = 0, jj = 0, k = 0;
        int A_count = result_count;
        int B_count = recv_counts[index];
        tmp_count = A_count + B_count;
        while (i < A_count && jj < B_count) {
            if (result[i] <= recv_buf[recv_displs[index] + jj]) tmp[k++] = result[i++];
            else {
                tmp[k++] = recv_buf[recv_displs[index] + jj];
                jj++;
            }
        }
        while (i < A_count) tmp[k++] = result[i++];
        while (jj < B_count) {
            tmp[k++] = recv_buf[recv_displs[index] + jj];
            jj++;
        }
        result_count = tmp_count;
        for (int l = 0; l < tmp_count; l++) {
            result[l] = tmp[l];
        }
    }
    // 这里出过一个 BUG，差不多调了 1.5 小时，结果发现是上面的该写为 index 的地方写为了 i
    int* A_counts = (int*)malloc(NUM_THREADS * sizeof(int));
    int* A_displs = (int*)malloc(NUM_THREADS * sizeof(int));
    MPI_Gather(&result_count, 1, MPI_INT, A_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (id == 0) {
        A_displs[0] = 0;
        for (int i = 1; i < NUM_THREADS; i++) {
            A_displs[i] = A_displs[i - 1] + A_counts[i - 1];
        }
    }
    MPI_Gatherv(result, result_count, MPI_INT, A, A_counts, A_displs, MPI_INT, 0, MPI_COMM_WORLD);
    if (id == 0) {
        for (int i = 0; i < N; i++) {
            printf("%d ", A[i]);
        }
        printf("\n");
    }
    free(samples);
    free(global_samples);
    free(pivots);
    free(send_buf);
    free(recv_buf);
    free(send_counts);
    free(recv_counts);
    free(send_displs);
    free(recv_displs);
    free(A_counts);
    free(A_displs);
    MPI_Finalize();
    return 0;
}