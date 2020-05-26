# Some Supercomputer example scripts #



## Single node tests ##

### Prepare setup ###

See guidebook instructions how to configure the single node environment.
Please remember to add the --with-exahype argument to configure. 


Before we start, please edit Euler.cpp and set the refinement criterion as follows:


  const double MaxHOfVolume  = 1.0/3.0/3.0/13.0 * 0.9;


After these two edits, I run the following steps to build the executable:

export PYTHONPATH=../../..
python3 example-scripts/finitevolumes-with-ExaHyPE2-shared-memory-benchmark.py

or, on csh, I use

setenv PYTHONPATH ../../..
python3 example-scripts/finitevolumes-with-ExaHyPE2-shared-memory-benchmark.py

instead.


### SuperMUC-NG ###

Submit jobs with

sbatch --account=myproject example-scripts/SuperMUC-NG-single-node.slurm-script

So I use for example 

sbatch --account=pr48ma example-scripts/SuperMUC-NG-single-node.slurm-script

### Hamilton ###

sbatch example-scripts/Hamilton-single-node.slurm-script

