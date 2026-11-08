// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "peano4/datamanagement/FaceMarker.h"
#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"

#include <bitset>
#include <functional>


namespace exahype2 {
  namespace blockstructured {
    /**
     * Set the ghost cells of a face along the domain boundary through a
     * Dirichlet boundary condition
     *
     * A face of a block-structured patch carries an overlap of 2*overlap
     * grid cells (Finite Volumes or Finite Difference grid cells) along its
     * normal. The left half belongs to the cell left of the face, the right
     * half to the cell right of it. On the domain boundary, one of these
     * two cells lies outside of the domain, so nobody ever writes its half
     * of the face data. This routine fills exactly this half: for each of
     * the outside's grid cells, it invokes
     *
     * @code
     * boundaryCondition( x, t, Q );
     * @endcode
     *
     * with x being the centre of the (ghost) grid cell, t the time stamp
     * handed in, and Q pointing to the grid cell's unknowns and auxiliary
     * variables within faceData. Layer 0 is the ghost grid cell adjacent to
     * the face.
     *
     * Whether the face is on the domain boundary is derived from its
     * position, i.e. compared against domainOffset and
     * domainOffset+domainSize. Faces along periodic axes and faces which are
     * not on the domain boundary are left unchanged. Hence, the routine can
     * be called for any face.
     *
     * The routine replaces the code which the action sets
     * exahype2.api.fv.actionsets.ApplyDirichletBoundaryConditions and
     * exahype2.api.rkfd4.actionsets.ApplyDirichletBoundaryConditions used to
     * generate. Both generate a call to it now. The geometry of a Finite
     * Volume and of a Finite Difference grid cell is the same, so the two
     * schemes share this routine.
     *
     * @param boundaryCondition Functor setting the unknowns and auxiliary
     *   variables Q at position x and time t. Typically a lambda which
     *   forwards to the solver's dirichletBoundaryConditions().
     * @param marker Marker of the face, i.e. the face's centre, the size of
     *   the adjacent cells and the number of the face.
     * @param domainOffset Lower left corner of the computational domain.
     * @param domainSize Size of the computational domain.
     * @param periodicBC Periodic boundary conditions per axis. Faces along
     *   periodic axes are left unchanged.
     * @param numberOfGridCellsPerAxis Number of grid cells per axis within
     *   one patch.
     * @param overlap Number of grid cell layers per side of the face, i.e.
     *   the halo size.
     * @param unknownsPlusAuxiliaryVariables Number of doubles per grid cell.
     * @param t Time stamp handed over to boundaryCondition.
     * @param faceData The face's overlap data. Its layout is the one of
     *   toolbox::blockstructured::serialiseVoxelIndexInOverlap().
     */
    void applyDirichletBoundaryConditions(
      std::function<void(
        const tarch::la::Vector<Dimensions, double>& x,
        double                                       t,
        double*                                      Q
      )>                                           boundaryCondition,
      const peano4::datamanagement::FaceMarker&    marker,
      const tarch::la::Vector<Dimensions, double>& domainOffset,
      const tarch::la::Vector<Dimensions, double>& domainSize,
      const std::bitset<Dimensions>&               periodicBC,
      int                                          numberOfGridCellsPerAxis,
      int                                          overlap,
      int     unknownsPlusAuxiliaryVariables,
      double  t,
      double* faceData
    );
  } // namespace blockstructured
} // namespace exahype2
