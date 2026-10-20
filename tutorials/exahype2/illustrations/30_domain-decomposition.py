# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the PNG illustrations of the tutorial 30_domain-decomposition.ipynb

The script builds the tutorial's Euler setup with load balancing, runs it
with four threads, converts the output into VTU files, and lets ParaView's
pvbatch render the subdomains into 30_domain-decomposition-subdomains.png.
It also renders the workflow diagram 30_domain-decomposition-workflow.dot
with Graphviz. The implementation of EulerFV is taken from the tutorial's
%%writefile cell. Call the script from anywhere:

     python3 tutorials/exahype2/illustrations/30_domain-decomposition.py

All builds and outputs go into a temporary directory (or --work-directory).
The script needs Graphviz' dot and ParaView's pvbatch (--pvbatch).

"""
import argparse
import json
import os
import subprocess
import sys
import tempfile

Here = os.path.dirname(os.path.abspath(__file__))
PeanoRoot = os.path.abspath(os.path.join(Here, "..", "..", ".."))
sys.path.insert(0, os.path.join(PeanoRoot, "src"))

import peano4.api
import exahype2

NumberOfThreads = 4
SubdomainsPicture = os.path.join(Here, "30_domain-decomposition-subdomains.png")
WorkflowDiagram = os.path.join(Here, "30_domain-decomposition-workflow")

# Renders the first snapshot of the pvd file argv[1] into the PNG argv[2],
# coloured by the subdomain number, i.e. the last component of the unknowns
ParaViewScript = """
import sys
from paraview.simple import *

reader = PVDReader(FileName=sys.argv[1])
reader.UpdatePipeline(reader.TimestepValues[0] if reader.TimestepValues else 0.0)
array_name = reader.CellData.keys()[0]
data_range = reader.CellData[array_name].GetRange(-1)
subdomain = reader.CellData[array_name].GetNumberOfComponents() - 1

view = GetActiveViewOrCreate("RenderView")
view.OrientationAxesVisibility = 0
view.InteractionMode = "2D"
view.Background = [1.0, 1.0, 1.0]
view.UseColorPaletteForBackground = 0
display = Show(reader, view)
display.Representation = "Surface"
# Flat colours which match the legend
display.Ambient = 1.0
display.Diffuse = 0.0
ColorBy(display, ("CELLS", array_name))
lut = GetColorTransferFunction(array_name)
lut.VectorMode = "Component"
lut.VectorComponent = subdomain
subdomain_range = reader.CellData[array_name].GetRange(subdomain)
subdomains = range(int(subdomain_range[0]), int(subdomain_range[1]) + 1)
lut.InterpretValuesAsCategories = 1
lut.Annotations = [item for number in subdomains for item in (str(number), str(number))]
lut.ApplyPreset("Brewer Qualitative Set2", True)
bar = GetScalarBar(lut, view)
bar.Title = "subdomain"
bar.ComponentTitle = ""
bar.TitleColor = [0.0, 0.0, 0.0]
bar.LabelColor = [0.0, 0.0, 0.0]
display.SetScalarBarVisibility(view, True)
view.ResetCamera()
SaveScreenshot(sys.argv[2], view, ImageResolution=[800, 650])
"""


def euler_implementation():
    """! Body of the tutorial's cell which writes EulerFV.cpp"""
    with open(os.path.join(Here, "..", "30_domain-decomposition.ipynb")) as notebook_file:
        cells = json.load(notebook_file)["cells"]
    for cell in cells:
        source = "".join(cell["source"])
        if source.startswith("%%writefile 30_domain-decomposition/EulerFV.cpp"):
            return source.split("\n", 1)[1]
    raise Exception("tutorial has no cell writing EulerFV.cpp")


def build_and_run(directory, peano_root):
    """! Generate, compile and run the tutorial setup within directory"""
    project = exahype2.api.Project(
        namespace=["tutorials", "exahype2", "domaindecomposition"],
        project_name="Euler",
        directory=directory,
        executable="euler",
    )

    solver = exahype2.api.Solver("EulerFV", 4, 0)
    solver.add_pde_term(exahype2.api.Flux(stateless=False))
    solver.add_pde_term(exahype2.api.MaxEigenvalue(stateless=False))
    solver.add_pde_term(exahype2.api.InitPointWisely(stateless=False))
    solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(solver, 9)
    solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
        solver, volume_size=0.001
    )
    solver.plotter = exahype2.api.fv.PlotAsVTKFile(
        solver, first_plot_time_stamp=0.0, time_in_between_plots=0.01
    )

    project.add_solver(solver)
    project.set_global_simulation_parameters(
        dimensions=2,
        offset=[-0.1, -0.1],
        size=[0.2, 0.2],
        min_end_time=0.02,
        max_end_time=0.02,
        periodic_BC=[True, True, True],
    )
    project.set_load_balancing(
        "toolbox::loadbalancing::strategies::SpreadOutOnceGridStagnates",
        "new ::exahype2::LoadBalancingConfiguration()",
    )
    project.set_Peano4_installation(peano_root, peano4.api.output.CompileMode.RELEASE)
    project.generate_Peano4_project().generate()
    with open(os.path.join(directory, "EulerFV.cpp"), "w") as implementation_file:
        implementation_file.write(euler_implementation())
    subprocess.run(["make", "-j"], cwd=directory, check=True, stdout=subprocess.DEVNULL)
    with open(os.path.join(directory, "run.log"), "w") as log_file:
        subprocess.run(
            ["./euler"],
            cwd=directory,
            check=True,
            stdout=log_file,
            env=dict(os.environ, OMP_NUM_THREADS=str(NumberOfThreads)),
        )


def render_subdomains(directory, pvbatch):
    """! Convert the output into VTU and let ParaView render the subdomains"""
    subprocess.run(
        [
            sys.executable,
            os.path.join(PeanoRoot, "src", "tarch", "plotter", "api", "render.py"),
            "solution-EulerFV.peano-patch-file",
        ],
        cwd=directory,
        env=dict(os.environ, PYTHONPATH=os.path.join(PeanoRoot, "src")),
        check=True,
        stdout=subprocess.DEVNULL,
    )
    script = os.path.join(directory, "render-subdomains.py")
    with open(script, "w") as script_file:
        script_file.write(ParaViewScript)
    if os.path.exists(SubdomainsPicture):
        os.remove(SubdomainsPicture)
    # pvbatch might crash when it shuts down, so we check for the picture
    # rather than for the return code
    subprocess.run(
        [
            pvbatch,
            "--force-offscreen-rendering",
            script,
            os.path.join(directory, "solution-EulerFV.EulerFVQ.pvd"),
            SubdomainsPicture,
        ],
        cwd=directory,
    )
    if not os.path.exists(SubdomainsPicture):
        raise Exception("pvbatch did not write {}".format(SubdomainsPicture))


def render_workflow():
    subprocess.run(
        ["dot", "-Tpng", "-Gdpi=100", WorkflowDiagram + ".dot", "-o", WorkflowDiagram + ".png"],
        check=True,
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the PNGs of the tutorial 30_domain-decomposition.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    parser.add_argument(
        "--peano-root",
        default=PeanoRoot,
        help="Peano installation with the compiled libraries (default: repository holding this script)",
    )
    parser.add_argument("--pvbatch", default="pvbatch", help="ParaView's pvbatch executable")
    args = parser.parse_args()

    render_workflow()
    work_directory = args.work_directory or tempfile.mkdtemp(prefix="30_domain-decomposition-")
    print("build and run in {}".format(work_directory))
    build_and_run(work_directory, os.path.abspath(args.peano_root))
    render_subdomains(work_directory, args.pvbatch)
    print("wrote PNGs into {}".format(Here))
