#!/bin/bash

#sizes="5 6 7 8 9 10"
sizes="7 8 9 10"
depths="10 0.5 0.4 0.3 0.2 0.1"
coverages="0.02 0.04 0.06 0.08 0.1"
seeds="24125162166 12512616216 254125521 20958359219 2012904219"

espresso="../orig/espresso/espresso -s"

genout="/tmp/test.zone"
cover="/tmp/test.cover"
esprin="/tmp/test.es"
esprout="/tmp/test.min"

# this one runs until the end
last_stage ()
{
    stagetype=$1

    ./compute -i ${esprout} ${stagetype}
}

# start last stage with all 4 params
stage3()
{
    in=$1
    stagetype=$2

    ./espresso -i ${in} -o ${esprin} ${stagetype}
    ${espresso} ${esprin} > ${esprout}

    # 2 pbc optimizations -> 4 cases
    last_stage ""
    last_stage "-c"
    last_stage "-p"
    last_stage "-cp"
}

# this one starts the pipeline with the output to espresso
stage2 ()
{
    # gray encoding
    stage3 ${genout} ""
    # hierarchical
    #stage3 ${genout} "-h"

    for depth in ${depths}; do
        ./cover -i ${genout} -o ${cover} -d $depth

        # gray encoding
        stage3 ${cover} ""
        # hierarchical
        #stage3 ${cover} "-h"
    done
}

# this one generates the zone and starts the pipeline
start_test ()
{
    size=$1
    seed=$2
    coverage=$3
    testtype=$4

    ./gen -o ${genout} -R ${seed} -s ${size} -c ${coverage} ${testtype}
    stage2
}

for size in ${sizes}; do
    echo "Doing size ${size}"
    for seed in ${seeds}; do
        for coverage in ${coverages}; do
            # Gaussian tests
            start_test ${size} ${seed} ${coverage} ""
            # uniform tests
            #start_test ${size} ${seed} ${coverage} "-u"
        done
    done
    echo ""
    echo ""
done
