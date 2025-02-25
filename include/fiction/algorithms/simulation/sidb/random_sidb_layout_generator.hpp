//
// Created by Jan Drewniok on 05.04.23.
//

#ifndef FICTION_RANDOM_SIDB_LAYOUT_GENERATOR_HPP
#define FICTION_RANDOM_SIDB_LAYOUT_GENERATOR_HPP

#include "fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/execution_utils.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <cstdint>
#include <optional>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * This struct stores the parameters for the `generate_random_sidb_layout` algorithm.
 */
template <typename CoordinateType>
struct generate_random_sidb_layout_params
{
    /**
     * An enumeration of modes to use for the generation of random SiDB layouts to control control the appearance of
     * positive charges.
     */
    enum class positive_charges : uint8_t
    {
        /**
         * Positive charges can occur (i.e. SiDBs can be placed right next to each other).
         */
        ALLOWED,
        /**
         * Positive charges are not allowed to occur (i.e. SiDBs need to be seperated by a few lattice points).
         */
        FORBIDDEN,
        /**
         * Positive charges can occur, which means that the `can_positive_charges_occur` function returns `true`.
         */
        MAY_OCCUR
    };
    /**
     * Two coordinates that span the region where SiDBs may be placed (order is not important). The first coordinate is
     * the upper left corner and the second coordinate is the lower right corner of the area.
     */
    std::pair<CoordinateType, CoordinateType> coordinate_pair;
    /**
     * Number of SiDBs that are placed on the layout.
     */
    uint64_t number_of_sidbs = 0;
    /**
     * If positively charged SiDBs should be prevented, SiDBs are not placed closer than the minimal_spacing.
     */
    positive_charges positive_sidbs = positive_charges::ALLOWED;
    /**
     * Simulation parameters.
     */
    sidb_simulation_parameters simulation_parameters{};
    /**
     * Maximum number of steps to place the specified number of SiDBs. Example: If the area, where SiDBs can be placed,
     * is small and many SiDBs are to be placed, several tries are required to generate a layout with no positively
     * charged SiDBs.
     */
    uint64_t maximal_attempts = static_cast<uint64_t>(10E6);
    /**
     * The desired number of unique layouts to be generated.
     */
    uint64_t number_of_unique_generated_layouts = 1;
    /**
     * The maximum number of attempts allowed to generate the given number of unique layouts (default: \f$10^{6}\f$).
     * Example: If the area, where SiDBs can be placed, is small and many SiDBs are to be placed, it may be difficult or
     * even impossible to find several unique (given by number_of_unique_generated_layouts) layouts. Therefore, this
     * parameter sets a limit for the maximum number of tries.
     */
    uint64_t maximal_attempts_for_multiple_layouts = 1'000'000;
};

/**
 * Generates a random layout of SiDBs by adding them to the provided layout layout.
 * The optional layout serves as the starting layout to which SiDBs are added to create the final layout.
 *
 * @tparam Lyt SiDB cell-level SiDB layout type.
 * @param params The parameters for generating the random layout.
 * @param layout Optional layout to which random cells are added.
 * @return A randomly generated layout of SiDBs, or `std::nullopt` if the process failed due to conflicting parameters.
 */
template <typename Lyt>
[[nodiscard]] std::optional<Lyt>
generate_random_sidb_layout(const generate_random_sidb_layout_params<coordinate<Lyt>>& params,
                            const std::optional<Lyt>&                                  layout = std::nullopt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    std::unordered_set<typename Lyt::coordinate> sidbs_affected_by_defects = {};

    uint64_t number_of_sidbs_of_final_layout = params.number_of_sidbs;

    Lyt lyt{};

    if (layout.has_value())
    {
        lyt = layout.value().clone();
        number_of_sidbs_of_final_layout += lyt.num_cells();

        if constexpr (is_sidb_defect_surface_v<Lyt>)
        {
            sidbs_affected_by_defects = layout.value().all_affected_sidbs(std::make_pair(0, 0));
        }
    }

    // counts the attempts to place the given number of SiDBs
    uint64_t attempt_counter = 0;

    // stops if either all SiDBs are placed or the maximum number of attempts was performed
    while (lyt.num_cells() < number_of_sidbs_of_final_layout && attempt_counter < params.maximal_attempts)
    {
        // random coordinate within the area specified by two coordinates
        const auto random_coord = random_coordinate(params.coordinate_pair.first, params.coordinate_pair.second);
        bool       next_to_neutral_defect = false;

        if (sidbs_affected_by_defects.count(random_coord) > 0)
        {
            next_to_neutral_defect = true;
        }

        bool random_cell_is_identical_wih_defect = false;
        // check if a defect does not yet occupy random coordinate.
        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            random_cell_is_identical_wih_defect = (lyt.get_sidb_defect(random_coord).type != sidb_defect_type::NONE);
        }

        // if the constraints that no positive SiDBs occur and the cell is not yet occupied by a defect are satisfied,
        // the SiDB is added to the layout
        if (!random_cell_is_identical_wih_defect && !next_to_neutral_defect)
        {
            if (layout.has_value())
            {
                lyt.assign_cell_type(random_coord, technology<Lyt>::cell_type::LOGIC);
            }
            else
            {
                lyt.assign_cell_type(random_coord, technology<Lyt>::cell_type::NORMAL);
            }

            if (params.positive_sidbs ==
                    generate_random_sidb_layout_params<coordinate<Lyt>>::positive_charges::FORBIDDEN &&
                can_positive_charges_occur(lyt, params.simulation_parameters))
            {
                lyt.assign_cell_type(random_coord, technology<Lyt>::cell_type::EMPTY);
            }
        }
        attempt_counter += 1;
    }

    if (params.positive_sidbs == generate_random_sidb_layout_params<coordinate<Lyt>>::positive_charges::MAY_OCCUR &&
        !can_positive_charges_occur(lyt, params.simulation_parameters))
    {
        return generate_random_sidb_layout(params, layout);
    }

    if (lyt.num_cells() == number_of_sidbs_of_final_layout)
    {
        return lyt;
    }

    // in case some SiDBs could not be placed, return std::nullopt
    return std::nullopt;
}

/**
 * Generates multiple unique random SiDB layouts by adding them to the provided layout layout.
 * The optional layout serves as the starting layout to which SiDBs are added to create unique SiDB layouts.
 *
 * @tparam Lyt SiDB cell-level SiDB layout type.
 * @param params The parameters for generating the random SiDB layouts.
 * @param layout Optional layout to which random cells are added.
 * @return A vector containing the unique randomly generated SiDB layouts. If the design is impossible, `std::nullopt`
 */
template <typename Lyt>
[[nodiscard]] std::optional<std::vector<Lyt>>
generate_multiple_random_sidb_layouts(const generate_random_sidb_layout_params<coordinate<Lyt>>& params,
                                      const std::optional<Lyt>& layout = std::nullopt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    // collects all unique SiDB layouts
    std::vector<Lyt> unique_lyts{};
    unique_lyts.reserve(params.number_of_unique_generated_layouts);

    // counter for unsuccessful generation attempts
    uint64_t unsuccessful_generation_attempt_counter = 0;

    while (unique_lyts.size() < params.number_of_unique_generated_layouts &&
           unsuccessful_generation_attempt_counter < params.maximal_attempts_for_multiple_layouts)
    {
        if (auto random_lyt = generate_random_sidb_layout(params, layout); random_lyt.has_value())
        {
            // check if the layout is unique
            const auto is_identical = std::any_of(FICTION_EXECUTION_POLICY_PAR_UNSEQ unique_lyts.cbegin(),
                                                  unique_lyts.cend(), [&](const auto& old_lyt)
                                                  { return are_cell_layouts_identical(random_lyt.value(), old_lyt); });

            // add layout if unique
            if (!is_identical)
            {
                unique_lyts.emplace_back(std::move(random_lyt.value()));
                continue;
            }
        }
        ++unsuccessful_generation_attempt_counter;
    }

    // return std::nullopt if no layouts were generated
    return unique_lyts.empty() ? std::nullopt : std::optional{std::move(unique_lyts)};
}

}  // namespace fiction

#endif  // FICTION_RANDOM_SIDB_LAYOUT_GENERATOR_HPP
