"""!
Run the ExaParty test cases through pytest

Each ExaParty test case lives in its own directory and ships a run script
which generates the code, compiles it, runs the executable and, where
available, compares the outcome against a reference. This module runs these
scripts one by one. A test passes if its script returns with exit code zero.

All tests are integration tests, i.e. you have to request them explicitly:

    python3 -m pytest -m integration tests/exaparty
    python3 -m pytest -m "integration and not slow" tests/exaparty
    python3 -m pytest -m integration tests/exaparty -k sod-1d

They require a Peano build with --enable-particles --enable-exaparty and are
skipped otherwise. The scripts write their generated code, executables and
outputs into their own directories, exactly as if you ran them by hand.
"""

import os
import subprocess
import sys
from pathlib import Path

import pytest

TESTS_DIRECTORY = Path(__file__).resolve().parent
PEANO_ROOT_DIRECTORY = TESTS_DIRECTORY.parent.parent

FUNCTIONAL_TESTS = [
    "dastgen-test/run.sh",
    "test-density-calculation/run.sh",
    "test-boundary-conditions/periodic_1D/run.sh",
    "test-boundary-conditions/periodic_2D/run.sh",
    "test-boundary-conditions/periodic_3D/run.sh",
    "test-smoothing-length-computation/test-smoothing-length-computation-1D/run.sh",
    "test-smoothing-length-computation/test-smoothing-length-computation-1D/run_multiscale.sh",
    "test-smoothing-length-computation/test-smoothing-length-computation-2D/run.sh",
    "test-smoothing-length-computation/test-smoothing-length-computation-2D/run_multiscale.sh",
    "test-smoothing-length-computation/test-smoothing-length-computation-3D/run.sh",
    "test-smoothing-length-computation/test-smoothing-length-computation-3D/run_multiscale.sh",
]

PHYSICS_TESTS = [
    "planet-orbit/run.sh",
    "hydro-simple-sph-physics/sod-1d/run.sh",
    "hydro-simple-sph-physics/noh-2d/run_32.sh",
    "hydro-simple-sph-physics/noh-3d/run_20.sh",
    "hydro-simple-sph-physics/sedov-1d/run.sh",
    "hydro-simple-sph-physics/sedov-2d/run.sh",
    "hydro-simple-sph-physics/sedov-3d/run.sh",
]


def exaparty_is_built():
    """!
    Check if the ExaParty libraries exist

    We look into the directories which the generated Makefiles use as well,
    i.e. PEANO_CMAKE_BUILD_DIR and PEANO_SRC_ROOT_DIR take precedence over
    the repository this file belongs to.
    """
    if "PEANO_CMAKE_BUILD_DIR" in os.environ:
        candidates = Path(os.environ["PEANO_CMAKE_BUILD_DIR"]).rglob(
            "libExaPartyCore2d*"
        )
        return any(True for _ in candidates)
    root = Path(os.environ.get("PEANO_SRC_ROOT_DIR", PEANO_ROOT_DIRECTORY))
    return (root / "src" / "exaparty" / "libExaPartyCore2d.a").exists()


def run_script(script):
    """!
    Run one ExaParty run script from within its directory

    @param script: str
      Path of the run script relative to tests/exaparty.

    We run the script through bash with pipefail, as most scripts pipe the
    executable's output through tee, which otherwise hides a crash. python3
    resolves to the interpreter that runs pytest, and src/ is added to the
    PYTHONPATH, so the generator scripts find Peano's Python API.
    """
    if not exaparty_is_built():
        pytest.skip("Peano is not built with --enable-particles --enable-exaparty")
    script_path = TESTS_DIRECTORY / script
    environment = dict(os.environ)
    environment["PATH"] = (
        str(Path(sys.executable).parent) + os.pathsep + environment.get("PATH", "")
    )
    environment["PYTHONPATH"] = os.pathsep.join(
        filter(None, [str(PEANO_ROOT_DIRECTORY / "src"), environment.get("PYTHONPATH")])
    )
    environment["MPLBACKEND"] = "Agg"
    result = subprocess.run(
        ["bash", "-o", "pipefail", script_path.name],
        cwd=script_path.parent,
        env=environment,
    )
    assert result.returncode == 0, f"{script} failed with exit code {result.returncode}"


@pytest.mark.integration
@pytest.mark.parametrize("script", FUNCTIONAL_TESTS)
def test_functional(script):
    run_script(script)


@pytest.mark.integration
@pytest.mark.slow
@pytest.mark.parametrize("script", PHYSICS_TESTS)
def test_physics(script):
    run_script(script)
