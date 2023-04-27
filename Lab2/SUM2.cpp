#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 8
#define NUM_THREADS 8
int A[8] = { 0,1,2,3,4,5,6,7 };

int main(int argc, char** argv) {
    int id, thread_num;
    thread_num = NUM_THREADS;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &thread_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    int sum = A[id], nextsum = 0;
    int cnt = 1;
    int flag = 0;
    // 自底向上，执行 logN 次。
    while (cnt < N) {
        flag = id & cnt;
        if (flag == 0) {
            MPI_Recv(&nextsum, 1, MPI_INT, id + cnt, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum = sum + nextsum;
        }
        else {
            MPI_Send(&sum, 1, MPI_INT, id - cnt, 0, MPI_COMM_WORLD);
        }
        cnt = cnt << 1;
    }
    // 自顶向下，传递全和
    cnt = cnt >> 1;
    while (cnt >= 1) {
        flag = id & cnt;
        if (flag == 0) {
            MPI_Send(&sum, 1, MPI_INT, id + cnt, 0, MPI_COMM_WORLD);
        }
        else {
            MPI_Recv(&sum, 1, MPI_INT, id - cnt, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        cnt = cnt >> 1;
    }
    printf("thread id: %d, sum: %d\n", id, sum);
    MPI_Finalize();
    return 0;
}
