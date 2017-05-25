#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "immintrin.h"

typedef unsigned long uint64;

#define ALIGNED(N, T) ((N) / (T) + 1) * (T);

void measure_plain(uint64 *a, uint64 *b, uint64 *r, size_t sz) {
	uint64 *end = (uint64 *)(((void *)a) + sz);
	while (a < end) {
		*r = *(a++) & *(b++);
		r++;
	}
}

void measure_avx(uint64 *a, uint64 *b, uint64 *r, size_t sz) {
	uint64 *end = (uint64 *)(((void *)a) + sz);
	while (a < end) {

		__m256i a8 = _mm256_loadu_si256((__m256i*)a);
		__m256i b8 = _mm256_loadu_si256((__m256i*)b);
		__m256i c8 = _mm256_castps_si256(
			_mm256_and_ps(_mm256_castsi256_ps(a8), _mm256_castsi256_ps(b8)));
		_mm256_storeu_si256((__m256i*)r, c8);
		
		a = (uint64 *)((void *)a + 256);
		b = (uint64 *)((void *)b + 256);
		r = (uint64 *)((void *)r + 256);
	}
}

uint64 rand_long() {
	return rand() | ((uint64)rand() << 32);
}

int main(int argc, char** argv) {
	size_t sz = ALIGNED(150000000, sizeof(uint64) * 4);
	uint64 *a = (uint64 *)malloc(sz);
	uint64 *b = (uint64 *)malloc(sz);
	uint64 *r = (uint64 *)malloc(sz);

	for (size_t i=(sz/sizeof(uint64)) - 1; i > 0; i--) {
		a[i] = rand_long();
		b[i] = rand_long();
	}

	clock_t before = clock();
	measure_plain(a, b, r, sz);
	clock_t after = clock();

	printf("plain time: %.2f ms\n", (double)(after - before)/CLOCKS_PER_SEC * 1000);

	before = clock();
	measure_avx(a, b, r, sz);
	after = clock();

	printf("avx time: %.2f ms\n", (double)(after - before)/CLOCKS_PER_SEC * 1000);

	if ( *r != (*a & *b) ) {
		printf("RESULT IS INCORRECT");
	}

	free(a);
	free(b);
	free(r);
}

