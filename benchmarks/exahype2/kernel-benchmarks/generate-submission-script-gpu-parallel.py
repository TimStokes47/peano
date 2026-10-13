from itertools import combinations
import os
from pathlib import Path

# Configurable Options
application = "euler"
solver = "fd4"
machine = "grace_hopper"
runtime_hours = 24
patch_sizes = [3, 6, 16]
number_of_samples = 10

numbers_of_patches = {
    3: ["1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096", "8192"],
    6: ["1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096", "8192"],
    16: ["1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096", "8192"]
}

if machine == "grace_hopper":
    partition = "gracehopper"
    account = "do016"
    set_environment = f"source {Path.home()}/set_env_arm.sh"
if machine == "a100":
    partition = "cosma8-shm"
    account = "do016"
    set_environment = f"source {Path.home()}/set_env_x86_nvidia.sh"


optional_passes = {
    "memory-reuse": "--enable-memory-reuse",
    "perfect-loop-nesting": "--enable-imperfect-nested-loops-to-perfect",
    "memory-fusion": "--enable-memory-fusion",
    "loop-fusion": "--enable-loop-fusion"
}

output_name = f"{application}_kernel_benchmark"
job_name = f"{application}_kernel_benchmark"
output_root = f"{Path.home()}/Peano/benchmarks/exahype2/kernel-benchmarks/"
script_name = f"submit_kernel_benchmark-gpu-parallel-{application}-{solver}.sh"
compile_script_name = f"compile_kernel_benchmark-gpu-parallel-{application}-{solver}.sh"
run_script_name = f"run_kernel_benchmark-gpu-parallel-{application}-{solver}.sh"

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
python {benchmark_script_name} -d 3 -gpu {"--enable-cpp" if use_cpp else "--enable-mlir"} -ps {patch_size} -p {" ".join(number_of_patches)} -samples {number_of_samples} {" ".join(pass_flags)}
./{benchmark_app_name} > {output_directory}/patch_size_{patch_size}.txt
"""

def generate_compilation_script_fragment(use_cpp: bool, patch_size: int, number_of_patches: list[int], passes: list[str], output_directory: str):
    if use_cpp:
        print_statement = f"""echo "Patch Size: {patch_size}, C++" """
        benchmark_executable_name = f"cpp_{patch_size}"
    else:
        print_statement = f"""echo "Patch Size: {patch_size}, MLIR, {", ".join(passes)}" """
        if len(passes) == 0:
            benchmark_executable_name = f"no-passes_{patch_size}"
        else:
            benchmark_executable_name = f"{'_'.join(passes)}_{patch_size}"
    pass_flags = [optional_passes[enabled_pass] for enabled_pass in passes]
    return f"""{print_statement}
python {benchmark_script_name} -d 3 -gpu {"--enable-cpp" if use_cpp else "--enable-mlir"} -ps {patch_size} -p {" ".join(number_of_patches)} -samples {number_of_samples}  {" ".join(pass_flags)} -o {benchmark_executable_name}
"""

def generate_running_script_fragment(use_cpp: bool, patch_size: int, number_of_patches: list[int], passes: list[str], output_directory: str):
    if use_cpp:
        print_statement = f"""echo "Patch Size: {patch_size}, C++" """
        benchmark_executable_name = f"cpp_{patch_size}"
    else:
        print_statement = f"""echo "Patch Size: {patch_size}, MLIR, {", ".join(passes)}" """
        if len(passes) == 0:
            benchmark_executable_name = f"no-passes_{patch_size}"
        else:
            benchmark_executable_name = f"{'_'.join(passes)}_{patch_size}"
    return f"""{print_statement}
./{benchmark_executable_name}.Release > {output_directory}/patch_size_{patch_size}.txt
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
    output_directory = output_root + f"results/{application}/{solver}/gpu_parallel/{machine}/cpp"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    submission_scripts.append(generate_submission_script_fragment(True, patch_size, number_of_patches, [], output_directory))
    compile_scripts.append(generate_compilation_script_fragment(True, patch_size, number_of_patches, [], output_directory))
    run_scripts.append(generate_running_script_fragment(True, patch_size, number_of_patches, [], output_directory))
    
    # MLIR No Passes
    output_directory = output_root + f"results/{application}/{solver}/gpu_parallel/{machine}/no-passes"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    submission_scripts.append(generate_submission_script_fragment(False, patch_size, number_of_patches, [], output_directory))
    compile_scripts.append(generate_compilation_script_fragment(False, patch_size, number_of_patches, [], output_directory))
    run_scripts.append(generate_running_script_fragment(False, patch_size, number_of_patches, [], output_directory))

    # MLIR Passes
    for optional_pass in optional_passes.keys():
        output_directory = output_root + f"results/{application}/{solver}/gpu_parallel/{machine}/{optional_pass}"
        if not os.path.exists(output_directory):
            os.makedirs(output_directory)
        submission_scripts.append(generate_submission_script_fragment(False, patch_size, number_of_patches, [optional_pass], output_directory))
        compile_scripts.append(generate_compilation_script_fragment(False, patch_size, number_of_patches, [optional_pass], output_directory))
        run_scripts.append(generate_running_script_fragment(False, patch_size, number_of_patches, [optional_pass], output_directory))

with open(compile_script_name, "w") as file:
    file.write(f"""#!/bin/bash -l

#SBATCH -o {output_name}.out
#SBATCH -e {output_name}.err
#SBATCH -t {runtime_hours}:00:00
#SBATCH --nodes=1
#SBATCH -J {job_name}
#SBATCH -p {partition}
#SBATCH -A {account}

export PATH=~/Peano/src/exahype2/mlir/build/bin/:$PATH
export PYTHONPATH=~/Peano/python/:~/Peano/applications/exahype2/ccz4/:~/Peano/src/
{set_environment}
""")
    for script in compile_scripts:
        file.write(script + "\n")

with open(run_script_name, "w") as file:
    file.write(f"""#!/bin/bash -l

#SBATCH -o {output_name}.out
#SBATCH -e {output_name}.err
#SBATCH -t {runtime_hours}:00:00
#SBATCH --nodes=1
#SBATCH -J {job_name}
#SBATCH -p {partition}
#SBATCH -A {account}

export PATH=~/Peano/src/exahype2/mlir/build/bin/:$PATH
export PYTHONPATH=~/Peano/python/:~/Peano/applications/exahype2/ccz4/:~/Peano/src/
{set_environment}
""")
    for script in run_scripts:
        file.write(script + "\n")

with open(script_name, "w") as file:
    file.write(f"""#!/bin/bash -l

#SBATCH -o {output_name}.out
#SBATCH -e {output_name}.err
#SBATCH -t {runtime_hours}:00:00
#SBATCH --nodes=1
#SBATCH -J {job_name}
#SBATCH -p {partition}
#SBATCH -A {account}

export PATH=~/Peano/src/exahype2/mlir/build/bin/:$PATH
export PYTHONPATH=~/Peano/python/:~/Peano/applications/exahype2/ccz4/:~/Peano/src/
{set_environment}
""")

    for script in submission_scripts:
        file.write(script + "\n")
    