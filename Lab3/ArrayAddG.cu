#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//CUDA RunTime API
#include <cuda_runtime.h>
//单个block大小
#define THREAD_NUM 256

__global__ void ArrayAdd(int *A, int *B, int *C, int N){
    int index = blockDim.x * blockIdx.x + threadIdx.x;
    if (index < N) {
        C[index] = A[index] + B[index];
    }
}

int main()
{
    int N;
    scanf("%d", &N);
    int blocks_num = (N + THREAD_NUM - 1) / THREAD_NUM;
    int *A = (int *) malloc(N *sizeof(int));
    int *B = (int *) malloc(N *sizeof(int));
    int *C = (int *) malloc(N *sizeof(int));
    for(int i = 0; i < N; i++){
        A[i] = i;
        B[i] = i;
    }

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    int *CUDA_A, *CUDA_B, *CUDA_C;
    cudaMalloc(&CUDA_A, N * sizeof(int));
    cudaMalloc(&CUDA_B, N * sizeof(int));
    cudaMalloc(&CUDA_C, N * sizeof(int));

    cudaMemcpy(CUDA_A, A, N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(CUDA_B, B, N * sizeof(int), cudaMemcpyHostToDevice);
    
    ArrayAdd<<<blocks_num, THREAD_NUM, 0>>>(CUDA_A, CUDA_B, CUDA_C, N);

    cudaMemcpy(C, CUDA_C, sizeof(int) * N, cudaMemcpyDeviceToHost);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    float timecost;
    cudaEventElapsedTime(&timecost, start, stop);
    printf("CUDA time %.4fms\n", timecost);
    
    cudaFree(CUDA_A);
    cudaFree(CUDA_B);
    cudaFree(CUDA_C);
    free(A);
    free(B);
    free(C);

    return 0;
}
