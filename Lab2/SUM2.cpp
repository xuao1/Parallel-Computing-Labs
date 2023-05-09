#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
int N = 8;
int NUM_THREADS = 8;

int main(int argc, char** argv) {
    srand(time(NULL));
    ptintf("please input the number of numbers: \n");
    scanf("%d", &N);
    int* A = (int*)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        A[i] = rand % 100;
    }
    printf("Array: \n");
    for (int i = 0; i < N; i++)
        printf("%d ", A[i]);
    printf("\n");
    ptintf("please input the number of threads: \n");
    scanf("%d", &NUM_THREADS);
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
