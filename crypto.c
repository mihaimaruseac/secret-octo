#include <stdlib.h>
#include <stdio.h>

#include <pbc.h>

#include "crypto.h"

struct crypto {
	pairing_t pairing;
};

struct crypto* init_pbc()
{
	struct crypto *pbc = calloc(1, sizeof(*pbc));
	int paramLength;
	size_t count;
	char *param;
	FILE *fp;

	fp = fopen("param", "rb");
	if (!fp) {
		perror("Cannot load pbc params");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	paramLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	param = calloc(paramLength, sizeof(param[0]));
	count = fread(param, 1, paramLength, fp);
	pairing_init_set_buf(pbc->pairing, param, count);
	free(param);
	fclose(fp);
	return pbc;
}

void clean_pbc(struct crypto *pbc)
{
	pairing_clear(pbc->pairing);
	free(pbc);
}
