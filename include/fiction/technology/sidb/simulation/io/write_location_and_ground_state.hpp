/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Writer that pairs SiDB positions with their ground state charge distributions.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/simulation/analysis/minimum_energy.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/utils/math/math_utils.hpp"
#include "fmt/format.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <ostream>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace fiction::sidb::simulation::io
{

namespace detail
{

/**
 * Writes the SiDB positions of a `result` and the charge states of every ground state as CSV.
 */
class location_and_ground_state_writer
{
  public:
    location_and_ground_state_writer(const sidb::simulation::result& src, std::ostream& s) : sim_result{src}, os{s} {}

    void run()
    {
        const auto min_energy = fiction::utils::math::round_to_n_decimal_places(
            simulation::analysis::minimum_energy(sim_result.charge_distributions.cbegin(),
                                                 sim_result.charge_distributions.cend()),
            6);

        std::vector<const charge_distribution*> ground_states{};

        for (const auto& cd : sim_result.charge_distributions)
        {
            if (std::fabs(fiction::utils::math::round_to_n_decimal_places(cd.energy(), 6) - min_energy) <
                utils::math::ERROR_MARGIN)
            {
                ground_states.push_back(&cd);
            }
        }

        if (ground_states.empty())
        {
            return;
        }

        os << "x [nm]; y [nm];";

        for (const auto i : std::views::iota(std::size_t{0}, ground_states.size()))
        {
            os << fmt::format("GS_{};", i);
        }
        os << '\n';

        for (std::size_t i = 0; i < sim_result.lyt.num_cells(); ++i)
        {
            const auto pos = sim_result.lyt.get_lattice().nm_position(sim_result.lyt.sidbs()[i]);
            os << fmt::format("{:.3f};{:.3f};", pos.first, pos.second);

            for (const auto* cd : ground_states)
            {
                os << fmt::format("{};", sidb::model::charge_state_to_sign(cd->get_charge_state_by_index(i)));
            }
            os << "\n";
        }
    }

  private:
    const sidb::simulation::result& sim_result;
    std::ostream&                   os;
};

}  // namespace detail

/**
 * Writes the SiDB positions of a simulation result and the charge states of its ground states as CSV to a stream.
 *
 * @param sim_result Result to write.
 * @param os Output stream to write into.
 */
inline void write_location_and_ground_state(const sidb::simulation::result& sim_result, std::ostream& os)
{
    detail::location_and_ground_state_writer p{sim_result, os};

    p.run();
}
/**
 * Writes the SiDB positions of a simulation result and the charge states of its ground states as CSV.
 *
 * @param sim_result Result to write.
 * @param filename File to write into.
 * @throws std::ofstream::failure if the file cannot be opened.
 */
inline void write_location_and_ground_state(const sidb::simulation::result& sim_result,
                                            const std::string_view&         filename)
{
    std::ofstream os{std::string{filename}, std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_location_and_ground_state(sim_result, os);
    os.close();
}

}  // namespace fiction::sidb::simulation::io
