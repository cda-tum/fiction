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
 * @brief Generates SiDB layouts with randomly placed dots under placement constraints.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"

#include <cstdint>
#include <optional>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction::sidb::generators
{

/**
 * Parameters of the random layout generator.
 */
struct generate_random_layout_params
{
    /**
     * Whether positively charged SiDBs may occur in the generated layout.
     */
    enum class positive_charges : uint8_t
    {
        /**
         * Positive charges are allowed.
         */
        ALLOWED,
        /**
         * Positive charges are forbidden: SiDBs that would enable them are removed again.
         */
        FORBIDDEN,
        /**
         * Positive charges have to be possible: generation is repeated until they are.
         */
        MAY_OCCUR
    };
    /**
     * The area to place SiDBs in, as two opposite corners.
     */
    std::pair<lattice_site, lattice_site> coordinate_pair{};
    /**
     * Number of SiDBs to place.
     */
    uint64_t number_of_sidbs = 0;
    /**
     * Positive charge policy.
     */
    positive_charges positive_sidbs = positive_charges::ALLOWED;
    /**
     * Physical parameters for the positive charge check.
     */
    model::simulation_parameters sim_params{};
    /**
     * Maximum number of placement attempts.
     */
    uint64_t maximal_attempts = static_cast<uint64_t>(10E6);
    /**
     * Number of unique layouts to generate with `generate_multiple_random_layouts`.
     */
    uint64_t number_of_unique_generated_layouts = 1;
    /**
     * Maximum number of generation attempts for multiple layouts.
     */
    uint64_t maximal_attempts_for_multiple_layouts = 1'000'000;
};

namespace detail
{

/**
 * The generator's random source, one per thread.
 *
 * @return The generator.
 */
[[nodiscard]] inline std::mt19937_64& random_generator() noexcept
{
    thread_local std::mt19937_64 generator{std::random_device{}()};

    return generator;
}

}  // namespace detail

/**
 * Generates a random SiDB layout by placing SiDBs at random sites of an area, optionally on top of a skeleton. The
 * skeleton's lattice, cells, and defects carry over; sites that hold a defect or that a neutral defect affects are
 * left empty. Depending on the positive charge policy, SiDBs that would allow positive charges are removed again,
 * or the generation is repeated until positive charges are possible.
 *
 * @param params Parameters.
 * @param skeleton The skeleton to place SiDBs on, if any.
 * @return The generated layout, or `std::nullopt` if not all SiDBs could be placed within the attempt limit.
 */
[[nodiscard]] inline std::optional<layout> generate_random_layout(const generate_random_layout_params& params,
                                                                  const std::optional<layout>& skeleton = std::nullopt)
{
    std::unordered_set<lattice_site> sidbs_affected_by_defects{};

    uint64_t number_of_sidbs_of_final_layout = params.number_of_sidbs;

    layout lyt{};

    if (skeleton.has_value())
    {
        lyt = *skeleton;
        number_of_sidbs_of_final_layout += lyt.num_cells();
        sidbs_affected_by_defects = skeleton->all_affected_sidbs(std::make_pair(uint16_t{0}, uint16_t{0}));
    }

    const auto cell_type =
        skeleton.has_value() ? sidb_technology::cell_type::LOGIC : sidb_technology::cell_type::NORMAL;

    uint64_t attempt = 0;
    while (attempt < params.maximal_attempts)
    {
        if (lyt.num_cells() == number_of_sidbs_of_final_layout)
        {
            if (params.positive_sidbs != generate_random_layout_params::positive_charges::MAY_OCCUR ||
                simulation::analysis::can_positive_charges_occur(lyt, params.sim_params))
            {
                return lyt;
            }

            if (params.number_of_sidbs == 0)
            {
                return std::nullopt;
            }

            lyt = skeleton.value_or(layout{});
            continue;
        }

        const auto random_site = random_site_in_area(params.coordinate_pair.first, params.coordinate_pair.second,
                                                     detail::random_generator());
        ++attempt;

        // a defect occupies or affects the site
        if (sidbs_affected_by_defects.contains(random_site) ||
            lyt.get_defect(random_site).type != model::defect_type::NONE || !lyt.is_empty_cell(random_site))
        {
            continue;
        }

        lyt.assign_cell_type(random_site, cell_type);

        if (params.positive_sidbs == generate_random_layout_params::positive_charges::FORBIDDEN &&
            simulation::analysis::can_positive_charges_occur(lyt, params.sim_params))
        {
            lyt.assign_cell_type(random_site, sidb_technology::cell_type::EMPTY);
        }
    }

    if (lyt.num_cells() == number_of_sidbs_of_final_layout &&
        (params.positive_sidbs != generate_random_layout_params::positive_charges::MAY_OCCUR ||
         simulation::analysis::can_positive_charges_occur(lyt, params.sim_params)))
    {
        return lyt;
    }

    return std::nullopt;
}
/**
 * Generates several unique random SiDB layouts with `generate_random_layout`.
 *
 * @param params Parameters; `number_of_unique_generated_layouts` layouts are requested.
 * @param skeleton The skeleton to place SiDBs on, if any.
 * @return The layouts, or `std::nullopt` if none could be generated within the attempt limit.
 */
[[nodiscard]] inline std::optional<std::vector<layout>>
generate_multiple_random_layouts(const generate_random_layout_params& params,
                                 const std::optional<layout>&         skeleton = std::nullopt)
{
    std::vector<layout> unique_lyts{};
    unique_lyts.reserve(params.number_of_unique_generated_layouts);

    std::unordered_set<layout> seen{};

    for (uint64_t unsuccessful = 0; unique_lyts.size() < params.number_of_unique_generated_layouts &&
                                    unsuccessful < params.maximal_attempts_for_multiple_layouts;)
    {
        if (auto random_lyt = generate_random_layout(params, skeleton);
            random_lyt.has_value() && seen.insert(*random_lyt).second)
        {
            unique_lyts.push_back(std::move(*random_lyt));
            continue;
        }

        ++unsuccessful;
    }

    return unique_lyts.empty() ? std::nullopt : std::optional{std::move(unique_lyts)};
}
}  // namespace fiction::sidb::generators
