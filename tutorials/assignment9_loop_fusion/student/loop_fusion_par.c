#include <omp.h>

void compute(unsigned long **a, unsigned long **b, unsigned long **c, unsigned long **d, int N, int num_threads) {

	// perform loop fusion and use OpenMP directives appropriately. 

 
	/*
	for (int i = 1; i < N; i++) {
		for (int j = 1; j < N; j++) {
			a[i][j] = 2 * b[i][j] +d[i][j];
		}
	}

	for (int i = 1; i < N; i++) {
		for (int j = 1; j < N; j++) {
			d[i][j + 1] = a[i][j + 1] * c[i][j    ];
			c[i][j - 1] = a[i][j - 1] - a[i][j + 1];
		}
	}
	*/
	#pragma omp parallel for num_threads(num_threads)
	for (int i = 1; i < N; i++) {
		a[i][1] = 2 * b[i][1] +d[i][1];
		for (int j = 2; j < N; j++) {
			a[i][j] = 2 * b[i][j] +d[i][j];
			c[i][j - 2] = a[i][j - 2] - a[i][j];
			d[i][j] = a[i][j] * c[i][j -1];
		}

		c[i][N-2] = a[i][N-2] - a[i][N];
		d[i][N] = a[i][N] * c[i][N-1];
	}

}
