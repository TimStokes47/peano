// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org

/**
 * This file defines some macros and definitions used by most of the files of
 * the project. Among the definition of assertions and the global constants
 * (DIMENSIONS and NUMBER_OF_VERTICES_PER_ELEMENT) are the numerical comparison
 * operations.
 *
 * @version $Revision: 1.70 $
 * @author Tobias Weinzierl, Tobias Neckel
 *
 * @todo Doku fuer macros in Globals.h vervollstaendigen!
 */
#ifndef _PEANO4_UTILS_GLOBALS_H_
#define _PEANO4_UTILS_GLOBALS_H_

  template <int base, int exponent>
  class BasePowerExponent {
    public:
    enum {value = base*BasePowerExponent<base,exponent-1>::value};
  };

  template <int base>
  class BasePowerExponent<base,0> {
    public:
    enum {value = 1};
  };

  #define TWO_POWER_D   (BasePowerExponent<2,DIMENSIONS>::value)
  #define TWO_POWER_D_TIMES_D   (BasePowerExponent<2,DIMENSIONS>::value*DIMENSIONS)
  #define TWO_POWER_D_MINUS_ONE (BasePowerExponent<2,DIMENSIONS>::value-1)
  #define TWO_POWER_D_TIMES_TWO_POWER_D (BasePowerExponent<2,DIMENSIONS>::value*BasePowerExponent<2,DIMENSIONS>::value)
  #define TWO_POWER_D_TIMES_TWO_POWER_D_DIVIDED_BY_TWO (BasePowerExponent<2,DIMENSIONS>::value*BasePowerExponent<2,DIMENSIONS-1>::value)
  #define TWO_POWER_D_TIMES_THREE_POWER_D (BasePowerExponent<2,DIMENSIONS>::value*BasePowerExponent<3,DIMENSIONS>::value)
  #define TWO_POWER_D_TIMES_FIVE_POWER_D (BasePowerExponent<2,DIMENSIONS>::value*BasePowerExponent<5,DIMENSIONS>::value)
  #define TWO_POWER_D_TIMES_THREE_POWER_D_DIVIDED_BY_THREE (BasePowerExponent<2,DIMENSIONS>::value*BasePowerExponent<3,DIMENSIONS-1>::value)
  #define TWO_POWER_D_DIVIDED_BY_TWO (BasePowerExponent<2,DIMENSIONS-1>::value)
  #define TWO_POWER_D_TIMES_D (BasePowerExponent<2,DIMENSIONS>::value*DIMENSIONS)
  #define TWO_POWER_TWO_POWER_D (BasePowerExponent<2,TWO_POWER_D>::value)
  #define THREE_POWER_D (BasePowerExponent<3,DIMENSIONS>::value)
  #define THREE_POWER_D_MINUS_ONE (BasePowerExponent<3,DIMENSIONS>::value-1)
  #define THREE_POWER_D_DIVIDED_BY_THREE (BasePowerExponent<3,DIMENSIONS-1>::value)
  #define THREE_POWER_D_TIMES_FOUR_POWER_D (BasePowerExponent<3,DIMENSIONS>::value*BasePowerExponent<4,DIMENSIONS>::value)
  #define FOUR_POWER_D  (BasePowerExponent<4,DIMENSIONS>::value)
  #define FOUR_POWER_D_DIVIDED_BY_FOUR  (BasePowerExponent<4,DIMENSIONS-1>::value)
  #define FIVE_POWER_D  (BasePowerExponent<5,DIMENSIONS>::value)
  #define FIVE_POWER_D_DIVIDED_BY_FIVE  (BasePowerExponent<5,DIMENSIONS-1>::value)
  #define SIX_POWER_D  (BasePowerExponent<6,DIMENSIONS>::value)
  #define SEVEN_POWER_D  (BasePowerExponent<7,DIMENSIONS>::value)
  #define NINE_POWER_D  (BasePowerExponent<9,DIMENSIONS>::value)
  #define ELEVEN_POWER_D  (BasePowerExponent<11,DIMENSIONS>::value)
  #define THIRTEEN_POWER_D  (BasePowerExponent<13,DIMENSIONS>::value)
  #define FIFTEEN_POWER_D  (BasePowerExponent<15,DIMENSIONS>::value)
  #define SEVENTEEN_POWER_D  (BasePowerExponent<17,DIMENSIONS>::value)
  #define NUMBER_OF_VERTICES_PER_ELEMENT TWO_POWER_D
  #define DIMENSIONS_TIMES_TWO (2*DIMENSIONS)
  #define DIMENSIONS_TIMES_TWO_POWER_D (DIMENSIONS*TWO_POWER_D)
  #define DIMENSIONS_TIMES_FOUR (4*DIMENSIONS)
  #define DIMENSIONS_TIMES_DIMENSIONS (DIMENSIONS*DIMENSIONS)

#endif
