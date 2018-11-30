// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_STACKS_STACK_H_
#define _PEANO4_STACKS_STACK_H_


#include "STDVectorStack.h"

#include "peano4/grid/GridVertex.h"


namespace peano4 {
 namespace stacks {
//   typedef std::vector< peano4::grid::GridVertex >   GridVertexStack;

   typedef STDVectorStack< peano4::grid::GridVertex >   GridVertexStack;
 }
}


#endif
