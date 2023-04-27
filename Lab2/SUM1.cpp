#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 8
#define NUM_THREADS 8
int A[8] = { 0,1,2,3,4,5,6,7 };

int main(int argc, char** argv) {
    int id, thread_num;
    thread_num = NUM_THREADS;
    // int * recv_buf, sum = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &thread_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    int sum = A[id], nextsum = 0;
    int cnt = 1; // 跨步
    int chatwith;
    while (cnt <= N / 2) {
        if (id % (cnt * 2) < cnt) chatwith = id + cnt;
        else chatwith = id - cnt;
        MPI_Sendrecv(&sum, 1, MPI_INT, chatwith, 0, &nextsum, 1, MPI_INT, chatwith, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("%d %d\n", sum, nextsum);
        sum = sum + nextsum;
        cnt = cnt << 1;
    }
    printf("进程 %d: 数组全和为 %d\n", id, sum);
    MPI_Finalize();
    return 0;
}
