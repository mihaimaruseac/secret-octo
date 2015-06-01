#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "space.h"

/* depth: bounds and default */
#define MIN_DEPTH 0.0
#define MAX_DEPTH 100.0
#define DEFAULT_DEPTH 1.0

/* options as string */
#define OPTSTR "d:"

/* Command line arguments */
static struct {
	/* max depth */
	float depth;
} args;

static void usage(const char *prg)
{
	fprintf(stderr, "Usage: %s [OPTIONS]\n", prg);
	fprintf(stderr, "\n");
	fprintf(stderr, "OPTIONS:\n");
	fprintf(stderr, "\t-d depth\tmax expansion depth in percents of the max depth(real, default 1.0)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "CONSTRAINTS:\n");
	fprintf(stderr, "\t%f <= depth <= %f\n", MIN_DEPTH, MAX_DEPTH);
	exit(EXIT_FAILURE);
}

static void print_args()
{
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

	args.depth = DEFAULT_DEPTH;

	while((opt = getopt(argc, argv, OPTSTR)) != -1)
		switch(opt) {
		case 'd':
			if (sscanf(optarg, "%f%c", &args.depth, &extra) != 1)
				usage(argv[0]);
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
}

int main(int argc, char **argv)
{
	struct space *space;
	int c;

	parse_arguments(argc, argv);
	print_args();

#if 0
	/* TODO: separate in different execs in a pipeline */

	space = space_gen(args.sz);
	c = space_generate_zone(space, args.coverage, args.seed);
	space_print(space);

	if (args.dc)
		space_fill_dc(space, args.depth, c);

	//space_print(space);

	//space_2_espresso(space, stdout);
#endif

	free_space(space);
	return 0;
}
