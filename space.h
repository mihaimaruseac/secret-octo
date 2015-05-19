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

struct space *gen_space(int sz);
void space_print(struct space *space);
void free_space(struct space *space);

#endif
