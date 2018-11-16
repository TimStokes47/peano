// Copyright (C) 2009 Technische Universitaet Muenchen
// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www5.in.tum.de/peano#ifndef _PEANO_KERNEL_MULTICORE_MULTILEVELSCHEDULER_ACTION_H_
#ifndef _TARCH_TIMING_GLIDING_AVERAGE_MEASUREMENT_H_
#define _TARCH_TIMING_GLIDING_AVERAGE_MEASUREMENT_H_


#include <string>
#include <vector>

#include "tarch/logging/Log.h"


namespace tarch {
  namespace timing {
    class GlidingAverageMeasurement;
  }
}


/**
 * GlidingAverageMeasurement
 *
 * This is alternative implementation to Measurement which uses a gliding
 * average rather than the real average over all data.
 *
 * @author Tobias Weinzierl
 */
class tarch::timing::GlidingAverageMeasurement {
  private:
    static tarch::logging::Log _log;

    double          _accuracy;
    /**
     * The weights q determine the weight via $q^0,q^1,q^2,...$.
     */
    double          _weight;
    int             _maxEntries;

    /**
     * Data
     */
    std::vector<double>  _values;

    /**
     * Needed to compute average value and variance.
     */
    bool            _isAccurateValue;
  public:
    GlidingAverageMeasurement(double accuracy=0.0, double weight=0.9, int maxEntries=1024);

    /**
     * @return Averaged value (mean value) of all measurements.
     */
    double getValue() const;

    /**
     * To compute the standard deviation, we rely on the formula
     *
     * sigma =sqrt( E(x^2) - E(x)^2 )
     *
     * with E being the weighted mean value.
     */
    double getStandardDeviation() const;

    /**
     * Is value accurate
     *
     * Whether a value is accurate depends on the last setValue() call. The
     * class internally holds the mean value of all setValue() calls. If a new
     * value is set/added, the object checks whether this additional
     * measurement modifies the mean value more than the given accuracy.
     */
    bool isAccurateValue() const;

    /**
     * @see isAccurateValue()
     */
    void setAccuracy(const double& value);

    void increaseAccuracy( const double& factor );

    /**
     * Set the value. If the measurement already holds a value, this value is
     * not overwritten. Instead, the measurement accumulates all values and
     * returns the average.
     */
    void setValue(const double& value);
    int getNumberOfMeasurements() const;
    std::string toString() const;

    double max() const;
    double min() const;

    void erase();

    double getAccuracy() const;
};

#endif

