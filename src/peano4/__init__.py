# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org

import peano4.api

HEADER = """

+-------+-------+-------+
|       |       |       |
+-------+-------+-------+
|       |       |       |
+-------+-------+-------+
|       |       |       |
+-------+-------+-------+


Peano 4 (C) www.peano-framework.org
"""
peano4.api.Logger().log(HEADER, peano4.api.LoggerPrintOption.ALWAYS_PRINT)

try:
    import jinja2
except ImportError:
    print(
        "Jinja2 is not available, cannot generate glue code. Most Peano 4 features will be unavailable."
    )


