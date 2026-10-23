# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from ..Project import Project


class DefaultSimulationMain(Project.Generator):
    """!

    Default realisation of exahype2.api.Project.mainfile_generator

    Mirrors SolverToCPPFiles on the solver side: turns the project
    configuration into the header/implementation file pair that ends up
    holding main(), similar in spirit to
    python/exahype2/ExaHyPEMain.template.h/.cpp. Rather than emitting that
    file pair itself, it replaces peano_project's generic peano4-level main
    file generator (peano4.api.runner.DefaultSequence) with
    _ExaHyPEMainFileSequence below, which speaks the same
    construct_output(output, main_name) protocol but fills the file pair
    with ExaHyPE-flavoured content instead of the mesh-agnostic default.

    The generated main() contains only what is specific to the project:
    the runners, the data types, the domain, the log filter and the load
    balancing. The runtime configuration, the main loop over the runners
    and the shutdown are plain C++, i.e. exahype2::configureRuntime(),
    exahype2::runSimulation() and exahype2::shutdownRuntime().

    """

    def __init__(self, project: Project):
        super().__init__(project)

    def construct_output(self, peano_project):
        peano4.api.Logger().log(
            "Create main file for {}".format(self.project._project_name)
        )
        peano_project.main = _ExaHyPEMainFileSequence(peano_project, self.project)


class _ExaHyPEMainFileSequence:
    """!

    Adapter that speaks the construct_output(output, main_name) protocol
    peano4.api.Project.generate() expects from its main attribute (see
    peano4.api.runner.DefaultSequence, whose job this class takes over for
    an ExaHyPE 2 project), but fills the header/implementation pair with
    ExaHyPE-flavoured content instead of the mesh-agnostic default.

    project is the peano4-level project (peano4.api.Project) this sequence
    is attached to as .main - it is where SUBDIRECTORIES/SUBNAMESPACE/
    rootnamespace come from, mirroring peano4.api.runner.DefaultSequence.

    exahype_project is the exahype2.api.Project this main file is generated
    for - it is where the runtime settings (TIMEOUT, NUMBER_OF_THREADS,
    GPUS, MULTICORE_ORCHESTRATION) come from, as those live on the ExaHyPE
    2 project rather than on the plain peano4 one.

    """

    HEADER_TEMPLATE = """
// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

int main(int argc, char** argv);
"""

    CPP_TEMPLATE = """
// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "{{CLASSNAME}}.h"

#include "tarch/logging/Log.h"
#include "tarch/logging/LogFilterFileReader.h"
#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Core.h"
#include "tarch/multicore/orchestration/Strategy.h"
#include "peano4/peano.h"
#include "peano4/DefaultStepReport.h"
#include "exahype2/LoadBalancingConfiguration.h"
#include "exahype2/Simulation.h"
#include "toolbox/loadbalancing/LoadBalancingService.h"
{% if LOAD_BALANCER!="" -%}
#include "{{LOAD_BALANCER | replace('::', '/') }}.h"
{% else -%}
#include "toolbox/loadbalancing/strategies/NoLoadBalancing.h"
{% endif %}

{% if SUBDIRECTORIES -%}
{% for subdirectory in SUBDIRECTORIES %}
#include "{{subdirectory}}Constants.h"
#include "{{subdirectory}}repositories/DataRepository.h"
#include "{{subdirectory}}repositories/StepRepository.h"
#include "{{subdirectory}}repositories/SolverRepository.h"
#include "{{subdirectory}}runners/SetupSimulation.h"
#include "{{subdirectory}}runners/TimeStep.h"
#include "{{subdirectory}}runners/Plot.h"
{% endfor -%}
{% else %}
#include "Constants.h"
#include "repositories/DataRepository.h"
#include "repositories/StepRepository.h"
#include "repositories/SolverRepository.h"
#include "runners/SetupSimulation.h"
#include "runners/TimeStep.h"
#include "runners/Plot.h"
{%- endif %}

tarch::logging::Log _log( "{{NAMESPACE | join("::")}}" );


{{NAMESPACE | join("::")}}::{{SUBNAMESPACE}}runners::SetupSimulation SetupSimulation;
{{NAMESPACE | join("::")}}::{{SUBNAMESPACE}}runners::Plot Plot;
{{NAMESPACE | join("::")}}::{{SUBNAMESPACE}}runners::TimeStep TimeStep;

{{STEP_REPORT_CLASS}} stepReport;


int main(int argc, char** argv) {
  constexpr int ExitCodeSuccess          = 0;
  constexpr int ExitCodeUnitTestsFailed  = 1;
  constexpr int ExitCodeInvalidArguments = 2;
  constexpr int ExitCodeInvalidBuild     = 3;

  peano4::initParallelEnvironment(&argc, &argv);

  // Do this early, so people can use logInfo/logWarning/... properly from
  // here on.
  {% if LOG_FILTER_FILE!="" %}
  if ( not tarch::logging::LogFilterFileReader::parsePlainTextFile( "{{LOG_FILTER_FILE}}" ) ) {
    if (tarch::mpi::Rank::getInstance().isGlobalMaster()) {
      logWarning( "main()", "filter file {{LOG_FILTER_FILE}} not found or file has been corrupted. Use default logging configuration" );
    }
  }
  {% endif %}

  peano4::fillLookupTables();
  {{NAMESPACE | join("::")}}::{{SUBNAMESPACE}}repositories::DataRepository::initDatatypes();
  peano4::initSingletons(
    {{NAMESPACE | join("::")}}::DomainOffset,
    {{NAMESPACE | join("::")}}::DomainSize,
    {{NAMESPACE | join("::")}}::PeriodicBC
  );

  if ( not ::exahype2::configureRuntime(
    {{TIMEOUT}},
    {{NUMBER_OF_THREADS}},
    { {{GPUS|join(",")}} },
    {{MULTICORE_ORCHESTRATION}}
  ) ) {
    return ExitCodeInvalidBuild;
  }

  {% if LOAD_BALANCER!="" -%}
  toolbox::loadbalancing::LoadBalancingService::getInstance().setLoadBalancing(
    new {{LOAD_BALANCER}}({{LOAD_BALANCER_ARGUMENTS}})
  );
  {% else -%}
  toolbox::loadbalancing::LoadBalancingService::getInstance().setLoadBalancing(
    new toolbox::loadbalancing::strategies::NoLoadBalancing()
  );
  {% endif %}

  {% if FENV_ARGS is defined and FENV_ARGS != "" -%}
  feenableexcept({{FENV_ARGS}} );
  {% endif -%}

  ::exahype2::runSimulation(
    SetupSimulation,
    Plot,
    TimeStep,
    stepReport,
    {{NAMESPACE | join("::")}}::{{SUBNAMESPACE}}repositories::wantsToPlot,
    {{NAMESPACE | join("::")}}::{{SUBNAMESPACE}}repositories::getMinTimeStamp,
    {{NAMESPACE | join("::")}}::{{SUBNAMESPACE}}repositories::getMaxTimeStamp,
    {{NAMESPACE | join("::")}}::{{SUBNAMESPACE}}repositories::getMaxTimeStepSize,
    {{NAMESPACE | join("::")}}::MinTerminalTime,
    {{NAMESPACE | join("::")}}::MaxTerminalTime
  );

  ::exahype2::shutdownRuntime(
    {{NAMESPACE | join("::")}}::{{SUBNAMESPACE}}repositories::DataRepository::shutdownDatatypes
  );

  return ExitCodeSuccess;
}
"""

    def __init__(self, project, exahype_project):
        self.project = project
        self.exahype_project = exahype_project

    def construct_output(self, output, main_name):
        d = {
            "SUBDIRECTORIES": list(self.project.subdirectories),
            "SUBNAMESPACE": self.project._subdirectory + "::" if self.project._subdirectory else "",
            "TIMEOUT": self.exahype_project._timeout,
            "NUMBER_OF_THREADS": self.exahype_project._number_of_threads,
            "MULTICORE_ORCHESTRATION": self.exahype_project._multicore_orchestration,
            "GPUS": self.exahype_project._gpus,
            "LOG_FILTER_FILE": self.exahype_project._log_filter_file,
            "STEP_REPORT_CLASS": self.exahype_project._step_report_class,
            "LOAD_BALANCER": self.exahype_project._load_balancer_name,
            "LOAD_BALANCER_ARGUMENTS": self.exahype_project._load_balancer_arguments,
        }

        generated_files = peano4.api.output.Jinja2TemplateStringHeaderImplementationFilePair(
            headerfile_template = self.HEADER_TEMPLATE,
            cppfile_template    = self.CPP_TEMPLATE,
            classname           = main_name,
            namespace           = self.project.rootnamespace,
            subdirectory        = "./",
            dictionary          = d,
            default_overwrite   = True,
        )

        output.makefile.add_h_file(main_name + ".h", generated=True)
        output.makefile.add_cpp_file(main_name + ".cpp", generated=True)
        output.add(generated_files)
