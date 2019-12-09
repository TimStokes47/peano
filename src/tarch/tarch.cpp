#include "tarch.h"
#include "tarch/mpi/Rank.h"

#include <iostream>

void tarch::writeCopyrightMessage() {
  static bool wrote = false;
  if (
    not wrote and tarch::mpi::Rank::getInstance().getRank() == tarch::mpi::Rank::getGlobalMasterRank() ) {
    std::cout << "Peano 4 (C) www.peano-framework.org " << std::endl
		      << "Written by Tobias Weinzierl et al" << std::endl;
    std::string buildInfo = "build: ";
    #ifdef Parallel
    buildInfo += "mpi";
    #else
    buildInfo += "no mpi";
    #endif
    #if defined(SharedOMP)
    buildInfo += ",omp";
    #elif defined(SharedTBB)
    buildInfo += ",tbb";
    #elif defined(SharedCPP)
    buildInfo += ",C++ threading";
    #else
    buildInfo += ",no threading";
    #endif
    #if PeanoDebug>0
    buildInfo += ",debug level=" + std::to_string(PeanoDebug);
    #else
    buildInfo += ",no debug";
    #endif
    std::cout << buildInfo << std::endl;
  }
  wrote = true;
}
