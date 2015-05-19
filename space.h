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
void space_print(struct space *space);
void free_space(struct space *space);

#endif
