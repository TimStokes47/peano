"""

Extract a profile along a line from the checkpoints of the single black hole

The checkpoints of single-black-hole.py are the snapshots of the patch file
solution-CCZ4.peano-patch-file. The script samples one snapshot at points
equidistantly distributed along a line from --start to --end with Peano's
PointProbe and writes one csv file with the columns x, y, z, r (the distance
to the puncture), followed by all unknowns found in the checkpoint. The
default line is the x-axis shifted by half a volume in y and z, as the
puncture itself sits on a volume face and the initial data is singular
there.

    python3 probe-single-black-hole.py --snapshot -1 --start -20 0.1 0.1 --end 20 0.1 0.1 --samples 400

Peano's PointProbe takes the value of the volume or grid cell that contains
the sampling point, i.e. it does not interpolate. It parses the patch files
once per sampling point, so keep --samples moderate for large checkpoints.
With --list, the script only prints the time stamps of the snapshots.

"""

import argparse
import csv
import math
import os
import sys
import tempfile

import tarch.plotter.api.output


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Profile along a line through a single black hole checkpoint",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--directory",
        default=".",
        help="Directory holding the checkpoints, i.e. the output directory of single-black-hole.py.",
    )
    parser.add_argument(
        "--file",
        default="solution-CCZ4.peano-patch-file",
        help="Meta file of the checkpoints within --directory.",
    )
    parser.add_argument(
        "--snapshot",
        type=int,
        default=-1,
        help="Index of the checkpoint (0 is the first one). Negative values count from the end, i.e. -1 is the latest checkpoint.",
    )
    parser.add_argument(
        "--start",
        type=float,
        nargs=3,
        default=[-20.0, 0.1, 0.1],
        metavar=("X", "Y", "Z"),
        help="Start of the line.",
    )
    parser.add_argument(
        "--end",
        type=float,
        nargs=3,
        default=[20.0, 0.1, 0.1],
        metavar=("X", "Y", "Z"),
        help="End of the line.",
    )
    parser.add_argument("--samples", type=int, default=200, help="Points along the line.")
    parser.add_argument(
        "--output",
        default="profile.csv",
        help="Name of the csv file with the profile.",
    )
    parser.add_argument(
        "--list",
        action="store_true",
        help="Only list the time stamps of the checkpoints.",
    )
    arguments = parser.parse_args()
    if arguments.samples < 2:
        parser.error("--samples has to be at least 2")
    return arguments


def snapshot_range(arguments):
    """! (start, end) of the PointProbe selecting the one snapshot requested"""
    if arguments.snapshot < 0:
        return arguments.snapshot, sys.maxsize
    return arguments.snapshot, arguments.snapshot


def list_snapshots(file_name):
    probe = tarch.plotter.api.output.PointProbe(
        file_name=file_name, point=[0.0, 0.0, 0.0], output_file=os.devnull, start=0, end=sys.maxsize
    )
    probe.read_metadata()
    for i, snapshot in enumerate(probe.snapshots):
        print(f"{i}: t={snapshot.timestamp}")


def probe_line(arguments, file_name):
    """!

    Probe all sampling points and return the rows (x, y, z, r, time, unknowns...)
    along with the header of the unknowns

    """
    start, end = snapshot_range(arguments)
    rows = []
    header = None
    with tempfile.TemporaryDirectory() as scratch:
        for i in range(arguments.samples):
            lambda_ = i / (arguments.samples - 1)
            point = [
                a + lambda_ * (b - a) for a, b in zip(arguments.start, arguments.end)
            ]
            output = os.path.join(scratch, "probe.csv")
            probe = tarch.plotter.api.output.PointProbe(
                file_name=file_name, point=point, output_file=output, start=start, end=end
            )
            probe.display()
            with open(output) as csv_file:
                reader = csv.reader(csv_file)
                header = next(reader)
                values = next(reader)
            rows.append(point + [math.sqrt(sum(x * x for x in point))] + values)
    return ["x", "y", "z", "r"] + header, rows


def main():
    arguments = parse_arguments()
    file_name = os.path.join(arguments.directory, arguments.file)
    if arguments.list:
        list_snapshots(file_name)
        return

    header, rows = probe_line(arguments, file_name)
    with open(arguments.output, "w", newline="") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(header)
        writer.writerows(rows)
    print(f"Wrote {len(rows)} sampling points (t={rows[0][4]}) into {arguments.output}")


if __name__ == "__main__":
    main()
