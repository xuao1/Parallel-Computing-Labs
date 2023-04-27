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
    // step 4. 采样排序
    MPI_Allgather(samples, NUM_THREADS, MPI_INT, global_samples + id * NUM_THREADS, NUM_THREADS, MPI_INT, MPI_COMM_WORLD);
    if (id == 0) {
        std::sort(global_samples, global_samples + NUM_THREADS * NUM_THREADS);
    }
    for (int i = 0; i < NUM_THREADS * NUM_THREADS; i++) {
        printf("%d ", global_samples[i]);
    }
    /*
    int pivots[NUM_THREADS - 1];
    if (world_rank == 0) {
        for (int i = 0; i < world_size - 1; i++) {
            pivots[i] = global_samples[(i + 1) * world_size];
        }
    }

    MPI_Bcast(pivots, world_size - 1, MPI_INT, 0, MPI_COMM_WORLD);

    int index = 0;
    int j = pstart;

    int local_parts[NUM_THREADS][N] = { 0 };
    int local_count[NUM_THREADS] = { 0 };

    while (j < pend && index < world_size - 1) {
        if (A[j] < pivots[index]) {
            local_parts[index][local_count[index]] = A[j];
            local_count[index]++;
            j++;
        }
        else {
            index++;
        }
    }

    while (j < pend) {
        local_parts[index][local_count[index]] = A[j];
        local_count[index]++;
        j++;
    }

    int result[N];
    int result_count = 0;
    for (int i = 0; i < world_size; i++) {
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