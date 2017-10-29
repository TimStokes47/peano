// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_PERFORMANCE_ANALYIS_SPEEDUP_LAWS_H_
#define _PEANO_PERFORMANCE_ANALYIS_SPEEDUP_LAWS_H_


#include "peano/performanceanalysis/Analyser.h"
#include "tarch/logging/Log.h"
#include "tarch/la/Vector.h"


namespace peano {
  namespace performanceanalysis {
    class SpeedupLaws;
  }
}



/**
 * Class to compute standard speedup laws
 *
 * Currently, I support solely Amdahl's law, i.e. strong speedup.
 *
 * The class is designed to allow users to determine the parameters of
 * Amdahl's law (strong speedup) from multiple measurements. The class
 * requires to be fed with at least two measurements and then determines
 * the serial fraction f of the overall code and the time the code would
 * need to run on a single core.
 *
 * As soon as we have more than two measurements, the problem is obviously
 * overdetermined. Which makes sense, as all measurement data is typically
 * noisy. So we want to construct a model
 *
 * @f$ t(p) = f \cdot t(1) + (1-f) \cdot \frac{t(1)}{p} @f$
 *
 * from measurement tuples @f$ ( p_i, t(p_i) ) @f$. Let N be the number of
 * measurements in total.
 *
 *
 * @author Tobias Weinzierl
 */
class peano::performanceanalysis::SpeedupLaws {
  private:
    static constexpr int  Entries = 8;

    static tarch::logging::Log _log;

    double     _f;
    double     _t1;

    tarch::la::Vector<Entries, double> _t;
    tarch::la::Vector<Entries, double> _p;

    int        _samples;
  public:
    /**
     * @param MaxEntries Maximum number of measurements taken into account.
     *        Everytime more entries than this one are added, we remove the
     *        oldest measurement.
     */
    SpeedupLaws();

    void addMeasurement( int p, double t );

    /**
     * This operation determines a new iterate (approximation) for f and t1
     * following Amdahl's law.
     */
    void relaxAmdahlsLaw();

    double getSerialTime() const;
    double getSerialCodeFraction() const;
};

#endif
