# Delta demonstrator #


## Configure build ##

We assume Delta is built. Before you do anything, ensure your LDFLAGS are set
properly:

export LDFLAGS=-L/home/tobias/git/Delta/src/delta

or where ever the libDelta.so file is held. Then, rerun configure with 

./configure --with-multithreading=cpp --with-delta=/home/tobias/git/Delta/src --with-vtk=/usr/include/vtk-8.2

and recompile. As our code tends to link statically, this is it. No need to 
set any further compile flags.


## Run and visualise outcome ##

./Delta_debug
~/git/Peano/src/visualisation/convert inspect marker.peano-patch-file
~/git/Peano/src/visualisation/convert convert-file marker.peano-patch-file cell-data output vtu

