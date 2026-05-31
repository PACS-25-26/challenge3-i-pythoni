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

write_svg_plot() {
    kind=$1
    output_file=$2
    title=$3
    y_label=$4

    awk -F, -v kind="$kind" -v output_file="$output_file" -v title="$title" -v y_label="$y_label" '
        function series_label(mode, processes) {
            return mode == "serial" ? "serial" : "mpi_np" processes
        }

        function point_x(idx) {
            if (n_count == 1) {
                return left + plot_width / 2.0
            }
            return left + (idx - 1) * plot_width / (n_count - 1)
        }

        function point_y(value) {
            return top + plot_height * (1.0 - (value - y_min) / (y_max - y_min))
        }

        function polyline_points(label_name,     i, n_value, value, result, first) {
            result = ""
            first = 1
            for (i = 1; i <= n_count; ++i) {
                n_value = ns[i]
                value = plot_value[label_name, n_value]
                if (value == "") {
                    continue
                }
                if (!first) {
                    result = result " "
                }
                result = result sprintf("%.2f,%.2f", point_x(i), point_y(value))
                first = 0
            }
            return result
        }

        BEGIN {
            width = 760
            height = 460
            left = 80
            right = 30
            top = 50
            bottom = 70
            plot_width = width - left - right
            plot_height = height - top - bottom

            colors[1] = "#1f77b4"
            colors[2] = "#d62728"
            colors[3] = "#2ca02c"
            colors[4] = "#ff7f0e"
            colors[5] = "#9467bd"
        }

        NR == 1 {
            next
        }

        {
            label_name = series_label($1, $3)
            n_value = $2 + 0
            time_value = $9 + 0.0
            l2_value = $8 + 0.0

            if (!(label_name in seen_label)) {
                seen_label[label_name] = 1
                labels[++label_count] = label_name
            }

            if (!(n_value in seen_n)) {
                seen_n[n_value] = 1
                ns[++n_count] = n_value
            }

            if ($1 == "serial") {
                serial_time[n_value] = time_value
            }

            time_series[label_name, n_value] = time_value
            l2_series[label_name, n_value] = l2_value
        }

        END {
            y_min = 0.0
            y_max = 0.0

            for (label_index = 1; label_index <= label_count; ++label_index) {
                label_name = labels[label_index]
                for (i = 1; i <= n_count; ++i) {
                    n_value = ns[i]
                    if (kind == "time") {
                        value = time_series[label_name, n_value]
                    } else if (kind == "speedup") {
                        value = serial_time[n_value] / time_series[label_name, n_value]
                    } else {
                        value = l2_series[label_name, n_value]
                    }

                    plot_value[label_name, n_value] = value
                    if (value > y_max) {
                        y_max = value
                    }
                }
            }

            if (y_max <= 0.0) {
                y_max = 1.0
            } else {
                y_max *= 1.08
            }

            print "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" width "\" height=\"" height "\" viewBox=\"0 0 " width " " height "\">" > output_file
            print "<rect x=\"0\" y=\"0\" width=\"" width "\" height=\"" height "\" fill=\"white\"/>" >> output_file
            printf "<text x=\"%.1f\" y=\"26\" text-anchor=\"middle\" font-size=\"18\" font-family=\"sans-serif\">%s</text>\n", width / 2.0, title >> output_file
            printf "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"black\" stroke-width=\"1\"/>\n", left, top + plot_height, width - right, top + plot_height >> output_file
            printf "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"black\" stroke-width=\"1\"/>\n", left, top, left, top + plot_height >> output_file

            for (tick = 0; tick <= 4; ++tick) {
                tick_value = y_min + (y_max - y_min) * tick / 4.0
                tick_y = point_y(tick_value)
                printf "<line x1=\"%d\" y1=\"%.2f\" x2=\"%d\" y2=\"%.2f\" stroke=\"#d9d9d9\" stroke-width=\"1\"/>\n", left - 5, tick_y, width - right, tick_y >> output_file
                printf "<text x=\"%d\" y=\"%.2f\" text-anchor=\"end\" font-size=\"12\" font-family=\"sans-serif\">%.3g</text>\n", left - 10, tick_y + 4.0, tick_value >> output_file
            }

            for (i = 1; i <= n_count; ++i) {
                tick_x = point_x(i)
                printf "<line x1=\"%.2f\" y1=\"%d\" x2=\"%.2f\" y2=\"%d\" stroke=\"black\" stroke-width=\"1\"/>\n", tick_x, top + plot_height, tick_x, top + plot_height + 5 >> output_file
                printf "<text x=\"%.2f\" y=\"%d\" text-anchor=\"middle\" font-size=\"12\" font-family=\"sans-serif\">%s</text>\n", tick_x, height - 34, ns[i] >> output_file
            }

            printf "<text x=\"%.1f\" y=\"%d\" text-anchor=\"middle\" font-size=\"14\" font-family=\"sans-serif\">n</text>\n", width / 2.0, height - 10 >> output_file
            printf "<text x=\"18\" y=\"%.1f\" text-anchor=\"middle\" font-size=\"14\" font-family=\"sans-serif\" transform=\"rotate(-90 18 %.1f)\">%s</text>\n", height / 2.0, height / 2.0, y_label >> output_file

            for (label_index = 1; label_index <= label_count; ++label_index) {
                label_name = labels[label_index]
                color = colors[(label_index - 1) % 5 + 1]
                printf "<polyline fill=\"none\" stroke=\"%s\" stroke-width=\"2\" points=\"%s\"/>\n", color, polyline_points(label_name) >> output_file

                for (i = 1; i <= n_count; ++i) {
                    n_value = ns[i]
                    value = plot_value[label_name, n_value]
                    printf "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"3.5\" fill=\"%s\"/>\n", point_x(i), point_y(value), color >> output_file
                }

                legend_x = width - right - 150
                legend_y = top + 20 + 22 * (label_index - 1)
                printf "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"%s\" stroke-width=\"2\"/>\n", legend_x, legend_y, legend_x + 20, legend_y, color >> output_file
                printf "<text x=\"%d\" y=\"%d\" font-size=\"12\" font-family=\"sans-serif\">%s</text>\n", legend_x + 28, legend_y + 4, label_name >> output_file
            }

            print "</svg>" >> output_file
        }
    ' "$OUTPUT_CSV"

    echo "Wrote $output_file"
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

write_svg_plot "time" "$DATA_DIR/time_vs_n.svg" "Runtime vs n" "time [s]"
write_svg_plot "speedup" "$DATA_DIR/speedup_vs_n.svg" "Speedup vs n" "speedup"
write_svg_plot "l2" "$DATA_DIR/l2_error_vs_n.svg" "L2 error vs n" "L2 error"

echo "Saved benchmark results to $OUTPUT_CSV"
echo "Saved SVG plots to $DATA_DIR"
