#include <stdlib.h>
#include <stdio.h>

#include <pbc.h>

#include "crypto.h"

struct crypto {
	pairing_t pairing;
	mpz_t p;
	mpz_t q;
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

	fp = fopen("factoring", "rb");
	if (!fp) {
faulty_file:
		perror("Cannot load pbc factoring");
		exit(EXIT_FAILURE);
	}

	if (!mpz_inp_str(pbc->p, fp, 10))
		goto faulty_file;
	if (!mpz_inp_str(pbc->q, fp, 10))
		goto faulty_file;

	fclose(fp);

	return pbc;
}

void clean_pbc(struct crypto *pbc)
{
	mpz_clear(pbc->p);
	mpz_clear(pbc->q);
	pairing_clear(pbc->pairing);
	free(pbc);
}
