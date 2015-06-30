#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>

#include "globals.h"

static double laplace(double lambda, struct drand48_data *buffer)
{
	double rnd;

	drand48_r(buffer, &rnd); /* rnd \in [0, 1)      */
	rnd = 0.5 - rnd;         /* rnd \in (-0.5, 0.5] */

	if (signbit(rnd)) /* rnd < 0 */
		return lambda * log(1 + 2 * rnd);
	return -lambda * log(1 - 2 * rnd);
}

void init_rng(long int seed, struct drand48_data *buffer)
{
	srand48_r(seed, buffer);
}

int random_int_pow2(int max, struct drand48_data *buffer)
{
	long int ret;

	lrand48_r(buffer, &ret);
	return ret & (max - 1);
}

int random_int(int max, struct drand48_data *buffer)
{
	double v;
	drand48_r(buffer, &v);
	return max * v;
}

int int_cmp(const void *a, const void *b)
{
	const int *ia = a, *ib = b;
	return *ia - *ib;
}

int int_cmp_r(const void *a, const void *b)
{
	const int *ia = a, *ib = b;
	return *ib - *ia;
}

int double_cmp(const void *a, const void *b)
{
	const double *fa = a, *fb = b;
	const double ret = *fa - *fb;
	return (ret > 0) - (ret < 0);
}

int double_cmp_r(const void *a, const void *b)
{
	return -double_cmp(a, b);
}

double laplace_mechanism(double x, double eps, double sens,
		struct drand48_data *buffer)
{
	return x + laplace(sens/eps, buffer);
}

int bsearch_i(const void *key, const void *base, size_t nmemb, size_t size,
		int (*compar)(const void *, const void *))
{
	int low = 0, high = nmemb - 1, mid, test;

	while (low <= high) {
		mid = low + ((high - low) >> 1);
		test = compar(base + mid * size, key);
		if (test > 0)
			high = mid - 1;
		else if (test == 0)
			return mid;
		else
			low = mid + 1;
	}

	return -low-1;
}

inline double noisy_div(double x, double y, double t)
{
	// TODO: abs tests
	// TODO: abs on y?
	if (y <= t)
		return 0;
	return x/y;
}

#define MICROSECONDS 1000000L
inline long time_diff(struct timeval *tv1, struct timeval *tv2)
{
	long t1 = tv1->tv_sec * MICROSECONDS + (0.0 + tv1->tv_usec);
	long t2 = tv2->tv_sec * MICROSECONDS + (0.0 + tv2->tv_usec);
	return t2 - t1;
}

inline int lnp2(int x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return ffs(x) - 1;
}
