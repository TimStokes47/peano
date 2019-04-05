#ifndef _VISUALISATION_FILTER_INTERSECTIONS_H_
#define _VISUALISATION_FILTER_INTERSECTIONS_H_


#include "Filter.h"


namespace visualisation {
  namespace filter {
    class Intersections;
  }
}


class visualisation::filter::Intersections public visualisation::filter::Filter {
  public:
	void removeIntersectedCoarseGrainPatches();
};


#endif
