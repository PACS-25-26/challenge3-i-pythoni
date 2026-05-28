#!/usr/bin/env python3

import csv
import math
from collections import defaultdict
from pathlib import Path


ROOT_DIR = Path(__file__).resolve().parent.parent
DATA_DIR = ROOT_DIR / "test" / "data"
CSV_PATH = DATA_DIR / "performance.csv"


def read_rows(path):
    rows = []
    with path.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            rows.append(
                {
                    "mode": row["mode"],
                    "n": int(row["n"]),
                    "processes": int(row["processes"]),
                    "threads": int(row["threads"]),
                    "iterations": int(row["iterations"]),
                    "converged": row["converged"],
                    "final_increment": float(row["final_increment"]),
                    "l2_error": float(row["l2_error"]),
                    "time_seconds": float(row["time_seconds"]),
                }
            )
    return rows


def label_for_row(row):
    if row["mode"] == "serial":
        return "serial"
    return f"mpi_np{row['processes']}"


def group_rows(rows, value_key):
    grouped = defaultdict(list)
    for row in rows:
        grouped[label_for_row(row)].append((row["n"], row[value_key]))

    for values in grouped.values():
        values.sort(key=lambda item: item[0])
    return dict(grouped)


def compute_speedup_rows(rows):
    serial_times = {row["n"]: row["time_seconds"] for row in rows if row["mode"] == "serial"}
    grouped = defaultdict(list)

    for row in rows:
        n = row["n"]
        if n not in serial_times:
            continue
        grouped[label_for_row(row)].append((n, serial_times[n] / row["time_seconds"]))

    for values in grouped.values():
        values.sort(key=lambda item: item[0])
    return dict(grouped)


def svg_escape(text):
    return (
        text.replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace('"', "&quot;")
    )


def write_svg_plot(series, output_path, title, y_label):
    width = 760
    height = 460
    left = 80
    right = 30
    top = 50
    bottom = 70
    plot_width = width - left - right
    plot_height = height - top - bottom
    colors = ["#1f77b4", "#d62728", "#2ca02c", "#ff7f0e", "#9467bd"]

    ns = sorted({point[0] for points in series.values() for point in points})
    y_values = [point[1] for points in series.values() for point in points]
    y_min = min(y_values)
    y_max = max(y_values)

    if math.isclose(y_min, y_max):
        y_min = 0.0
        y_max = y_max + 1.0

    if y_min > 0.0:
        y_min = 0.0

    padding = 0.08 * (y_max - y_min)
    y_max += padding

    x_positions = {}
    for index, n_value in enumerate(ns):
        if len(ns) == 1:
            x_positions[n_value] = left + plot_width / 2.0
        else:
            x_positions[n_value] = left + index * plot_width / float(len(ns) - 1)

    def y_pos(value):
        return top + plot_height * (1.0 - (value - y_min) / (y_max - y_min))

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
        f'<rect x="0" y="0" width="{width}" height="{height}" fill="white"/>',
        f'<text x="{width / 2:.1f}" y="26" text-anchor="middle" font-size="18" font-family="sans-serif">{svg_escape(title)}</text>',
        f'<line x1="{left}" y1="{top + plot_height}" x2="{width - right}" y2="{top + plot_height}" stroke="black" stroke-width="1"/>',
        f'<line x1="{left}" y1="{top}" x2="{left}" y2="{top + plot_height}" stroke="black" stroke-width="1"/>',
    ]

    for tick_index in range(5):
        value = y_min + (y_max - y_min) * tick_index / 4.0
        y = y_pos(value)
        parts.append(
            f'<line x1="{left - 5}" y1="{y:.2f}" x2="{width - right}" y2="{y:.2f}" stroke="#d9d9d9" stroke-width="1"/>'
        )
        parts.append(
            f'<text x="{left - 10}" y="{y + 4:.2f}" text-anchor="end" font-size="12" font-family="sans-serif">{value:.3g}</text>'
        )

    for n_value in ns:
        x = x_positions[n_value]
        parts.append(
            f'<line x1="{x:.2f}" y1="{top + plot_height}" x2="{x:.2f}" y2="{top + plot_height + 5}" stroke="black" stroke-width="1"/>'
        )
        parts.append(
            f'<text x="{x:.2f}" y="{height - 34}" text-anchor="middle" font-size="12" font-family="sans-serif">{n_value}</text>'
        )

    parts.append(
        f'<text x="{width / 2:.1f}" y="{height - 10}" text-anchor="middle" font-size="14" font-family="sans-serif">n</text>'
    )
    parts.append(
        f'<text x="18" y="{height / 2:.1f}" text-anchor="middle" font-size="14" font-family="sans-serif" transform="rotate(-90 18 {height / 2:.1f})">{svg_escape(y_label)}</text>'
    )

    for color_index, (label, points) in enumerate(series.items()):
        color = colors[color_index % len(colors)]
        line_points = []

        for n_value, value in points:
            line_points.append(f"{x_positions[n_value]:.2f},{y_pos(value):.2f}")

        parts.append(
            f'<polyline fill="none" stroke="{color}" stroke-width="2" points="{" ".join(line_points)}"/>'
        )

        for n_value, value in points:
            parts.append(
                f'<circle cx="{x_positions[n_value]:.2f}" cy="{y_pos(value):.2f}" r="3.5" fill="{color}"/>'
            )

        legend_x = width - right - 150
        legend_y = top + 20 + 22 * color_index
        parts.append(
            f'<line x1="{legend_x}" y1="{legend_y}" x2="{legend_x + 20}" y2="{legend_y}" stroke="{color}" stroke-width="2"/>'
        )
        parts.append(
            f'<text x="{legend_x + 28}" y="{legend_y + 4}" font-size="12" font-family="sans-serif">{svg_escape(label)}</text>'
        )

    parts.append("</svg>")
    output_path.write_text("\n".join(parts), encoding="utf-8")


def main():
    if not CSV_PATH.exists():
        raise SystemExit(f"Missing input file: {CSV_PATH}")

    rows = read_rows(CSV_PATH)
    write_svg_plot(group_rows(rows, "time_seconds"), DATA_DIR / "time_vs_n.svg", "Runtime vs n", "time [s]")
    print(f"Wrote {DATA_DIR / 'time_vs_n.svg'}")
    write_svg_plot(compute_speedup_rows(rows), DATA_DIR / "speedup_vs_n.svg", "Speedup vs n", "speedup")
    print(f"Wrote {DATA_DIR / 'speedup_vs_n.svg'}")
    write_svg_plot(group_rows(rows, "l2_error"), DATA_DIR / "l2_error_vs_n.svg", "L2 error vs n", "L2 error")
    print(f"Wrote {DATA_DIR / 'l2_error_vs_n.svg'}")


if __name__ == "__main__":
    main()
