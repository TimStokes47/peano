"""

Compare the decompositions of the hardcoded load balancing

Runs the executables that single-black-hole.py generated with different
--decomposition and --split-mode settings. Each directory below the study
directory holds one build. For each build, each thread count and each
repetition, the script runs the code and reports

- the number of local and virtual (remote) cells over all trees,
- the largest number of local cells of any tree, i.e. the imbalance, and
- the total time and the average time of the TimeStep sweeps.

    python3 studies-load-balancing.py studies-quantised --threads 1 8 --repetitions 3

"""

import argparse
import os
import re
import subprocess


def run(directory, threads):
    environment = dict(os.environ, OMP_NUM_THREADS=str(threads))
    log = os.path.join(directory, f"run-{threads}.log")
    with open(log, "w") as output:
        subprocess.run(["./single-black-hole"], cwd=directory, env=environment, stdout=output, stderr=subprocess.STDOUT)
    text = open(log).read()
    statistics = re.findall(r"dumpStatistics\(\)\s+info\s+(\d+) tree\(s\): (.*) total=(\d+)/(\d+)", text)
    trees, per_tree, local, virtual = statistics[-1]
    local_per_tree = [int(x) for x in re.findall(r"#\d+:(\d+)/", per_tree)]
    step = re.search(r"TimeStep: ([0-9.e+-]+)s\s+\(avg=([0-9.e+-]+),#measurements=(\d+)", text)
    return dict(
        trees=int(trees), local=int(local), virtual=int(virtual), max_local=max(local_per_tree),
        total=float(step.group(1)) * int(step.group(3)), average=float(step.group(2)),
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("study", help="Directory with one subdirectory per build")
    parser.add_argument("--threads", type=int, nargs="+", default=[8])
    parser.add_argument("--repetitions", type=int, default=3)
    arguments = parser.parse_args()

    print("build,threads,repetition,trees,local,virtual,virtual/local,max_local,step_total_s,step_average_s")
    for name in sorted(os.listdir(arguments.study)):
        directory = os.path.join(arguments.study, name)
        if not os.path.exists(os.path.join(directory, "single-black-hole")):
            continue
        for threads in arguments.threads:
            for repetition in range(arguments.repetitions):
                r = run(directory, threads)
                print(f"{name},{threads},{repetition},{r['trees']},{r['local']},{r['virtual']},"
                      f"{r['virtual'] / r['local']:.2f},{r['max_local']},{r['total']:.3f},{r['average']:.4f}", flush=True)
