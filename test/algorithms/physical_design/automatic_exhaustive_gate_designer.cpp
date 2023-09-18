//
// Created by Jan Drewniok on 12.09.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/physical_design/automatic_exhaustive_gate_designer.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

using namespace fiction;

TEST_CASE("Use SiQAD XNOR skeleton and generate SiQAD XNOR gate", "[automatic_exhaustive_gate_designer]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 9, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 10, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 12, 1}, sidb_technology::cell_type::NORMAL);

    CHECK(lyt.num_cells() == 13);

    const automatic_exhaustive_gate_designer_params<tt> params{sidb_simulation_parameters{2, -0.32},
                                                               {{10, 4, 0}, {10, 5, 1}},
                                                               {create_xnor_tt()},
                                                               1,
                                                               sidb_simulation_engine::QUICKEXACT};

    const auto found_gate_layouts = automatic_exhaustive_gate_designer(lyt, params);
    REQUIRE(found_gate_layouts.size() == 1);
    CHECK(found_gate_layouts[0].num_cells() == 14);
    CHECK(found_gate_layouts[0].get_cell_type({10, 4, 0}) == layout::technology::NORMAL);
}

TEST_CASE("Use SiQAD's AND gate skeleton to generate all possible AND gate implementations",
          "[automatic_exhaustive_gate_designer]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{};

    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 9, 1}, sidb_technology::cell_type::NORMAL);

    const automatic_exhaustive_gate_designer_params<tt> params{sidb_simulation_parameters{2, -0.28},
                                                               {{4, 4, 0}, {14, 5, 1}},
                                                               {create_and_tt()},
                                                               1,
                                                               sidb_simulation_engine::EXGS};

    const auto found_gate_layouts = automatic_exhaustive_gate_designer(lyt, params);
    CHECK(!found_gate_layouts.empty());
}

TEST_CASE("Use fo2 Bestagon gate without SiDB at {17, 11, 0} and generate original one",
          "[automatic_exhaustive_gate_designer]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    // SiDB, originally part of the Bestagon fo2 gate, is excluded.
    // lyt.assign_cell_type({17, 11, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({21, 11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 13, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 7, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 15, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 19, 0}, sidb_technology::cell_type::NORMAL);

    CHECK(lyt.get_cell_type({17, 11, 0}) == layout::technology::EMPTY);

    // generate gate by placing one SiDB
    const automatic_exhaustive_gate_designer_params<tt> params{sidb_simulation_parameters{2, -0.32},
                                                               {{17, 11, 0}, {17, 11, 0}},
                                                               create_fan_out_tt(),
                                                               1,
                                                               sidb_simulation_engine::QUICKEXACT};

    const auto found_gate_layouts = automatic_exhaustive_gate_designer(lyt, params);
    REQUIRE(found_gate_layouts.size() == 1);
    CHECK(found_gate_layouts[0].num_cells() == 21);
    CHECK(found_gate_layouts[0].get_cell_type({17, 11, 0}) == layout::technology::NORMAL);
}
