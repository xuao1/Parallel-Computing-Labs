#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int N;
    scanf("%d", &N);
    int *A = (int *)malloc(N * sizeof(int));
    int *B = (int *)malloc(N * sizeof(int));
    int *C = (int *)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = i;
    }

    clock_t start = clock();
    for (int i = 0; i < N; i++) {
        C[i] = A[i] + B[i];
    }
    clock_t end = clock();
    double timecost = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);
    printf("CPU time %.4fms\n", timecost);
    
    free(A);
    free(B);
    free(C);

    return 0;
}
