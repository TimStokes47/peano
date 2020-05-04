# Some Supercomputer example scripts #



## Single node tests ##

### Prepare setup ###

See guidebook instructions how to configure the single node environment.
Please remember to add the --with-exahype argument to configure. Once 
Peano 4 is built, we adopt the setup slightly. For this, please edit 
finitevolumes-with-ExaHyPE2-parallel.py. These are the changes I do 
propose:


time_step_size = 0.00001
build_mode = peano4.output.CompileMode.Release
dimensions = 3

  project.set_global_simulation_parameters(
    dimensions, [0.0,0.0,0.0], [1.0,1.0,1.0],
    0.1,                       # end time
    0.0, 0
  )


I also edit Euler.cpp and set the refinement criterion as follows:


  const double MaxHOfVolume  = 1.0/3.0/3.0/25.0 * 0.9;


After these two edits, I run the following steps to build the executable:


cd python/examples/exahype2/finitevolumes/
export PYTHONPATH=../../..
python3 finitevolumes-with-ExaHyPE2-parallel.py

On csh, I use

setenv PYTHONPATH ../../..

instead.


## SuperMUC-NG ##


### Single node tests ###


Submit jobs with

sbatch --account=myproject example-scripts/SuperMUC-NG-single-node.slurm-script

So I use for example 

sbatch --account=pr48ma example-scripts/SuperMUC-NG-single-node.slurm-script




