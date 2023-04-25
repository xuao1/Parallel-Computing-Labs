#include <stdio.h>
#include <time.h>

static long num_steps = 1000000000;//越大值越精确
long double step;

int main() {
	int i;
	long double x, pi, sum = 0.0;
	// scanf("%ld", &num_steps);

	clock_t start = clock();

	step = 1.0 / (long double)num_steps;
	for (i = 1; i <= num_steps; i++) {
		x = (i - 0.5) * step;
		sum = sum + 4.0 / (1.0 + x * x);
	}
	pi = step * sum;

	clock_t end = clock();
	double duration = (double)(end - start) / CLOCKS_PER_SEC;
	printf("Time: %lf seconds\n", duration);

	printf("%.10Lf\n", pi);
	return 0;
}
