//
// Created by Jan Drewniok on 12.08.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp>
#include <fiction/layouts/cell_level_layout.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("One BDL pair with one perturber", "[can-positive-charges-occur]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);

    SECTION("Default values")
    {
        const sidb_simulation_parameters params{2, -0.32};
        CHECK(!can_positive_charges_occur<TestType>(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 1")
    {
        const sidb_simulation_parameters params{2, -0.32, 1, 1};
        CHECK(can_positive_charges_occur<TestType>(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 10")
    {
        const sidb_simulation_parameters params{2, -0.32, 1, 10};
        CHECK(can_positive_charges_occur<TestType>(lyt, params));
    }
}

TEMPLATE_TEST_CASE("Y-shape SiDB OR gate with input 01, using siqad coordinates", "[can-positive-charges-occur]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

    SECTION("Default values")
    {
        const sidb_simulation_parameters params{2, -0.32};
        CHECK(!can_positive_charges_occur<TestType>(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 1")
    {
        const sidb_simulation_parameters params{2, -0.32, 1, 1};
        CHECK(can_positive_charges_occur<TestType>(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 10")
    {
        const sidb_simulation_parameters params{2, -0.32, 1, 10};
        CHECK(can_positive_charges_occur<TestType>(lyt, params));
    }
}

TEMPLATE_TEST_CASE("Y-shape SiDB OR gate with input 01, using cube coordinates", "[can-positive-charges-occur]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 2, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{8, 3, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{12, 3, 0}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 2, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 5, 0}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 6, 1}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 8, 1}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{16, 1, 0}), TestType::cell_type::NORMAL);

    SECTION("Default values")
    {
        const sidb_simulation_parameters params{2, -0.32};
        CHECK(!can_positive_charges_occur<TestType>(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 1")
    {
        const sidb_simulation_parameters params{2, -0.32, 1, 1};
        CHECK(can_positive_charges_occur<TestType>(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 10")
    {
        const sidb_simulation_parameters params{2, -0.32, 1, 10};
        CHECK(can_positive_charges_occur<TestType>(lyt, params));
    }
}

TEMPLATE_TEST_CASE("Y-shape SiDB OR gate with input 01, using offset coordinates", "[can-positive-charges-occur]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{6, 2, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{8, 3, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{12, 3, 0}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{14, 2, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 5, 0}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 6, 1}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 8, 1}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{16, 1, 0}),
                         TestType::cell_type::NORMAL);

    SECTION("Default values")
    {
        const sidb_simulation_parameters params{2, -0.32};
        CHECK(!can_positive_charges_occur<TestType>(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 1")
    {
        const sidb_simulation_parameters params{2, -0.32, 1, 1};
        CHECK(can_positive_charges_occur<TestType>(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 10")
    {
        const sidb_simulation_parameters params{2, -0.32, 1, 10};
        CHECK(can_positive_charges_occur<TestType>(lyt, params));
    }
}
