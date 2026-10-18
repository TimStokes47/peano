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

    """

    def createVertexGuard(self) -> str:
        return "not marker.hasBeenRefined()"

    def createFaceGuard(self) -> str:
        return "not marker.hasBeenRefined()"

    def createCellGuard(self) -> str:
        return "not marker.hasBeenRefined()"
