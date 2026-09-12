# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

The Python-side model of "what happens during one mesh sweep"

A @ref peano4.api.solversteps.Step.Step corresponds to one generated C++
Observer, i.e. one grid traversal. It holds the vertex/face/cell data it
touches and an ordered list of @ref peano4.api.solversteps.ActionSet.ActionSet
instances - the actual "reactions to events" (touch vertex first time,
create cell, ...) that get woven into that Observer.
@ref peano4.api.solversteps.UserActionSet.UserActionSet is the one action
set every Step gets by default: an empty, user-editable stub placed under
`actions/` rather than being regenerated every run.

@ref peano4.api.solversteps.Steps.Steps is the top-level collection: it
holds every Step a peano4.api.Project knows about and, via
StepsToStepRepository, additionally generates the C++ `StepRepository`
that lets the runtime look steps up by name/index at runtime.

Steps and Step do not write C++ themselves. Each delegates to a private
generator held by aggregation: Step to StepToActionSets/StepToObserver,
Steps to StepsToStepRepository. Those generators are the bridge into
@ref peano4.api.output "peano4.api.output" - the actual C++ code writers -
turning one Python-side ActionSet/Step/Steps object into a
peano4.api.output.ActionSet/Observer/TemplatedHeaderImplementationFilePair
that is what ends up on disk.

\dot
digraph SolverStepsClasses {
  rankdir=BT
  fontname="Helvetica"
  node [shape=record, fontname="Helvetica", fontsize=10]
  edge [fontname="Helvetica", fontsize=9]

  ActionSet             [label="{«abstract»\nActionSet|+ descend_invocation_order : int\l+ parallel : bool\l|+ get_body_of_operation(name) : str\l+ get_action_set_name() : str\l+ user_should_modify_template() : bool\l+ get_attributes() : str\l+ get_includes() : str\l}"]
  UserActionSet         [label="{UserActionSet|+ name : str\l}"]
  Step                  [label="{Step|+ name : str\l+ action_sets : [ActionSet]\l+ vertex_data, face_data, cell_data : [...]\l|+ add_action_set(action_set) : void\l+ construct_output(output) : void\l}"]
  Steps                 [label="{Steps|+ _steps : [Step]\l|+ add_step(step) : void\l+ construct_output(output) : void\l}"]
  StepToActionSets      [label="{StepToActionSets|+ step : Step\l|+ construct_output(output, action_set) : str\l}"]
  StepToObserver        [label="{StepToObserver|+ step : Step\l|+ construct_output(output, included_action_sets) : void\l}"]
  StepsToStepRepository [label="{StepsToStepRepository|+ steps : Steps\l|+ construct_output(output) : void\l}"]

  OutputActionSet    [label="{«external»\npeano4.api.output.ActionSet}", style=dashed]
  OutputObserver     [label="{«external»\npeano4.api.output.Observer}", style=dashed]
  OutputTemplatePair [label="{«external»\npeano4.api.output.TemplatedHeaderImplementationFilePair}", style=dashed]

  edge [arrowhead=empty, style=solid, dir=forward]
  UserActionSet -> ActionSet

  edge [dir=both, arrowtail=diamond, arrowhead=none, style=solid]
  Step  -> ActionSet             [taillabel="1", headlabel="0..*", label="action_sets"]
  Step  -> StepToActionSets      [taillabel="1", headlabel="1",    label="action_set_generator"]
  Step  -> StepToObserver        [taillabel="1", headlabel="1",    label="observer_generator"]
  Steps -> Step                  [taillabel="1", headlabel="0..*", label="_steps"]
  Steps -> StepsToStepRepository [taillabel="1", headlabel="1",    label="generator"]

  edge [dir=forward, arrowhead=open, arrowtail=none, style=solid]
  StepToActionSets      -> Step  [label="step"]
  StepToObserver        -> Step  [label="step"]
  StepsToStepRepository -> Steps [label="steps"]

  edge [dir=forward, arrowhead=vee, arrowtail=none, style=dashed]
  StepToActionSets      -> OutputActionSet    [label="«creates»"]
  StepToObserver        -> OutputObserver     [label="«creates»"]
  StepsToStepRepository -> OutputTemplatePair [label="«creates»"]
}
\enddot

Solid hollow-triangle arrows are inheritance (UserActionSet extends
ActionSet); diamond-ended lines are aggregation/composition (a Step owns
its list of action sets and its two generators; a Steps owns its list of
Steps and its one generator); open-arrow solid lines are the plain
back-references the generators keep to the object they serve; dashed
open arrows are "creates" dependencies into @ref peano4.api.output.

@see peano4.api.solversteps.Step.Step
@see peano4.api.solversteps.Steps.Steps
@see peano4.api.solversteps.ActionSet.ActionSet
@see peano4.api.solversteps.UserActionSet.UserActionSet
@see peano4.api.output.ActionSet.ActionSet

"""

from .Step            import Step
from .Steps           import Steps
from .ActionSet       import ActionSet
from .UserActionSet   import UserActionSet
