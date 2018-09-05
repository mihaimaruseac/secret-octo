#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include <pbc.h>

#include "crypto.h"
#include "globals.h"

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

void pbc_decrypt(struct crypto *pbc, int tl, int *nse, int ts,
		int use_caching, int use_product)
{
	/* e(C_0, K_0) -> pairing(C, K[i])
	 * e(C_{i1}, K_{i1}) -> pairing(Ci[i], Ki[i])
	 * e(C_{i2}, K_{i2}) -> pairing(Ci[i], Ki[i])
	 */
	//element_t C, K[ts], Ci[tl], Ki[tl], R, A, B, M, T;
	int num_mul=0, num_pair=0;
	struct timeval st, en;
	int i, j;

	/* -------------------- setup --------------------- */
	gettimeofday(&st, NULL);
	//element_init_G1(C, pbc->pairing);
	//element_random(C);
	for (i = 0; i < ts; i++) {
		//element_init_G2(K[i], pbc->pairing);
		//element_random(K[i]);
	}
	for (i = 0; i < tl; i++) {
		//element_init_G1(Ci[i], pbc->pairing);
		//element_random(Ci[i]);
	}
	for (i = 0; i < tl; i++) {
		//element_init_G2(Ki[i], pbc->pairing);
		//element_random(Ki[i]);
	}
	//element_init_GT(R, pbc->pairing);
	//element_init_GT(A, pbc->pairing);
	//element_init_GT(B, pbc->pairing);
	//element_init_GT(M, pbc->pairing);
	//element_init_GT(T, pbc->pairing);
	gettimeofday(&en, NULL);
	printf("Setup: %lu us\n", time_diff(&st, &en));

	printf("tl=%d ts=%d|", tl, ts);
	for (i = 0; i < ts; i++)
		printf("%d ", nse[i]);
	printf("\n");

	/* ------------------ decryption ------------------ */
	gettimeofday(&st, NULL);
	/* e(C0, K0) across all tokens */
	if (use_caching) {
		//pairing_pp_t pp;
		//pairing_pp_init(pp, C, pbc->pairing);
		//for (i = 0; i < ts; i++)
		//	pairing_pp_apply(R, K[i], pp);
		//pairing_pp_clear(pp);
	} else
		for (i = 0; i < ts; i++) {
			//pairing_apply(R, C, K[i], pbc->pairing);
			num_pair++;
		}

	/* prod{e(Ci1, Ki1)}prod{e(Ci2, Ki2)} across all tokens */
	if (use_product) {
		for (i = 0; i < ts; i++) {
			if (!nse[i])continue;
			//element_prod_pairing(A, Ci, Ki, nse[i]);
			//element_prod_pairing(B, Ci, Ki, nse[i]);
			//element_mul(M, A, B);
		}
	} else
		for (i = 0; i < ts; i++) {
			//element_set1(A);
			for (j = 0; j < nse[i]; j++) {
				//element_pairing(T, Ci[j], Ki[j]);
				//element_mul(A, A, T);
				num_pair++;
				num_mul++;
			}
			//element_set1(B);
			for (j = 0; j < nse[i]; j++) {
				//element_pairing(T, Ci[j], Ki[j]);
				//element_mul(B, B, T);
				num_pair++;
				num_mul++;
			}
			//element_mul(M, A, B);
			num_mul++;
		}
	gettimeofday(&en, NULL);
	printf("Decryption: %lu us\n", time_diff(&st, &en));
	if (use_caching || use_product) {
		num_pair = -1;
		num_mul = -1;
	}
	printf("Used %d pairings and %d multiplications\n", num_pair, num_mul);

	/* ------------------- cleanup -------------------- */
	gettimeofday(&st, NULL);
	//element_clear(T);
	//element_clear(M);
	//element_clear(B);
	//element_clear(A);
	//element_clear(R);
	//element_clear(C);
	//for (i = 0; i < ts; i++)
	//	element_clear(K[i]);
	//for (i = 0; i < tl; i++)
	//	element_clear(Ci[i]);
	//for (i = 0; i < tl; i++)
	//	element_clear(Ki[i]);
	gettimeofday(&en, NULL);
	printf("Cleanup: %lu us\n", time_diff(&st, &en));
}
