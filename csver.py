import itertools
import sys

def skip_lines_read_next(f, num):
    for i in range(num):
        l = f.readline()
        while len(l) == 1: l = f.readline()
    return f.readline()

def parse_last_item_line(line, cvtf=int):
    return cvtf(line.split()[-1])

exps = {}
counts = {}
sums = {}

def record_exp(side, coverage, seed, fillMode, dr, filled, covered, depth,
        maxDepth, cost):
    k = side, coverage, dr if fillMode else 0.0, seed
    k1 = k[:-1]
    v = depth, maxDepth, filled, covered, cost

    assert(not exps.has_key(k))
    exps[k] = v

    counts[k1] = counts.get(k1, 0) + 1
    v0 = sums.get(k1, [0])
    sums[k1] = list(itertools.imap(lambda x,y: x + y, v, itertools.cycle(v0)))

for fname in sys.argv[1:]:
    with open(fname, "r") as f:
        try:
            while True:
                side = parse_last_item_line(skip_lines_read_next(f, 4), int)
                coverage = parse_last_item_line(f.readline(), float)
                seed = parse_last_item_line(f.readline(), int)
                fillMode = parse_last_item_line(f.readline(), lambda x: x == 'Y')
                dr = parse_last_item_line(f.readline(), float)
                filled = parse_last_item_line(f.readline().split()[-2], int)
                covered = parse_last_item_line(f.readline().split()[-2], int) if fillMode else 0
                depth = parse_last_item_line(f.readline(), int) if fillMode else 0
                maxDepth = parse_last_item_line(f.readline(), int) if fillMode else 0
                cost = parse_last_item_line(skip_lines_read_next(f, 4).split()[9],
                        lambda x: int(x.split('=')[-1]))
                record_exp(side, coverage, seed, fillMode, dr, filled,
                    covered, depth, maxDepth, cost)
        except IndexError:
            pass # finish the loop

print "size\tcov\tfdr\tRandom_seed\tdepth\tmxDpth\tfilled\tcovered\tcost\toverhead\tperformance"
for k in sorted(exps.keys()):
    v = list(exps[k])
    k = list(k)
    k1 = k[:]; k1[2] = 0; k1 = tuple(k1); cost0 = exps[k1][-1]
    print '\t'.join([
        '{:3}'.format(k[0]) # size
        , '{:.2f}'.format(k[1]) # coverage
        , '{:.3f}'.format(k[2]) # fdr
        , '{:12}'.format(k[3]) # seed
        , '{:4}'.format(v[0]) # depth
        , '{:4}'.format(v[1]) # max depth
        , '{:5}'.format(v[2]) # filled
        , '{:5}'.format(v[3]) # covered
        , '{:5}'.format(v[4]) # cost
        , '{:.2f}'.format((0.0 + v[3])/v[2])
        , '{:.2f}'.format((0.0 + cost0)/v[-1])
        ])
for k in sorted(counts.keys()):
    v = [x / counts[k] for x in sums[k]]
    k = list(k)
    k1 = k[:]; k1[2] = 0; k1 = tuple(k1); cost0 = sums[k1][-1] / counts[k1]
    print '\t'.join([
        '{:3}'.format(k[0]) # size
        , '{:.2f}'.format(k[1]) # coverage
        , '{:.3f}'.format(k[2]) # fdr
        , 'averaged'
        , '{:4}'.format(v[0]) # depth
        , '{:4}'.format(v[1]) # max depth
        , '{:5}'.format(v[2]) # filled
        , '{:5}'.format(v[3]) # covered
        , '{:5}'.format(v[4]) # cost
        , '{:.2f}'.format((0.0 + v[3])/v[2])
        , '{:.2f}'.format((0.0 + cost0)/v[-1])
        ])
