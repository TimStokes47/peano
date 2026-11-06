# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Decide which boundary treatment a solver's numerical scheme has to generate

All numerical schemes evaluate the same two helpers: has_domain_boundary()
tells whether the domain has a boundary at all, i.e. whether at least one
axis is not periodic. get_boundary_conditions() returns the solver's
boundary condition term and checks that the solver's PDE terms are
consistent with the domain:

- If all axes are periodic, the domain has no boundary. The schemes then
  generate no boundary treatment at all, and the solver must not hold a
  boundary condition term.
- Otherwise, the solver has to hold exactly one boundary condition term,
  i.e. either an exahype2.api.DirichletBoundaryConditions or an
  exahype2.api.SommerfeldBoundaryConditions term.

"""
from .DirichletBoundaryConditions import DirichletBoundaryConditions
from ..MaxEigenvalue import MaxEigenvalue
from .SommerfeldBoundaryConditions import SommerfeldBoundaryConditions


BOUNDARY_CONDITION_TERMS = (DirichletBoundaryConditions, SommerfeldBoundaryConditions)


def has_domain_boundary(exahype2_project) -> bool:
    """!

    Does the domain have a boundary, i.e. is at least one axis not periodic?

    @param exahype2_project: exahype2.api.Project
      Provides the periodic boundary conditions (periodic_BC).

    @return bool
      False if and only if all axes are periodic.

    """
    return not all(exahype2_project.periodic_BC)


def get_access_to_persistent_face_data(solver, exahype2_project, default):
    """!

    Access qualifier of the persistent face data for a task of the time step

    Sommerfeld boundary conditions advance the ghost cells within the roll
    over of the projection onto the faces, and they read the old face data
    there (@ref exahype2::blockstructured::applySommerfeldBoundaryConditions()).
    The face data hence has to survive the reconstruction of the halo, and
    the projection has to load it:
    peano4.api.taskgraph.compiler.DeriveLoadStoreComputeFlag only stores
    data after tasks which write it, and only loads it before tasks which
    read it. With Sommerfeld boundary conditions, all tasks of the time step
    which use the persistent face data therefore have to declare
    READ_AND_WRITE.

    @param solver: exahype2.api.Solver

    @param exahype2_project: exahype2.api.Project

    @param default: peano4.api.taskgraph.Task.DataAccess
      The qualifier the task uses without Sommerfeld boundary conditions,
      typically READ_AND_DISCARD for the reconstruction and WRITE for the
      projection.

    @return peano4.api.taskgraph.Task.DataAccess

    """
    import peano4.api.taskgraph

    if isinstance(
        get_boundary_conditions(solver, exahype2_project), SommerfeldBoundaryConditions
    ):
        return peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE
    return default


def get_max_eigenvalue_term_for_sommerfeld(solver):
    """!

    The MaxEigenvalue term Sommerfeld boundary conditions take their wave speed from

    The wave speed does not depend on any solver state. We therefore prefer
    the stateless variant of the eigenvalue computation if the solver holds
    one, and fall back to the variant with state otherwise.

    @param solver: exahype2.api.Solver

    @return exahype2.api.MaxEigenvalue
      The stateless term if there is one, otherwise the term with state.
      Check its stateless attribute to see which variant is used.

    @exception ValueError
      If the solver holds no MaxEigenvalue term.

    """
    candidates = [term for term in solver.pde_terms if isinstance(term, MaxEigenvalue)]
    for term in candidates:
        if term.stateless:
            return term
    if candidates:
        return candidates[0]
    raise ValueError(
        f"Solver {solver.name} uses Sommerfeld boundary conditions, which take "
        "their wave speed from the maximum eigenvalue, but has no MaxEigenvalue "
        "PDE term. Add one via solver.add_pde_term(exahype2.api.MaxEigenvalue(...))."
    )


def get_boundary_conditions(solver, exahype2_project):
    """!

    The solver's boundary condition term, or None if the domain has no boundary

    @param solver: exahype2.api.Solver
      The solver whose PDE terms are checked.

    @param exahype2_project: exahype2.api.Project
      Provides the periodic boundary conditions (periodic_BC).

    @return exahype2.api.DirichletBoundaryConditions,
      exahype2.api.SommerfeldBoundaryConditions or None
      None if and only if all axes are periodic.

    @exception ValueError
      If all axes are periodic and the solver holds a boundary condition
      term, if at least one axis is not periodic and the solver holds no or
      more than one boundary condition term, if the term is stateless, or if
      the solver holds a SommerfeldBoundaryConditions term but no
      MaxEigenvalue term.

    """
    terms = [
        term for term in solver.pde_terms if isinstance(term, BOUNDARY_CONDITION_TERMS)
    ]
    periodic_BC = exahype2_project.periodic_BC

    if not has_domain_boundary(exahype2_project):
        if terms:
            raise ValueError(
                f"Solver {solver.name} holds the boundary condition term(s) "
                f"{', '.join(type(term).__name__ for term in terms)}, but all axes "
                f"are periodic (periodic_BC={periodic_BC}), i.e. the domain has "
                "no boundary. Remove the term(s) via solver.remove_pde_term(...), "
                "or make at least one axis non-periodic."
            )
        return None

    if not terms:
        raise ValueError(
            f"Solver {solver.name} has non-periodic boundaries "
            f"(periodic_BC={periodic_BC}), but no boundary condition PDE term. "
            "Add one via solver.add_pde_term(exahype2.api.DirichletBoundaryConditions(...)) "
            "or solver.add_pde_term(exahype2.api.SommerfeldBoundaryConditions(...)), "
            "or make all axes periodic."
        )
    if len(terms) > 1:
        raise ValueError(
            f"Solver {solver.name} holds more than one boundary condition term "
            f"({', '.join(type(term).__name__ for term in terms)}). Keep exactly one."
        )

    [term] = terms
    if term.stateless:
        raise ValueError(
            f"Solver {solver.name} holds a stateless {type(term).__name__} term, "
            "but only the non-stateless variant is supported. Add it with stateless=False."
        )
    if isinstance(term, SommerfeldBoundaryConditions):
        # Validates that there is an eigenvalue term
        get_max_eigenvalue_term_for_sommerfeld(solver)
        # Validates the number of entries of the origin
        term.get_origin(len(periodic_BC))
    return term
