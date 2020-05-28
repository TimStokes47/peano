// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_TESTS_SPACETREE_TEST_H_
#define _PEANO4_GRID_TESTS_SPACETREE_TEST_H_

#include "tarch/tests/TestCase.h"
#include "tarch/logging/Log.h"
#include "tarch/tests/TestCase.h"


namespace peano4 {
  namespace grid {
    namespace tests {
      class SpacetreeTest;
    }
  }
}


class peano4::grid::tests::SpacetreeTest: public tarch::tests::TestCase {
  private:
    /**
     * Logging device
     */
    static tarch::logging::Log _log;

    void testRestrictToCoarseGrid();
    void testCreateLeaveCellTraversalEvent();
    void testCreateNeighbourExchangeLists();
    void testAreFacesLocal();

    /**
     * Test face state identification
     *
     * @image html Spacetree_testCreateEnterCellTraversalEvent.png
     */
    void testGetFaceType();

    /**
     * Test to find out where faces go to and come from
     *
     *
     * @image html Spacetree_testCreateEnterCellTraversalEvent.png
     *
     *
     * This examples stems from the following (buggy) output:
     *
     * <pre>
18465645121  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               in:vertex.toString():(state=Unrefined,adjacentRanks=[3,3,3,3],backupOfAdjacentRanks=[3,3,3,3],hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=0,x=[0.62963,0.407407],level=3),splittingIsConsideredLocal:1,joiningIsConsideredLocal:1 (file:peano4/grid/Spacetree.cpp,line:95)
18465660468  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               out:result:0 (file:peano4/grid/Spacetree.cpp,line:108)
18465677543  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               in:vertex.toString():(state=Unrefined,adjacentRanks=[3,2,3,2],backupOfAdjacentRanks=[3,2,3,2],hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=0,x=[0.666667,0.407407],level=3),splittingIsConsideredLocal:1,joiningIsConsideredLocal:1 (file:peano4/grid/Spacetree.cpp,line:95)
18465692370  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               out:result:1 (file:peano4/grid/Spacetree.cpp,line:108)
18465709288  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               in:vertex.toString():(state=New,adjacentRanks=[3,3,3,3],backupOfAdjacentRanks=[0,0,0,0],hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=1684632135,x=[0.62963,0.444444],level=3),splittingIsConsideredLocal:1,joiningIsConsideredLocal:1 (file:peano4/grid/Spacetree.cpp,line:95)
18465723996  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               out:result:0 (file:peano4/grid/Spacetree.cpp,line:108)
18465740694  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               in:vertex.toString():(state=New,adjacentRanks=[3,2,3,2],backupOfAdjacentRanks=[0,0,0,0],hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=1684632135,x=[0.666667,0.444444],level=3),splittingIsConsideredLocal:1,joiningIsConsideredLocal:1 (file:peano4/grid/Spacetree.cpp,line:95)
18466102574  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               out:result:1 (file:peano4/grid/Spacetree.cpp,line:108)
18466130181  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::createGenericCellTraversalEvent(...)             out:event.toString():(x=[0.648148,0.425926],h=[0.037037,0.037037],isRefined=0000,isVertexLocal=1010,isFaceLocal=0100,isCellLocal=0,vertexDataFrom=[0,0,0,0],vertexDataTo=[0,0,0,0],faceDataFrom=[0,0,0,0],faceDataTo=[0,0,0,0],cellData=2,relativePositionToFather=[2,2]) (file:peano4/grid/Spacetree.cpp,line:1735)
18466167134  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::createEnterCellTraversalEvent(...)           out:state.toString():(level=3,x=[0.62963,0.407407],h=[0.037037,0.037037],inverted=1,evenFlags=11,accessNumber=[-1,-2,2,1]),event.toString():(x=[0.648148,0.425926],h=[0.037037,0.037037],isRefined=0000,isVertexLocal=1010,isFaceLocal=0100,isCellLocal=0,vertexDataFrom=[5,6,5,-2],vertexDataTo=[0,1,2,3],faceDataFrom=[4,3,-2,2],faceDataTo=[1,0,3,2],cellData=2,relativePositionToFather=[2,2]),_id:2 (file:peano4/grid/Spacetree.cpp,line:1826)
18466200951  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::receiveAndMergeUserDataAtHorizontalBoundary(...)           in:state.toString():(level=3,x=[0.62963,0.407407],h=[0.037037,0.037037],inverted=1,evenFlags=11,accessNumber=[-1,-2,2,1]),enterCellTraversalEvent.toString():(x=[0.648148,0.425926],h=[0.037037,0.037037],isRefined=0000,isVertexLocal=1010,isFaceLocal=0100,isCellLocal=0,vertexDataFrom=[5,6,5,-2],vertexDataTo=[0,1,2,3],faceDataFrom=[4,3,-2,2],faceDataTo=[1,0,3,2],cellData=2,relativePositionToFather=[2,2]),_id:2 (file:peano4/grid/Spacetree.cpp,line:1832)
18466219530  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::getAdjacentDomainIds(...)                  in:faceNumber:2,calledByReceivingProcess:1,_id:2 (file:peano4/grid/Spacetree.cpp,line:1194)
18466237899  00:00:18     rank:0       core:0       trace        peano4::grid::Spacetree::getAdjacentDomainIds(...)                  out:neighbour:0 (file:peano4/grid/Spacetree.cpp,line:1261)
18466259586  00:00:18     rank:0       core:0       info         peano4::grid::Spacetree::receiveAndMergeUserDataAtHorizontalBoundary(...)             receive and merge 3th face on stack 2 of tree 2 (relative position=0) with neighbour 0. Local position in cell=2, state=(level=3,x=[0.62963,0.407407],h=[0.037037,0.037037],inverted=1,evenFlags=11,accessNumber=[-1,-2,2,1]), inFaceStack=2
18466347886  00:00:18     rank:0       core:0       trace        examples::exahype2::euler::observers::CreateGrid::receiveAndMergeFaceHorizontally(...)             in:event.toString():(x=[0.648148,0.425926],h=[0.037037,0.037037],isRefined=0000,isVertexLocal=1010,isFaceLocal=0100,isCellLocal=0,vertexDataFrom=[5,6,5,-2],vertexDataTo=[0,1,2,3],faceDataFrom=[4,3,-2,2],faceDataTo=[1,0,3,2],cellData=2,relativePositionToFather=[2,2]),positionWithinCell:2,inOutStack:2,relativePositionOnInOutStack:0,fromTree:0,marker.toString():(x=[0.648148,0.425926],h=[0.037037,0.037037],select=2,is-cell-local=0),_spacetreeId:2 (file:observers/CreateGrid.cpp,line:1302)
assertion in file observers/CreateGrid.cpp, line 1313 failed: tarch::la::equals( data.getDebugX(),incomingData.getDebugX() )
parameter data.getDebugX(): [0.666667,0.425926]
parameter incomingData.getDebugX(): [0.833333,0.333333]
parameter data.getDebugX(): [0.666667,0.425926]
parameter incomingData.getDebugX(): [0.833333,0.333333]
parameter fromTree: 0
parameter fromStack: 34
parameter inOutStack: 2
parameter relativePositionOnInOutStack: 0
parameter marker.toString(): (x=[0.648148,0.425926],h=[0.037037,0.037037],select=2,is-cell-local=0)
parameter _spacetreeId: 2
peano4: observers/CreateGrid.cpp:1313: virtual void examples::exahype2::euler::observers::CreateGrid::receiveAndMergeFaceHorizontally(const peano4::grid::GridTraversalEvent&, int, int, int, int): Assertion `false' failed.
Aborted (core dumped)
  </pre>
     *
     * We run through this cell from left bottom to top right and this cell is
     * created while we span this whole third level from scratch. That is, all
     * grid elements are new. Indeed, only the bottom two vertices are unrefined,
     * as they have been created by another cell before (the one below), while
     * the top two vertices are new. The left top one has been created before
     * (through the left neighbour), so the right top vertex is the one which is
     * actually to be created by this event.
     *
     * The vertex indices in the output above are correct. Only one vertex (number
     * three) is to be created. The flags for the faces are however wrong: The
     * left and bottom one indeed are to be taken from temporary stacks, as we've
     * visited their other adjacent neighbour/cell before. But the top one is to
     * be created (correct) as well as the right one (wrong in dump above).
     *
     */
    void testCreateEnterCellTraversalEvent1();

    /**
     * This is another use case from a debugging session. We study the following event/cell:

<pre>
 21759514404  00:00:21     rank:0       core:0       trace        examples::exahype2::euler::observers::CreateGrid::receiveAndMergeFaceHorizontally(...)             in:event.toString():(x=[0.796296,0.462963],h=[0.037037,0.037037],isRefined=0000,isVertexLocal=1111,isFaceLocal=1111,isCellLocal=1,vertexDataFrom=[3,-2,3,2],vertexDataTo=[1,0,3,2],faceDataFrom=[2,5,2,2],faceDataTo=[1,2,3,0],cellData=2,relativePositionToFather=[0,0]),positionWithinCell:1,inOutStack:2,relativePositionOnInOutStack:0,fromTree:0,marker.toString():(x=[0.796296,0.462963],h=[0.037037,0.037037],select=1,is-cell-local=1),_spacetreeId:1 (file:observers/CreateGrid.cpp,line:1302)
</pre>

       face number 1 on tree 1. The code yields a neighbour 0:

<pre>
 21759285470  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::getAdjacentDomainIds(...)                  in:faceNumber:1,calledByReceivingProcess:1,_id:1 (file:peano4/grid/Spacetree.cpp,line:1186)
 21759300951  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::getAdjacentDomainIds(...)                  out:neighbour:0 (file:peano4/grid/Spacetree.cpp,line:1253)
</pre>

       Only one adjacent vertex is tested:

<pre>
 21759257940  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)             in:vertex.toString():(state=Unrefined,adjacentRanks=[1,1,1,1],backupOfAdjacentRanks=[1,1,1,1],hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=0,x=[0.777778,0.481481],level=3),splittingIsConsideredLocal:1,joiningIsConsideredLocal:0 (file:peano4/grid/Spacetree.cpp,line:95)
</pre>

       but this is the total vertex set:

<pre>
 21758389044  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               in:vertex.toString():(state=New,adjacentRanks=[1,1,1,1],backupOfAdjacentRanks=[0,0,0,0],hasBeenAntecessorOfRefinedVertexInPrevio
usTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=1684632135,x=[0.777778,0.444444],level=3),splittingIsConsideredLocal:1,joiningIsConsideredLocal:1 (file:peano4/grid/Spacetree.cpp,line:95)
 21758403257  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               out:result:1 (file:peano4/grid/Spacetree.cpp,line:108)
 21758419707  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               in:vertex.toString():(state=Unrefined,adjacentRanks=[1,1,1,1],backupOfAdjacentRanks=[1,1,1,1],hasBeenAntecessorOfRefinedVertexIn
PreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=0,x=[0.814815,0.444444],level=3),splittingIsConsideredLocal:1,joiningIsConsideredLocal:1 (file:peano4/grid/Spacetree.cpp,line:95)
 21758432638  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               out:result:1 (file:peano4/grid/Spacetree.cpp,line:108)
 21758448543  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               in:vertex.toString():(state=Unrefined,adjacentRanks=[1,1,1,1],backupOfAdjacentRanks=[1,1,1,1],hasBeenAntecessorOfRefinedVertexIn
PreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=0,x=[0.777778,0.481481],level=3),splittingIsConsideredLocal:1,joiningIsConsideredLocal:1 (file:peano4/grid/Spacetree.cpp,line:95)
 21758461048  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               out:result:1 (file:peano4/grid/Spacetree.cpp,line:108)
 21758476937  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               in:vertex.toString():(state=Unrefined,adjacentRanks=[1,1,1,1],backupOfAdjacentRanks=[1,1,1,1],hasBeenAntecessorOfRefinedVertexIn
PreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=0,x=[0.814815,0.481481],level=3),splittingIsConsideredLocal:1,joiningIsConsideredLocal:1 (file:peano4/grid/Spacetree.cpp,line:95)
 21758489494  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(...)               out:result:1 (file:peano4/grid/Spacetree.cpp,line:108)
 21758516166  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::createGenericCellTraversalEvent(...)             out:event.toString():(x=[0.796296,0.462963],h=[0.037037,0.037037],isRefined=0000,isVertexLocal=1111,isFaceLocal=1111,isCellLocal=1,
</pre>

       with coarse grid vertices:

<pre>
 21758314591  00:00:21     rank:0       core:0       trace        peano4::grid::Spacetree::createEnterCellTraversalEvent(...)           in:
   state.toString():(level=3,x=[0.777778,0.444444],h=[0.037037,0.037037],inverted=1,evenFlags=01,accessNumber=[2,3,-1,1]),
   _id:1,
   relativePositionToFather:[0,0],
   coarseGridVertices[0].toString():(state=Refining,adjacentRanks=[1,1,1,1],backupOfAdjacentRanks=[1,1,1,1],hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=1,x=[0.777778,0.444444],level=2),
   coarseGridVertices[1].toString():(state=Refined, adjacentRanks=[1,1,1,0],backupOfAdjacentRanks=[1,1,1,0],hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=1,x=[0.888889,0.444444],level=2),
   coarseGridVertices[2].toString():(state=Refined, adjacentRanks=[1,1,0,0],backupOfAdjacentRanks=[1,1,0,0],hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=1,x=[0.777778,0.555556],level=2),
   coarseGridVertices[3].toString():(state=Refined, adjacentRanks=[1,0,0,0],backupOfAdjacentRanks=[1,0,0,0],hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep=0,isAntecessorOfRefinedVertexInCurrentTreeSweep=0,numberOfAdjacentRefinedLocalCells=1,x=[0.888889,0.555556],level=2) (file:peano4/grid/Spacetree.cpp,line:1744)
</pre>

       Again, there are two issues to investigate:

       #1 How can it be that a coarse vertex is set to refining while its
          neighbours are refined? This is a regular grid setup.
       #2 How can it be that we get the 0 here?

       I found out that it is the user code which (rightly so) asks the bottom
       left vertex to refine. This is not the only vertex where this happens.
       There's plenty of them which are already refined at this point. The
       pattern is obvious: All the vertices that have completely been given
       away are not yet refined and fall back. The other guys have already set
       their flag and thus do refine.

       I wonder why this is and the only obvious explanation is that the user
       code does not mark cells that are splitting as refine.


     */
    void testCreateEnterCellTraversalEvent2();

  public:
    SpacetreeTest();
    void run() override;
};


#endif
