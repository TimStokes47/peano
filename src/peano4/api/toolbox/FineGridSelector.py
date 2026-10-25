# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .GridSelector import GridSelector


class FineGridSelector(GridSelector):
    """!

    Select the fine grid entities

    Each guard holds for a vertex, face or cell which has not been refined
    in the mesh sweep before, i.e. which has not been refined when the
    current mesh traversal has kicked off. Consult hasBeenRefined() of the
    corresponding marker for details.

    The store guards anticipate the refinement state of the next mesh
    sweep, i.e. they hold for entities which will not be refined at the end
    of the current sweep (willBeRefined()). If the mesh changes, the data of
    an entity which is refined in this sweep is hence not stored, as the
    next sweep will not load it. This keeps the stores and loads of
    dynamically adaptive meshes consistent.

    """

    def createVertexGuard(self) -> str:
        return "not marker.hasBeenRefined()"

    def createFaceGuard(self) -> str:
        return "not marker.hasBeenRefined()"

    def createCellGuard(self) -> str:
        return "not marker.hasBeenRefined()"

    def createVertexStoreGuard(self) -> str:
        return "not marker.willBeRefined()"

    def createFaceStoreGuard(self) -> str:
        return "not marker.willBeRefined()"

    def createCellStoreGuard(self) -> str:
        return "not marker.willBeRefined()"
