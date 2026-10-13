from itertools import combinations
import os
from pathlib import Path

# Configurable Options
application = "euler"
solver = "fd4"
machine = "sapphire_rapid"
runtime_hours = 24
patch_sizes = [3, 6, 16]
number_of_samples = 10

numbers_of_patches = {
    3: ["1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096", "8192"],
    6: ["1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096", "8192"],
    16: ["1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096", "8192"]
}

partition = "dine2"
account = "do015"
set_environment = f"source {Path.home()}/set_env_x86_cpu_only.sh"
cores = 64
memory = "2TB"

optional_passes = {
    "memref-flattening": "--enable-memref-flattening",
    "function-inlining": "--enable-function-inlining",
    "memory-reuse": "--enable-memory-reuse",
    "memory-fusion": "--enable-memory-fusion"
}

output_name = f"{application}_kernel_benchmark"
job_name = f"{application}_kernel_benchmark"
output_root = f"{Path.home()}/Peano/benchmarks/exahype2/kernel-benchmarks/"
script_name = f"submit_kernel_benchmark-cpu-serial-{application}-{solver}.sh"

if solver == "fd4":
    benchmark_script_name = "kernel-benchmarks-fd4-dsl.py"
    benchmark_app_name = "KernelBenchmarksFD4DSL.Release"
if solver == "rusanov":
    benchmark_script_name = "kernel-benchmarks-fv-rusanov-dsl.py"
    benchmark_app_name = "KernelBenchmarksFVRusanovDSL.Release"

def generate_submission_script_fragment(use_cpp: bool, patch_size: int, number_of_patches: list[str], passes: list[str], output_directory: str):
    if use_cpp:
        print_statement = f"""echo "Patch Size: {patch_size}, C++" """
    else:
        print_statement = f"""echo "Patch Size: {patch_size}, MLIR, {", ".join(passes)}" """
    pass_flags = [optional_passes[enabled_pass] for enabled_pass in passes]
    return f"""{print_statement}
python {benchmark_script_name} -d 3 -cpu {"--enable-cpp" if use_cpp else "--enable-mlir"} -serial -ps {patch_size} -p {" ".join(number_of_patches)} -samples {number_of_samples} {" ".join(pass_flags)}
./{benchmark_app_name} > {output_directory}/patch_size_{patch_size}.txt
"""
compile_scripts = []
run_scripts = []
submission_scripts = []

for patch_size in patch_sizes:
    number_of_patches = numbers_of_patches[patch_size]

    pass_list = []
    for i in range(0, len(optional_passes) + 1):
        for combination in list(combinations(optional_passes, i)):
            pass_list.append(combination)

    # CPP
    output_directory = output_root + f"results/{application}/{solver}/cpu_serial/{machine}/cpp"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    submission_scripts.append(generate_submission_script_fragment(True, patch_size, number_of_patches, [], output_directory))
    
    # MLIR No Passes
    output_directory = output_root + f"results/{application}/{solver}/cpu_serial/{machine}/no-passes"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    submission_scripts.append(generate_submission_script_fragment(False, patch_size, number_of_patches, [], output_directory))

    # MLIR Passes
    for optional_pass in optional_passes.keys():
        output_directory = output_root + f"results/{application}/{solver}/cpu_serial/{machine}/{optional_pass}"
        if not os.path.exists(output_directory):
            os.makedirs(output_directory)
        submission_scripts.append(generate_submission_script_fragment(False, patch_size, number_of_patches, [optional_pass], output_directory))

with open(script_name, "w") as file:
    file.write(f"""#!/bin/bash -l

#SBATCH -o {output_name}.out
#SBATCH -e {output_name}.err
#SBATCH -t {runtime_hours}:00:00
#SBATCH --nodes=1
#SBATCH -J {job_name}
#SBATCH -c {cores}
#SBATCH -p {partition}
#SBATCH -A {account}
#SBATCH --mem={memory}

export PATH=~/Peano/src/exahype2/mlir/build/bin/:$PATH
export PYTHONPATH=~/Peano/python/:~/Peano/applications/exahype2/ccz4/:~/Peano/src/
{set_environment}
""")

    for script in submission_scripts:
        file.write(script + "\n")
    