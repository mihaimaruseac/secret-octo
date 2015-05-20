#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "space.h"

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

void free_space(struct space *space)
{
	int i;

	for (i = 0; i < space->sz; i++)
		free(space->data[i]);
	free(space->data);
	free(space);
}

static inline char expand(char v)
{
	switch (v) {
	case empty: return '.';
	case danger: return '*';
	case dontcare: return '-';
	default: return '?';
	}
}

void space_print(struct space *space)
{
	int i, j;

	for (i = 0; i < space->sz; i++) {
		for (j = 0; j < space->sz; j++)
			printf("%c", expand(space->data[i][j]));
		printf("\n");
	}
}

void space_generate_zone(struct space *space, float coverage, long int seed)
{
	int cells_to_fill = space->sz * space->sz * coverage;
	struct drand48_data randbuffer;
	int x, y, i, j, k;
	double s, v;

	init_rng(seed, &randbuffer);

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
				s += exp(-sqrt((i - x) * (i - x) + (j - y) * (j - y)));
			}

		drand48_r(&randbuffer, &v);
		s *= v;

		for (i = 0; i < space->sz; i++)
			for (j = 0; j < space->sz; j++) {
				if (space->data[i][j] == danger)
					continue;
				s -= exp(-sqrt((i - x) * (i - x) + (j - y) * (j - y)));
				if (s < 0) {
					space->data[i][j] = danger;
					i = j = space->sz;
				}
			}
	}
}
