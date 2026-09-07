# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

import abc
from typing import Dict, List, Union

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

        def __init__(self, 
                     solver: "Solver"):
            self._solver = solver

        @abc.abstractmethod
        def construct_output(self,
                             exahype2_project,
                             solver_realisation,
                             verbose):
            """!
            Has to be implemented by subclass
            """


    class NumericalScheme(abc.ABC):
        """!

        The concrete discretisation (Finite Volumes, ADER-DG, ...) realised
        by a Solver. Abstract - concrete solvers have to provide a subclass
        that implements construct_output().

        """

        def __init__(self,
                     solver: "Solver"):
            self._solver = solver

        @abc.abstractmethod
        def construct_output(self,
                             exahype2_project,
                             solver_realisation,
                             verbose):
            """!
            Has to be implemented by subclass
            """


    class GridConstruction(abc.ABC):
        """!

        How a Solver drives the construction of the mesh it runs on.
        Abstract - concrete solvers have to provide a subclass that
        implements construct_output().

        """

        def __init__(self,
                     solver: "Solver"):
            self._solver = solver

        @abc.abstractmethod
        def construct_output(self,
                             exahype2_project,
                             solver_realisation,
                             verbose):
            """!
            Has to be implemented by subclass
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

        self._pde_terms: Dict[str, PDETerm] = {}

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
        # subclasses have to assign their own subclass instance to it.
        self.numerical_scheme: "Solver.NumericalScheme" = None

        from .VTKPlotter import VTKPlotter
        self.plotter: "exahype2.plotter.api.Plotter" = VTKPlotter(self)

        from .DefaultInitialisation import DefaultInitialisation
        self.initialisation: "exahype2.initialisation.api.HardcodedInitialisation" = DefaultInitialisation(self)

        from .HardcodedGridConstruction import HardcodedGridConstruction
        self.grid_construction: "Solver.GridConstruction" = HardcodedGridConstruction(self)


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
        in registration order. Read-only - use add_pde_term() to add one and
        remove_pde_term() to take one away again.
        """
        return list(self._pde_terms.values())

    def add_pde_term(self, term: PDETerm):
        """!

        Register a PDE term with this solver, or replace the one already
        registered under the same name. A solver aggregates an arbitrary
        number of terms - the well-known ones (Flux(), PDETerm.eigenvalues(),
        .nonconservative_product(), .source_term()) as well as new,
        solver-specific ones users construct themselves via PDETerm(name=...,
        signature=..., implementation=...).

        """
        if not isinstance(term, PDETerm):
            raise TypeError(f"term has to be a PDETerm, but got {type(term)}")
        self._pde_terms[term.name] = term

    def remove_pde_term(self, name: str):
        """! Remove a previously registered PDE term by name. """
        del self._pde_terms[name]

    def get_pde_term(self, name: str) -> PDETerm:
        """! Look up a previously registered PDE term by name. """
        return self._pde_terms[name]

# @todo This one is important
    def create_readme_descriptor(self, domain_offset, domain_size) -> str:
        """!
        Return a human-readable description of this solver instance, to be
        embedded into the generated project's README.
        """
        pass

    def generate_solver_realisation(self,
                                exahype2_project,
                                verbose):
        """!
        Turn this solver specification into a realisation.Solver

        Constructs a fresh realisation.Solver tied to this solver, lets the
        solver_generator populate it (and the surrounding exahype2_project),
        and returns it. The caller - typically the exahype2_project's
        generator - collects the realisations of all registered solvers and
        mangles them into one Peano 4 project.

        @return exahype2.api.realisation.Solver
        """
        if verbose:
            print("---------------------------------------")
            print("Create data for solver " + self._name)
            print("---------------------------------------")

        from .realisation import Solver as SolverRealisation
        solver_realisation = SolverRealisation(self)

        self.solver_generator.construct_output(exahype2_project,solver_realisation,verbose)

        return solver_realisation

