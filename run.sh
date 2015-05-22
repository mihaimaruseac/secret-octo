#!/bin/bash

sizes="5 6 7 8 9"
depths="1.0 0.5 0.25 0.1 0.05 0.025 0.01"
coverages="0.02 0.04 0.06 0.08 0.1"
seeds="24125162166 12512616216 254125521"

espresso="../orig/espresso/espresso -sx -"

for seed in ${seeds}; do
    for size in ${sizes}; do
        for coverage in ${coverages}; do
            ./zones -s ${size} -c ${coverage} -R ${seed}    | ${espresso}
            for depth in ${depths}; do
                ./zones -s ${size} -c ${coverage} -d ${depth} -R ${seed} -f | ${espresso}
            done
            echo ""
            echo ""
        done
    done
done
