import exahype2.api
import exagrype.api


def test_fo_and_so_solver_are_equivalent_for_the_time_being():
    first_order = exagrype.api.FOCCZ4Solver("CCZ4")
    second_order = exagrype.api.FOSOCCZ4Solver("CCZ4")
    assert second_order.unknowns == first_order.unknowns == 59
    assert second_order.variable_names_per_entry() == first_order.variable_names_per_entry()
    assert {t.name for t in second_order.pde_terms} == {
        t.name for t in first_order.pde_terms
    }
    assert (
        second_order.get_pde_term("nonconservativeProduct").implementation
        == first_order.get_pde_term("nonconservativeProduct").implementation
    )
