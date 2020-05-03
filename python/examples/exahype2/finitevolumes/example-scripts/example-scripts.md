# Some Supercomputer example scripts #



## SuperMUC-NG ##


### Single node tests ###

See guidebook instructions how to configure the single node environment.
Please remember to add the --with-exahype argument to configure.
Once Peano 4 is built, we run the following steps to build the executable:


cd python/examples/exahype2/finitevolumes/
export PYTHONPATH=../../..
python3 finitevolumes-with-ExaHyPE2-parallel.py


