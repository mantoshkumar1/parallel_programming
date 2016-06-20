#include <omp.h>

void compute(unsigned long **a, unsigned long **b, unsigned long **c, unsigned long **d, int N, int num_threads) {

	// apply the loop transformation and use omp directives to get speedup

	#pragma omp parallel for num_threads(num_threads)
	for (int j = 1; j < N; j++) {
		for (int i = 1; i < N; i++) {
			//a[i][j]   = a[i][j] * b[i][j+1]+d[i][j-1];  ABD
			//b[i][j] = 2 * a[i][j] * c[i - 1][j];        BAC
			//c[i][j] = 3 * d[i][j];                      CD
			//d[i][j]   = 2 * c[i-1][j];                  DC

			c[i][j] = 3 * d[i][j];                      
			d[i][j]   = 2 * c[i-1][j];                 
		}
	}

	for (int j = 1; j < N; j++) {
		for (int i = 1; i < N; i++) {
			a[i][j]   = a[i][j] * b[i][j+1]+d[i][j-1]; 
			b[i][j] = 2 * a[i][j] * c[i - 1][j];       

		}
	}
}
