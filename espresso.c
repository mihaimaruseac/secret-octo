#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "space.h"

/* options as string */
#define OPTSTR "i:o:h"

/* Command line arguments */
static struct {
	/* file to read the zone from */
	char *in;
	/* file to write the espresso input to */
	char *out;
	/* flags */
	int hierarchical;
} args;

static void usage(const char *prg)
{
	fprintf(stderr, "Usage: %s [OPTIONS]\n", prg);
	fprintf(stderr, "\n");
	fprintf(stderr, "OPTIONS:\n");
	fprintf(stderr, "\t-i file\t\tfile to read zone from, \"-\" for stdin\n");
	fprintf(stderr, "\t-o file\t\toutput input to espresso to this file, \"-\" for stdout\n");
	fprintf(stderr, "\t-h\t\tgenerate hierarchical encoding instead of gray\n");
	exit(EXIT_FAILURE);
}

static void print_args()
{
	printf("Reading zone from %s\n", args.in);
	printf("Printing to espresso to %s\n", args.out);
	printf("Hierarchical: %s\n", args.hierarchical ? "YES" : "NO");
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
	args.out = NULL;
	args.hierarchical = 0;

	while((opt = getopt(argc, argv, OPTSTR)) != -1)
		switch(opt) {
		case 'i':
			args.in = strdup(optarg);
			break;
		case 'o':
			args.out = strdup(optarg);
			break;
		case 'h':
			args.hierarchical = 1;
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

	if (args.out == NULL) {
		fprintf(stderr, "Need output file. Use \"-\" for stdout\n");
		usage(argv[0]);
	}
}

static struct space* load(int *covered)
{
	struct space *space;
	FILE *f;

	if (strncmp(args.in, "-", 1) == 0)
		f = stdin;
	else if (!(f = fopen(args.in, "r"))) {
		perror("Invalid input file");
		exit(EXIT_FAILURE);
	}

	space = space_load(covered, f);

	fclose(f);
	return space;
}

static void save_2_espresso(struct space* space, int hierarchical)
{
	FILE *f;

	if (strncmp(args.out, "-", 1) == 0)
		f = stdout;
	else if (!(f = fopen(args.out, "w"))) {
		perror("Invalid output file");
		exit(EXIT_FAILURE);
	}

	space_2_espresso(space, f, hierarchical);

	fclose(f);
}

int main(int argc, char **argv)
{
	struct space *space;

	parse_arguments(argc, argv);
	print_args();

	space = load(NULL);
	save_2_espresso(space, args.hierarchical);

	free(args.in);
	free(args.out);
	free_space(space);
	return 0;
}
