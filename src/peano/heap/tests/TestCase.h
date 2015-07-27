// This file originally was created by pdt (Peano Development Toolkit) as part
// of a code based upon the Peano project by Tobias Weinzierl. For conditions 
// of distribution and use of this project, please see the copyright notice at
// www.peano-framework.org. Feel free to adopt the license and authorship of 
// this file and your project to your needs as long as the license is in 
// agreement with the original Peano user constraints. A reference to/citation  
// of  Peano and its author is highly appreciated.
#ifndef _PEANO_HEAP_TESTS_COMPRESSED_FLOATING_POINT_NUMBERS_TESTS_
#define _PEANO_HEAP_TESTS_COMPRESSED_FLOATING_POINT_NUMBERS_TESTS_
 


#include "tarch/tests/TestCase.h"


namespace peano {
  namespace heap {
    namespace tests {
      class TestCase;
    }
  }
}
 

/**
 * This is just a default test case that demonstrated how to write unit tests 
 * in Peano. Feel free to rename, remove, or duplicate it. 
 */ 
class peano::heap::TestCase: public tarch::tests::TestCase {
  private:
    void testDecomposeWithLongInt();
    void testDecomposeWithInt();
    void testDecomposeWithChar();
    void testBatchDecomposition();
  public: 
    TestCase(); 
    virtual ~TestCase();
     
    virtual void run();
};


#endif
