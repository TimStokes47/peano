# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os
import sys
import argparse

import tarch.plotter.api.output

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Peano 4 - Probe Script. Extract the values at one point over time from a Peano patch file into a csv file. The first column is the time stamp, the remaining columns are the unknowns of all data sets. There is no interpolation: we take the value of the finest cell (cell data) or nearest dof (vertex data)."
    )
    parser.add_argument(
        "--point",
        type=float,
        nargs="+",
        required=True,
        help="Coordinates of probe point (2 or 3 entries; z is ignored for 2d data)",
    )
    parser.add_argument(
        "-o",
        "--output",
        default="probe.csv",
        help="Name of the output csv file",
    )
    parser.add_argument(
        "--set-identifier",
        default="",
        help="Probe only this data set (default: all data sets in file)",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        default=False,
        help="Run in a chatty mode",
    )
    parser.add_argument(
        "-s",
        "--single-file",
        action="store_true",
        default=False,
        help="Input file is single snapshot (and not meta file). Time stamp is then 0",
    )
    parser.add_argument(
        "-j",
        "--max-workers",
        type=int,
        nargs="?",
        const=None,
        default=1,
        help="Number of workers processing the snapshots in parallel",
    )
    parser.add_argument(
        "-start",
        "--start-snapshot-index",
        dest="start",
        type=int,
        help="First snapshot to probe, default is 0",
        default=0,
    )
    parser.add_argument(
        "-end",
        "--end-snapshot-index",
        dest="end",
        type=int,
        help="Last snapshot to probe, default is -1 (last snapshot)",
        default=-1,
    )
    parser.add_argument(dest="filename", help="Input file name")
    args = parser.parse_args()

    if not os.path.exists(args.filename):
        raise Exception(
            "Specified input file '{}' does not exist, exiting...".format(args.filename)
        )
    if len(args.point) not in [2, 3]:
        raise Exception("--point requires 2 or 3 coordinates")
    if (args.start > args.end and args.end != -1) or args.start < 0 or args.end < -1:
        raise Exception("Specified snapshot index is wrong, please check.")

    probe = tarch.plotter.api.output.PointProbe(
        file_name=args.filename,
        point=args.point,
        output_file=args.output,
        start=args.start,
        end=args.end if args.end != -1 else sys.maxsize,
        verbose=args.verbose,
    )
    probe.identifier = args.set_identifier

    if args.single_file:
        probe.display_single_file()
    else:
        probe.display(args.max_workers)
