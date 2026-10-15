# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .GridSelector import GridSelector


class FixedResolutionGridSelector(GridSelector):
    """!

    Select the mesh entities of one level of the spacetree

    The root of the spacetree, i.e. the bounding box of the whole domain,
    is level 0. Each further level splits the cells of its parent into
    three along each axis, i.e. a cell on level l has the size
    domain_size/3^l. The guards select exactly the vertices, faces and
    cells of this size, no matter whether they are refined further or not.
    If the mesh is coarser than the level somewhere, the guards do not
    hold there.

    The markers do not know their level, so the guards compare the mesh
    size h() of the vertex, face or cell with the size of a cell on the
    selected level. Different to FineGridSelector, the selection does not
    depend on the refinement state, i.e. it is the same in each and every
    mesh sweep.

    """

    def __init__(self, level: int, domain_size):
        """!

        level: int
          Level of the spacetree, with 0 being the root.

        domain_size: list of float
          Size of the computational domain along each axis, i.e. the same
          value as handed over to the project's global simulation
          parameters.

        """
        assert level >= 0, "level has to be non-negative"
        self._level = level
        self._mesh_size = [size / 3**level for size in domain_size]

    def _guard(self) -> str:
        """!

        Compare each entry of marker.h() with the size of a cell on the
        selected level. The tolerance is relative to this size, as the
        sizes become tiny on fine levels.

        """
        return " and ".join(
            f"tarch::la::equals(marker.h()({axis}), {size!r}, {size * 1e-6!r})"
            for axis, size in enumerate(self._mesh_size)
        )

    def createVertexGuard(self) -> str:
        return self._guard()

    def createFaceGuard(self) -> str:
        return self._guard()

    def createCellGuard(self) -> str:
        return self._guard()
