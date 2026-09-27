# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet

from .ReconstructPatchWithHalo import ReconstructPatchWithHalo


class ReconstructPatchAndApplyFunctor(ReconstructPatchWithHalo):
  """!

  ReconstructPatchWithHalo plus a functor invoked once the reconstruction
  (into reconstructed_patch) is done - @see ReconstructPatchWithHalo for the
  reconstruction itself, which this class does not repeat. The functor gets
  access to the reconstructed data (oldQWithHalo, aliasing
  reconstructed_patch's own accessor - no separate scratch array is
  allocated) and writes its result into patch's own accessor (newQ).

  If you want to swap the functor, please replace self.functor_implementation,
  and ensure that all external references (text replacements) are already
  resolved. Consult _add_action_set_entries_to_dictionary() for details.

  """

  def __init__(self,
               patch,
               patch_overlap,
               reconstructed_patch,
               functor_implementation,
               guard="true",
               add_assertions_to_halo_exchange = True,
               additional_includes = ""
               ):
    """!

  patch, patch_overlap, reconstructed_patch, guard, add_assertions_to_halo_exchange, additional_includes:
    @see ReconstructPatchWithHalo.

  ## Functor_implementation

  The functor implementation is a plain C/C++

  - If you want to use brackets in your implementation, please use double brackets {{ }} as
    the template system otherwise gets confused.
  - The following C++ variables are defined:

    oldQWithHalo
    newQ

    Both are plain double pointers.

    """
    super(ReconstructPatchAndApplyFunctor,self).__init__(
      patch, patch_overlap, reconstructed_patch, guard, add_assertions_to_halo_exchange, additional_includes
    )
    self.functor_implementation = functor_implementation


  def _add_action_set_entries_to_dictionary(self,d):
    """!

    Befill dictionary - @see ReconstructPatchWithHalo._add_action_set_entries_to_dictionary(),
    which this calls first. On top of that, this one also resolves the
    functor implementation (CELL_FUNCTOR_IMPLEMENTATION).

    Please note that we do not replace entries in the functor implementation:
    The overall code created by this action set consists of different parts
    realised as different templates. The templates are subject to a text
    replacement system and then are concatenated. One of the things that is
    replaced within the templates is the actual functor implementation
    (CELL_FUNCTOR_IMPLEMENTATION). If this one contains again free parameters,
    it is your responsibility to first resolve those parameters, and then to
    reset the functor in this routine.

    If you want to replace the functor, you can hence either first call this
    routine and then change d["CELL_FUNCTOR_IMPLEMENTATION"] or you set
    self.functor_implementation and then you call this routine, i.e. the
    superclass.

    """
    super(ReconstructPatchAndApplyFunctor,self)._add_action_set_entries_to_dictionary(d)
    d[ "CELL_FUNCTOR_IMPLEMENTATION" ] = self.functor_implementation


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  _Template_TouchCellFirstTime_Core = """
    double* newQ = {CELL_ACCESSOR}.value;
    {CELL_FUNCTOR_IMPLEMENTATION}
"""


  def get_body_of_operation(self,operation_name):
    result = "\n"
    if operation_name==ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
      d = {}
      self._add_action_set_entries_to_dictionary(d)
      result  = self._Template_TouchCellFirstTime_Preamble.format(**d)
      result += self._Template_TouchCellFirstTime_Fill_Patch.format(**d)
      result += self._Template_TouchCellFirstTime_Fill_Halos.format(**d)
      result += self._Template_TouchCellFirstTime_Core.format(**d)
      result += self._Template_TouchCellFirstTime_Epilogue.format(**d)
      pass
    return result
