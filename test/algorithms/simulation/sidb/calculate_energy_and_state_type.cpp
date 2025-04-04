//
// Created by Jan Drewniok on 03.04.25.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp>
#include <fiction/algorithms/simulation/sidb/energy_distribution.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <vector>

using namespace fiction;

using layout = sidb_cell_clk_lyt_siqad;

TEST_CASE("Single SiDB", "[calculate-energy-and-state-type]")
{
    const auto bestagon_AND = blueprints::bestagon_and_gate<layout>();

    auto bii = bdl_input_iterator{bestagon_AND};

    // set input index 1
    bii = 1;

    const auto output_bdls = detect_bdl_pairs(*bii, technology<layout>::cell_type::OUTPUT);

    const auto simulation_results = quickexact(*bii);

    const auto simulated_charge_distributions = simulation_results.charge_distributions;

    REQUIRE(!simulated_charge_distributions.empty());

    const auto energy_distribution = calculate_energy_distribution(simulated_charge_distributions);

    const auto energy_state_with_info = calculate_energy_and_state_type_with_kinks_accepted(
        energy_distribution, simulated_charge_distributions, output_bdls, std::vector<tt>{create_and_tt()}, 1);

    CHECK(energy_state_with_info.size() == 8);
}
