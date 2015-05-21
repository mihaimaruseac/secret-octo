#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "space.h"

/* min, max space size (log scale) */
#define MIN_SZ 2
#define MAX_SZ 20

/* coverage bounds */
#define MIN_COVERAGE 0.0199999
#define MAX_COVERAGE 0.4000001

/* default seed */
#define DEFAULT_SEED 42

/* depth: bounds and coverage */
#define MIN_DEPTH 0.0999999
#define MAX_DEPTH 1
#define DEFAULT_DEPTH 1

/* options as string */
#define OPTSTR "R:s:c:d:f"

/* Command line arguments */
static struct {
	/* size of the grid */
	int sz;
	/* coverage */
	float coverage;
	/* max depth */
	float depth;

	/* use don't care */
	int dc;

	/* random seed */
	long int seed;
} args;

static void usage(const char *prg)
{
	fprintf(stderr, "Usage: %s [OPTIONS]\n", prg);
	fprintf(stderr, "\n");
	fprintf(stderr, "OPTIONS:\n");
	fprintf(stderr, "\t-s sz\tlog(size) of the length of the space (int)\n");
	fprintf(stderr, "\t-c coverage\tratio of alert zone's area to the area of the space (real)\n");
	fprintf(stderr, "\t-d depth\tmax expansion depth in percents of the max depth(real, default 1.0)\n");
	fprintf(stderr, "\t-R seed\t\tseed of the random number generator, (int, default 42)\n");
	fprintf(stderr, "\t-f\t\tgenerate new don't care tiles to help minimization\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "CONSTRAINTS:\n");
	fprintf(stderr, "\t%d <= sz <= %d\n", MIN_SZ, MAX_SZ);
	fprintf(stderr, "\t%f <= coverage <= %f\n", MIN_COVERAGE, MAX_COVERAGE);
	fprintf(stderr, "\t%f <= depth <= %f\n", MIN_DEPTH, MAX_DEPTH);
	exit(EXIT_FAILURE);
}

static void print_args()
{
	printf("Grid side length: %d\n", args.sz);
	printf("Coverage: %f\n", args.coverage);
	printf("Seed: %ld\n", args.seed);
	printf("Extra space: %c\n", args.dc ? 'Y' : 'N');
	printf("Depth: %f\n", args.depth);
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

	args.seed = DEFAULT_SEED;
	args.depth = DEFAULT_DEPTH;
	args.dc = 0;

	while((opt = getopt(argc, argv, OPTSTR)) != -1)
		switch(opt) {
		case 's':
			if (sscanf(optarg, "%d%c", &args.sz, &extra) != 1)
				usage(argv[0]);
			break;
		case 'c':
			if (sscanf(optarg, "%f%c", &args.coverage, &extra) != 1)
				usage(argv[0]);
			break;
		case 'd':
			if (sscanf(optarg, "%f%c", &args.depth, &extra) != 1)
				usage(argv[0]);
			break;
		case 'R':
			if (sscanf(optarg, "%ld%c", &args.seed, &extra) != 1)
				usage(argv[0]);
			break;
		case 'f':
			args.dc = 1;
			break;
		default: usage(argv[0]);
		}

	/* consistency checks */
	if (optind != argc)
		usage(argv[0]); /* extra arguments */

	if (MIN_SZ > args.sz || args.sz > MAX_SZ) {
		fprintf(stderr, "Invalid sz argument");
		usage(argv[0]);
	} else
		args.sz = 1 << args.sz;

	if (MIN_COVERAGE > args.coverage || args.coverage > MAX_COVERAGE) {
		fprintf(stderr, "Invalid coverage argument");
		usage(argv[0]);
	}

	if (MIN_DEPTH > args.depth || args.depth > MAX_DEPTH) {
		fprintf(stderr, "Invalid depth argument");
		usage(argv[0]);
	}
}

int main(int argc, char **argv)
{
	struct space *space;

	parse_arguments(argc, argv);
	print_args();

	space = space_gen(args.sz);
	space_generate_zone(space, args.coverage, args.seed);

	if (args.dc)
		space_fill_dc(space, args.depth);

	//space_print(space);

	space_2_espresso(space, stdout);

	free_space(space);
	return 0;
}
