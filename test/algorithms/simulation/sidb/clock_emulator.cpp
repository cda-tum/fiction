//
// Created by GitHub Copilot on 26.02.26.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/clock_emulator.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Clock emulator instantiation test", "[clock-emulator]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    const auto result = emulate_clocks(lyt, 4);

    CHECK(result.clock_phase_results.size() == 4);
}
