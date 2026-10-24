# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Runge-Kutta time integration tools which do not depend on the discretisation
in space, shared by all Runge-Kutta schemes of exahype2.api, such as
@ref exahype2.api.rkfd4

"""
from .ButcherTableau import ButcherTableau
