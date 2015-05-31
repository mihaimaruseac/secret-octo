#ifndef _SPACE_H
#define _SPACE_H

struct space {
	int sz;
	char **data;
};

enum space_tile {
	empty,
	danger,
	dontcare,
};

struct space *space_gen(int sz);
void space_print(struct space *space, FILE *f);
int space_generate_zone(struct space *space, float coverage, long int seed);
void space_fill_dc(struct space *space, float depth_ratio, int cells_to_fill);
void free_space(struct space *space);

void space_2_espresso(struct space *space, FILE *f);

#endif
