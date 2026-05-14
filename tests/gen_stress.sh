#!/bin/bash
# Generates a maximum-size stress test: linear chain with N=255, M=255.
# Usage: ./gen_stress.sh <output_file>
set -eu
out="${1:?usage: $0 <output_file>}"
N=255
{
    echo "$N"
    echo "0 1 0 0 0 0"
    for ((i = 1; i < N; i++)); do
        echo "$i $((i - 1)),$((i + 1)) 1 1 1 1"
    done
    echo "$N $((N - 1)) 1 1 1 1"
    echo "255 gems"
} > "$out"