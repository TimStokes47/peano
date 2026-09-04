// This file is part of the ExaParty project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "peano4/datamanagement/CellMarker.h"
#include "peano4/datamanagement/VertexMarker.h"
#include "exaparty/kernels/ParticleUpdatePredicates.h"


namespace exaparty {
  namespace timestepping {
    /**
     * Reset the marker of a particle from whatever it has been before to
     * NotMoved. This routine should be called in any task graph before a
     * particle's position is actually altered. The name is not brilliant,
     * as you should also call this routine prior to a change of the interaction
     * radius.
     */
    template <typename Particle>
    void reset_moved_particle_marker(Particle& particle);

    template <typename Particle>
    bool resetMovedParticleMarkerUpdateParticlePredicate(
      const peano4::datamanagement::VertexMarker& marker,
      const Particle&                             particle
    ) {
      return ::exaparty::kernels::localParticleCanBeUpdatedInVertexKernel(
        marker,
        particle
      );
    }

    template <typename Particle>
    void resetMovedParticleMarker(
      const peano4::datamanagement::VertexMarker& marker,
      Particle&                                   particle
    ) {
      assertion2(
        ::exaparty::kernels::
          localParticleCanBeUpdatedInVertexKernel(marker, particle),
        marker.toString(),
        particle.toString()
      );
      reset_moved_particle_marker(particle);
    }
  } // namespace timestepping
} // namespace exaparty


#include "TimeStepping.cpph"
