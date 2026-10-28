# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

import abc
import re
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
            """!

            @param solver: exahype2.api.Solver
              The solver which the scheme discretises.

            """
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

        def create_default_mesh_adaptation(self) -> "Solver.MeshAdaptation":
            """!

            The mesh adaptation a solver uses if the user does not assign
            one to solver.mesh_adaptation

            @return Solver.MeshAdaptation or None
              The default returns None, i.e. the solver does not contribute
              to the mesh adaptation phase.

            """
            return None

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

        A solver can have an arbitrary number of plotters
        (@ref exahype2.api.Solver.Solver.add_plotter()), e.g. one which
        writes VTK files for visualisation and one which writes
        checkpoints. Each plotter has a name which is unique per solver,
        and it decides itself when it is active. For this, it gives its
        solver's C++ class a method

        ~~~~~~~~~~~~~~~~~~~~~~~
        bool is<Name>Active() const;
        ~~~~~~~~~~~~~~~~~~~~~~~

        The solver's

        ~~~~~~~~~~~~~~~~~~~~~~~
        bool wantsToPlot() const;
        ~~~~~~~~~~~~~~~~~~~~~~~

        holds if any of its plotters is active. The generated main asks all
        solvers with a plotter (repositories::wantsToPlot()) and runs the
        plot phase if any of them wants to plot. The task graphs of all
        plotters of all solvers are added to the one plot graph, which the
        graph compiler then maps onto grid sweeps. Within the plot phase,
        every action set of a plotter is guarded by its plotter's
        is<Name>Active(), so only the active plotters do any work.

        is<Name>Active() must not change throughout the plot phase. Each
        plotter therefore calls its

        ~~~~~~~~~~~~~~~~~~~~~~~
        void finish<Name>();
        ~~~~~~~~~~~~~~~~~~~~~~~

        which moves it on to its next output, in the unprepareTraversal() of
        its last task, i.e. after all of its tasks have run.

        The class provides this bookkeeping for plotters which write output
        in regular time intervals (_add_time_interval_bookkeeping_to_solver()).

        """

        def __init__(self, solver: "Solver", name: str = None):
            """!

            @param solver: exahype2.api.Solver
              The solver whose solution is written.

            @param name: str or None
              Name of the plotter, which is unique per solver. It becomes
              part of the generated C++ method names, i.e. it has to be a
              valid C++ identifier. If None, the solver derives a name from
              the plotter's class name once the plotter is added
              (@ref exahype2.api.Solver.Solver.add_plotter()).

            """
            self._solver = solver
            if name is not None and not name.isidentifier():
                raise ValueError(f"Plotter name {name} is no valid C++ identifier")
            self.name = name

        @abc.abstractmethod
        def construct_output(self, exahype2_project, peano_project, plot):
            """!
            Has to be implemented by subclass

            plot: peano4.api.taskgraph.Graph
              The task graph of the surrounding exahype2_project's plotting
              phase, so the plotter can add its own tasks into it.
            """

        def output_file_prefix(self) -> str:
            """!

            Prefix of the files the plotter writes, including the path

            The solver uses it to ensure that no two plotters write into
            the same files. Plotters which write no files return None.

            """
            return None

        def create_is_active_predicate(self) -> str:
            """!

            C++ predicate which holds if the plotter is active

            Combine the guards of all of the plotter's action sets with it.

            """
            return (
                "repositories::"
                + self._solver.get_name_of_global_instance()
                + ".is"
                + self.name
                + "Active()"
            )

        def create_finish_call(self) -> str:
            """!

            C++ statement which moves the plotter on to its next output

            Insert it into the unprepareTraversal() of the plotter's last
            task.

            """
            return (
                "repositories::"
                + self._solver.get_name_of_global_instance()
                + ".finish"
                + self.name
                + "();\n"
            )

        def _add_time_interval_bookkeeping_to_solver(
            self, first_time_stamp: float, time_in_between: float
        ):
            """!

            Make the plotter active in regular time intervals

            Add the attribute _next<Name>TimeStamp plus the methods
            is<Name>Active() and finish<Name>() to the solver's C++ class.
            The plotter is active as soon as the solver's min time stamp has
            reached _next<Name>TimeStamp. finish<Name>() then increments
            _next<Name>TimeStamp by time_in_between.

            The solver keeps these members once added, so we add them only
            once, even if the Peano 4 project is generated multiple times.

            @param first_time_stamp: float
              Time stamp of the first output.

            @param time_in_between: float
              Time in between two outputs. Has to be positive.

            """
            attribute = (
                "double _next"
                + self.name
                + "TimeStamp = "
                + repr(float(first_time_stamp))
                + ";"
            )
            if attribute in self._solver._abstract_solver_attributes:
                return

            self._solver.add_abstract_solver_attribute(attribute)
            self._solver.add_abstract_solver_method(
                "bool is" + self.name + "Active() const;",
                lambda qualified_class_name: "bool "
                + qualified_class_name
                + "::is"
                + self.name
                + "Active() const {\n"
                "  return tarch::la::greaterEquals(getMinTimeStamp(), _next"
                + self.name
                + "TimeStamp);\n"
                "}\n",
            )
            self._solver.add_abstract_solver_method(
                "void finish" + self.name + "();",
                lambda qualified_class_name: "void "
                + qualified_class_name
                + "::finish"
                + self.name
                + "() {\n"
                "  if (is" + self.name + "Active()) {\n"
                "    _next"
                + self.name
                + "TimeStamp += "
                + repr(float(time_in_between))
                + ";\n"
                "  }\n"
                "}\n",
            )

    class MeshAdaptation(abc.ABC):
        """!

        How a Solver adapts its mesh and rebalances its load between two
        time steps. Abstract - concrete solvers have to provide a subclass
        that implements construct_output().

        The mesh adaptation is the fourth phase of an ExaHyPE 2 project
        next to the setup, the time stepping and the plotting. The
        generated main runs it right after a time step, and only if this
        time step has asked for a mesh refinement or erase, or if the load
        balancing has split recently (@ref exahype2::isMeshAdaptationRequired()).
        The phase is optional: Solvers without a MeshAdaptation do not
        contribute to it, and the main never runs an empty phase.

        The mesh adaptation does not decide where to refine or erase. The
        numerical scheme's last task of the time step collects these
        requests (@ref exahype2.api.blockstructured.DynamicMeshRefinement),
        and the mesh adaptation realises them.

        """

        def __init__(self, solver: "Solver"):
            self._solver = solver

        @abc.abstractmethod
        def construct_output(self, exahype2_project, peano_project, mesh_adaptation):
            """!
            Has to be implemented by subclass

            mesh_adaptation: peano4.api.taskgraph.Graph
              The task graph of the surrounding exahype2_project's mesh
              adaptation phase, so the mesh adaptation can add its own tasks
              into it.
            """

    ## Tag of all tasks which write a new solution
    #
    # Tags are plain strings (@ref peano4.api.taskgraph.Task.Task). Every
    # numerical scheme adds this tag to each task of its time stepping task
    # graph which writes a new solution:
    #
    # ~~~~~~~~~~~~~~~~~~~~~~~
    # task.tags.add(Solver.SolutionUpdateTaskTag)
    # ~~~~~~~~~~~~~~~~~~~~~~~
    #
    # Code which wants to postprocess the solution, typically a subclass of
    # a numerical scheme which extends the scheme's construct_output(),
    # searches for the tag via peano4.api.taskgraph.Graph.find_tasks() and
    # inserts its own tasks after the tagged ones via
    # peano4.api.taskgraph.Graph.insert_after(). The tag is a contract
    # between the numerical schemes and any such code: A scheme has to tag
    # all of its solution writers, and it must not read the outcome of a
    # writer within the writer's own task, i.e. it has to project the
    # solution onto the faces or compute eigenvalues from it in a subsequent
    # task. The tag does not say which data or cells a task writes. Code
    # which inserts tasks has to know the scheme it works with (@ref
    # page_exahype2_solver_manipulate_solver_steps).
    SolutionUpdateTaskTag = "exahype2.api.Solver.SolutionUpdateTaskTag"

    ## Tag of all tasks which write an intermediate solution
    #
    # Runge-Kutta schemes add this tag to each task which writes the input of
    # a further Runge-Kutta stage, and @ref exahype2.api.Solver.Solver.SolutionUpdateTaskTag to the task
    # which writes the new solution after the last stage. Postprocessing which
    # has to act on each stage, such as the enforcement of constraints, hence
    # searches for both tags. The contract of SolutionUpdateTaskTag applies.
    IntermediateSolutionUpdateTag = "exahype2.api.Solver.IntermediateSolutionUpdateTag"

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

        # Plotters have no default realisation - users add their own
        # Solver.Plotter subclass instances, e.g.
        # exahype2.api.fv.PlotAsVTKFile, via add_plotter().
        self._plotters: List["Solver.Plotter"] = []

        # MeshAdaptation is optional. Without one, the solver does not
        # contribute to the mesh adaptation phase. Users can assign their
        # own Solver.MeshAdaptation subclass instance to it through the
        # mesh_adaptation attribute below.
        self.mesh_adaptation: "Solver.MeshAdaptation" = None

        # Selects the mesh entities on which all action sets of this solver
        # act: Each action set's guard is the corresponding output of this
        # selector. By default, the solver works on the finest mesh level
        # only. Users can assign any other peano4.api.toolbox.GridSelector.
        self.grid_selector: peano4.api.toolbox.GridSelector = (
            peano4.api.toolbox.FineGridSelector()
        )

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
    def plotters(self) -> List["Solver.Plotter"]:
        """!
        All plotters of the solver, in the order they have been added.
        """
        return list(self._plotters)

    def add_plotter(self, plotter: "Solver.Plotter"):
        """!

        Add a plotter to the solver

        A solver can have an arbitrary number of plotters, which are
        independent of each other (@ref exahype2.api.Solver.Solver.Plotter).
        If the plotter has no name yet, it gets the name of its class,
        followed by a number if the solver has another plotter of that name
        already.

        @param plotter: exahype2.api.Solver.Plotter
          The plotter. Its solver has to be this solver.

        """
        if not isinstance(plotter, Solver.Plotter):
            raise TypeError(
                f"plotter has to be a Solver.Plotter, but got {type(plotter)}"
            )
        if plotter._solver is not self:
            raise ValueError(
                f"Plotter {type(plotter).__name__} belongs to another solver than {self._name}"
            )
        names = [p.name for p in self._plotters]
        if plotter.name is None:
            name = type(plotter).__name__
            counter = 2
            while name in names:
                name = type(plotter).__name__ + str(counter)
                counter += 1
            plotter.name = name
        elif plotter.name in names:
            raise ValueError(
                f"Solver {self._name} has a plotter named {plotter.name} already"
            )
        prefix = plotter.output_file_prefix()
        if prefix is not None and prefix in [
            p.output_file_prefix() for p in self._plotters
        ]:
            raise ValueError(
                f"Plotter {plotter.name} of solver {self._name} would write into "
                f"the files {prefix}* of another plotter. Give it another file "
                "name or output path."
            )
        self._plotters.append(plotter)

    @property
    def plotter(self) -> "Solver.Plotter":
        """!

        Shorthand for solvers with at most one plotter

        Reading it yields the solver's only plotter, or None if it has none.
        Assigning a plotter replaces all plotters of the solver by this one,
        assigning None removes all plotters. Use add_plotter() to give a
        solver more than one plotter.

        """
        if len(self._plotters) > 1:
            raise RuntimeError(
                f"Solver {self._name} has {len(self._plotters)} plotters. Use the plotters property."
            )
        return self._plotters[0] if self._plotters else None

    @plotter.setter
    def plotter(self, value: "Solver.Plotter"):
        self._plotters = []
        if value is not None:
            self.add_plotter(value)

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

    def _add_wants_to_plot_to_solver(self):
        """!

        Add wantsToPlot() to the solver's C++ class: It holds if any of the
        solver's plotters is active (@ref exahype2.api.Solver.Solver.Plotter).
        Solvers without plotters do not get the method, as nobody asks them.
        The method is added only once, even if the Peano 4 project is
        generated multiple times, and it evaluates the plotters the solver
        has when the C++ code is written.

        """
        declaration = "bool wantsToPlot() const;"
        if not self._plotters or declaration in [
            existing for existing, _ in self._abstract_solver_methods
        ]:
            return
        self.add_abstract_solver_method(
            declaration,
            lambda qualified_class_name: "bool "
            + qualified_class_name
            + "::wantsToPlot() const {\n"
            "  return "
            + " or ".join("is" + p.name + "Active()" for p in self._plotters)
            + ";\n"
            "}\n",
        )

    def _add_checkpoint_meta_data_to_solver(self):
        """!

        Override exahype2::Solver::toCheckpointMetaData() in the solver's
        C++ class such that it also writes the attributes added through
        add_abstract_solver_attribute(), e.g. the time stamps of the next
        outputs of the plotters. Only attributes of type double, float or
        int are written, with their name without leading underscore as key.

        The counterpart override of exahype2::Solver::fromCheckpointMetaData()
        restores these attributes. Afterwards, it calls finish<Name>() of
        all plotters: A plotter which has written output at the time of the
        checkpoint has done so before it has moved on to its next output, as
        it only moves on at the end of the plot phase
        (@ref exahype2.api.Solver.Solver.Plotter). The checkpoint hence holds
        the time stamp of an output which has already been written.
        finish<Name>() moves such a plotter on, exactly as the original run
        has done after the checkpoint, so the restarted simulation does not
        write this output again.

        The overrides evaluate the attributes and plotters the solver has
        when the C++ code is written, and they are added only once, even if
        several checkpoints use them or the Peano 4 project is generated
        multiple times.

        """
        declaration = "std::string toCheckpointMetaData() const override;"
        if declaration in [existing for existing, _ in self._abstract_solver_methods]:
            return

        def checkpointed_attributes():
            for attribute in self._abstract_solver_attributes:
                match = re.match(r"\s*(?:double|float|int)\s+(\w+)\s*(=|;)", attribute)
                if match is not None:
                    yield match.group(1)

        def to_definition(qualified_class_name):
            result = (
                "std::string "
                + qualified_class_name
                + "::toCheckpointMetaData() const {\n"
                "  return ::exahype2::Solver::toCheckpointMetaData()"
            )
            for attribute_name in checkpointed_attributes():
                result += (
                    '\n    + "\\n" + toCheckpointMetaDataEntry("'
                    + attribute_name.lstrip("_")
                    + '", '
                    + attribute_name
                    + ")"
                )
            return result + ";\n}\n"

        def from_definition(qualified_class_name):
            result = (
                "void "
                + qualified_class_name
                + "::fromCheckpointMetaData(const std::string& metaData) {\n"
                "  ::exahype2::Solver::fromCheckpointMetaData(metaData);\n"
                "  double value;\n"
            )
            for attribute_name in checkpointed_attributes():
                result += (
                    '  if (getCheckpointMetaDataEntry(metaData, "'
                    + attribute_name.lstrip("_")
                    + '", value)) {\n'
                    "    "
                    + attribute_name
                    + " = static_cast<decltype("
                    + attribute_name
                    + ")>(value);\n"
                    "  }\n"
                )
            for plotter in self._plotters:
                result += "  finish" + plotter.name + "();\n"
            return result + "}\n"

        self.add_abstract_solver_method(declaration, to_definition)
        self.add_abstract_solver_method(
            "void fromCheckpointMetaData(const std::string& metaData) override;",
            from_definition,
        )

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
        mesh_adaptation,
    ):
        """!
        Turn this solver specification into a task graph and create the required C++ kernels/stubs

        peano_project: peano4.api.Project
          The Peano 4 project that exahype2_project.generate_Peano4_project() is
          currently assembling. It is a local variable there, not an attribute -
          merely passed through here, so this solver can add its own artefacts to it.

        setup_simulation, time_step, plot, mesh_adaptation: peano4.api.taskgraph.Graph
          The task graphs of the four algorithmic phases of the surrounding
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

        # First, as the numerical scheme might ask for the mesh adaptation
        if self.mesh_adaptation is None:
            self.mesh_adaptation = (
                self.numerical_scheme.create_default_mesh_adaptation()
            )

        self.grid_construction.construct_output(
            exahype2_project, peano_project, setup_simulation
        )
        self.numerical_scheme.construct_output(
            exahype2_project, peano_project, time_step
        )
        for plotter in self._plotters:
            plotter.construct_output(exahype2_project, peano_project, plot)
        self._add_wants_to_plot_to_solver()
        if self.mesh_adaptation is not None:
            self.mesh_adaptation.construct_output(
                exahype2_project, peano_project, mesh_adaptation
            )
        # Last, so grid construction, numerical scheme, plotter and mesh
        # adaptation can add their own members to the solver class first
        # (@see add_abstract_solver_attribute()/add_abstract_solver_method()).
        self.solver_generator.construct_output(exahype2_project, peano_project)
