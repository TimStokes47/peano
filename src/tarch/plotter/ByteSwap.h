#ifndef _TARCH_PLOTTER_BYTESWAP_H
#define _TARCH_PLOTTER_BYTESWAP_H

namespace tarch {
  namespace plotter {
    /**
     * Transform inVal vom little-endian into big-endian. Is required by some
     * of the vtk plotter.
     */
    template <class InType>
    InType ByteSwap(const InType inVal);
  }
}

#include "tarch/plotter/ByteSwap.cpph"

#endif
