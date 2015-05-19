#include <stdlib.h>

#include "space.h"

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
