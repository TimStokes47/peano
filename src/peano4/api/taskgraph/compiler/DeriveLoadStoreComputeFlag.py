# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import Dict, List

import peano4.api.solversteps

from ..Task import Task


# Has to match peano4.api.datamodel.PatchToDoubleArray.PatchToDoubleArray.__init__()'s
# own default for load_store_compute_flag - that attribute is never None, so
# comparing against its factory-default value is the only way to tell
# "nobody has touched this yet" apart from "a task graph author (or an
# earlier call to this very function) already customised it".
_DEFAULT_LOAD_STORE_COMPUTE_FLAG = (
    "::peano4::grid::LoadStoreComputeFlag::LoadFromInputStream_ProvideToCalculations_StoreToOutputStream"
)


def derive_load_store_compute_flag(steps: List["peano4.api.solversteps.Step"]) -> None:
    """!

    For every submodel used anywhere in steps, derive a
    ::peano4::grid::constructLoadStoreComputeFlag(...) expression from
    which of its using Steps are active, and assign it to that submodel's
    generator.load_store_compute_flag - the runtime counterpart to
    peano4.api.taskgraph.compiler.PropagateUseStatements.propagate_use_statements(),
    which has to run before whichever compiler produces these steps: it
    decides *which* tasks have to declare a submodel at all; this function
    decides, for every Step that does, what its generated Observer should
    actually do with the data once the submodel's stack storage puts it
    there (load a real value, or start from a fresh dummy; store the
    result back, or discard it) - by asking, at runtime, which Step is
    presently executing.

    This has to run *after* every algorithmic phase has actually been
    compiled (@see peano4.api.taskgraph.compiler.Sequential.translate(),
    or whichever compiler a caller configured instead), not before: a
    compiled Step's name, and which submodels it ends up using with which
    qualifier, are the compiler's call to make - Sequential.translate()
    happens to map one Task to one identically-qualified Step today, but
    it explicitly documents itself as the naive "no fusion, no reordering"
    baseline that smarter compilers (task fusion, dead-task elimination,
    reordering) are meant to be measured against and eventually replace.
    Reasoning about Tasks before compilation - e.g. by predicting a future
    Step's name from a Task's - would silently assume every such compiler
    keeps that 1:1 mapping, which is exactly what they need not do. Working
    from the compiled Steps themselves keeps this function correct no
    matter which compiler produced them.

    Unlike propagate_use_statements(), this needs no notion of order,
    distance or edges between graphs - it only has to know, per submodel,
    which Steps touch it and how; a flat scan over every Step given is
    enough.

    ## Building the three predicates

    ::peano4::grid::constructLoadStoreComputeFlag(predicateToUseData,
    predicateForLoad, predicateForStore) takes three C++ boolean
    expressions and combines them into the 5-way
    peano4::grid::LoadStoreComputeFlag enum
    (@see src/peano4/grid/LoadStoreComputeFlag.h/.cpp). For a given
    submodel, each predicate is an "or"-chain of

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    peano4::parallel::Node::getInstance().getCurrentProgramStep() == repositories::StepRepository::toProgramStep(repositories::StepRepository::Steps::<name>)
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    terms - one per Step that qualifies, <name> being that Step's own
    .name (exactly as the compiler set it - @see
    peano4.api.solversteps.Step, src/peano4/api/solversteps/StepsToStepRepository.py
    for how a Step's name turns into a repositories::StepRepository::Steps
    enumerator):

    - predicateToUseData: every Step that declares the submodel at all,
      whichever Task.DataAccess qualifier it carries - all four qualifiers
      (including TEMPORARY) contribute here, since all of them mean the
      Step needs the submodel to be considered "in play" while it runs,
      even if - as for TEMPORARY - it never actually looks at the value.
    - predicateForLoad: only Steps qualified READ_AND_DISCARD or
      READ_AND_WRITE - the ones that actually need the real, previously
      stored value.
    - predicateForStore: only Steps qualified WRITE or READ_AND_WRITE -
      the ones that produce a value worth keeping.

    TEMPORARY therefore only ever contributes to predicateToUseData - @see
    Task.DataAccess.TEMPORARY. A predicate with no contributing Step at
    all becomes the literal "false".

    ## Grid selector

    If a submodel carries a grid_selector (@see
    peano4.api.datamodel.DoF, peano4.api.toolbox.GridSelector), the
    selector's guard for the submodel's kind (createCellGuard(),
    createFaceGuard() or createVertexGuard()) is AND-combined with each of
    the three predicates:

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ::peano4::grid::constructLoadStoreComputeFlag((guard) and (use), (guard) and (load), (guard) and (store))
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    That is, data are only used, loaded or stored if the selector holds
    and the steps above conclude that they are needed. The guard is
    evaluated within the generated loadStoreComputeFlag(marker), i.e. it
    can query the marker.

    @note Peano's stacks require that a datum stored in one mesh sweep is
    loaded in the next one. A selector which depends on the refinement
    state, such as peano4.api.toolbox.FineGridSelector, therefore has to
    yield the same answer for the store and the subsequent load. This holds
    as long as the mesh does not change between the two sweeps, but not
    in general for dynamically adaptive meshes.

    This does not touch generator.send_condition/receive_and_merge_condition -
    those are a separate, MPI rank-boundary concern this function has
    nothing to say about.

    ## Includes

    Every submodel this function touches gets
    '#include "repositories/StepRepository.h"',
    '#include "repositories/SolverRepository.h"' and
    '#include "peano4/parallel/Node.h"' appended to its generator.includes
    (skipped if already present) - the generated
    constructLoadStoreComputeFlag(...) expression references
    repositories::StepRepository::toProgramStep/::Steps (declared in the
    first header) and peano4::parallel::Node::getInstance() (declared in
    the third); the second is added alongside the first to match this
    codebase's existing convention of always pairing the two (@see
    exahype2.api.generators.DefaultSimulationMain,
    mghype.matrixfree.api.MatrixFreeMain.template.cpp,
    mghype.petsc.api.PETScMain.template.cpp).

    ## DaStGen2 data

    Submodels modelled via DaStGen2 (@ref peano4.api.datamodel.DaStGen2.DaStGen2)
    do not hold the load/store/compute flag in their generator but in their
    peano4_mpi_and_storage_aspect (@ref peano4.api.dastgen2.MPIAndStorageAspect.MPIAndStorageAspect).
    For these, the function writes the flag into the aspect, and appends the
    includes above to the aspect's implementation_file_includes, as the
    aspect evaluates the flag within the implementation file. Adding them to
    the header might yield cyclic includes.

    ## Combining with a pre-existing expression

    If a submodel's generator.load_store_compute_flag already holds something
    other than its factory default (@see _DEFAULT_LOAD_STORE_COMPUTE_FLAG) -
    because a task graph author set it explicitly, or an earlier call to this
    very function already ran - the newly derived
    constructLoadStoreComputeFlag(...) call is not allowed to simply
    overwrite it. Instead the two are AND-combined, existing expression
    first, new one parenthesised: "existing and (new)". This makes repeated
    calls (e.g. once per algorithmic phase, if a caller preferred that over
    passing every phase's steps at once) accumulate rather than clobber each
    other.

    @note peano4::grid::LoadStoreComputeFlag is a plain enum class with no
    operator&& (or any other combining function) defined anywhere in the
    C++ sources as of this writing - an "existing and (new)" expression
    built from two such enum-valued sub-expressions will not type-check
    until such an operator is added on the C++ side. This function produces
    exactly that string regardless, since the string-level behaviour is
    what was asked for; the missing operator is a separate, outstanding
    C++-side gap.

    @param steps: [peano4.api.solversteps.Step]
      Every compiled Step to consider, across every algorithmic phase (e.g.
      setup_simulation_steps + time_step_steps + plot_steps) - has to be
      the full set the surrounding project will actually run, so every
      submodel's predicates see every Step that might touch it.

    """
    for attribute_name, access_attribute_name, guard_routine_name in (
        ("cell_data", "cell_data_access", "createCellGuard"),
        ("face_data", "face_data_access", "createFaceGuard"),
        ("vertex_data", "vertex_data_access", "createVertexGuard"),
    ):
        _derive_load_store_compute_flag_for_one_kind(steps, attribute_name, access_attribute_name, guard_routine_name)


def _step_active_expression(step: "peano4.api.solversteps.Step") -> str:
    return (
        "peano4::parallel::Node::getInstance().getCurrentProgramStep() == "
        f"repositories::StepRepository::toProgramStep(repositories::StepRepository::Steps::{step.name})"
    )


# Every expression _step_active_expression() builds references
# repositories::StepRepository::toProgramStep/::Steps - repositories/StepRepository.h
# is what actually declares those (confirmed against a generated
# repositories/StepRepository.h: repositories/SolverRepository.h does not
# include it and declares no Steps-related symbol at all). The second
# include is not load-bearing for this particular expression, but every
# other place in this codebase that references repositories::StepRepository
# (@see exahype2.api.generators.DefaultSimulationMain,
# mghype.matrixfree.api.MatrixFreeMain.template.cpp,
# mghype.petsc.api.PETScMain.template.cpp) pairs it with
# repositories/SolverRepository.h, so this follows that same convention.
# The same expression also calls peano4::parallel::Node::getInstance(), so
# peano4/parallel/Node.h (@see src/peano4/parallel/Node.h) is required too.
_STEP_REPOSITORY_INCLUDES = (
    '#include "repositories/StepRepository.h"\n',
    '#include "repositories/SolverRepository.h"\n',
    '#include "peano4/parallel/Node.h"\n',
)


def _load_store_compute_flag_holder(submodel):
    """!

    The object holding the submodel's load_store_compute_flag: The
    peano4_mpi_and_storage_aspect for DaStGen2 data, the generator otherwise.

    """
    aspect = getattr(submodel, "peano4_mpi_and_storage_aspect", None)
    return submodel.generator if aspect is None else aspect


def _ensure_step_repository_includes(holder) -> None:
    attribute_name = "implementation_file_includes" if hasattr(holder, "implementation_file_includes") else "includes"
    for include in _STEP_REPOSITORY_INCLUDES:
        if include not in getattr(holder, attribute_name):
            setattr(holder, attribute_name, getattr(holder, attribute_name) + include)


def _derive_load_store_compute_flag_for_one_kind(
    steps: List["peano4.api.solversteps.Step"],
    attribute_name: str,
    access_attribute_name: str,
    guard_routine_name: str,
) -> None:
    use_terms: Dict["peano4.api.datamodel.DoF", List[str]] = {}
    load_terms: Dict["peano4.api.datamodel.DoF", List[str]] = {}
    store_terms: Dict["peano4.api.datamodel.DoF", List[str]] = {}

    for step in steps:
        access = getattr(step, access_attribute_name)
        for submodel in getattr(step, attribute_name):
            expression = _step_active_expression(step)
            use_terms.setdefault(submodel, []).append(expression)

            qualifier = access[submodel]
            if qualifier in (Task.DataAccess.READ_AND_DISCARD, Task.DataAccess.READ_AND_WRITE):
                load_terms.setdefault(submodel, []).append(expression)
            if qualifier in (Task.DataAccess.WRITE, Task.DataAccess.READ_AND_WRITE):
                store_terms.setdefault(submodel, []).append(expression)
            # TEMPORARY contributes to use_terms only - see docstring.

    for submodel, terms in use_terms.items():
        use_expression = " or ".join(terms)
        load_expression = " or ".join(load_terms.get(submodel, [])) or "false"
        store_expression = " or ".join(store_terms.get(submodel, [])) or "false"

        # getattr: submodels need not be peano4.api.datamodel.DoF instances
        grid_selector = getattr(submodel, "grid_selector", None)
        if grid_selector is not None:
            guard = getattr(grid_selector, guard_routine_name)()
            use_expression = f"({guard}) and ({use_expression})"
            load_expression = f"({guard}) and ({load_expression})"
            store_expression = f"({guard}) and ({store_expression})"
        new_flag = (
            f"::peano4::grid::constructLoadStoreComputeFlag({use_expression}, {load_expression}, {store_expression})"
        )

        holder = _load_store_compute_flag_holder(submodel)
        existing_flag = holder.load_store_compute_flag
        if existing_flag is not None and existing_flag != _DEFAULT_LOAD_STORE_COMPUTE_FLAG:
            # Something (a task graph author, or an earlier call to this
            # function) already customised this submodel's flag - do not
            # discard that, combine with it instead.
            holder.load_store_compute_flag = f"{existing_flag} and ({new_flag})"
        else:
            holder.load_store_compute_flag = new_flag

        _ensure_step_repository_includes(holder)
