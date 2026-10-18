from peano4.api.output.Helper import shortened_class_name


def test_short_class_name_is_unchanged():
    assert shortened_class_name("TimeStep_Update_Cell") == "TimeStep_Update_Cell"


def test_long_class_names_are_cut_off_and_numbered_consistently():
    first = "A" * 300
    second = "B" * 300
    shortened_first = shortened_class_name(first)
    shortened_second = shortened_class_name(second)
    assert len(shortened_first) <= 200
    assert len(shortened_second) <= 200
    assert shortened_first != shortened_second
    # The same long name always yields the same short one
    assert shortened_class_name(first) == shortened_first
    # The short names end in a consecutive number
    assert int(shortened_second.rsplit("_", 1)[1]) == int(shortened_first.rsplit("_", 1)[1]) + 1


def test_step_cuts_observer_class_name_down_to_64_characters():
    import peano4.api.solversteps

    short = peano4.api.solversteps.Step(name="TimeStep_Update_Cell", add_user_defined_actions=False)
    long = peano4.api.solversteps.Step(name="TimeStep_" + "Update_Cell__" * 10, add_user_defined_actions=False)
    assert short.get_observer_class_name() == "TimeStep_Update_Cell"
    assert len(long.get_observer_class_name()) <= 64
    assert long.get_observer_class_name() == long.get_observer_class_name()
