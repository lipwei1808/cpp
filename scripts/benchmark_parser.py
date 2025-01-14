import json
import matplotlib.pyplot as plt
import sys


def parse(json_data):
    benchmarks = json_data["benchmarks"]
    names = set([b["name"].split("/")[0] for b in benchmarks])  # Example: sizes from the benchmark name
    print(names)
    x = []
    for name in names:
        d = []
        y = []
        for data in benchmarks:
            if name in data["name"]:
                d.append(data["real_time"])
                y.append(data["name"].split("/")[-1])
        x.append(d)
        plt.plot(y, d, marker="o", label=name)
        

# Plot
    plt.xlabel("Input Size")
    plt.ylabel("Time (ns)")
    plt.legend()
    plt.title("Benchmark Performance")
    plt.grid(True)
    plt.show()


def read_json(filepath):
    with open(filepath, 'r') as f:
        return json.load(f)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Require filepath")
        sys.exit(1)

    data = read_json(sys.argv[1])
    parse(data)

