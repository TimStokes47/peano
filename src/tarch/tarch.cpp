#include "tarch.h"
#include "tarch/mpi/Rank.h"

#include <iostream>


void tarch::writeCopyrightMessage() {
  static bool wrote = false;
  if (not wrote and tarch::mpi::Rank::getInstance().getRank() == tarch::mpi::Rank::getGlobalMasterRank() ) {
    std::cout << "Peano 4 (C) www.peano-framework.org " << std::endl
		      << "Written by Tobias Weinzierl et al" << std::endl;
  }
  wrote = true;
}
