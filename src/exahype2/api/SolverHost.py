# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import abc
import dataclasses
from typing import List

import peano4.api
import peano4.api.taskgraph

from .Solver import Solver


@dataclasses.dataclass
class PhaseGraphs:
    """!

    The task graphs of the four algorithmic phases of an ExaHyPE 2 project

    """

    setup_simulation: peano4.api.taskgraph.Graph = dataclasses.field(
        default_factory=peano4.api.taskgraph.Graph
    )
    time_step: peano4.api.taskgraph.Graph = dataclasses.field(
        default_factory=peano4.api.taskgraph.Graph
    )
    plot: peano4.api.taskgraph.Graph = dataclasses.field(
        default_factory=peano4.api.taskgraph.Graph
    )
    mesh_adaptation: peano4.api.taskgraph.Graph = dataclasses.field(
        default_factory=peano4.api.taskgraph.Graph
    )

    def graphs(self) -> List[peano4.api.taskgraph.Graph]:
        """! The graphs in the order of the arguments of Solver.generate_solver_realisation()"""
        return [self.setup_simulation, self.time_step, self.plot, self.mesh_adaptation]

    def append(self, other: "PhaseGraphs"):
        """! Append each graph of other to the corresponding graph of this object"""
        for mine, theirs in zip(self.graphs(), other.graphs()):
            mine.append(theirs)


class SolverHost(abc.ABC):
    """!

    Hosts several solvers which are coupled to each other

    A host is what a user adds to an exahype2.api.Project instead of its
    solvers (Project.add_solver() accepts it). The project registers the
    host's solvers (solvers) with its solver repository, README and so forth,
    but asks the host, rather than each solver, to realise them. The host
    follows the recipe of @ref page_exahype_programmers_guide_coupling :

    1. Each solver builds its own task graphs (one per phase, PhaseGraphs).
       Each solver reports its own graph, so the user sees what each
       solver contributes.
    2. The host concatenates the graphs of all solvers into graphs of its own
       (Graph.append()), and calls couple(), which adds the dependencies and
       tasks that tie the solvers together.
    3. The host reports the concatenated, coupled graphs once more, and then
       appends them to the graphs of the project.

    Whatever belongs to the mesh as a whole, i.e. the load balancing switch and the
    dynamic mesh refinement, shall exist once: Only the solver in
    global_tasks_solver adds these tasks (@ref
    exahype2.api.Solver.Solver.generate_solver_realisation(), flag
    add_global_tasks). All other solvers leave them to it.

    """

    def __init__(self, name: str, solvers: List[Solver]):
        """!

        @param name: str
          Name of the host. It is used in the titles of the task graphs.

        @param solvers: List[exahype2.api.Solver]
          The solvers in the order in which they are realised. A solver can
          rely on the data of its predecessors.

        """
        if not solvers:
            raise ValueError("A host needs at least one solver")
        self._name = name
        self._solvers = list(solvers)

    @property
    def name(self) -> str:
        return self._name

    @property
    def solvers(self) -> List[Solver]:
        """! The solvers in the order of their realisation"""
        return list(self._solvers)

    @property
    def global_tasks_solver(self) -> Solver:
        """!

        The one solver which adds the global tasks (load balancing switch,
        dynamic mesh refinement and mesh adaptation) to the task graphs. By
        default the first one.

        """
        return self._solvers[0]

    @abc.abstractmethod
    def couple(
        self,
        exahype2_project,
        peano_project,
        own_graphs: dict,
        host_graphs: PhaseGraphs,
    ):
        """!

        Tie the solvers together

        Called after the graphs of the solvers have been concatenated into
        host_graphs. The routine adds the tasks and dependencies that
        couple the solvers. It may look up the tasks of the solvers in
        host_graphs or in the graphs they built (own_graphs).

        @param exahype2_project: exahype2.api.Project
          The project that hosts the solvers.

        @param peano_project: peano4.api.Project
          The Peano project that the exahype2 project assembles.

        @param own_graphs: Dict[exahype2.api.Solver, PhaseGraphs]
          The graphs which each solver has built.

        @param host_graphs: PhaseGraphs
          The concatenation of the graphs of all solvers. The routine
          modifies them.

        """

    def generate_solver_realisation(
        self,
        exahype2_project,
        peano_project,
        setup_simulation,
        time_step,
        plot,
        mesh_adaptation,
    ):
        """!

        Realise all solvers into graphs of their own, couple them, and add the outcome to the project's graphs

        The arguments are those of exahype2.api.Solver.generate_solver_realisation().

        """
        peano4.api.Logger().new_section("Host " + self._name)
        own_graphs = {}
        host_graphs = PhaseGraphs()
        for solver in self._solvers:
            graphs = PhaseGraphs()
            solver.generate_solver_realisation(
                exahype2_project,
                peano_project,
                *graphs.graphs(),
                add_global_tasks=solver is self.global_tasks_solver,
            )
            own_graphs[solver] = graphs
            host_graphs.append(graphs)
        self.couple(exahype2_project, peano_project, own_graphs, host_graphs)

        title = "Time stepping task graph for host " + self._name
        if peano4.api.Logger().is_visualisation:
            host_graphs.time_step.visualise(title=title)
        if peano4.api.Logger().is_text:
            host_graphs.time_step.log_as_text(title=title)
        for graph, project_graph in zip(
            host_graphs.graphs(),
            [setup_simulation, time_step, plot, mesh_adaptation],
        ):
            project_graph.append(graph)
