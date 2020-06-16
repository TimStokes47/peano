# Some Supercomputer example scripts #



## Single node tests ##

### Prepare Peano 4 core ###

./configure --enable-exahype --with-multithreading=tbb
./configure --enable-exahype --with-multithreading=cpp
./configure --enable-exahype --with-multithreading=omp



### Prepare application ###

See guidebook instructions how to configure the single node environment.
Please remember to add the --with-exahype argument to configure. 

Before I code anything, I usually open Euler.cpp and ensure that the 
mesh size is set properly:

  const double MaxHOfVolume  = 0.02;


I run the following steps to build the executable:

export PYTHONPATH=../../..
python3 example-scripts/finitevolumes-with-ExaHyPE2-shared-memory-benchmark.py

or, on csh, I use

setenv PYTHONPATH ../../..
python3 example-scripts/finitevolumes-with-ExaHyPE2-shared-memory-benchmark.py

instead.

I got plenty of linker errors recently. It was all my fault. If you get linker errors,
ensure you do the following two things:

- Ensure you've re-created the configure environment, as something in the build
  system might have changed (libtoolize; aclocal; autoconf; autoheader; cp src/config.h.in .; automake --add-missing).
- Ensure you've built Peano with the --enable-exahype flag.

### SuperMUC-NG ###

Submit jobs with

sbatch --account=myproject example-scripts/SuperMUC-NG-single-node.slurm-script

So I use for example 

sbatch --account=pr48ma example-scripts/SuperMUC-NG-single-node.slurm-script

### Hamilton ###

sbatch example-scripts/Hamilton-single-node.slurm-script

sbatch example-scripts/Hamilton-1-node.slurm-script
sbatch example-scripts/Hamilton-2-nodes.slurm-script
sbatch example-scripts/Hamilton-3-nodes.slurm-script
sbatch example-scripts/Hamilton-4-nodes.slurm-script
sbatch example-scripts/Hamilton-5-nodes.slurm-script
sbatch example-scripts/Hamilton-6-nodes.slurm-script
sbatch example-scripts/Hamilton-7-nodes.slurm-script
sbatch example-scripts/Hamilton-8-nodes.slurm-script
sbatch example-scripts/Hamilton-10-nodes.slurm-script
sbatch example-scripts/Hamilton-12-nodes.slurm-script
sbatch example-scripts/Hamilton-14-nodes.slurm-script
sbatch example-scripts/Hamilton-16-nodes.slurm-script
sbatch example-scripts/Hamilton-18-nodes.slurm-script
sbatch example-scripts/Hamilton-20-nodes.slurm-script
sbatch example-scripts/Hamilton-22-nodes.slurm-script
sbatch example-scripts/Hamilton-24-nodes.slurm-script


## Postprocessing ##

export PYTHONPATH=../../../..
python3 ../../../../exahype2/postprocessing/plot-scaling.py results-Hamilton-tbb.tar.gz

Unpack the archive and run 

python3 ../../../../../src/toolbox/loadbalancing/plot-load-distribution.py

on the files of interest.

