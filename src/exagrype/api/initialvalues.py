""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

import exahype2.api


def _init_point_wisely(cpp_function: str) -> exahype2.api.InitPointWisely:
    return exahype2.api.InitPointWisely(
        implementation=f"::exagrype::initialvalues::{cpp_function}(Q, x, 0.0);"
    )


def gauge_wave() -> exahype2.api.InitPointWisely:
    """!

    Gauge wave along the x-axis, @see exagrype::initialvalues::gaugeWave()

    """
    return _init_point_wisely("gaugeWave")


def diagonal_gauge_wave() -> exahype2.api.InitPointWisely:
    """!

    Gauge wave along the diagonal, @see exagrype::initialvalues::diagonal_gaugeWave()

    """
    return _init_point_wisely("diagonal_gaugeWave")


def linear_wave() -> exahype2.api.InitPointWisely:
    """!

    Linearised gravitational wave, @see exagrype::initialvalues::linearWave()

    """
    return _init_point_wisely("linearWave")


def flat() -> exahype2.api.InitPointWisely:
    """!

    Flat (Minkowski) spacetime, @see exagrype::initialvalues::flat()

    """
    return _init_point_wisely("flat")
