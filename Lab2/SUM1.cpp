#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int N = 8;
int NUM_THREADS = 8;

int main(int argc, char** argv) {
    int id, thread_num;
    thread_num = NUM_THREADS;
    // int * recv_buf, sum = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &thread_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    N = NUM_THREADS = thread_num;
    srand(time(NULL));
    int* A = (int*)malloc(N * sizeof(int));
    if (id == 0) {
        for (int i = 0; i < N; i++)
            A[i] = rand() % 100;
        printf("Array: \n");
        for (int i = 0; i < N; i++)
            printf("%d ", A[i]);
        printf("\n");
    }
    MPI_Bcast(A, N, MPI_INT, 0, MPI_COMM_WORLD);
    int sum = A[id], nextsum = 0;
    int cnt = 1; // ¿ç²½
    int chatwith;
    while (cnt <= N / 2) {
        if (id % (cnt * 2) < cnt) chatwith = id + cnt;
        else chatwith = id - cnt;
        MPI_Sendrecv(&sum, 1, MPI_INT, chatwith, 0, &nextsum, 1, MPI_INT, chatwith, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("%d %d\n", sum, nextsum);
        sum = sum + nextsum;
        cnt = cnt << 1;
    }
    printf("thread id: %d: sum: %d\n", id, sum);
    MPI_Finalize();
    return 0;
}
