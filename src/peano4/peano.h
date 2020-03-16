// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_H_
#define _PEANO4_H_

/**
 *
 * @mainpage Peano 4
 *
 * Welcome to the source code documentation of Peano4.
 *
 * @image html architecture-layers.png
 *
 * \section Build and run unit, integration and performance tests
 *
 * You can quickly check whether your system is properly configured if you
 * you build the code with the following options, and run one of the standard
 * examples (it is just invoked, i.e. not actually run; so you see that the
 * code actually starts up, but you do not actually execute anything):
 *
 * <pre>
   ./configure
   make clean; rm src/examples/regulargridupscaling/PeanoRegularGridUpscaling*
   make -j
   src/examples/regulargridupscaling/PeanoRegularGridUpscaling2d_debug
   src/examples/regulargridupscaling/PeanoRegularGridUpscaling3d_debug

   ./configure --with-multithreading=cpp
   make clean; rm src/examples/regulargridupscaling/PeanoRegularGridUpscaling*
   make -j
   src/examples/regulargridupscaling/PeanoRegularGridUpscaling2d_debug
   src/examples/regulargridupscaling/PeanoRegularGridUpscaling3d_debug

   ./configure --with-mpi=mpicxx
   make clean; rm src/examples/regulargridupscaling/PeanoRegularGridUpscaling*
   make -j
   mpirun -n 1 src/examples/regulargridupscaling/PeanoRegularGridUpscaling2d_debug
   mpirun -n 1 src/examples/regulargridupscaling/PeanoRegularGridUpscaling3d_debug

   ./configure --with-multithreading=cpp --with-mpi=mpicxx
   make clean; rm src/examples/regulargridupscaling/PeanoRegularGridUpscaling*
   make -j
   mpirun -n 1 src/examples/regulargridupscaling/PeanoRegularGridUpscaling2d_debug
   mpirun -n 1 src/examples/regulargridupscaling/PeanoRegularGridUpscaling3d_debug
   </pre>
 *
 * Each individual program invocation runs all unit tests, as I make my
 * examples always run the unit tests first before they try to start up the
 * real simulation. If you pipe the outcomes of the program runs into a file
 * (all in one file please) and then run the Python script on this file, you
 * get an image of all the builds that have been successful.
 */


/**
 * @dir peano
 *
 * Peano's core layer. It sits on top of the technical architecture.
 */
namespace peano4 {
  void writeCopyrightMessage();

  /**
   * Fill Lookup Tables
   *
   * Fill all the lookup tables used within the application. As lookup
   * tables are used by many operations, I suggest to call this operation
   * as soon as possible.
   *
   * There shall no error occur in this operation. Thus, it does not return
   * any code.
   */
  void fillLookupTables();

  /**
   * Init Parallel Environment
   *
   * Inits the parallel environment. If the parallel mode is not set, the
   * operation detoriates to nop. The function returns 0 if everything
   * is o.k., it returns -2 otherwise. Please call this operation before
   * you call any other operation that could result in an error. I suggest
   * to call it right after fillLookupTables().
   *
   * init might change the variables passed. If you want to parse the
   * command line arguments, use the values returned. If you use the
   * arguments directly without calling initParallelEnvironment() they
   * might contain MPI values not important to the program.
   *
   * \section Rationale
   *
   * You may not use the trace macros before this operation has invoked the init
   * operation. Otherwise, the getRank() assertion fails, as the node has not
   * been configured correctly.
   *
   * \section Usage
   *
   * Invoke with an address operator before that.
   * <pre>
  peano4::initParallelEnvironment(&argc,&argv);
     </pre>
   * This has to be done as one of the very first things, i.e. before you init
   * the logging, or run tests, or ...
   */
  int initParallelEnvironment(int* argc, char*** argv);

  /**
   * Shutdown all the parallel environment, i.e. free all MPI datatypes and
   * close down MPI.
   *
   * @see peano4::parallel::Node::shutdown()
   * @see shutdownSharedMemoryEnvironment()
   */
  void shutdownParallelEnvironment();

  /**
   * Init shared memory environment.
   *
   * If you work in a hybrid environment, i.e. if you use MPI and shared memory
   * parallelisation, please invoke initParallelEnvironment() before.
   *
   * If an error occurs, it returns -3.
   */
  int initSharedMemoryEnvironment();

  /**
   * This routine adds a barrier. This barrier is necessary. If the very last
   * activity of all ranks is for example to plot stuff, they typically use
   * global semaphores as well. To make these semaphores work, we still require
   * that all nodes call receiveDanglingMessages(). It is only after everyone
   * has done their dump, that we can shut down the whole system. This is the
   * reason the barrier has to come after the node's shutdown and then has to
   * be a Peano4 barrier which still invokes receiveDanglingMessages() on all
   * services.
   *
   * Once this routine has terminated, do not add a barrier() anymore!
   */
  void shutdownSharedMemoryEnvironment();
}


#endif
