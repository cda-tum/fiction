//
// Created by Jan Drewniok on 24.10.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/parameterized_gate_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Parameterized gate library traits", "[parameterized-gate-library]")
{
    CHECK(!has_post_layout_optimization_v<parameterized_gate_library, qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<parameterized_gate_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<parameterized_gate_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<parameterized_gate_library, cart_gate_clk_lyt>);
}
