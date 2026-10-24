# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

import abc
from typing import Callable, Dict, List, Tuple, Union

import peano4.api
import peano4.api.toolbox

from .PDETerm import PDETerm

VariableSpecification = Union[int, Dict[str, int]]


class Solver(abc.ABC):
    """!

    Abstract base class for all ExaHyPE 2 solvers

    Every discretisation ExaHyPE 2 ships (Finite Volumes, ADER-DG, Runge-Kutta
    Discontinuous Galerkin, Runge-Kutta Finite Differences, ...) is, from the
    project's point of view, a black box that has to answer a small set of
    questions: what data does it need on the mesh, which action sets realise
    its algorithmic steps, and how does it plug into the code generation. This
    class holds exactly that common contract plus the bits of bookkeeping
    (name, includes, solver constants, the unknowns/auxiliary_variables
    variable-shortcut logic) that every concrete solver has historically
    reimplemented identically. It does not know anything about a concrete
    discretisation - it neither creates the actual mesh data structures nor
    the actual action sets, as this remains solver-specific and thus has to be
    added by all subclasses.

    A concrete solver typically extends the constructor with its own
    discretisation-specific parameters (patch size, polynomial order, time
    stepping scheme, ...), calls `Solver.__init__()` for the shared part, and
    then implements the abstract methods below.

    @see exahype2.api.Project

    """

    class SolverGenerator:
        """!

        Turns a Solver into actual classes.

        """

        def __init__(self, solver: "Solver"):
            self._solver = solver

        @abc.abstractmethod
        def construct_output(self, exahype2_project, peano_project):
            """!
            Has to be implemented by subclass
            """

    class NumericalScheme(abc.ABC):
        """!

        The concrete discretisation (Finite Volumes, ADER-DG, ...) realised
        by a Solver. Abstract - concrete solvers have to provide a subclass
        that implements construct_output().

        """

        def __init__(self, solver: "Solver"):
            self._solver = solver

        @abc.abstractmethod
        def construct_output(self, exahype2_project, peano_project, time_step):
            """!
            Has to be implemented by subclass

            time_step: peano4.api.taskgraph.Graph
              The task graph of the surrounding exahype2_project's time
              stepping phase, so the numerical scheme can add its own tasks
              into it.
            """

    class GridConstruction(abc.ABC):
        """!

        How a Solver drives the construction of the mesh it runs on.
        Abstract - concrete solvers have to provide a subclass that
        implements construct_output().

        """

        def __init__(self, solver: "Solver"):
            self._solver = solver

        @abc.abstractmethod
        def construct_output(self, exahype2_project, peano_project, setup_simulation):
            """!
            Has to be implemented by subclass

            setup_simulation: peano4.api.taskgraph.Graph
              The task graph of the surrounding exahype2_project's setup
              phase, so the grid construction can add its own tasks into it.
            """

    class Plotter(abc.ABC):
        """!

        How a Solver writes out its solution. Abstract - concrete solvers
        have to provide a subclass that implements construct_output().

        Each plotter decides itself when to plot. The plotter has to give
        its solver's C++ class a method

        ~~~~~~~~~~~~~~~~~~~~~~~
        bool wantsToPlot() const;
        ~~~~~~~~~~~~~~~~~~~~~~~

        (@see Solver.add_abstract_solver_method()). The generated main asks
        all solvers with a plotter (repositories::wantsToPlot()) and runs
        the plot phase if any of them wants to plot. Within the plot phase,
        each plotter then checks its own solver's wantsToPlot() and only
        writes output if it is true. This way, every solver can have its own
        plotter with its own settings - @see exahype2.api.fv.PlotAsVTKFile.

        """

        def __init__(self, solver: "Solver"):
            self._solver = solver

        @abc.abstractmethod
        def construct_output(self, exahype2_project, peano_project, plot):
            """!
            Has to be implemented by subclass

            plot: peano4.api.taskgraph.Graph
              The task graph of the surrounding exahype2_project's plotting
              phase, so the plotter can add its own tasks into it.
            """

    class SolutionUpdateTaskTag:
        """!

        Tag of a task which writes a new solution

        Every numerical scheme tags each task of its time stepping task
        graph which produces a new solution, and, for Runge-Kutta schemes,
        each task which produces the input of a further Runge-Kutta stage.
        The tag's key is the class SolutionUpdateTaskTag itself, its value an
        instance of this class:

        ~~~~~~~~~~~~~~~~~~~~~~~
        task.tags[Solver.SolutionUpdateTaskTag] = Solver.SolutionUpdateTaskTag(solver, patch, guard)
        ~~~~~~~~~~~~~~~~~~~~~~~

        Code which wants to postprocess the solution, e.g. to enforce
        constraints, searches for these tags via
        peano4.api.taskgraph.Graph.find_tasks() and inserts its own tasks
        after the tagged ones (@ref Solver.postprocess_task_graphs()). The
        tags hence are the contract between the numerical schemes and any
        postprocessing: A scheme has to tag all of its solution writers, and
        it must not read the outcome of a writer within the writer's own
        task, i.e. it has to project the solution onto the faces or compute
        eigenvalues from it in a subsequent task.

        """

        def __init__(self, solver: "Solver", patch, guard: str, guard_data=None):
            """!

            @param solver: exahype2.api.Solver
              The solver whose solution the tagged task writes.

            @param patch: peano4.api.datamodel.Patch
              The cell data the tagged task writes the solution into, i.e.
              the solution patch or a Runge-Kutta stage input.

            @param guard: str
              C++ boolean expression which selects the cells the tagged task
              updates. Postprocessing should use the same guard. The guard
              has to evaluate to the same value before and after the update.

            @param guard_data: List[peano4.api.datamodel.DoF]
              Cell data besides the patch which the guard reads.
              Postprocessing tasks have to declare these data, too, as the
              generated code would not see them otherwise.

            """
            self.solver = solver
            self.patch = patch
            self.guard = guard
            self.guard_data = list(guard_data) if guard_data is not None else []

    def __init__(
        self,
        name: str,
        unknowns: VariableSpecification,
        auxiliary_variables: VariableSpecification,
    ):
        """!

        Create solver

        name: str
          A unique name for the solver. This one is used for all generated
          classes. The generated C++ object instance also incorporates this
          name.

        unknowns: int or dict
          Number of unknowns per mesh entry (Finite Volume voxel, DG degree of
          freedom, ...). Eventually, unknowns and auxiliary_variables are
          merged into one big vector, but the solver still has to be able to
          distinguish them, as only the unknowns are subject to the actual PDE
          formulation. Can be specified either as a plain int, or as a dict
          mapping variable name onto multiplicity (e.g.
          `{"rho": 1, "j": 3}` for a scalar density plus a 3d momentum). If
          dicts are used, the generated Abstract<SolverName> class hosts a
          nested struct Variables with one constant per name, so user code
          can write Q[Variables::j+1] instead of having to remember the
          offset into the unknowns array by hand.

        auxiliary_variables: int or dict
          Same semantics as unknowns, but for variables that may be
          space-dependent yet do not change over time. Their names follow
          the unknowns' names, as the auxiliary variables follow the
          unknowns in Q.

        If names are used at all, they have to cover all unknowns and all
        auxiliary variables, i.e. you cannot pass a dict for one and a
        non-zero int for the other.

        The variables cannot be changed after construction: the numerical
        scheme sizes its data structures from them as soon as it is assigned
        to the solver.

        """
        self._name = name

        # Offset of each named variable within Q. Empty if no names are used.
        self._variable_positions: Dict[str, int] = {}

        self._unknowns = self._register_variables(unknowns, 0)
        self._auxiliary_variables = self._register_variables(
            auxiliary_variables, self._unknowns
        )

        named_entries = sum(
            sum(variables.values())
            for variables in (unknowns, auxiliary_variables)
            if isinstance(variables, dict)
        )
        if self._variable_positions and named_entries != (
            self._unknowns + self._auxiliary_variables
        ):
            raise ValueError(
                f"Solver {name}: the variable names {list(self._variable_positions)} "
                f"cover only {named_entries} of the {self._unknowns} unknowns and "
                f"{self._auxiliary_variables} auxiliary variables. If you name "
                f"variables, name all unknowns and all auxiliary variables, i.e. "
                f"pass dicts for both (or 0 for the one that is empty)."
            )

        self._user_solver_includes = ""
        self._solver_constants = ""

        self._pde_terms: Dict[Tuple[str, bool], PDETerm] = {}

        # Point-wise postprocessing steps as (name, implementation, includes)
        # triples, @see add_point_wise_postprocessing()
        self._point_wise_postprocessing: List[Tuple[str, str, str]] = []

        # Raw C++ member declarations/methods to splice into the generated
        # Abstract<SolverName> class, on top of the PDE terms above -
        # nothing here by default, @see add_abstract_solver_attribute()/
        # add_abstract_solver_method().
        self._abstract_solver_attributes: List[str] = []
        self._abstract_solver_methods: List[Tuple[str, Callable[[str], str]]] = []

        # Deferred/local imports: these classes all subclass either one of
        # the Solver.* nested classes above or a class from a sibling
        # exahype2 subpackage, and some of those in turn import Solver at
        # their own module top-level. Importing them here too (at Solver.py's
        # top level) would be a genuine circular import, since this module's
        # own import of them would then run before class Solver is even
        # defined.
        from .SolverToCPPFiles import SolverToCPPFiles

        self.solver_generator = SolverToCPPFiles(self)

        # NumericalScheme has no default realisation - concrete solver
        # subclasses have to assign their own subclass instance to it,
        # through the numerical_scheme property below.
        self._numerical_scheme: "Solver.NumericalScheme" = None

        # GridConstruction has no default realisation - concrete solver
        # subclasses (or their users) have to assign their own
        # Solver.GridConstruction subclass instance to it, e.g.
        # exahype2.api.fv.SetupSimulationOverRegularGrid, through the
        # grid_construction attribute below.
        self.grid_construction: "Solver.GridConstruction" = None

        # Plotter has no default realisation - concrete solver subclasses
        # (or their users) have to assign their own Solver.Plotter subclass
        # instance to it, e.g. exahype2.api.fv.PlotAsVTKFile, through the
        # plotter attribute below.
        self.plotter: "Solver.Plotter" = None

        # Selects the mesh entities on which all action sets of this solver
        # act: Each action set's guard is the corresponding output of this
        # selector. By default, the solver works on the finest mesh level
        # only. Users can assign any other peano4.api.toolbox.GridSelector.
        self.grid_selector: peano4.api.toolbox.GridSelector = peano4.api.toolbox.FineGridSelector()

        # The kernel function calls are generated as late as possible, to
        # ensure that all PDE terms and the numerical scheme have been
        # specified
        self.compute_kernel_call: str = None

    def _register_variables(self, variables: VariableSpecification, offset: int) -> int:
        """!

        Translate a plain int or a name-to-multiplicity dict into a total
        variable count, and - for the dict case - add each name's position
        within Q to _variable_positions. Called once for unknowns (offset 0)
        and once for auxiliary_variables (offset = number of unknowns), so
        the two end up in one contiguous name/offset table.

        """
        if isinstance(variables, dict):
            for variable_name, multiplicity in variables.items():
                if variable_name in self._variable_positions:
                    raise ValueError(
                        f"Solver {self._name}: variable name {variable_name} is used twice"
                    )
                self._variable_positions[variable_name] = offset
                offset += multiplicity
            return sum(variables.values())
        elif isinstance(variables, int):
            return variables
        else:
            raise TypeError(
                "unknowns/auxiliary_variables have to be an int or a dict "
                "mapping variable name onto multiplicity, but got "
                f"{type(variables)}"
            )

    def __str__(self) -> str:
        return (
            "Name:              " + self._name + "\n"
            "Type:              " + self.__class__.__module__ + "\n"
            "Unknowns:          " + str(self._unknowns) + "\n"
            "Auxiliary vars:    " + str(self._auxiliary_variables)
        )

    @property
    def name(self) -> str:
        """!
        Unique solver name. Feeds into all generated class and instance names.
        """
        return self._name

    @property
    def numerical_scheme(self) -> "Solver.NumericalScheme":
        return self._numerical_scheme

    @numerical_scheme.setter
    def numerical_scheme(self, value: "Solver.NumericalScheme"):
        if not isinstance(value, Solver.NumericalScheme):
            raise TypeError(
                "numerical_scheme has to be a Solver.NumericalScheme, but got "
                f"{type(value)}"
            )
        self._numerical_scheme = value

    @property
    def unknowns(self) -> int:
        return self._unknowns

    @property
    def auxiliary_variables(self) -> int:
        return self._auxiliary_variables

    @property
    def user_solver_includes(self) -> str:
        """!
        Add further includes to this property, if your solver requires some
        additional routines from other header files.
        """
        return self._user_solver_includes

    def add_user_solver_includes(self, value: str):
        """!
        Add further includes to this property, if your solver requires some
        additional routines from other header files.
        """
        self._user_solver_includes += value

    def _get_default_includes(self) -> str:
        """!
        Includes every generated action set can rely on being available,
        regardless of which action sets a concrete discretisation (e.g.
        exahype2.api.fv) actually attaches - @see
        exahype2.api.fv.actionsets.AbstractFVActionSet.get_includes().
        """
        return """
#include "tarch/la/Vector.h"

#include "peano4/utils/Globals.h"
#include "peano4/utils/Loop.h"

#include "repositories/SolverRepository.h"
"""

    def set_solver_constants(self, datastring: str):
        self._solver_constants = datastring

    def add_solver_constants(self, datastring: str):
        self._solver_constants += datastring + "\n"

    def _unknown_identifier(self) -> str:
        return self._name + "Q"

    def variable_names_per_entry(self) -> List[str]:
        """!

        One name per entry of Q, i.e. for the unknowns followed by the
        auxiliary variables, or the empty list if the variables carry no
        names. A variable with multiplicity one keeps its name, whereas a
        variable v with multiplicity k yields the names v[0], ..., v[k-1],
        i.e. the entries Q[Variables::v+0], ..., Q[Variables::v+k-1].
        Plotters use these names to label the entries in their output.

        """
        positions = list(self._variable_positions.items())
        names = []
        for i, (variable_name, position) in enumerate(positions):
            next_position = (
                positions[i + 1][1]
                if i + 1 < len(positions)
                else self._unknowns + self._auxiliary_variables
            )
            multiplicity = next_position - position
            if multiplicity == 1:
                names.append(variable_name)
            else:
                names += [f"{variable_name}[{j}]" for j in range(multiplicity)]
        return names

    def get_name_of_global_instance(self) -> str:
        return "instanceOf" + self._name

    @property
    def pde_terms(self) -> List[PDETerm]:
        """!
        All PDE terms (flux, eigenvalues, ...) registered with this solver,
        in registration order. A given name can appear at most twice - once
        stateless, once not, @see add_pde_term() - since each generates a
        different C++ method. Read-only - use add_pde_term() to add one and
        remove_pde_term() to take one away again.
        """
        return list(self._pde_terms.values())

    @property
    def requires_user_class(self) -> bool:
        """!
        True iff at least one PDE term has no implementation. Only then
        does the user have to provide one, i.e. only then is a
        <SolverName> class derived from Abstract<SolverName> generated
        and added to the Makefile (@see exahype2.api.SolverToCPPFiles).
        Otherwise, Abstract<SolverName> is complete and the solver
        repository instantiates it directly. Stateless terms always carry
        an implementation (@see PDETerm), so they never require one.
        """
        return any(term.implementation is None for term in self.pde_terms)

    def get_name_of_solver_class(self) -> str:
        """!
        Name of the C++ class the solver repository instantiates:
        <SolverName> if requires_user_class, Abstract<SolverName> otherwise.
        """
        return self._name if self.requires_user_class else "Abstract" + self._name

    def add_pde_term(self, term: PDETerm):
        """!

        Register a PDE term with this solver, or replace the one already
        registered under the same name and statelessness. A solver
        aggregates an arbitrary number of terms - the well-known ones
        (Flux(), MaxEigenvalue(), ...) as well as new, solver-specific ones
        users construct themselves via PDETerm(name=..., implementation=...).

        Each term generates exactly one C++ method: the static,
        GPU-offloadable overload if term.stateless, otherwise the virtual
        instance method (@see PDETerm.Generator.generate_declaration()),
        never both. To get both variants of the same conceptual operation
        (e.g. a stateless flux for GPU kernels and a stateful one for
        everything else), add the term twice - once with stateless=True,
        once without - which is why terms are keyed by (name, stateless)
        rather than by name alone.

        """
        if not isinstance(term, PDETerm):
            raise TypeError(f"term has to be a PDETerm, but got {type(term)}")
        self._pde_terms[(term.name, term.stateless)] = term

    def remove_pde_term(self, name: str, stateless: bool = False):
        """! Remove a previously registered PDE term by name and statelessness."""
        del self._pde_terms[(name, stateless)]

    def get_pde_term(self, name: str, stateless: bool = False) -> PDETerm:
        """! Look up a previously registered PDE term by name and statelessness."""
        return self._pde_terms[(name, stateless)]

    def add_point_wise_postprocessing(
        self, name: str, implementation: str, additional_includes: str = ""
    ):
        """!

        Postprocess every new solution point-wisely

        After the numerical scheme has written a new solution, or, for
        Runge-Kutta schemes, the input of a further Runge-Kutta stage, the
        solver runs implementation for each volume (or grid point) of each
        patch. The snippet may read and write Q, i.e. all unknowns and
        auxiliary variables of one volume, and read its position x, @see
        toolbox.blockstructured.api.ApplyPointWiselyToPatch.

        The postprocessing becomes a task of its own in the time stepping
        task graph, which runs directly after each task that writes a new
        solution. @see postprocess_task_graphs() for the mechanism.

        @param name: str
          Name of the step. The task names read
          "{solver}_{name} after {task}".

        @param implementation: str
          C++ snippet which is applied to each volume.

        @param additional_includes: str
          Includes that the snippet needs.

        """
        self._point_wise_postprocessing.append(
            (name, implementation, additional_includes)
        )

    def postprocess_task_graphs(
        self,
        exahype2_project,
        peano_project,
        setup_simulation,
        time_step,
        plot,
    ):
        """!

        Alter the task graphs after they have been created, but before they are compiled

        generate_solver_realisation() calls this routine after the grid
        construction, the numerical scheme and the plotter have added their
        tasks to the three task graphs. Afterwards, the project hands the
        graphs over to the task graph compiler. Subclasses can override the
        routine to add further tasks or to modify the graphs. In this case,
        they should call the superclass' implementation, too.

        The default implementation inserts the point-wise postprocessing
        steps (@ref add_point_wise_postprocessing()) into the time stepping
        task graph. For each task tagged with Solver.SolutionUpdateTaskTag of this
        solver, and for each postprocessing step, it creates a new task and
        inserts it directly after the tagged task (@ref
        peano4.api.taskgraph.Graph.insert_after()). A Runge-Kutta scheme
        tags the writer of each stage, so the postprocessing runs after each
        Runge-Kutta stage.

        @param setup_simulation, time_step, plot: peano4.api.taskgraph.Graph
          The task graphs of the three algorithmic phases.

        """
        if not self._point_wise_postprocessing:
            return

        import toolbox.blockstructured.api

        includes = self._get_default_includes() + self.user_solver_includes
        anchors = [
            task
            for task in time_step.find_tasks(Solver.SolutionUpdateTaskTag)
            if task.tags[Solver.SolutionUpdateTaskTag].solver is self
        ]
        assert anchors, (
            f"Solver {self._name} has point-wise postprocessing, but its numerical "
            "scheme does not tag any task with exahype2.api.Solver.SolutionUpdateTaskTag"
        )
        for anchor in anchors:
            update = anchor.tags[Solver.SolutionUpdateTaskTag]
            anchor_name = anchor.name
            if anchor_name.startswith(self._name + "_"):
                anchor_name = anchor_name[len(self._name) + 1 :]
            for name, implementation, additional_includes in self._point_wise_postprocessing:
                task = peano4.api.taskgraph.Task(
                    name=f"{self._name}_{name} after {anchor_name}",
                    entity=peano4.api.taskgraph.Task.GridEntity.CELL,
                )
                task.use_cell(update.patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)
                ## READ_AND_WRITE keeps the data alive for later tasks
                for data in update.guard_data:
                    task.use_cell(data, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)
                task.add_action_set(
                    toolbox.blockstructured.api.ApplyPointWiselyToPatch(
                        patch=update.patch,
                        implementation=implementation,
                        guard=update.guard,
                        additional_includes=includes + additional_includes,
                    )
                )
                time_step.insert_after(anchor, task)
                anchor = task

        if peano4.api.Logger().is_visualisation:
            time_step.visualise(title=f"Time stepping task graph for {self._name} with postprocessing")
        if peano4.api.Logger().is_text:
            time_step.log_as_text(title=f"Time stepping task graph for {self._name} with postprocessing")

    def add_abstract_solver_attribute(self, declaration: str):
        """!

        Add a raw C++ member attribute declaration to the generated
        Abstract<SolverName> class (@see exahype2.api.SolverToCPPFiles),
        e.g. "double _timeStamp = 0.0;" - use an in-class initialiser if
        the attribute needs one, since there is no hook into the generated
        constructor for this.

        Nothing calls this unconditionally: a plain Solver carries no
        attributes beyond what PDE terms need. It is up to whichever
        NumericalScheme/GridConstruction/Plotter a solver uses to call
        this, if it needs solver-level state of its own that genuinely
        has no home on exahype2::Solver already - e.g.
        exahype2.api.fv.RusanovWithExplicitEuler deliberately does *not*
        use this for time stamp/time step size bookkeeping, since
        ::exahype2::Solver (which every generated Abstract<SolverName>
        already inherits from) already carries
        _minTimeStamp/_maxTimeStamp/_minTimeStepSize/_maxTimeStepSize and
        the updateCell()/startTimeStep()/finishTimeStep() lifecycle to
        maintain them - @see exahype2.api.fv.actionsets.
        ComputeGlobalCFLCondition, which drives that
        lifecycle instead of introducing parallel state via this method.

        """
        self._abstract_solver_attributes.append(declaration)

    def add_abstract_solver_method(
        self, declaration: str, definition_generator: Callable[[str], str]
    ):
        """!

        Add a raw C++ method to the generated Abstract<SolverName> class:
        declaration goes into the class body (public section), verbatim.
        definition_generator is called with the class' fully qualified
        name once it is known (@see exahype2.api.SolverToCPPFiles,
        exactly the way PDETerm.Generator.generate_out_of_class_definitions()
        already is) and has to return the out-of-class definition - a
        plain string with a "{qualified_class_name}" placeholder is
        deliberately not supported here, since a C++ method body is full
        of literal braces of its own; build the string with concatenation
        instead, e.g.:

        ~~~~~~~~~~~~~~~~~~~~~~~
        solver.add_abstract_solver_method(
          "void increaseTimestamp(double dt);",
          lambda qualified_class_name:
            "void " + qualified_class_name + "::increaseTimestamp(double dt) {\\n"
            "  _timeStamp += dt;\\n"
            "}\\n"
        )
        ~~~~~~~~~~~~~~~~~~~~~~~

        @see add_abstract_solver_attribute() for the attribute counterpart,
        and exahype2.api.PDETerm.Generator for the more structured
        alternative this deliberately is not (that one is for PDE terms a
        user implements; this one is for solver-level bookkeeping a
        NumericalScheme/etc. needs that has no equivalent on
        ::exahype2::Solver already - @see add_abstract_solver_attribute()'s
        own note on why RusanovWithExplicitEuler does not actually use
        either method for time stamp/time step size bookkeeping).

        """
        self._abstract_solver_methods.append((declaration, definition_generator))

    # @todo This one is important
    def create_readme_descriptor(self, domain_offset, domain_size) -> str:
        """!
        Return a human-readable description of this solver instance, to be
        embedded into the generated project's README.
        """
        pass

    def generate_solver_realisation(
        self,
        exahype2_project,
        peano_project,
        setup_simulation,
        time_step,
        plot,
    ):
        """!
        Turn this solver specification into a task graph and create the required C++ kernels/stubs

        peano_project: peano4.api.Project
          The Peano 4 project that exahype2_project.generate_Peano4_project() is
          currently assembling. It is a local variable there, not an attribute -
          merely passed through here, so this solver can add its own artefacts to it.

        setup_simulation, time_step, plot: peano4.api.taskgraph.Graph
          The task graphs of the three algorithmic phases of the surrounding
          exahype2_project. They are owned by exahype2_project.generate_Peano4_project()
          and merely passed through here, so this solver can add its own
          tasks into them.

        @return peano4.api.taskgraph.Graph
        """
        peano4.api.Logger().new_section("Solver " + self._name)

        assert self.grid_construction is not None, (
            f"Solver {self._name} has no grid_construction set. Assign a "
            "Solver.GridConstruction subclass instance to grid_construction "
            "before calling generate_solver_realisation()."
        )
        assert self.numerical_scheme is not None, (
            f"Solver {self._name} has no numerical_scheme set. Assign a "
            "Solver.NumericalScheme subclass instance to numerical_scheme "
            "before calling generate_solver_realisation()."
        )

        self.grid_construction.construct_output(
            exahype2_project, peano_project, setup_simulation
        )
        self.numerical_scheme.construct_output(
            exahype2_project, peano_project, time_step
        )
        if self.plotter is not None:
            self.plotter.construct_output(exahype2_project, peano_project, plot)
        self.postprocess_task_graphs(
            exahype2_project, peano_project, setup_simulation, time_step, plot
        )
        # Last, so grid construction, numerical scheme and plotter can add
        # their own members to the solver class first (@see
        # add_abstract_solver_attribute()/add_abstract_solver_method()).
        self.solver_generator.construct_output(exahype2_project, peano_project)
