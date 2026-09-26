from peano4.api.solversteps import Step
from peano4.api.taskgraph import Task
from peano4.api.taskgraph.compiler.DeriveLoadStoreComputeFlag import (
    _DEFAULT_LOAD_STORE_COMPUTE_FLAG,
    derive_load_store_compute_flag,
)


READ_AND_DISCARD = Task.DataAccess.READ_AND_DISCARD
WRITE = Task.DataAccess.WRITE
READ_AND_WRITE = Task.DataAccess.READ_AND_WRITE
TEMPORARY = Task.DataAccess.TEMPORARY


class _FakeGenerator:
    """Minimal stand-in for peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer:
    derive_load_store_compute_flag() only ever needs somewhere to write
    load_store_compute_flag to."""

    def __init__(self):
        self.load_store_compute_flag = None
        self.includes = ""


class _FakeSubmodel:
    """Minimal stand-in for a peano4.api.datamodel.Patch, with a generator
    attribute like the real thing has."""

    def __init__(self, name):
        self.name = name
        self.generator = _FakeGenerator()

    def __repr__(self):
        return self.name


def _make_step(name):
    return Step(name=name, add_user_defined_actions=False)


def _use_cell(step, submodel, qualifier):
    step.use_cell(submodel)
    step.cell_data_access[submodel] = qualifier


def _use_face(step, submodel, qualifier):
    step.use_face(submodel)
    step.face_data_access[submodel] = qualifier


def _use_vertex(step, submodel, qualifier):
    step.use_vertex(submodel)
    step.vertex_data_access[submodel] = qualifier


def _active(step) -> str:
    """The exact "this Step is active" expression, for asserting against -
    built the same way the production code builds it, so these tests break
    if that expression's shape ever changes."""
    return (
        "peano4::parallel::Node::getInstance().getCurrentProgramStep() == "
        f"repositories::StepRepository::toProgramStep(repositories::StepRepository::Steps::{step.name})"
    )


def _flag(use, load, store) -> str:
    return f"::peano4::grid::constructLoadStoreComputeFlag({use}, {load}, {store})"


def test_read_and_discard_only_contributes_to_use_and_load():
    step = _make_step("TimeStep_Reconstruction")
    submodel = _FakeSubmodel("Q")
    _use_cell(step, submodel, READ_AND_DISCARD)

    derive_load_store_compute_flag([step])

    active = _active(step)
    assert submodel.generator.load_store_compute_flag == _flag(active, active, "false")


def test_write_only_contributes_to_use_and_store():
    step = _make_step("Setup_InitialCondition")
    submodel = _FakeSubmodel("Q")
    _use_cell(step, submodel, WRITE)

    derive_load_store_compute_flag([step])

    active = _active(step)
    assert submodel.generator.load_store_compute_flag == _flag(active, "false", active)


def test_read_and_write_contributes_to_all_three():
    step = _make_step("TimeStep_UpdateCell")
    submodel = _FakeSubmodel("Q")
    _use_cell(step, submodel, READ_AND_WRITE)

    derive_load_store_compute_flag([step])

    active = _active(step)
    assert submodel.generator.load_store_compute_flag == _flag(active, active, active)


def test_temporary_contributes_only_to_use():
    step = _make_step("TimeStep_Relay")
    submodel = _FakeSubmodel("Q")
    _use_cell(step, submodel, TEMPORARY)

    derive_load_store_compute_flag([step])

    active = _active(step)
    assert submodel.generator.load_store_compute_flag == _flag(active, "false", "false")


def test_terms_from_different_steps_are_ored_together():
    """One Step reads, a different Step (elsewhere) writes the same
    submodel: use has both, load only the reader's term, store only the
    writer's term."""
    reader = _make_step("TimeStep_Reader")
    writer = _make_step("TimeStep_Writer")
    submodel = _FakeSubmodel("Q")
    _use_cell(reader, submodel, READ_AND_DISCARD)
    _use_cell(writer, submodel, WRITE)

    derive_load_store_compute_flag([reader, writer])

    active_reader = _active(reader)
    active_writer = _active(writer)
    assert submodel.generator.load_store_compute_flag == _flag(
        f"{active_reader} or {active_writer}", active_reader, active_writer
    )


def test_terms_across_different_phases_use_each_steps_own_name():
    writer = _make_step("SetupSimulation_InitialCondition")
    reader = _make_step("TimeStep_Reconstruction")
    submodel = _FakeSubmodel("Q")
    _use_cell(writer, submodel, WRITE)
    _use_cell(reader, submodel, READ_AND_DISCARD)

    derive_load_store_compute_flag([writer, reader])

    active_writer = _active(writer)
    active_reader = _active(reader)
    assert submodel.generator.load_store_compute_flag == _flag(
        f"{active_writer} or {active_reader}", active_reader, active_writer
    )


def test_cell_face_and_vertex_are_derived_independently():
    step = _make_step("TimeStep_Step")
    cell_submodel = _FakeSubmodel("Cell")
    face_submodel = _FakeSubmodel("Face")
    vertex_submodel = _FakeSubmodel("Vertex")
    _use_cell(step, cell_submodel, WRITE)
    _use_face(step, face_submodel, READ_AND_DISCARD)
    _use_vertex(step, vertex_submodel, READ_AND_WRITE)

    derive_load_store_compute_flag([step])

    active = _active(step)
    assert cell_submodel.generator.load_store_compute_flag == _flag(active, "false", active)
    assert face_submodel.generator.load_store_compute_flag == _flag(active, active, "false")
    assert vertex_submodel.generator.load_store_compute_flag == _flag(active, active, active)


def test_submodel_never_used_is_left_untouched():
    step = _make_step("TimeStep_Step")
    used_submodel = _FakeSubmodel("Used")
    unused_submodel = _FakeSubmodel("Unused")
    _use_cell(step, used_submodel, WRITE)

    derive_load_store_compute_flag([step])

    assert used_submodel.generator.load_store_compute_flag is not None
    assert unused_submodel.generator.load_store_compute_flag is None


def test_factory_default_is_overwritten_not_combined():
    """A submodel's generator starts out at the factory default (never
    customised by anyone) - the derived flag should replace it outright,
    not get AND-combined with it."""
    step = _make_step("TimeStep_Step")
    submodel = _FakeSubmodel("Q")
    submodel.generator.load_store_compute_flag = _DEFAULT_LOAD_STORE_COMPUTE_FLAG
    _use_cell(step, submodel, WRITE)

    derive_load_store_compute_flag([step])

    active = _active(step)
    assert submodel.generator.load_store_compute_flag == _flag(active, "false", active)


def test_pre_existing_custom_expression_is_combined_with_and():
    """A task graph author (or an earlier call to this function) already
    set a custom load_store_compute_flag - the newly derived expression
    must be AND-combined with it, existing first, new one parenthesised."""
    step = _make_step("TimeStep_Step")
    submodel = _FakeSubmodel("Q")
    submodel.generator.load_store_compute_flag = "someCustomExpression"
    _use_cell(step, submodel, WRITE)

    derive_load_store_compute_flag([step])

    active = _active(step)
    new_flag = _flag(active, "false", active)
    assert submodel.generator.load_store_compute_flag == f"someCustomExpression and ({new_flag})"


def test_touched_submodel_gets_step_repository_includes():
    step = _make_step("TimeStep_Step")
    submodel = _FakeSubmodel("Q")
    _use_cell(step, submodel, WRITE)

    derive_load_store_compute_flag([step])

    assert '#include "repositories/StepRepository.h"' in submodel.generator.includes
    assert '#include "repositories/SolverRepository.h"' in submodel.generator.includes
    assert '#include "peano4/parallel/Node.h"' in submodel.generator.includes


def test_untouched_submodel_gets_no_includes():
    step = _make_step("TimeStep_Step")
    used_submodel = _FakeSubmodel("Used")
    unused_submodel = _FakeSubmodel("Unused")
    _use_cell(step, used_submodel, WRITE)

    derive_load_store_compute_flag([step])

    assert unused_submodel.generator.includes == ""


def test_includes_are_not_duplicated_on_repeated_calls():
    step = _make_step("TimeStep_Step")
    submodel = _FakeSubmodel("Q")
    _use_cell(step, submodel, WRITE)

    derive_load_store_compute_flag([step])
    derive_load_store_compute_flag([step])

    assert submodel.generator.includes.count('#include "repositories/StepRepository.h"') == 1
    assert submodel.generator.includes.count('#include "repositories/SolverRepository.h"') == 1
    assert submodel.generator.includes.count('#include "peano4/parallel/Node.h"') == 1
