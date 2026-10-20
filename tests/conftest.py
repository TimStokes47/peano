"""!
Repository-wide pytest configuration for everything under tests/

Some ExaParty experiment scripts are called test_*.py but are command-line
generator scripts, not pytest modules. Importing them would run the
generator, so we exclude them from collection. Their pytest entry point is
tests/exaparty/test_exaparty.py.
"""

import tempfile
from pathlib import Path

collect_ignore_glob = [
    "exaparty/*/test_periodic_BC-*.py",
    "exaparty/*/test_smoothing_length_computation_*.py",
]


def pytest_configure(config):
    """!
    Write the log of peano4.api.Logger into a temporary directory

    The logger flushes its log into log.md in the current working directory
    when the interpreter exits. Without this hook, every pytest run would
    leave a log-<timestamp>.md file in the repository root.
    """
    from peano4.api.Logger import Logger

    Logger().file_name = str(Path(tempfile.mkdtemp(prefix="peano-log-")) / "log.md")
