# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

@page benchmarks_exahype2_euler_point_source Euler equations with a point source and dynamic AMR

Generates and builds the point source benchmark
(@ref benchmarks_exahype2_euler_point_source). A gas at rest is heated by an
energy source at the origin. The source is a narrow Gaussian injected through
the volumetric source term. We have multiple levels of AMR which follow the 
point source.

"""
import argparse
import os

import peano4.api
import exahype2.api


parser = argparse.ArgumentParser(
    description="Euler equations with a point source (narrow Gaussian volumetric source)"
)
parser.add_argument(
    "--output-directory",
    default=".",
    help="Directory the code is generated and built in (default: .)",
)
parser.add_argument(
    "--amr-levels",
    type=int,
    default=0,
    help="Number of adaptive mesh refinement levels (default: 0). With 0, the "
    "simulation runs on the regular grid of the smallest volume size. With "
    "l>0, it starts from the same regular grid, but the gradient-based "
    "refinement criterion is on and coarsens the mesh by up to l levels (a "
    "factor of 3 each)",
)
parser.add_argument(
    "--volume-size",
    type=float,
    default=None,
    help="Smallest volume size, i.e. the size on the regular start grid. By "
    "default, the script picks the finest regular grid that fits into the "
    "main memory (see --memory-fraction)",
)
parser.add_argument(
    "--memory-fraction",
    type=float,
    default=0.8,
    help="Fraction of the available main memory that the start grid may "
    "occupy if --volume-size is not given (default: 0.8)",
)
parser.add_argument(
    "--patch-size",
    type=int,
    default=None,
    help="Volumes per mesh cell and axis. By default, the script picks the "
    "patch size such that the regular start grid fills the memory best",
)
parser.add_argument(
    "--refine-threshold",
    type=float,
    default=2.0,
    help="Mesh cells ask for refinement if the one-sided gradient of density "
    "plus energy exceeds this value (default: 2)",
)
parser.add_argument(
    "--erase-threshold",
    type=float,
    default=0.2,
    help="Mesh cells ask for coarsening if the one-sided gradient of density "
    "plus energy falls below this value (default: 0.2)",
)
parser.add_argument(
    "--end-time", type=float, default=0.2, help="Simulated time (default: 0.2)"
)
parser.add_argument(
    "--plots", type=int, default=20, help="Number of snapshots (default: 20)"
)
parser.add_argument(
    "--source-width",
    type=float,
    default=0.02,
    help="Standard deviation of the Gaussian that models the point (default: 0.02)",
)
parser.add_argument(
    "--source-strength",
    type=float,
    default=100.0,
    help="Energy per time and volume at the origin (default: 100)",
)
parser.add_argument(
    "--dimensions", type=int, default=2, choices=[2, 3], help="Dimension (default: 2)"
)
parser.add_argument(
    "--optimistic",
    action="store_true",
    help="Use the optimistic Finite Volume scheme",
)
parser.add_argument(
    "--build-mode",
    default="release",
    choices=["release", "debug"],
    help="Peano build flavour to link against (default: release)",
)
parser.add_argument(
    "--peano-root",
    default=os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."
    ),
    help="Configured and built Peano directory (default: the repository this script resides in)",
)
parser.add_argument(
    "--verbosity",
    default="visualisation",
    choices=["visualisation", "text"],
    help="Show the task graphs in a window or print them as text (default: visualisation)",
)
args = parser.parse_args()


## Estimated memory footprint of one volume in bytes (per dimension count).
#
# Patch data with halo, the time step's copies, the face data, and the
# bookkeeping of the spacetree. This is an estimate which is refined by
# measurement (see the script header).
BYTES_PER_VOLUME = {2: 600.0, 3: 900.0}


def available_memory_in_bytes() -> float:
    """!
    Main memory that is available right now (MemAvailable in /proc/meminfo)
    """
    with open("/proc/meminfo") as meminfo:
        for line in meminfo:
            if line.startswith("MemAvailable:"):
                return float(line.split()[1]) * 1024.0
    raise RuntimeError("cannot read MemAvailable from /proc/meminfo")


def pick_regular_grid(dimensions: int, budget_in_bytes: float, patch_sizes):
    """!
    Pick the finest regular start grid that fits into the budget

    A regular grid over the unit cube consists of 3^k cells per axis, each
    hosting a patch of p volumes per axis. This yields p*3^k volumes per
    axis. The patch size p is free within a range, which makes the grid
    granularity much finer than the factor 3^d between subsequent k.

    @return (patch_size, k)
    """
    best = None
    for patch_size in patch_sizes:
        k = 0
        while (patch_size * 3 ** (k + 1)) ** dimensions * BYTES_PER_VOLUME[
            dimensions
        ] <= budget_in_bytes:
            k += 1
        volumes = (patch_size * 3**k) ** dimensions
        if volumes * BYTES_PER_VOLUME[dimensions] <= budget_in_bytes and (
            best is None or volumes > best[0]
        ):
            best = (volumes, patch_size, k)
    return best[1], best[2]


if args.volume_size is None:
    patch_size, k = pick_regular_grid(
        args.dimensions,
        args.memory_fraction * available_memory_in_bytes(),
        [args.patch_size] if args.patch_size is not None else range(6, 33),
    )
    # The setup refines every cell whose size is not smaller than
    # volume_size * patch_size. The factor makes the cell 3^-k the final one.
    volume_size = 1.001 / (patch_size * 3**k)
else:
    patch_size = args.patch_size if args.patch_size is not None else 9
    volume_size = args.volume_size
print(
    f"Start grid: {patch_size} volumes per cell and axis, volume size {volume_size}, "
    f"AMR levels {args.amr_levels}"
)


project = exahype2.api.Project(
    namespace=["benchmarks", "exahype2", "euler", "point_source"],
    project_name="point-source",
    directory=args.output_directory,
    executable="point-source",
)

solver = exahype2.api.Solver("EulerPointSource", {"rho": 1, "j": args.dimensions, "E": 1})
solver.add_user_solver_includes(
    """
#include <algorithm>
#include <cmath>
"""
)

PRESSURE = """
      constexpr double gamma = 1.4;
      const double irho = 1.0 / Q[0];
      double kineticEnergy = 0.0;
      for (int d=0; d<Dimensions; d++) {
        kineticEnergy += 0.5 * irho * Q[1+d] * Q[1+d];
      }
      const double p = (gamma - 1.0) * (Q[Dimensions+1] - kineticEnergy);
"""

solver.add_pde_term(
    exahype2.api.Flux(
        implementation=PRESSURE
        + """
      const double velocity = Q[1+normal] * irho;
      F[0] = Q[1+normal];
      for (int d=0; d<Dimensions; d++) {
        F[1+d] = velocity * Q[1+d];
      }
      F[1+normal] += p;
      F[Dimensions+1] = velocity * (Q[Dimensions+1] + p);
"""
    )
)
solver.add_pde_term(
    exahype2.api.MaxEigenvalue(
        implementation=PRESSURE
        + """
      const double c = std::sqrt(gamma * p * irho);
      const double u = Q[1+normal] * irho;
      lambdaMax[0] = std::max(std::abs(u + c), std::abs(u - c));
"""
    )
)

# The point is smeared over a few grid cells, as a delta distribution cannot be
# represented by a Finite Volume scheme. Only the energy equation is fed.
solver.add_pde_term(
    exahype2.api.VolumetricSource(
        implementation=f"""
      constexpr double sigma    = {args.source_width};
      constexpr double strength = {args.source_strength};
      for (int i=0; i<NumberOfUnknowns; i++) {{
        S[i] = 0.0;
      }}
      S[Dimensions+1] = strength * std::exp(-0.5 * tarch::la::norm2Squared(x) / (sigma * sigma));
"""
    )
)

# Ambient state: gas at rest with density 1 and pressure 1
AMBIENT_STATE = """
      constexpr double gamma = 1.4;
      Q[0] = 1.0;
      for (int d=0; d<Dimensions; d++) {
        Q[1+d] = 0.0;
      }
      Q[Dimensions+1] = 1.0 / (gamma - 1.0);
"""
solver.add_pde_term(exahype2.api.InitPointWisely(implementation=AMBIENT_STATE))
solver.add_pde_term(exahype2.api.DirichletBoundaryConditions(implementation=AMBIENT_STATE))

# With AMR, the simulation starts from the finest regular grid, and the
# gradient criterion is always on. It may only coarsen: the start grid
# is the finest grid that we allow, and the coarsest volumes are those
# amr_levels levels (factor 3 each) above the start volumes.
if args.amr_levels > 0:
    solver.add_pde_term(
        exahype2.api.GradientRefinement(
            implementation=f"""
      const double indicator = std::abs(gradient[0]) + std::abs(gradient[Dimensions+1]);
      if (indicator > {args.refine_threshold}) {{
        *refinementCommand = ::exahype2::RefinementCommand::Refine;
      }}
      else if (indicator < {args.erase_threshold}) {{
        *refinementCommand = ::exahype2::RefinementCommand::Erase;
      }}
"""
        )
    )
amr_bounds = dict(
    min_volume_size=volume_size,
    max_volume_size=volume_size * 3**args.amr_levels,
)
if args.optimistic:
    solver.numerical_scheme = exahype2.api.fv.RusanovWithOptimisticExplicitEuler(
        solver, patch_size, **amr_bounds
    )
else:
    solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(
        solver, patch_size, **amr_bounds
    )
solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
    solver, volume_size=volume_size
)
solver.plotter = exahype2.api.fv.PlotAsVTKFile(
    solver,
    first_plot_time_stamp=0.0,
    time_in_between_plots=args.end_time / args.plots,
)
project.add_solver(solver)

project.set_global_simulation_parameters(
    dimensions=args.dimensions,
    offset=[-0.5] * args.dimensions,
    size=[1.0] * args.dimensions,
    min_end_time=args.end_time,
    max_end_time=args.end_time,
    periodic_BC=[False] * 3,
)
project.set_load_balancing(
    "toolbox::loadbalancing::strategies::SpreadOutOnceGridStagnates",
    "new ::exahype2::LoadBalancingConfiguration()",
)
project.compiler = (
    peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate_with_fusion
)

project.set_Peano4_installation(
    args.peano_root,
    peano4.api.output.CompileMode.RELEASE
    if args.build_mode == "release"
    else peano4.api.output.CompileMode.DEBUG,
)
peano4_project = project.generate_Peano4_project(
    verbosity=peano4.api.LoggerVerbosity.TEXT
    if args.verbosity == "text"
    else peano4.api.LoggerVerbosity.VISUALISATION
)
peano4_project.generate()
peano4_project.build(make_clean_first=True)
