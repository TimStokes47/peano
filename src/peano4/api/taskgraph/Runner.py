# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api.output


TEMPLATE_RUNNER_HEADER_FILE = """
// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "peano4/RunMultipleSteps.h"

{% for item in NAMESPACE -%}
namespace {{ item }} {
{%- endfor %}
  class {{CLASSNAME}};
{% for item in NAMESPACE -%}
}
{%- endfor %}


class {{NAMESPACE | join("::")}}::{{CLASSNAME}}: public peano4::RunMultipleSteps {
  public:
    {{CLASSNAME}}();

    virtual void run() override;
};
"""

TEMPLATE_RUNNER_CPP_FILE = """
#include "{{CLASSNAME}}.h"

#include "{{SUBDIRECTORY}}repositories/StepRepository.h"
#include "tarch/logging/LogFilter.h"
#include "peano4/parallel/SpacetreeSet.h"

{% for step in STEPS -%}
#include "{{SUBDIRECTORY}}observers/{{step}}.h"
{% endfor %}

{{NAMESPACE | join("::")}}::{{CLASSNAME}}::{{CLASSNAME}}():
  peano4::RunMultipleSteps(
    "{{CLASSNAME}}",
    {{PROJECT_NAMESPACE | join("::")}}::{{SUBNAMESPACE}}repositories::StepRepository::toProgramStep(
      {{PROJECT_NAMESPACE | join("::")}}::{{SUBNAMESPACE}}repositories::StepRepository::Steps::{{FIRST_STEP}}
    )
  ) {
}


void {{NAMESPACE | join("::")}}::{{CLASSNAME}}::run() {
  _watch.start();
  {% for step in STEPS %}
  {
    tarch::logging::LogFilter::getInstance().switchProgramPhase("{{step}}");
    {{PROJECT_NAMESPACE | join("::")}}::{{SUBNAMESPACE}}observers::{{step}} observer;
    peano4::parallel::SpacetreeSet::getInstance().traverse(observer);
  }
  {% endfor %}
  _watch.stop();
  _runtimeMeasurement.setValue(_watch.getCalendarTime());
}
"""


def create_runner(peano_project, identifier, steps):
    """!

    Create the header/implementation file pair that represents one of a
    project's algorithmic phases (setup, time stepping, plotting, ...) as a
    runner in the generated kernels/ subdirectory. Its run() traverses the
    spacetree set once per step, in the order the phase's task graph was
    compiled into.

    peano_project: peano4.api.Project
      The Peano 4 project to add the generated files to.

    identifier: str
      Same identifier used for the phase's Observers, e.g.
      "SetupSimulation", "TimeStep", "Plot".

    steps: [peano4.api.solversteps.Step]
      The sequence of Observers realising this algorithmic phase, in the
      order compiled from the phase's task graph (@see
      peano4.api.taskgraph.compiler.Sequential.translate). Its first
      element's name is the repositories::StepRepository::Steps enumerator
      the generated runner hands to peano4::RunMultipleSteps's constructor
      to kick the phase off. run() instantiates each step's Observer in
      turn and traverses the spacetree set with it, switching the log
      filter's program phase beforehand and timing the traversal via the
      inherited _watch/_runtimeMeasurement. "Undef" (and no traversals at
      all) if the phase has no steps, e.g. no solver contributed to it, so
      the generated code still compiles.

    """
    step_names = [step.name for step in steps]
    first_step = step_names[0] if step_names else "Undef"

    generated_files = peano4.api.output.Jinja2TemplateStringHeaderImplementationFilePair(
        headerfile_template = TEMPLATE_RUNNER_HEADER_FILE,
        cppfile_template    = TEMPLATE_RUNNER_CPP_FILE,
        classname           = identifier,
        namespace           = peano_project.namespace + ["kernels"],
        subdirectory        = peano_project.subdirectory + "kernels",
        dictionary          = {
            "PROJECT_NAMESPACE": peano_project.namespace,
            "SUBNAMESPACE":      peano_project._subdirectory + "::" if peano_project._subdirectory else "",
            "SUBDIRECTORY":      peano_project.subdirectory,
            "STEPS":             step_names,
            "FIRST_STEP":        first_step,
        },
        default_overwrite   = True,
        apply_iteratively   = False,
    )
    peano_project.output.add(generated_files)
    peano_project.output.makefile.add_cpp_file(
        peano_project.subdirectory + "kernels/" + identifier + ".cpp", generated=True
    )
