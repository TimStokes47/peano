# Regular Grid upscaling studies # 

This example simply constructs a regular grid, and tries to balance it as equal
as possible among all available nodes. After that, each rank (node) splits up 
its domain further into subdomains such that there is one subdomain (tree) per 
core. The code then runs a fixed number of grid sweeps. This allows for core
benchmarking. A command line arguments instructs Peano what grid size has to be
matched at least. The benchmark is available in 2d and 3d.

## Step 1: Prepare executables ##

This step creates the benchmark baseline. Configure you code without any optional
features, i.e. run 

./configure
make clean
make -j
mv src/examples/regulargridupscaling/PeanoRegularGridUpscaling2d src/examples/regulargridupscaling/PeanoRegularGridUpscaling2d-serial
mv src/examples/regulargridupscaling/PeanoRegularGridUpscaling3d src/examples/regulargridupscaling/PeanoRegularGridUpscaling3d-serial

./configure --with-multithreading=cpp --with-mpi=mpicxx
make clean
make -j
mv src/examples/regulargridupscaling/PeanoRegularGridUpscaling2d src/examples/regulargridupscaling/PeanoRegularGridUpscaling2d-parallel
mv src/examples/regulargridupscaling/PeanoRegularGridUpscaling3d src/examples/regulargridupscaling/PeanoRegularGridUpscaling3d-parallel


## Step 2: Single core run ##

This step creates the benchmark baseline. Switch into the directory holding the executables

cd src/examples/regulargridupscaling

Here, we run the following snippet:


for h in 0.1 0.5 0.01
do
  ./PeanoRegularGridUpscaling2d-serial $h > serial2d-$h.out
done


It might be convenient to paste those in to a SLURM file. 

  


