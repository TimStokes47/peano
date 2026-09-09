# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from .Project import Project


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

    """

    def __init__(self, project: Project):
        super().__init__(project)

    def construct_output(self, peano_project):
        peano4.api.Logger().log(
            "Create main file for {}".format(self.project._project_name)
        )
        peano_project.main = _ExaHyPEMainFileSequence(peano_project)


class _ExaHyPEMainFileSequence:
    """!

    Adapter that speaks the construct_output(output, main_name) protocol
    peano4.api.Project.generate() expects from its main attribute (see
    peano4.api.runner.DefaultSequence, whose job this class takes over for
    an ExaHyPE 2 project), but fills the header/implementation pair with
    ExaHyPE-flavoured content instead of the mesh-agnostic default.

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
#include "peano4/peano.h"
#include "peano4/parallel/SpacetreeSet.h"

{% if SUBDIRECTORIES -%}
{% for subdirectory in SUBDIRECTORIES %}
#include "{{subdirectory}}Constants.h"
#include "{{subdirectory}}repositories/DataRepository.h"
{% endfor -%}
{% else %}
#include "Constants.h"
#include "repositories/DataRepository.h"
{%- endif %}

tarch::logging::Log _log( "{{NAMESPACE | join("::")}}" );

int main(int argc, char** argv) {
  constexpr int ExitCodeSuccess = 0;

  peano4::initParallelEnvironment(&argc, &argv);
  peano4::fillLookupTables();
  {{NAMESPACE | join("::")}}::repositories::DataRepository::initDatatypes();
  peano4::initSingletons(
    {{NAMESPACE | join("::")}}::DomainOffset,
    {{NAMESPACE | join("::")}}::DomainSize,
    {{NAMESPACE | join("::")}}::PeriodicBC
  );

  // @todo The task graph compiler (peano4.api.taskgraph.compiler) only
  // decides which Observers exist per algorithmic phase and in which
  // order they may run - it does not yet say how a whole phase's
  // Observers together realise the actual
  // peano4::parallel::SpacetreeSet::traverse() sweeps. Fill this in
  // once that orchestration exists.

  peano4::shutdownSingletons();
  {{NAMESPACE | join("::")}}::repositories::DataRepository::shutdownDatatypes();
  peano4::shutdownParallelEnvironment();

  return ExitCodeSuccess;
}
"""

    def __init__(self, project):
        self.project = project

    def construct_output(self, output, main_name):
        d = {
            "SUBDIRECTORIES": list(self.project.subdirectories),
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
