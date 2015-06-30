#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "space.h"

static inline char expand(char v)
{
	switch (v) {
	case empty: return '.';
	case danger: return '*';
	case dontcare: return '-';
	default: return '0' + v;//'?';
	}
}

static inline char unexpand(char v)
{
	switch (v) {
	case '.': return empty;
	case '*': return danger;
	case '-': return dontcare;
	default: return v - '0';//'?';
	}
}

struct space *space_gen(int sz)
{
	struct space *space = calloc(1, sizeof(*space));
	int i;

	space->sz = sz;
	space->data = calloc(sz, sizeof(*space->data));
	for (i = 0; i < sz; i++)
		space->data[i] = calloc(sz, sizeof(**space->data));

	return space;
}

#define SZ 10
struct space *space_load(int *covered, FILE *f)
{
	struct space *space = calloc(1, sizeof(*space));
	char c;
	int i, j;

	fscanf(f, "%d", &space->sz);
	space->data = calloc(space->sz, sizeof(*space->data));
	for (i = 0; i < space->sz; i++)
		space->data[i] = calloc(space->sz, sizeof(**space->data));

	for (i = 0; i < space->sz; i++) {
		fscanf(f, "%c", &c);
		for (j = 0; j < space->sz; j++) {
			fscanf(f, "%c", &c);
			space->data[i][j] = unexpand(c);
		}
	}

	if (!covered)
		return space;

	*covered = 0;
	for (i = 0; i < space->sz; i++)
		for (j = 0; j < space->sz; j++)
			if (space->data[i][j] == danger)
				*covered = *covered + 1;

	return space;
}
#undef SZ

void space_print(struct space *space, FILE *f)
{
	int i, j;

	fprintf(f, "%d\n", space->sz);
	for (i = 0; i < space->sz; i++) {
		for (j = 0; j < space->sz; j++)
			fprintf(f, "%c", expand(space->data[i][j]));
		fprintf(f, "\n");
	}
}

void free_space(struct space *space)
{
	int i;

	for (i = 0; i < space->sz; i++)
		free(space->data[i]);
	free(space->data);
	free(space);
}

int space_generate_zone(struct space *space, float coverage, long int seed,
		int uniform)
{
	int cells_to_fill = space->sz * space->sz * coverage;
	struct drand48_data randbuffer;
	int x, y, i, j, k;
	double s, v;

	init_rng(seed, &randbuffer);
	printf("Filling %d cells\n", cells_to_fill);

	/* seed */
	x = random_int_pow2(space->sz, &randbuffer);
	y = random_int_pow2(space->sz, &randbuffer);
	space->data[x][y] = danger;

	/* fill */
	for (k = 1; k < cells_to_fill; k++) {
		s = 0;

		for (i = 0; i < space->sz; i++)
			for (j = 0; j < space->sz; j++) {
				if (space->data[i][j] == danger)
					continue;
				s += uniform ? 1 : exp(-sqrt((i - x) * (i - x) + (j - y) * (j - y)));
			}

		drand48_r(&randbuffer, &v);
		s *= v;

		for (i = 0; i < space->sz; i++)
			for (j = 0; j < space->sz; j++) {
				if (space->data[i][j] == danger)
					continue;
				s -= uniform ? 1 : exp(-sqrt((i - x) * (i - x) + (j - y) * (j - y)));
				if (s < 0) {
					space->data[i][j] = danger;
					i = j = space->sz;
				}
			}
	}

	return cells_to_fill;
}

void space_fill_dc(struct space *space, float depth_ratio, int cells_to_fill)
{
	int changed = 1, i, j, cnt, m = space->sz - 1, cover = 0, depth = 0, n;
	int max_depth = depth_ratio * sqrt(cells_to_fill);

	while (changed) {
		changed = 0;
		depth++;

		if (depth == max_depth)
			break;

		for (i = 0; i < space->sz; i++)
			for (j = 0; j < space->sz; j++) {
				if (space->data[i][j] != empty)
					continue;

				cnt = 0;
				if (i > 0 && space->data[i-1][j] != empty) cnt++;
				if (i < m && space->data[i+1][j] != empty) cnt++;
				if (j > 0 && space->data[i][j-1] != empty) cnt++;
				if (j < m && space->data[i][j+1] != empty) cnt++;

				if (cnt < 2)
					continue;

				n = space->sz * 2;
				if (i > 0 && space->data[i-1][j] != empty&& space->data[i-1][j] < n) n = space->data[i-1][j];
				if (i < m && space->data[i+1][j] != empty&& space->data[i+1][j] < n) n = space->data[i+1][j];
				if (j > 0 && space->data[i][j-1] != empty&& space->data[i][j-1] < n) n = space->data[i][j-1];
				if (j < m && space->data[i][j+1] != empty&& space->data[i][j+1] < n) n = space->data[i][j+1];

				if (n > depth)
					continue;

				space->data[i][j] = n + 1;
				cover++;
				changed = 1;
			}
	}

	for (i = 0; i < space->sz; i++)
		for (j = 0; j < space->sz; j++)
			if (space->data[i][j] > danger)
				space->data[i][j] = dontcare;

	printf("Covered %d cells\n", cover);
	printf("Depth %d\n", depth);
	printf("Max depth %d\n", max_depth);
}

static int gray(int x)
{
	return (x >> 1) ^ x;
}

static char* conv(int x, int sz, int hierarchical)
{
	char *ret = calloc(sz--, sizeof(*ret));

	if (!hierarchical)
		x = gray(x);

	while (x) {
		ret[sz--] = '0' + (x & 1);
		x >>= 1;
	}

	while (sz >= 0)
		ret[sz--] = '0';

	return ret;
}

void space_2_espresso(struct space *space, FILE *f, int hierarchical)
{
	int i, j, sz = lnp2(space->sz);
	char c, *s1, *s2;

	fprintf(f, ".i %d\n", 2 * sz);
	fprintf(f, ".o 1\n");
	for (i = 0; i < space->sz; i++)
		for (j = 0; j < space->sz; j++) {
			switch (space->data[i][j]) {
			case danger: c = '1'; break;
			case dontcare: c = '-'; break;
			default: continue;
			}

			s1 = conv(i, sz, hierarchical);
			s2 = conv(j, sz, hierarchical);
			fprintf(f, "%s%s %c\n", s1, s2, c);
			free(s1);
			free(s2);
		}
	fprintf(f, ".e\n");
}
