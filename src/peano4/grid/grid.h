// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_H_
#define _PEANO4_GRID_H_


namespace peano4 {
  /**
   * @namespace peano4::grid
   *
   * The grid namespace is Peano's core.
   *
   *
   * \section  Recreate all DaStGen objects
   *
   * <pre>
java -jar ~/git/DaStGen/DaStGen.jar --plugin PeanoSnippetGenerator --naming Peano4NameTranslator GridVertex.def  .
java -jar ~/git/DaStGen/DaStGen.jar --plugin PeanoSnippetGenerator --naming Peano4NameTranslator AutomatonState.def  .
java -jar ~/git/DaStGen/DaStGen.jar --plugin PeanoSnippetGenerator --naming Peano4NameTranslator GridStatistics.def  .
java -jar ~/git/DaStGen/DaStGen.jar --plugin PeanoSnippetGenerator --naming Peano4NameTranslator GridControlEvent.def  .
java -jar ~/git/DaStGen/DaStGen.jar --plugin PeanoSnippetGenerator --naming Peano4NameTranslator GridTraversalEvent.def  .
    </pre>
   *
   */
  namespace grid {
    /**
     * Forward declaration
     */
    class GridStatistics;

    /**
     * The term clear() is not 100% correct, as the number of stationary traversals
     * is not reset to a dummy but instead incremented.
     */
    void clear( GridStatistics& statistics, bool isGlobalMasterTree );
  }
}


peano4::grid::GridStatistics operator+( peano4::grid::GridStatistics lhs, peano4::grid::GridStatistics rhs );

#endif
