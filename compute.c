#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "crypto.h"

/* options as string */
#define OPTSTR "i:cp"

/* Command line arguments */
static struct {
	/* file to read the zone from */
	char *in;
	/* flags */
	int use_caching;
	int use_product;
} args;

static void usage(const char *prg)
{
	fprintf(stderr, "Usage: %s [OPTIONS]\n", prg);
	fprintf(stderr, "\n");
	fprintf(stderr, "OPTIONS:\n");
	fprintf(stderr, "\t-i file\t\tfile to read espresso output from, \"-\" for stdin\n");
	fprintf(stderr, "\t-c\t\tuse caching optimization -- pairing_pp_apply\n");
	fprintf(stderr, "\t-p\t\tuse product optimization -- element_prod_pairing\n");
	exit(EXIT_FAILURE);
}

static void print_args()
{
	printf("Reading espresso output from %s\n", args.in);
	printf("Caching: %s\n", args.use_caching ? "YES" : "NO");
	printf("Fast product: %s\n", args.use_product ? "YES" : "NO");
}

static void parse_arguments(int argc, char **argv)
{
	int opt;
	int i;

	printf("Called with: argc=%d\n", argc);
	for (i = 0; i < argc; i++)
		printf("%s ", argv[i]);
	printf("\n");

	args.in = NULL;
	args.use_caching = 0;
	args.use_product = 0;

	while((opt = getopt(argc, argv, OPTSTR)) != -1)
		switch(opt) {
		case 'i':
			args.in = strdup(optarg);
			break;
		case 'c':
			args.use_caching = 1;
			break;
		case 'p':
			args.use_product = 1;
			break;
		default: usage(argv[0]);
		}

	/* consistency checks */
	if (optind != argc)
		usage(argv[0]); /* extra arguments */

	if (args.in == NULL) {
		fprintf(stderr, "Need input file. Use \"-\" for stdin\n");
		usage(argv[0]);
	}
}

/**
 * Reads the output from espresso. Don't read as a structure since it is only
 * intermediate data and we can change it anyway later
 */
static int* load(int *tokenlen, int *tokens)
{
	char *line = NULL;
	int *data = NULL;
	int i = 0, j;
	size_t s;
	FILE *f;

	if (strncmp(args.in, "-", 1) == 0)
		f = stdin;
	else if (!(f = fopen(args.in, "r"))) {
faulty_file:
		free(line);
		perror("Invalid input file");
		exit(EXIT_FAILURE);
	}

	*tokenlen = 0;
	*tokens = 0;

	while(1) {
		free(line);
		line = NULL;
		if (getline(&line, &s, f) < 0)
			goto faulty_file;

		if (line[0] == '#') continue; /* skip comments */
		if (line[0] == '.')
			switch(line[1]) {
			case 'i': *tokenlen = atoi(line + 3); continue;
			case 'o': continue; /* ignore */
			case 'p': *tokens = atoi(line + 3);
				  data = calloc(*tokens, sizeof(*data));
				  continue;
			case 'e': goto end;
			default: goto faulty_file;
			}

		for (j = 0; j < *tokenlen; j++)
			if (line[j] != '-')
				data[i]++;
		i++;
	}
end:
	if (i != *tokens)
		goto faulty_file;

	fclose(f);
	free(line);
	return data;
}

int main(int argc, char **argv)
{
	struct crypto *pbc;
	int tl, ts;
	int *nse;

	parse_arguments(argc, argv);
	print_args();

	pbc = init_pbc();
	nse = load(&tl, &ts);
	pbc_decrypt(pbc, tl, nse, ts, args.use_caching, args.use_product);

	clean_pbc(pbc);
	free(args.in);
	free(nse);
	return 0;
}
