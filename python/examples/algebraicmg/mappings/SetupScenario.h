#ifndef _EXAMPLES_ALGEBRAICMG_MAPPINGS_SETUPSCENARIO_H_
#define _EXAMPLES_ALGEBRAICMG_MAPPINGS_SETUPSCENARIO_H_


#include "peano4/utils/Globals.h" 
#include "peano4/datamanagement/VertexEnumerator.h" 
#include "peano4/datamanagement/FaceEnumerator.h" 
#include "peano4/datamanagement/CellWrapper.h" 
#include "peano4/grid/GridControlEvent.h" 
#include "tarch/la/Vector.h" 


#include <vector>


// @todo Please implement


#include "vertexdata/MG.h"
namespace examples {
namespace algebraicmg {
namespace mappings {
  class SetupScenario;
}
}
}
class examples::algebraicmg::mappings::SetupScenario{
  private:
    static tarch::logging::Log  _log;

    enum class Scenario {
      Mitchell,
      Ruede,
	  Sin
    };

    static constexpr Scenario _scenario = Scenario::Sin;

  public:

    /**
     * Init Dirichlet problem following either
     *
     * https://math.nist.gov/~WMitchell/papers/test_problems_paper.pdf
     *
     * By default, I use the setup from Table 5.
     *
     * Alternatively, I implemented Uli Ruede's "Mathematical and
     * Computational Techniques for Multilevel Adaptive Methods".
     * You find the description in 3.10.2. The domain here's not the
     * unit cube, but the domain (-1,1)^d.
     */
    static examples::algebraicmg::vertexdata::MG::VertexType getVertexType(const tarch::la::Vector<Dimensions,double>& x);
    static double                                            getSolution(const tarch::la::Vector<Dimensions,double>& x);
    static double                                            getEpsilon(const tarch::la::Vector<Dimensions,double>& x);
    static void                                              init(const tarch::la::Vector<Dimensions,double>& x, examples::algebraicmg::vertexdata::MG& fineGridVertexMG);

    /**
     * Create mapping instance for one tree for one grid sweep
     *
     * <h2> Thread safety </h2>
     *
     * The creation of individual trees usually happens through peano4::parallel::SpacetreeSet::createObserverCloneIfRequired().
     * This routine is called lazily when we start to traverse a subtree. 
     * Therefore, the creation of mappings is not thread-safe.
     *
     *
     * @param treeNumber Number of the spacetree for which we create the tree instance. Is 
     *                   smaller 0 if this is the prototype mapping used on a rank from which 
     *                   the real mappings are constructed from.
     */      
    SetupScenario(int treeNumber);

    ~SetupScenario();

    std::vector< peano4::grid::GridControlEvent > getGridControlEvents();

    void beginTraversal(
      );

    void endTraversal(
      );

    void createPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG);

    void destroyPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG);

    void createHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG);

    void destroyHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG);

    void touchVertexFirstTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG);

    void touchVertexLastTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG);

    void touchCellFirstTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell);

    void touchCellLastTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell);

};


#endif

