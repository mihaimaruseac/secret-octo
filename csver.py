import itertools
import sys

# skip num lines from f and return num+1th line
def skip_lines_read_next(f, num):
    for i in range(num):
        l = f.readline()
        while len(l) == 1: l = f.readline()
    return f.readline()

# return the last word of the line, parsed
def parse_last_item_line(line, cvtf=int):
    return cvtf(line.split()[-1])

exps = {}
counts = {}
sums = {}
mins = {}
maxs = {}

def record_experiment(exp):
    k = exp.key()
    v = exp.data()

    # Record individual experiment
    assert(not exps.has_key(k))
    exps[k] = v

    # Prepare for averaging randomness out
    k = exp.avgkey()
    counts[k] = counts.get(k, 0) + 1
    v0 = sums.get(k, (0,))
    sums[k] = tuple(itertools.imap(lambda x,y: x + y, v, itertools.cycle(v0)))
    v0 = mins.get(k, (9999999999999999999999999999999999999999,))
    mins[k] = tuple(itertools.imap(min, v, itertools.cycle(v0)))
    v0 = maxs.get(k, (0,))
    maxs[k] = tuple(itertools.imap(max, v, itertools.cycle(v0)))

class Experiment:
    def __init__(self, seed, side, coverage, uniform, filled):
        self.seed = seed
        self.side = side
        self.coverage = coverage
        self.uniform = uniform
        self.filled = filled
        self.mdr = 0
        self.maxDepth = 0
        self.covered = 0
        self.depth = 0
        self.hierarchical = "NO"
        self.caching = "NO"
        self.fast_prod = "NO"

    def record_cover(self, mdr, maxDepth, covered, depth):
        self.mdr = mdr
        self.maxDepth = maxDepth
        self.covered = covered
        self.depth = depth

    def record_espresso(self, hierarchical):
        self.hierarchical = hierarchical

    def record_compute(self, caching, fast_prod, tks, atl, st, dt):
        self.caching = caching
        self.fast_prod = fast_prod
        # dependent data below
        self.tks = tks
        self.atl = atl
        self.st = st
        self.dt = dt
        record_experiment(self)

    def key(self):
        l = list(self.avgkey())
        l.insert(0, self.seed)
        return tuple(l)

    def avgkey(self):
        return self.side, self.coverage, self.uniform, self.mdr, self.hierarchical, self.caching, self.fast_prod

    def data(self):
        return self.filled, self.maxDepth, self.depth, self.covered, self.tks, self.atl, self.st, self.dt

for fname in sys.argv[1:]:
    with open(fname, "r") as f:
        while True:
            line = f.readline()
            if len(line) == 0: break #eof
            elif len(line) == 1: continue # empty
            line = line.split()
            if line[0] in ['Doing', 'Called']: continue
            if line[0] == './gen':
                side = parse_last_item_line(f.readline(), int)
                coverage = parse_last_item_line(f.readline(), float)
                seed = parse_last_item_line(f.readline(), int)
                uniform = parse_last_item_line(skip_lines_read_next(f,1), str)
                filled = parse_last_item_line(f.readline().split()[-2], int)
                exp = Experiment(seed, side, coverage, uniform, filled)
                continue
            if line[0] == './cover':
                mdr = parse_last_item_line(f.readline(), float)
                skip_lines_read_next(f, 1)
                covered = parse_last_item_line(f.readline().split()[-2], int)
                depth = parse_last_item_line(f.readline(), int)
                maxDepth = parse_last_item_line(f.readline(), int)
                exp.record_cover(mdr, maxDepth, covered, depth)
                continue
            if line[0] == './espresso':
                hierarchical = parse_last_item_line(skip_lines_read_next(f,2), str)
                exp.record_espresso(hierarchical)
                continue
            if line[0] == './compute':
                caching = parse_last_item_line(skip_lines_read_next(f,1), str)
                fast_prod = parse_last_item_line(f.readline(), str)
                setup_time = parse_last_item_line(f.readline().split()[-2], int)
                [h, t] = f.readline().split('|')
                t = map(int, t.split())
                avg = sum(t) / (0.0 + len(t))
                ts = parse_last_item_line(h.split('=')[-1], int)
                decr_time = parse_last_item_line(f.readline().split()[-2], int)
                exp.record_compute(caching, fast_prod, ts, avg, setup_time, decr_time)
                f.readline() # "Cleanup..." line ignored
                continue
            print line

print "seed\tsize\tcov\tUniform?\tDepthRate\tHierarchical?\tCaching?\tFastProd?\tfilled\tmxDpth\tdepth\tcovered\ttokens\tavgNSEs\tsetup\tdecrypt"

def print_exp(st, k, v):
    print '\t'.join(
        [ '{:10}' .format(st) # seed/mode
        , '{:3}'  .format(k[0]) # size
        , '{:.2f}'.format(k[1]) # coverage percent
        , '{:>3}' .format(k[2]) # uniform/gaussian
        , '{:4}'  .format(k[3]) # depth rate
        , '{:>3}' .format(k[4]) # hierarchical
        , '{:>3}' .format(k[5]) # caching
        , '{:>3}' .format(k[6]) # fast product
        , '{:5}'  .format(v[0]) # filled
        , '{:4}'  .format(v[1]) # max depth
        , '{:4}'  .format(v[2]) # depth
        , '{:5}'  .format(v[3]) # covered
        , '{:4}'  .format(v[4]) # tokens
        , '{:.2f}'.format(v[5]) # average non-star elements
        , '{:.3f}'.format(v[6]/1000000.0) # setup time
        , '{:.3f}'.format(v[7]/1000000.0) # decrypt time
        ])
    # TODO: compute and print overhead & speedup & etc

for k in sorted(exps.keys()):
    print_exp(k[0], k[1:], exps[k])
for k in sorted(sums.keys()):
    print_exp("averages",k, map(lambda x: (0.0 + x)/counts[k], sums[k]))
for k in sorted(mins.keys()):
    print_exp("min vals",k, mins[k])
for k in sorted(maxs.keys()):
    print_exp("max vals",k, maxs[k])
