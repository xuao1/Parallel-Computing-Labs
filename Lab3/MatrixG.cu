#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//CUDA RunTime API
#include <cuda_runtime.h>
//单个block大小
#define THREAD_NUM 256
///矩阵大小
#define MATRIX_SIZE 1000
///block个数
int blocks_num = (MATRIX_SIZE + THREAD_NUM - 1) / THREAD_NUM;

__global__ static void CUDAkernal(const float* a, const float* b, float* c, int n)
{
    //block内的threadID
    const int tid = threadIdx.x;
    //blockID
    const int bid = blockIdx.x;
    //全局threadID
    const int idx = bid * THREAD_NUM + tid;
    const int row = idx / n;
    const int column = idx % n;
    //计算矩阵乘法
    if (row < n && column < n)
    {
        float t = 0;
        for (int i = 0; i < n; i++)
        {
            t += a[row * n + i] * b[i * n + column];
        }
        c[row * n + column] = t;
    }
}

void generateMatrix(float *a, float *b, int n){
    for(int i = 0; i < n * n; i++) {
        a[i] = (float)i / 2;
        b[i] = (float)i / 3;
    }
}

int main() 
{
    //定义矩阵
    float *a, *b, *c;
    int n = MATRIX_SIZE;
    //分配主机端内存
    a = (float*)malloc(sizeof(float)* n * n); 
    b = (float*)malloc(sizeof(float)* n * n); 
    c = (float*)malloc(sizeof(float)* n * n); 

    ///生成矩阵a, b
    generateMatrix(a, b, n);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    float *cuda_a, *cuda_b, *cuda_c;
    //分配设备端显存 
    cudaMalloc((void**)&cuda_a, sizeof(float)* n * n);
    cudaMalloc((void**)&cuda_b, sizeof(float)* n * n);
    cudaMalloc((void**)&cuda_c, sizeof(float)* n * n);

    //cudaMemcpyHostToDevice - 从内存复制到显存
    //cudaMemcpyDeviceToHost - 从显存复制到内存
    cudaMemcpy(cuda_a, a, sizeof(float)* n * n, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_b, b, sizeof(float)* n * n, cudaMemcpyHostToDevice);

    ///设备端函数
    CUDAkernal <<< blocks_num, THREAD_NUM, 0 >>>(cuda_a , cuda_b , cuda_c , n);

    //cudaMemcpy 将结果从显存中复制回内存
    cudaMemcpy(c, cuda_c, sizeof(float)* n * n, cudaMemcpyDeviceToHost);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    float timecost;
    cudaEventElapsedTime(&timecost, start, stop);
    printf("CUDA time %.4fms\n", timecost);

    cudaFree(cuda_a);
    cudaFree(cuda_b);
    cudaFree(cuda_c);
    free(a);
    free(b);
    free(c);

    return 0;
}