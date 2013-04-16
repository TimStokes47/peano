#include "peano/MappingSpecification.h"
#include "tarch/Assertions.h"
#include "tarch/logging/Log.h"


#include <sstream>


peano::MappingSpecification::MappingSpecification(peano::MappingSpecification::Manipulates manipulates_, peano::MappingSpecification::Multithreading multithreading_, bool restartable_):
  manipulates(manipulates_),
  multithreading(multithreading_),
  restartable(restartable_) {
  assertion(!restartable || multithreading);
}


std::string peano::MappingSpecification::toString() const {
  std::ostringstream msg;

  msg << "(manipulates=";
  switch (manipulates) {
    case Nop:
      msg << "nop";
      break;
    case OnlyLeaves:
      msg << "only leaves";
      break;
    case WholeTree:
      msg << "whole tree";
      break;
  }
  msg << ",multithreading=";
  switch (multithreading) {
    case Serial:
      msg << "serial";
      break;
    case AvoidFineGridRaces:
      msg << "avoid-fine-grid-races";
      break;
    case AvoidCoarseGridRaces:
      msg << "avoid-coarse-grid-races";
      break;
  }
  msg << ",restartable=" << restartable
      << ")";
  return msg.str();
}


peano::MappingSpecification operator&(const peano::MappingSpecification& lhs, const peano::MappingSpecification& rhs) {
  static tarch::logging::Log _log( "peano::MappingSpecification" );
  logTraceInWith2Arguments("operator&(...)", lhs.toString(), rhs.toString() );

  #if defined(Asserts) || defined(Debug)
  if (lhs.manipulates==peano::MappingSpecification::Nop && lhs.multithreading!=peano::MappingSpecification::AvoidFineGridRaces) {
    logWarning( "operator&(...)", "lhs is nop though is not only avoid-fine-grid-races: " << lhs.toString() );
  }
  if (rhs.manipulates==peano::MappingSpecification::Nop && rhs.multithreading!=peano::MappingSpecification::AvoidFineGridRaces) {
    logWarning( "operator&(...)", "rhs is nop though is not only avoid-fine-grid-races: " << rhs.toString() );
  }
  #endif

  peano::MappingSpecification::Manipulates manipulates =
    (rhs.manipulates==peano::MappingSpecification::WholeTree  || lhs.manipulates==peano::MappingSpecification::WholeTree)  ? peano::MappingSpecification::WholeTree  :
    (rhs.manipulates==peano::MappingSpecification::OnlyLeaves || lhs.manipulates==peano::MappingSpecification::OnlyLeaves) ? peano::MappingSpecification::OnlyLeaves :
    peano::MappingSpecification::Nop;

  peano::MappingSpecification::Multithreading multithreading =
    (rhs.multithreading==peano::MappingSpecification::Serial || lhs.multithreading==peano::MappingSpecification::Serial) ? peano::MappingSpecification::Serial :
    (rhs.multithreading==peano::MappingSpecification::AvoidCoarseGridRaces || lhs.multithreading==peano::MappingSpecification::AvoidCoarseGridRaces) ? peano::MappingSpecification::AvoidCoarseGridRaces :
    peano::MappingSpecification::AvoidFineGridRaces;

  bool restartable    = lhs.restartable    && rhs.restartable;

  peano::MappingSpecification result(manipulates,multithreading,restartable);
  logTraceOutWith1Argument("operator&(...)",result.toString());
  return result;
}


peano::MappingSpecification peano::MappingSpecification::getMostGeneralSpecification() {
  return MappingSpecification(Nop,peano::MappingSpecification::AvoidFineGridRaces,true);
}
