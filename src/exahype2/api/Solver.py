# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

import abc
from typing import Dict, List, Tuple, Union

import peano4.api

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
          `{"rho": 1, "j": 3}` for a scalar density plus a 3d momentum). If a
          dict is used, the code generator emits a `VariableShortcuts` file
          that lets user code address a variable by name instead of having to
          remember its offset into the unknowns array by hand.

        auxiliary_variables: int or dict
          Same semantics as unknowns, but for variables that may be
          space-dependent yet do not change over time.

        """
        self._name = name

        # @todo Maybe can go away?
        self._use_var_shortcut = False
        self._variable_names: List[str] = []
        self._variable_pos: List[int] = [0]

        self._unknowns = self._register_variables(unknowns)
        self._auxiliary_variables = self._register_variables(auxiliary_variables)

        self._user_action_set_includes = ""
        self._user_solver_includes = ""
        self._solver_constants = ""

        self._pde_terms: Dict[Tuple[str, bool], PDETerm] = {}

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

    def _register_variables(self, variables: VariableSpecification) -> int:
        """!

        Translate a plain int or a name-to-multiplicity dict into a total
        variable count, and - for the dict case - extend the shared
        _variable_names/_variable_pos bookkeeping used to emit the
        VariableShortcuts file. Called once for unknowns and once for
        auxiliary_variables, so the two end up sharing one contiguous
        name/offset table.

        """
        if isinstance(variables, dict):
            self._use_var_shortcut = True
            self._variable_names += list(variables.keys())
            for multiplicity in variables.values():
                self._variable_pos.append(multiplicity + self._variable_pos[-1])
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

    @unknowns.setter
    def unknowns(self, value: VariableSpecification):
        self._unknowns = self._register_variables(value)
        self.create_data_structures()
        self.create_action_sets()

    @property
    def auxiliary_variables(self) -> int:
        return self._auxiliary_variables

    @auxiliary_variables.setter
    def auxiliary_variables(self, value: VariableSpecification):
        self._auxiliary_variables = self._register_variables(value)
        self.create_data_structures()
        self.create_action_sets()

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

    def set_solver_constants(self, datastring: str):
        self._solver_constants = datastring

    def add_solver_constants(self, datastring: str):
        self._solver_constants += datastring + "\n"

    def _unknown_identifier(self) -> str:
        return self._name + "Q"

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

        self.solver_generator.construct_output(exahype2_project, peano_project)
        self.grid_construction.construct_output(
            exahype2_project, peano_project, setup_simulation
        )
        self.numerical_scheme.construct_output(
            exahype2_project, peano_project, time_step
        )
        if self.plotter is not None:
            self.plotter.construct_output(exahype2_project, peano_project, plot)
