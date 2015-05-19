#ifndef _SPACE_H
#define _SPACE_H

struct space {
	int sz;
	char **data;
};

enum space_tile {
	empty = 0;
	danger = 1;
	dontcare = 2;
};

struct space *gen_space(int sz);
void space_print(struct space *space);
void free_space(struct space *space);

#endif
