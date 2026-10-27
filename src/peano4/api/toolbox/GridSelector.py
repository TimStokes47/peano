# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import abc


class GridSelector(abc.ABC):
    """!

    Select a subset of the mesh entities

    This is a sole helper class: It does not generate any C++ code itself
    and is not an action set. It merely hands out guards, i.e. C++
    snippets which other Python classes, such as action sets, can embed
    into their generated code to decide whether they should act on a
    particular vertex, face or cell.

    Each routine returns a string which has to be a C++ boolean expression.
    The expression is evaluated in a context where the corresponding marker
    is available as marker, i.e. it can query the
    peano4::datamanagement::VertexMarker, peano4::datamanagement::FaceMarker
    or peano4::datamanagement::CellMarker, respectively.

    @see FineGridSelector

    """

    @abc.abstractmethod
    def createVertexGuard(self) -> str:
        """!

        C++ boolean expression over a peano4::datamanagement::VertexMarker
        called marker

        """

    @abc.abstractmethod
    def createFaceGuard(self) -> str:
        """!

        C++ boolean expression over a peano4::datamanagement::FaceMarker
        called marker

        """

    @abc.abstractmethod
    def createCellGuard(self) -> str:
        """!

        C++ boolean expression over a peano4::datamanagement::CellMarker
        called marker

        """

    def createVertexStoreGuard(self) -> str:
        """!

        C++ boolean expression over a peano4::datamanagement::VertexMarker
        called marker which decides whether a vertex's data is stored at
        the end of a mesh sweep

        Peano's stacks require that data stored in one mesh sweep are
        loaded in the next one. The load is guarded by createVertexGuard().
        If this guard depends on the refinement state, the store therefore
        has to anticipate the state of the next sweep. By default, we use
        createVertexGuard(), which is correct for selectors that do not
        depend on the refinement state.

        """
        return self.createVertexGuard()

    def createFaceStoreGuard(self) -> str:
        """!

        Counterpart of createVertexStoreGuard() for faces

        """
        return self.createFaceGuard()

    def createCellStoreGuard(self) -> str:
        """!

        Counterpart of createVertexStoreGuard() for cells

        """
        return self.createCellGuard()
