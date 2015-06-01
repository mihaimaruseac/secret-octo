#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "space.h"

/* depth: bounds and default */
#define MIN_DEPTH 0.0
#define MAX_DEPTH 100.0
#define DEFAULT_DEPTH 1.0

/* options as string */
#define OPTSTR "d:i:o:"

/* Command line arguments */
static struct {
	/* max depth */
	float depth;
	/* file to read the zone from */
	char *in;
	/* file to write the zone to */
	char *out;
} args;

static void usage(const char *prg)
{
	fprintf(stderr, "Usage: %s [OPTIONS]\n", prg);
	fprintf(stderr, "\n");
	fprintf(stderr, "OPTIONS:\n");
	fprintf(stderr, "\t-d depth\tmax expansion depth in percents of the max depth(real, default 1.0)\n");
	fprintf(stderr, "\t-i file\t\tfile to read zone from, \"-\" for stdin\n");
	fprintf(stderr, "\t-o file\t\toutput zone to this file, \"-\" for stdout\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "CONSTRAINTS:\n");
	fprintf(stderr, "\t%f <= depth <= %f\n", MIN_DEPTH, MAX_DEPTH);
	exit(EXIT_FAILURE);
}

static void print_args()
{
	printf("Depth: %f\n", args.depth);
	printf("Reading zone from %s\n", args.in);
	printf("Printing zone to %s\n", args.out);
}

static void parse_arguments(int argc, char **argv)
{
	char extra;
	int opt;
	int i;

	printf("Called with: argc=%d\n", argc);
	for (i = 0; i < argc; i++)
		printf("%s ", argv[i]);
	printf("\n");

	args.depth = DEFAULT_DEPTH;
	args.in = NULL;
	args.out = NULL;

	while((opt = getopt(argc, argv, OPTSTR)) != -1)
		switch(opt) {
		case 'd':
			if (sscanf(optarg, "%f%c", &args.depth, &extra) != 1)
				usage(argv[0]);
			break;
		case 'i':
			args.in = strdup(optarg);
			break;
		case 'o':
			args.out = strdup(optarg);
			break;
		default: usage(argv[0]);
		}

	/* consistency checks */
	if (optind != argc)
		usage(argv[0]); /* extra arguments */

	if (MIN_DEPTH > args.depth || args.depth > MAX_DEPTH) {
		fprintf(stderr, "Invalid depth argument\n");
		usage(argv[0]);
	}

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

static void save(struct space* space)
{
	FILE *f;

	if (strncmp(args.out, "-", 1) == 0)
		f = stdout;
	else if (!(f = fopen(args.out, "w"))) {
		perror("Invalid output file");
		exit(EXIT_FAILURE);
	}

	space_print(space, f);

	fclose(f);
}

int main(int argc, char **argv)
{
	struct space *space;
	int c;

	parse_arguments(argc, argv);
	print_args();

	space = load(&c);
	space_fill_dc(space, args.depth, c);
	save(space);

	free(args.in);
	free(args.out);
	free_space(space);
	return 0;
}
