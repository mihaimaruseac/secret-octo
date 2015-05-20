#include <stdio.h>
#include <stdlib.h>

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

	printf("Need to fill: %d\n", cells_to_fill);
}
