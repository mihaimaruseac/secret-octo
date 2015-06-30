#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

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

void pbc_decrypt(struct crypto *pbc, int tl, int *nse, int ts)
{
	/* e(C_0, K_0) -> pairing(C, K[i])
	 * e(C_{i1}, K_{i1}) -> pairing(Ci[i], Ki[i])
	 * e(C_{i2}, K_{i2}) -> pairing(Ci[i], Ki[i])
	 */
	element_t C, K[ts], Ci[tl], Ki[tl];
	struct timeval st, en;
	int i;

	/* setup */
	gettimeofday(&st, NULL);
	element_init_G1(C, pbc->pairing);
	element_random(C);
	for (i = 0; i < ts; i++) {
		element_init_G2(K[i], pbc->pairing);
		element_random(K[i]);
	}
	for (i = 0; i < tl; i++) {
		element_init_G1(Ci[i], pbc->pairing);
		element_random(Ci[i]);
	}
	for (i = 0; i < tl; i++) {
		element_init_G2(Ki[i], pbc->pairing);
		element_random(Ki[i]);
	}
	gettimeofday(&en, NULL);
	printf("Setup: %lu us\n", time_diff(&st, &en));

	printf("tl=%d ts=%d|", tl, ts);
	for (i = 0; i < ts; i++)
		printf("%d ", nse[i]);
	printf("\n");

	element_clear(C);
	for (i = 0; i < ts; i++)
		element_clear(K[i]);
	for (i = 0; i < tl; i++)
		element_clear(Ci[i]);
	for (i = 0; i < tl; i++)
		element_clear(Ki[i]);
}
