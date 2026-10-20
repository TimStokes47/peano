# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the PNG illustration of the tutorial 35_offline-load-balancing.ipynb

The script computes the offline decomposition of the tutorial's regular
mesh, builds the tutorial's Euler setup with this hardcoded load balancing,
runs it with four threads, and lets ParaView's pvbatch render the subdomains
into 35_offline-load-balancing-subdomains.png. It reuses the build and
render steps of 30_domain-decomposition.py, as the tutorial builds upon the
setup of 30_domain-decomposition.ipynb. Call the script from anywhere:

     python3 tutorials/exahype2/illustrations/35_offline-load-balancing.py

All builds and outputs go into a temporary directory (or --work-directory).
The script needs ParaView's pvbatch (--pvbatch).

"""
import argparse
import importlib.util
import os
import tempfile

Here = os.path.dirname(os.path.abspath(__file__))

# The file name of the script starts with a digit, i.e. we cannot import it
_specification = importlib.util.spec_from_file_location(
    "domain_decomposition_illustrations", os.path.join(Here, "30_domain-decomposition.py")
)
domain_decomposition = importlib.util.module_from_spec(_specification)
_specification.loader.exec_module(domain_decomposition)

import toolbox.loadbalancing.api.input.RegularGridGenerator as RegularGridGenerator
import toolbox.loadbalancing.api.output.Hardcoded as Hardcoded
import toolbox.loadbalancing.api.strategies.Decomposition as Decomposition
import toolbox.loadbalancing.api.strategies.SplitTrees as SplitTrees

SubdomainsPicture = os.path.join(Here, "35_offline-load-balancing-subdomains.png")


def offline_decomposition():
    """! Arguments of the hardcoded load balancing, as computed by the tutorial"""
    tree = RegularGridGenerator.RegularGridGenerator(max_level=3, dimensions=2)
    SplitTrees.split_tree(
        tree,
        decomposition=Decomposition.equidistant_splitting(
            peano_tree=tree, number_of_subtrees=domain_decomposition.NumberOfThreads
        ),
    )
    destination_ranks = Hardcoded.assign_trees_to_ranks_round_Robin(
        number_of_subtrees=tree.number_of_subtrees, max_ranks=1
    )
    configuration, mode = Hardcoded.create_constructor_initialisation_string(
        tree, destination_ranks
    )
    return configuration


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the PNG of the tutorial 35_offline-load-balancing.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    parser.add_argument(
        "--peano-root",
        default=domain_decomposition.PeanoRoot,
        help="Peano installation with the compiled libraries (default: repository holding this script)",
    )
    parser.add_argument("--pvbatch", default="pvbatch", help="ParaView's pvbatch executable")
    args = parser.parse_args()

    work_directory = args.work_directory or tempfile.mkdtemp(prefix="35_offline-load-balancing-")
    print("build and run in {}".format(work_directory))
    domain_decomposition.build_and_run(
        work_directory,
        os.path.abspath(args.peano_root),
        tutorial="35_offline-load-balancing",
        namespace="offlineloadbalancing",
        load_balancer="toolbox::loadbalancing::strategies::Hardcoded",
        load_balancer_arguments=offline_decomposition(),
    )
    domain_decomposition.render_subdomains(work_directory, args.pvbatch, SubdomainsPicture)
    print("wrote PNG into {}".format(Here))
