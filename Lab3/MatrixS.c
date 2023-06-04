#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 1024

void matrixMultiply(const float* a, const float* b, float* c, int n) {
    for (int row = 0; row < n; row++) {
        for (int column = 0; column < n; column++) {
            float t = 0;
            for (int i = 0; i < n; i++) {
                t += a[row * n + i] * b[i * n + column];
            }
            c[row * n + column] = t;
        }
    }
}

int main() 
{
    srand(time(NULL));
    float *a, *b, *c;
    int n = MATRIX_SIZE;
    a = (float*)malloc(sizeof(float) * n * n);
    b = (float*)malloc(sizeof(float) * n * n);
    c = (float*)malloc(sizeof(float) * n * n);

    ///生成矩阵a, b
    for (int i = 0; i < n * n; i++) {
        a[i] = (float)rand() / (float)(RAND_MAX);
        b[i] = (float)rand() / (float)(RAND_MAX);
        c[i] = 0.0;
    }

    clock_t start, end;
    start = clock();

    matrixMultiply(a, b, c, n);

    end = clock();
    double timecost = 1000.0 * (end - start) / CLOCKS_PER_SEC;
    printf("CPU time %.4fms\n", timecost);

    free(a);
    free(b);
    free(c);

    return 0;
}
