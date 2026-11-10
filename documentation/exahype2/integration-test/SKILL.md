---
name: exahype2:integration-test
description: Re-run the ExaHyPE 2 / ExaGRyPE integration test, i.e. the convergence study of the gauge wave benchmark (benchmarks/exagrype/gauge-wave) over the schemes fv, rk1fd4, rk2fd4, rk3fd4 and rk4fd4 and over several mesh sizes. Use when the user asks to run/re-run the integration test or the gauge wave convergence study, or to check that changes to the numerical schemes, task graph, kernels or code generation have not broken the numerical correctness. Runtimes are recorded, but they are not an acceptance criterion (that is the job of exahype2:performance-test).
allowed-tools: [Bash, Read]
---

# Integration test: gauge wave convergence study

The gauge wave has a known exact solution. `benchmarks/exagrype/gauge-wave/convergence-study.py`
builds and runs `gauge-wave.py` for every scheme and mesh, measures the maximum error of the
lapse along the line y=z=0.5 at the end time against the exact solution, estimates the
convergence orders, writes a plot and a table, and exits non-zero if the order of any scheme
between its two finest meshes falls below `ExpectedOrders` in the script. The results of the
last reference run are documented in the page on top of `gauge-wave.py`.

## Prerequisites

- Peano is configured and built with `--enable-exahype --enable-blockstructured
  --enable-loadbalancing --enable-exagrype` (release libraries are enough).
- The generated code is compiled with the Intel compiler:
  `export PATH=/opt/intel/oneapi/compiler/2024.2/bin:$PATH`.
- `export PYTHONPATH=<repo>/src`.
- The machine is otherwise idle. The runs use all cores, and the coarse runs are packed next
  to each other; background load pollutes the recorded runtimes and the memory watchdog.

## Steps

1. Pick a work directory outside the repository (builds and outputs take a few GB) and run
   the study in the background (about an hour on an eight-core workstation):
   ```sh
   python3 benchmarks/exagrype/gauge-wave/convergence-study.py \
       --work-directory <work> --output-directory <work>/out
   ```
   Useful options: `--schemes fv rk4fd4` and `--meshes 9 27` restrict the study (the latter is
   a quick smoke test of a few minutes); `--cores N` limits the cores. The default meshes
   are `9 27 81`; `243` fits only into a machine with far more than 64 GB of memory.
   Completed runs in the work directory are reused (`results.json`), so an interrupted study
   continues where it stopped, and runs that were skipped for lack of memory are retried.
   Delete the run's entry in `results.json` to force a rerun (a rerun starts from a clean
   directory). Do not kill the study with `pkill -f` from the same shell that runs it.
2. Watch the log (`build ...`, `run ...`, `skip ...`) with the Monitor tool instead of
   polling. All schemes run at the same time, each walking its meshes from coarse to
   fine. The coarse runs use one thread; at most two large runs go at once.
3. The script prints the table and `PASSED` or `FAILED` plus the schemes whose order is too
   low. On `FAILED`, look at the table first: a single scheme dropping in order points to that
   scheme's kernels or task graph; all schemes dropping points to the common parts (CCZ4
   solver, grid, plotting, time step size).
4. Report the table and the verdict. If the user wants the documentation refreshed, copy
   `<work>/out/convergence.png` to `benchmarks/exagrype/gauge-wave/example-output/` and update
   the table in the page description on top of `gauge-wave.py`, then run the skill
   `peano:regen-docs`. Do not commit run directories or generated sources.

## What to expect

- Reference orders between the two finest meshes (h=1/27 to 1/81, end time 0.1, patch size 9),
  as documented in the page description of `gauge-wave.py`: fv 0.94, rk1fd4 1.02, rk2fd4 2.31,
  rk3fd4 4.00, rk4fd4 3.99. The thresholds in `ExpectedOrders` leave a margin
  (0.8, 0.8, 1.8, 3.2, 3.2). The Finite Difference schemes are limited by the smaller of
  their spatial order (four) and the order of their Runge-Kutta scheme. On coarse
  meshes the time step is so small that the time error is invisible: rk2fd4, rk3fd4
  and rk4fd4 produce nearly identical errors there (an order around 3.5 between h=1/9
  and 1/27 is pre-asymptotic and not a quantity to test).
- The error is measured at the last snapshot, whose time stamp overshoots the end time by
  less than one time step and differs a bit between meshes. This biases the orders by
  about 0.01.
- Memory is the limit. A run with 81 volumes per axis needs 0.8 GB (fv) to 3 GB
  (Runge-Kutta), and the footprint grows with the number of volumes: h=1/243 needs
  more than 16 GB and is not part of the study on a workstation (it was tried: all five
  schemes were killed). The study runs at most two large runs at once
  (`--concurrent-large-runs`). A watchdog kills the run with the biggest footprint if less
  than 1.5 GB remain available on the machine; a killed run is reported as skipped and
  retried on the next invocation. Close other programs if runs get skipped.
- Memory that grows with the simulated time (not just with the mesh) indicates data that
  piles up on Peano's stacks, i.e. an inconsistent access qualifier of a task's cell
  data (once found for a write-only first access to the Runge-Kutta estimates).
  Compare the peak memory of two runs with different end times.
- Compile errors in the generated code usually show up in `<work>/<scheme>-<volumes>/generate.log`;
  the output of the simulation is in `run.log` of the same directory.

## Performance

The script records runtime, thread count and peak memory per run. They are part of the table
but never decide on pass or fail. Performance regressions are to be checked with the
`exahype2:performance-test` skill.
