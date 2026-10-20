# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Peano is an open-source C++ framework for solvers on dynamically adaptive Cartesian
meshes (spacetrees). The C++ core is code-generated and driven by a Python API: users
describe an experiment in Python, call `.generate()`, and get plain, standalone C++ plus
a Makefile that is then compiled independently of Python. Do not expect to find
hand-written glue code — most `.cpp`/`.h` files under a `repositories/`, `observers/`,
`vertexdata/`, `celldata/`, or `globaldata/` subdirectory of a test/application/tutorial
are generated output and should not be hand-edited; edit the Python script that produced
them instead.

Two build systems coexist and are functionally equivalent: GNU Autotools (canonical,
in-tree build) and CMake (out-of-tree, `build/` directory). Do not assume one is
authoritative over the other; check which one the working tree is currently configured
for before adding build logic (`config.status`/`Makefile` at the root ⇒ Autotools;
a `build/` directory ⇒ CMake).

## Build

### Autotools (in-tree)

One-time setup after clone or after major changes to `configure.ac`/`Makefile.am`:
```
libtoolize; aclocal; autoconf; autoheader; cp src/config.h.in .
automake --add-missing
```
Then:
```
./configure --help                 # see all toolbox/extension flags
./configure --enable-exahype --enable-loadbalancing --enable-blockstructured \
            --with-multithreading=omp CXXFLAGS="-std=c++20 -O3 -fopenmp -Wno-attributes" LDFLAGS="-fopenmp"
make -j
make check                         # runs unit tests (non-MPI only, see below)
```
Key `--enable-*` flags: `exahype`, `exagrype`, `exaparty`, `mghype`, `particles`, `blockstructured`,
`loadbalancing`, `finiteelements`, `multiprecision`, `curvi`. ExaHyPE needs at least
`--enable-blockstructured --enable-exahype`; ExaParty needs `--enable-particles --enable-exaparty`;
ExaGRyPE (`src/exagrype`, 3d-only libraries) needs ExaHyPE plus `--enable-exagrype`, and
`--with-gsl` for its TwoPunctures (black hole) initial data.
`--with-mpi`, `--with-vtk`, `--with-hdf5`, `--with-gsl`, `--with-petsc`, `--with-libxsmm` etc. pull in
optional third-party support.

### CMake (out-of-tree)

```
mkdir build && cd build
cmake .. -DENABLE_EXAHYPE=ON -DENABLE_LOADBALANCING=ON -DENABLE_BLOCKSTRUCTURED=ON -DWITH_MULTITHREADING=omp
make -j
make test
```
CMake option names mirror the Autotools flags (`ENABLE_*`, `WITH_*`).

Both build systems build multiple flavours of each static library: plain (release),
`_debug` (assertions + full logging/tracing), `_asserts` (assertions + tracing macros
active), `_trace` (tracing only, no assertions/debug), `_stats` (release + statistics
counters). Pick the right archive when linking, e.g. `libPeano4Core3d_debug.a` vs
`libPeano4Core3d.a`. Each is also built separately for 2D and 3D (`Core2d`/`Core3d`);
higher dimensions must be built manually.

MPI builds: `make check`/`make test` does not know to prefix `mpirun`, so MPI unit tests
must be run manually via `src/unittests/UnitTests2d`/`UnitTests3d`.

### Python API

The Python packages are split across two roots: `python/` (`exahype2` legacy API)
and `src/` (`peano4.api`, `dastgen2`, `exaparty.api`, `exagrype.api`, and the in-progress `exahype2.api`,
`mghype`, `toolbox.loadbalancing.api` rewrites, colocated with each component's C++
sources where one exists). Both are installed via:
```
python3 -m venv .venv && source .venv/bin/activate
pip install --upgrade pip
pip install -e .
```
This also pulls in `requirements.txt` (numpy, scipy, sympy, jinja2, h5py, vtk, etc.).
Editable install (`-e`) is the recommended mode since these packages change frequently.
Note: `python/exahype2` and `src/exahype2` both currently define a top-level `exahype2`
package (the old one is mid-migration to `src/exahype2/api`, matching what already
happened for `peano4`) — whichever comes first on `sys.path` wins, so don't rely on
`import exahype2` resolving to the new API until that migration finishes.
A tutorial/application/test experiment is typically a single Python script (e.g.
`tutorials/peano4/finite-volumes/finite-volumes.py`) that builds up a `peano4.api.Project`
(or a domain-specific project such as `exahype2.api.Project`/`exaparty.api.Project`), adds
solvers/action sets, and calls `.generate()` to emit C++ + Makefile, which is then built
with the normal `make` from the generated directory.

### Formatting

```
python3 format.py                              # formats files changed vs. p4 (the default branch to diff against)
python3 format.py --branch <branch>             # diff against a different branch
python3 format.py --files a.py b.cpp c.h        # format specific files (bypasses git-tracked check)
```
Uses `clang-format` for C++ and `black` for Python, run inside `.formatting_python_env`
(a dedicated venv — see `documentation/conventions.dox` for setup). Only format files you
actually touched; do not reformat unrelated files.

### Documentation

```
doxygen documentation/Doxyfile
```
Output goes to `doxygen-html/html`.

Python documentation (Doxygen docstrings starting with `"""!`) follows these rules:

- Every parameter description starts with a `@param` statement, followed by the
  parameter's name, a colon and its type, with the description indented on the
  next lines. Write
  ```
  @param graph: peano4.api.taskgraph.Graph
    The task graph to compile.
  ```
  and not `graph: peano4.api.taskgraph.Graph`.

## Testing

- `tests/unit/` — Python unit tests (pytest). Run with `pytest` from repo root (see
  `pytest.ini`: default testpaths is `tests/unit`, pattern `test_*.py`). Markers
  `integration` and `slow` are opt-in (`pytest -m integration`); plain `pytest` skips them.
- `tests/exahype2/`, `tests/exaparty/`, `tests/multigrid/`, `tests/integration/` — C++/Python
  hybrid test cases, each typically a Python build script (e.g. `fv.py`, `aderdg.py`,
  `dastgen-test.py`) that generates and compiles a small C++ executable, plus generated
  build artifacts (`.o`, binaries) that should not be committed if regenerated locally.
- `src/unittests/` builds `UnitTests2d`/`UnitTests3d` executables covering the C++ core
  and enabled extensions/toolboxes — this is what `make check`/`make test` runs.

When changing Python-API code generation, prefer testing by running the relevant
generator script end-to-end (it will regenerate C++ and you can build/run it) rather than
assuming unit tests alone cover it — much of this repo's correctness is only observable
after code generation.

## Architecture

Peano is organised in strict layers, each shipped as static libraries; every Peano
application/extension follows the same blueprint (see `documentation/architecture.dox`
for the authoritative version):

1. **`tarch`** (`src/tarch`) — Technical architecture. Loosely coupled, dependency-hiding
   building blocks: tasking/multicore abstraction, logging, GPU offloading, small linear
   algebra. Hides third-party dependencies (OpenMP, MPI, TBB, ...) from everything above
   it. Pure C++, no Peano-specific concepts.
2. **`peano4`** (`src/peano4`, Python: `src/peano4/api`) — The core: spacetree traversal
   engine, mesh/vertex/cell data management, domain decomposition. Namespace `peano4`;
   the Python API is imported as `peano4.api` (e.g. `peano4.api.Project`).
3. **Toolboxes** (`src/toolbox`, Python: `src/peano4/api/toolbox`) — Generic,
   solver-independent building blocks layered on `peano4`/`tarch`: particle management,
   blockstructured mesh support, load balancing, finite elements, multigrid support, etc.
   Individually enabled via `--enable-*`/`ENABLE_*` flags.
4. **Extensions** (`src/exahype2`, `src/exagrype`, `src/exaparty`, `src/mghype`, Python packages of the same
   names) — Domain-specific frameworks built on Peano + toolboxes: ExaHyPE 2 (hyperbolic
   PDE solvers: Finite Volumes, ADER-DG, Limiting), ExaParty (SPH), MGHyPE (multigrid /
   PETSc coupling). Each provides its own Python `Project` API layered over `peano4`'s.
5. **User code / applications / benchmarks / tutorials** — Real experiments, at the top.
   `applications/` = real-world codes, `benchmarks/` = mini-apps for performance
   measurement, `tutorials/` = teaching examples, `tests/` = automated small cases.

### Control flow (the "Hollywood Principle")

This is the key non-obvious design point: **Peano's core drives the traversal, not the
user code.** The typical sequence:

1. User's `main()` builds a spacetree set (`peano4::parallel::SpacetreeSet`) and calls
   `.traverse(observer)`.
2. From that point Peano owns control — it decides traversal order, parallelisation,
   forking of spacetrees across ranks, etc. The caller does not iterate cells itself.
3. During traversal, Peano calls back into a `peano4::grid::TraversalObserver` passed in
   by the user, notifying it of events (entered/left a cell, touched a vertex first/last
   time, ...) with rich context.
4. The observer breaks these notifications down into "actions" (e.g. "update a finite
   volume patch", "move particles in this cell") and dispatches to user/extension code —
   these are the **action sets**.

The Python API exists because writing an observer + action-set mapping by hand per
algorithmic step is heavy boilerplate. Instead, users describe *what* they want (e.g. "add
a Finite Volume solver with these variables") via a domain-specific `Project` (ExaHyPE 2 /
ExaParty / MGHyPE) or the plain `peano4.Project`; the API works out the data, the grid
sweeps, and the action sets needed, assembles them into an internal Python model, and
`.generate()` emits the actual C++ (classes + Makefile). Only the bodies of action sets
and the main routine are left for the user/extension author to fill in (often via
templates such as `python/exahype2/ExaHyPEMain.template.cpp`).

### C++ conventions worth knowing before editing generated-adjacent code

- One class per file; header (`.h`) holds declarations only, implementation goes in
  `.cpp`, except where inlining for performance is required and documented explicitly.
  Templates: declaration in `.h`, implementation in `.cpph`, included by the header.
  Respect this split even in generated code — it mirrors what the Python API emits.
  See `documentation/conventions.dox`.
- One subdirectory per C++ namespace.
- Naming follows Oracle Java conventions translated into C++ (long, self-explanatory
  names, no acronyms); Python follows PEP 8 (snake_case functions/vars, CamelCase
  classes).
- `src/` also accumulates build artifacts (`.a` libraries, generated `.cpp`/`.prep.cpp`
  intermediate files from instrumentation tools, `unittests` binaries) — these are build
  output, not source, even though they live under version-controlled directories; check
  `git status`/`.gitignore` before assuming a file under `src/` is hand-written source.
