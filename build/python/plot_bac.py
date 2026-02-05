#!/usr/bin/env python3
import sys
import csv

try:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except Exception as exc:
    print("Chyba: matplotlib není nainstalovaný:", exc)
    sys.exit(1)


def read_csv(path):
    hours = []
    bacs = []
    with open(path, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            try:
                hours.append(float(row["hour"]))
                bacs.append(float(row["bac"]))
            except Exception:
                continue
    return hours, bacs


def main():
    if len(sys.argv) < 3:
        print("Použití: plot_bac.py <data.csv> <output.png>")
        sys.exit(2)

    data_path = sys.argv[1]
    output_path = sys.argv[2]

    hours, bacs = read_csv(data_path)
    if not hours:
        print("Chyba: žádná data pro graf.")
        sys.exit(3)

    plt.figure(figsize=(5.5, 3.2))
    plt.plot(hours, bacs, color="#c4653a", linewidth=2)
    plt.fill_between(hours, bacs, color="#f1cbb6", alpha=0.6)
    plt.xlabel("Hodiny")
    plt.ylabel("Promile (‰)")
    plt.title("Promile v čase")
    plt.grid(True, alpha=0.2)
    plt.tight_layout()
    plt.savefig(output_path, dpi=150)


if __name__ == "__main__":
    main()
