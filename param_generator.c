#include <stdio.h>
#include <stdlib.h>

#include <pbc.h>

#define KEYSIZE 1024

int main(void)
{
	pbc_param_t param;
	FILE *paramFile;
	mpz_t p, q, N;

	paramFile = fopen("param", "w");
	if (!paramFile) {
		perror("Cannot open paramFile");
		exit(EXIT_FAILURE);
	}

	mpz_init(p);
	mpz_init(q);
	mpz_init(N);

	pbc_mpz_randomb(p, KEYSIZE>>1);
	pbc_mpz_randomb(q, KEYSIZE>>1);

	mpz_nextprime(p, p);
	mpz_nextprime(q, q);
	mpz_mul(N, p, q);

	pbc_param_init_a1_gen(param,N);
	pbc_param_out_str(paramFile, param);

	pbc_param_clear(param);
	mpz_clear(p);
	mpz_clear(q);
	mpz_clear(N);

	fclose(paramFile);
	return 0;
}

