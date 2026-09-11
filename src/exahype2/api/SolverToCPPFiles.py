# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api
import peano4.api.output.Jinja2TemplatedHeaderImplementationFilePair

import dastgen2

import os
import sys

from .Solver import Solver


class SolverToCPPFiles(Solver.SolverGenerator):
    TEMPLATE_ABSTRACT_HEADER_FILE = """
// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "exahype2/RefinementControl.h"
#include "exahype2/Solver.h"
#include "tarch/la/Vector.h"
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/accelerator/accelerator.h"
#include "tarch/NonCriticalAssertions.h"

#include "peano4/utils/Globals.h"

#include "Constants.h"
    

{% for item in NAMESPACE -%}
  namespace {{ item }} {
  {# Empty line here #}
{%- endfor %}
  class {{CLASSNAME}};

{% for item in NAMESPACE -%}
  }
{%- endfor %}


class {{NAMESPACE | join("::")}}::{{CLASSNAME}}: public ::exahype2::Solver {
  public:
    {{CLASSNAME}}();

    double getMinTimeStamp() const final;
    double getMaxTimeStamp() const final;
    double getMinTimeStepSize() const final;
    double getMaxTimeStepSize() const final;

    /**
     * If you hook into this routine, ensure the abstract base class
     * operation is still invoked.
     */
    virtual void startGridConstructionStep() override;

    /**
     * If you hook into this routine, ensure the abstract base class
     * operation is still invoked.
     */
    virtual void finishGridConstructionStep() override;

    /**
     * If you hook into this routine, ensure the abstract base class
     * operation is still invoked.
     */
    virtual void startGridInitialisationStep() override;

    /**
     * If you hook into this routine, ensure the abstract base class
     * operation is still invoked.
     */
    virtual void finishGridInitialisationStep() override;

    /**
     * If you hook into this routine, ensure the abstract base class
     * operation is still invoked.
     */
    virtual void startTimeStep(
      double globalMinTimeStamp,
      double globalMaxTimeStamp,
      double globalMinTimeStepSize,
      double globalMaxTimeStepSize
    ) override;

    /**
     * If you hook into this routine, ensure the abstract base class
     * operation is still invoked.
     */
    virtual void finishTimeStep() override;

    /**
     * If you hook into this routine, ensure the abstract base class
     * operation is still invoked.
     */
    virtual void startPlottingStep(
      double globalMinTimeStamp,
      double globalMaxTimeStamp,
      double globalMinTimeStepSize,
      double globalMaxTimeStepSize
    ) override;

    /**
     * If you hook into this routine, ensure the abstract base class
     * operation is still invoked.
     */
    virtual void finishPlottingStep() override;

    /**
     * mesh is an aAlias for patch
     */
    virtual double getMaxMeshSize() const override final;
    virtual double getMinMeshSize() const override final;
    
    static constexpr int    NumberOfUnknowns           = {{NUMBER_OF_UNKNOWNS}};
    static constexpr int    NumberOfAuxiliaryVariables = {{NUMBER_OF_AUXILIARY_VARIABLES}};

    /**
     * It is important that we only plot after the secondary sweep.
     * All the switching happens in startTimeStep(), so it is fine if
     * we test against secondary. If an outer routine (the main) calls
     * this function, this means that the secondary traversal just has
     * terminated.
     */
     // @todo For all transitions?
    virtual bool mayPlot() const override;
    virtual bool patchCanUseStatelessPDETerms(
      const tarch::la::Vector<Dimensions, double>& patchCentre,
      const tarch::la::Vector<Dimensions, double>& patchH,
      double                                       t,
      double                                       dt
    ) const;

    /**
     * Feel free to overwrite in user code, but ensure the superclass
     * implementation is still invoked, too.
     */
    virtual void startSimulation() override;

    /**
     * Feel free to overwrite in user code, but ensure the superclass
     * implementation is still invoked, too.
     */
    virtual void finishSimulation() override;

  protected:
    static tarch::logging::Log  _log;

    SolverState  _solverState;

    double     _minTimeStamp;
    double     _maxTimeStamp;

    double     _minTimeStampThisTimeStep;
    double     _maxTimeStampThisTimeStep;

    double     _minVolumeH;
    double     _maxVolumeH;

    double     _minVolumeHThisTimeStep;
    double     _maxVolumeHThisTimeStep;

    double     _minTimeStepSize;
    double     _maxTimeStepSize;

    double     _minTimeStepSizeThisTimeStep;
    double     _maxTimeStepSizeThisTimeStep;

    int        _cellUpdates;

    tarch::multicore::BooleanSemaphore  _semaphore;

    {{ABSTRACT_SOLVER_USER_DECLARATIONS}}
};

{# Empty line here #}
"""


    TEMPLATE_ABSTRACT_CPP_FILE = """
#include "{{CLASSNAME}}.h"
#include "exahype2/RefinementControl.h"

tarch::logging::Log {{NAMESPACE | join("::")}}::{{CLASSNAME}}::_log( "{{NAMESPACE | join("::")}}::{{CLASSNAME}}" );
"""
    
    """!

    Default implementation that takes a solver and maps it onto two C++ classes:
    An abstract base class and a class for the user to implement it.

    """
    def __init__(self, solver: Solver):
        super().__init__(solver)


    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         ):
        """!

        Dump the solver into actual C++ classes.

        """
        class_name = "Abstract" + self._solver.name
        templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )
        
        peano4.api.Logger().log(
            "Map class {} onto C++ class {} using {}".format(self._solver.name, class_name, templatefile_prefix)
        )

        local_d = {
          "NUMBER_OF_UNKNOWNS": self._solver._unknowns,
          "NUMBER_OF_AUXILIARY_VARIABLES": self._solver._auxiliary_variables,
        }


        generated_files = peano4.api.output.Jinja2TemplateStringHeaderImplementationFilePair(
            headerfile_template = self.TEMPLATE_ABSTRACT_HEADER_FILE,
            cppfile_template    = self.TEMPLATE_ABSTRACT_CPP_FILE,
            classname           = class_name,
            namespace           = peano_project.namespace,
            subdirectory        = peano_project.subdirectory,
            dictionary          = local_d,
            default_overwrite=True,
            apply_iteratively=False,
        )
        peano_project.output.add(generated_files)
        peano_project.output.makefile.add_cpp_file(
            peano_project.subdirectory + class_name + ".cpp", generated=True
        )

