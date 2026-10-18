from enum import Enum

"""!

Target specifies the final architecture that will be used.
TODO: This file will later include variants to control which MLIR passes are used, and which backends

"""
class Target(Enum):
    Serial = 0
    Multicore = 1
    Accelerator = 2

