# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from enum import Enum


class CompileMode(Enum):
  Debug = 0
  Trace = 1
  Asserts = 2
  Release = 3
