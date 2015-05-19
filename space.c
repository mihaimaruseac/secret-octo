#include <stdio.h>
#include <stdlib.h>

#include "space.h"

static inline char expand(char v);

struct space *gen_space(int sz)
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

void space_print(struct space *space)
{
	int i, j;

	for (i = 0; i < space->sz; i++) {
		for (j = 0; j < space->sz; j++)
			printf("%c", expand(space->data[i][j]));
		printf("\n");
	}
}

static inline char expand(char v)
{
	char expand_vec[] = {'.', '*', '-', '?'};
	int s = sizeof(expand_vec) / sizeof(expand_vec[0]);

	if (0 > v || v >= s)
		v = s - 1;

	return expand_vec[v];
}
