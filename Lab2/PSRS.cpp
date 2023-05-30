#include <stdio.h>
#include <algorithm>
#include <mpi.h>
#include <time.h>
int N = 30000;
int NUM_THREADS = 3;

int main(int argc, char* argv[])
{
    cin>>N;
    srand(time(NULL));
    MPI_Init(&argc, &argv);
    int thread_num;
    int id;
    MPI_Comm_size(MPI_COMM_WORLD, &thread_num);
    int NUM_THREADS = thread_num;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    int* A = (int*)malloc(N * sizeof(int));
    if (id == 0) {
        for (int i = 0; i < N; i++)
            A[i] = rand() % 10000;
    }  
    double start, end;
    // 比较串行时间
    if (id == 0) {
        start = MPI_Wtime();
        std::sort(A, A + N);
        end = MPI_Wtime();
        printf("Serial Time: %lf\n", end - start);
    }
    // 统计时间
    if (id == 0) start = MPI_Wtime();
    MPI_Bcast(A, N, MPI_INT, 0, MPI_COMM_WORLD);
    // step 1. ���Ȼ���
    int pstart = id * N / NUM_THREADS;
    int pend = (id + 1) * N / NUM_THREADS;
    // step 2. �ֲ�����
    std::sort(A + pstart, A + pend);
    // step 3. ѡȡ����
    int step = N / (NUM_THREADS * NUM_THREADS);
    int* samples = (int*)malloc(NUM_THREADS * sizeof(int));
    for (int j = 0; j < NUM_THREADS; j++) {
        samples[j] = A[pstart + j * step];
    }
    // ȫ�� samples ֻ��Ҫ�߳� 0 ��ȡ���ɣ�������Щ������ѡ����Ԫ
    // �ٽ���Ԫ�㲥�������߳�
    int* global_samples = (int*)malloc(NUM_THREADS * NUM_THREADS * sizeof(int));
    int* pivots = (int*)malloc(NUM_THREADS * sizeof(int));
    MPI_Gather(samples, NUM_THREADS, MPI_INT, global_samples, NUM_THREADS, MPI_INT, 0, MPI_COMM_WORLD);
    if (id == 0) {
        // step 4. ��������
        std::sort(global_samples, global_samples + NUM_THREADS * NUM_THREADS);
        // step 5. ѡ����Ԫ
        for (int i = 0; i < NUM_THREADS - 1; i++) {
            pivots[i] = global_samples[(i + 1) * NUM_THREADS];
        }
    }
    MPI_Bcast(pivots, NUM_THREADS - 1, MPI_INT, 0, MPI_COMM_WORLD);
    // step 6. ��Ԫ����
    int index = 0;
    int j = pstart;
    int local_parts[NUM_THREADS][N] = { 0 };
    int local_count[NUM_THREADS] = { 0 };
    while (j < pend && index < NUM_THREADS - 1) {
        if (A[j] <= pivots[index]) {
            local_parts[index][local_count[index]] = A[j];
            local_count[index]++; // <= pivot[index] ���� count[index] ��
            j++;
        }
        else index++;
    }
    // �������һ����Ԫ�Ĳ��֣���ʱ index = NUM_THREADS - 1
    while (j < pend) {
        local_parts[index][local_count[index]] = A[j];
        local_count[index]++;
        j++;
    }
    // step 7. ȫ�ֽ���
    // ÿ���߳� p_id ��Ҫ���� NUM_THREADS �� local_parts[id][] 
    int* send_buf = (int*)malloc(N * NUM_THREADS * sizeof(int));
    int* recv_buf = (int*)malloc(N * NUM_THREADS * sizeof(int));
    int* send_counts = (int*)malloc(NUM_THREADS * sizeof(int));
    int* recv_counts = (int*)malloc(NUM_THREADS * sizeof(int));
    int* send_displs = (int*)malloc(NUM_THREADS * sizeof(int));
    int* recv_displs = (int*)malloc(NUM_THREADS * sizeof(int));
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
    //printf("id = %d\n", id);
    //for (int i = 0; i < recv_displs[NUM_THREADS - 1] + recv_counts[NUM_THREADS - 1]; i++) {
    //    printf("%d  ", recv_buf[i]);
    //}
    //printf("\n");
    //std::sort(recv_buf, recv_buf + N * NUM_THREADS);
    //std::sort(recv_buf, recv_buf + recv_displs[NUM_THREADS - 1] + recv_counts[NUM_THREADS - 1]); 

    // ���ˣ����ȫ�ֽ���
    // ÿ���߳���һ��һά���� recv_buf,�� recv_couts �ֶ����򣨱���������Ϊ��Դ��ȫ�ֽ�����
    // step 8. �鲢����
    // �� recv_buf �ֶ� merge������д�뵽 A ����
    int result[N] = { 0 };
    int tmp[N] = { 0 };
    int result_count = 0, tmp_count = 0;
    for (int index = 0; index < NUM_THREADS; index++) {
        // Merge: result[] �� parts[index][id][]
        int i = 0, jj = 0, k = 0;
        int A_count = result_count;
        int B_count = recv_counts[index];
        tmp_count = A_count + B_count;
        while (i < A_count && jj < B_count) {
            if (result[i] <= recv_buf[recv_displs[index] + jj]) tmp[k++] = result[i++];
            else {
                tmp[k++] = recv_buf[recv_displs[index] + jj];
                jj++;
            }
        }
        while (i < A_count) tmp[k++] = result[i++];
        while (jj < B_count) {
            tmp[k++] = recv_buf[recv_displs[index] + jj];
            jj++;
        }
        result_count = tmp_count;
        for (int l = 0; l < tmp_count; l++) {
            result[l] = tmp[l];
        }
    }
    // �������һ�� BUG�������� 1.5 Сʱ���������������ĸ�дΪ index �ĵط�дΪ�� i
    int* A_counts = (int*)malloc(NUM_THREADS * sizeof(int));
    int* A_displs = (int*)malloc(NUM_THREADS * sizeof(int));
    MPI_Gather(&result_count, 1, MPI_INT, A_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (id == 0) {
        A_displs[0] = 0;
        for (int i = 1; i < NUM_THREADS; i++) {
            A_displs[i] = A_displs[i - 1] + A_counts[i - 1];
        }
    }
    MPI_Gatherv(result, result_count, MPI_INT, A, A_counts, A_displs, MPI_INT, 0, MPI_COMM_WORLD);
    // 统计时间 
    if (id == 0) {
        end = MPI_Wtime();
        printf("Time: %lf\n", end - start);
    }
    if (id == 0) {
        for (int i = 0; i < N; i++) {
            printf("%d ", A[i]);
        }
        printf("\n");
    }
    free(samples);
    free(global_samples);
    free(pivots);
    free(send_buf);
    free(recv_buf);
    free(send_counts);
    free(recv_counts);
    free(send_displs);
    free(recv_displs);
    free(A_counts);
    free(A_displs);
    MPI_Finalize();
    return 0;
}