# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import List, Union


def resolve_quantities(
    solver,
    quantities: List[Union[str, int]],
) -> List[int]:
    """!

    Translate names and indices of the unknowns Q into sorted indices

    @param solver: exahype2.api.Solver
      The solver whose entries of Q we refer to.

    @param quantities: List[str or int]
      A string is the name of a variable as passed to the solver (e.g.
      "lapse" or "G" for exagrype.api.CCZ4Solver) and selects all of its
      entries. An int is the index of one entry of Q. The list must not be
      empty.

    @return List[int]
      Sorted indices without duplicates.

    """
    entries = solver.unknowns
    if len(quantities) == 0:
        raise ValueError(f"Solver {solver.name}: the list of quantities is empty")
    names = solver.variable_names_per_entry()[:entries]
    indices = set()
    for quantity in quantities:
        if isinstance(quantity, bool) or not isinstance(quantity, (int, str)):
            raise TypeError(
                f"Solver {solver.name}: quantities have to be names (str) or "
                f"indices (int) of entries of Q, but got {quantity!r}"
            )
        if isinstance(quantity, int):
            if not 0 <= quantity < entries:
                raise ValueError(
                    f"Solver {solver.name}: quantity index {quantity} is out of "
                    f"range, as only {entries} entries can be selected"
                )
            indices.add(quantity)
        else:
            matches = [
                index
                for index, name in enumerate(names)
                if name == quantity or name.startswith(quantity + "[")
            ]
            if not matches:
                raise ValueError(
                    f"Solver {solver.name}: {quantity!r} is not the name of a "
                    f"variable which can be selected. Available names are "
                    f"{sorted(set(name.split('[')[0] for name in names))}."
                )
            indices.update(matches)
    return sorted(indices)
