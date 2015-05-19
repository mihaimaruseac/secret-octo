#ifndef _SPACE_H
#define _SPACE_H

struct space {
	int sz;
	int **data;
};

struct space *gen_space(int sz);
void free_space(struct space *s);

#endif
