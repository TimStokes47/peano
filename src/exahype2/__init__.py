""" This file is part of the ExaHyPE 2 project within Peano 4. For conditions of distribution and 
 use, please see the copyright notice at www.peano-framework.org
"""
import peano4.api

import exahype2.api
import exahype2.plotter
import exahype2.initialisation
#import exahype2.tracer
#import exahype2.grid
#import exahype2.dslhype

#import exahype2.solvers.fv
#import exahype2.solvers.rkfd
#import exahype2.solvers.elliptic


peano4.api.Logger().log("ExaHyPE 2x extension", peano4.api.LoggerPrintOption.ALWAYS_PRINT)


havenumpy = False
try:
    import numpy
    havenumpy = True
except ImportError:
    print("Numpy is not available, not loading exahype2.symhype.")
    print("Numpy is not available, not loading exahype2.solvers.rkdg.")
    print("Numpy is not available, not loading exahype2.solvers.aderdg.")

havempmath = False
try:
    import mpmath
    havempmath = True
except ImportError:
    print("Mpmath is not available, not loading exahype2.solvers.rkdg.")
    print("Mpmath is not available, not loading exahype2.solvers.aderdg.")

if havenumpy and havempmath:
    #import exahype2.solvers.rkdg
    #import exahype2.solvers.aderdg
    #import exahype2.solvers.limiting
    pass

havesympy = False
try:
    import sympy
    havesympy = True
except ImportError:
    print("Sympy is not available, not loading exahype2.symhype.")

if havenumpy and havesympy:
    #import exahype2.symhype
    pass

havematplotlib = False
try:
    import matplotlib
    havematplotlib = True
except ImportError:
    print("Matplotlib is not available, not loading exahype2.postprocessing.")

if havematplotlib:
    #import exahype2.postprocessing
    pass

#from .Project import Project
#from .ExaHyPEMain import ExaHyPEMain

