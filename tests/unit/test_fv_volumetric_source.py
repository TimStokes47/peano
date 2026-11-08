import exahype2.api


def _scheme_with_volumetric_source():
    solver = exahype2.api.Solver("Probe", 2)
    solver.add_pde_term(exahype2.api.VolumetricSource(implementation="S[0] = t;"))
    solver.add_pde_term(exahype2.api.MaxEigenvalue(implementation="*lambdaMax = 1.0;"))
    solver.add_pde_term(exahype2.api.InitPointWisely(implementation="Q[0] = 0.0;"))
    scheme = exahype2.api.fv.RusanovWithOptimisticExplicitEuler(solver, 6)
    solver.numerical_scheme = scheme
    # Normally set once the scheme constructs its output for a project
    scheme.kernel_namespace = ["probe"]
    return scheme


def test_volumetric_source_signature_has_location_and_time_stamp_only():
    assert exahype2.api.VolumetricSource.SIGNATURE == (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "double t, "
        "double* __restrict__ S"
    )


def test_kernel_call_forwards_volumetric_source():
    scheme = _scheme_with_volumetric_source()
    call = scheme._RusanovWithOptimisticExplicitEuler__construct_kernel_call(
        exahype2.api.Target.Serial, False
    )
    assert "repositories::instanceOfProbe.volumetricSource(Q, x, t, S);" in call


def test_optimistic_kernels_evaluate_volumetric_source(tmp_path):
    scheme = _scheme_with_volumetric_source()
    scheme._RusanovWithOptimisticExplicitEuler__create_time_step_functions(
        str(tmp_path)
    )
    kernels = tmp_path / "kernels"
    for name in [
        "Probe_rusanov2d",
        "Probe_rusanov3d",
        "Probe_rusanov2d_patch_interior",
        "Probe_rusanov2d_shell",
        "Probe_rusanov3d_patch_interior",
        "Probe_rusanov3d_shell",
    ]:
        header = (kernels / (name + ".h")).read_text()
        assert (
            "const ::exahype2::fv::rusanov::VolumetricSourceFunctor& volumetricSource"
            in header
        ), name
        assert "volumetricSource(" in (kernels / (name + ".cpph")).read_text(), name
