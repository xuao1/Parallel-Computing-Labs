#include <stdio.h>
#include <algorithm>
#include <mpi.h>
#define N 27
#define NUM_THREADS 3

int A[N] = { 15,46,48,93,39,6,72,91,14,36,69,40,89,61,97,12,21,54,53,97,84,58,32,27,33,72,20 };
int global_samples[NUM_THREADS * NUM_THREADS];

int main(int argc, char* argv[])
{
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
    printf("id = %d\n", id);
    printf("local_count[0] = %d\n", local_count[0]);
    for (int i = 0; i < local_count[0]; i++) {
        printf("%d ", local_parts[0][i]);
    }
    printf("\n");

    // step 7. 全局交换
    // 每个线程 p_id 需要处理 NUM_THREADS 个 local_parts[id][] 
    int* send_buf = (int*)malloc(N * NUM_THREADS * sizeof(int));
    int* recv_buf = (int*)malloc(N * NUM_THREADS * sizeof(int));
    int* send_counts = (int*)malloc(NUM_THREADS * sizeof(int));
    int* recv_counts = (int*)malloc(NUM_THREADS * sizeof(int));
    int* send_displs = (int*)malloc(NUM_THREADS * sizeof(int));
    int* recv_displs = (int*)malloc(NUM_THREADS * sizeof(int));
    /*
    for (int i = 0; i < NUM_THREADS; i++) {
        for (int j = 0; j < local_count[i]; j++) {
            send_buf[i * N + j] = local_parts[i][j];
        }
        send_counts[i] = local_count[i];
    }
    MPI_Alltoall(send_counts, 1, MPI_INT, recv_counts, 1, MPI_INT, MPI_COMM_WORLD);
    send_displs[0] = recv_displs[0] = 0;
    for (int i = 1; i < NUM_THREADS; i++) {
        send_displs[i] = send_displs[i - 1] + send_counts[i - 1];
        recv_displs[i] = recv_displs[i - 1] + recv_counts[i - 1];
    }
    */
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

    /*
    printf("id = %d\n", id);
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("recv from %d  %d nums\n", i, recv_counts[i]);
        for (int j = 0; j < recv_counts[i]; j++) {
            printf("%d  %d\n", recv_displs[i]+j, recv_buf[recv_displs[i] + j]);
        }
        printf("\n");
    }
    printf("\n");
    */
    /*

    int result[N];
    int result_count = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        int recvbuf[N];
        int recvcount;
        MPI_Gather(local_count + i, 1, MPI_INT, &recvcount, 1, MPI_INT, i, MPI_COMM_WORLD);
        MPI_Gatherv(local_parts[i], local_count[i], MPI_INT, recvbuf, &recvcount, NULL, MPI_INT, i, MPI_COMM_WORLD);

        if (world_rank == i) {
            int p1 = 0, p2 = 0, p = 0;
            while (p1 < result_count && p2 < recvcount) {
                if (result[p1] <= recvbuf[p2]) {
                    A[p++] = result[p1++];
                }
                else {
                    A[p++] = recvbuf[p2++];
                }
            }
            while (p1 < result_count) {
                A[p++] = result[p1++];
            }
            while (p2 < recvcount) {
                A[p++] = recvbuf[p2++];
            }
            result_count += recvcount;
            memcpy(result, A, result_count * sizeof(int));
        }
    }
    if (world_rank == 0) {
        for (int i = 0; i < N; i++) {
            printf("%d ", A[i]);
        }
        printf("\n");
    }
    */
    MPI_Finalize();
    return 0;
}