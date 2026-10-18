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
