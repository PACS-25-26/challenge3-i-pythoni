#!/usr/bin/env bash

set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
ROOT_DIR=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
DATA_DIR="$ROOT_DIR/test/data"
OUTPUT_CSV="$DATA_DIR/performance.csv"

GRID_SIZES=${GRID_SIZES:-"16 32 64 128 256"}
MPI_PROCESSES=${MPI_PROCESSES:-"1 2 4"}
TOL=${TOL:-1e-6}
MAX_ITER=${MAX_ITER:-2000}
THREADS=${THREADS:-1}
PROBLEM_CASE=${PROBLEM_CASE:-sine}

extract_field() {
    printf '%s\n' "$1" | awk -F': ' -v field="$2" '$1 == field { print $2; exit }'
}

run_serial_case() {
    n=$1
    echo "Running serial case with n=$n"
    output=$(OMP_NUM_THREADS="$THREADS" "$ROOT_DIR/laplace_serial" \
        --n "$n" \
        --tol "$TOL" \
        --max-iter "$MAX_ITER" \
        --case "$PROBLEM_CASE")

    iterations=$(extract_field "$output" "iterations")
    converged=$(extract_field "$output" "converged")
    final_increment=$(extract_field "$output" "final_increment")
    l2_error=$(extract_field "$output" "l2_error")
    time_seconds=$(extract_field "$output" "time_seconds")

    printf 'serial,%s,1,%s,%s,%s,%s,%s,%s\n' \
        "$n" "$THREADS" "$iterations" "$converged" "$final_increment" "$l2_error" "$time_seconds" \
        >> "$OUTPUT_CSV"
}

run_mpi_case() {
    processes=$1
    n=$2

    echo "Running MPI case with np=$processes and n=$n"
    output=$(OMP_NUM_THREADS="$THREADS" mpirun -np "$processes" "$ROOT_DIR/laplace_solver" \
        --n "$n" \
        --tol "$TOL" \
        --max-iter "$MAX_ITER" \
        --case "$PROBLEM_CASE")

    iterations=$(extract_field "$output" "iterations")
    converged=$(extract_field "$output" "converged")
    final_increment=$(extract_field "$output" "final_increment")
    l2_error=$(extract_field "$output" "l2_error")
    time_seconds=$(extract_field "$output" "time_seconds")

    printf 'mpi,%s,%s,%s,%s,%s,%s,%s,%s\n' \
        "$n" "$processes" "$THREADS" "$iterations" "$converged" "$final_increment" "$l2_error" "$time_seconds" \
        >> "$OUTPUT_CSV"
}

mkdir -p "$DATA_DIR"

if [ ! -x "$ROOT_DIR/laplace_serial" ] || [ ! -x "$ROOT_DIR/laplace_solver" ]; then
    echo "Building project binaries"
    make -C "$ROOT_DIR" serial parallel
fi

printf 'mode,n,processes,threads,iterations,converged,final_increment,l2_error,time_seconds\n' \
    > "$OUTPUT_CSV"

for n in $GRID_SIZES; do
    run_serial_case "$n"
done

for processes in $MPI_PROCESSES; do
    for n in $GRID_SIZES; do
        run_mpi_case "$processes" "$n"
    done
done

echo "Saved benchmark results to $OUTPUT_CSV"
echo "To generate plots, run: python3 test/plot_results.py"
