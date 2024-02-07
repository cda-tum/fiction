//
// Created by Jan Drewniok on 28.12.23.
//

#ifndef FICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_OF_SIDB_GATE_HPP
#define FICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_OF_SIDB_GATE_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/sidb_surface.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/hash.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/phmap_utils.hpp"
#include "mockturtle/utils/stopwatch.hpp"

namespace fiction
{

struct defect_influence_params
{
    maximum_defect_influence_distance_params defect_influence_params{};
    is_operational_params                    is_operational_params{};
    detect_bdl_pairs_params                  bdl_pairs_params{};
};

template <typename Lyt>
struct defect_operational_domain
{
    locked_parallel_flat_hash_map<typename Lyt::cell, operational_status> operational_values{};
};

struct defect_influence_stats
{
    /**
     * The total runtime of the operational domain computation.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Number of simulator invocations.
     */
    std::size_t num_simulator_invocations{0};
    /**
     * Number of evaluated parameter combinations.
     */
    std::size_t num_evaluated_defect_positions{0};
    /**
     * Number of parameter combinations, for which the layout is operational.
     */
    std::size_t num_operational_defect_positions{0};
    /**
     * Number of parameter combinations, for which the layout is non-operational.
     */
    std::size_t num_non_operational_defect_positions{0};
};

namespace detail
{

template <typename Lyt, typename TT>
class defect_influence_impl
{
  public:
    defect_influence_impl(const Lyt& lyt, const std::vector<TT>& tt, const defect_influence_params& ps,
                          defect_influence_stats& st) :
            layout{lyt},
            truth_table{tt},
            params{ps},
            stats{st}
    {
        determine_edge_cells();
    }

    [[nodiscard]] defect_operational_domain<Lyt> contour_tracing(const std::size_t samples) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        // first, perform random sampling to find an operational starting point
        const auto non_operational_starting_point = find_non_operational_defect_position_via_random_sampling(samples);

        // if no operational point was found within the specified number of samples, return
        if (!non_operational_starting_point.has_value())
        {
            return defect_domain;
        }

        const auto next_clockwise_point = [](std::vector<typename Lyt::cell>& neighborhood,
                                             const typename Lyt::cell&        backtrack) noexcept -> typename Lyt::cell
        {
            assert(std::find(neighborhood.cbegin(), neighborhood.cend(), backtrack) != neighborhood.cend() &&
                   "The backtrack point must be part of the neighborhood");

            while (neighborhood.back() != backtrack)
            {
                std::rotate(neighborhood.begin(), neighborhood.begin() + 1, neighborhood.end());
            }

            return neighborhood.front();
        };

        assert(layout.num_defects() == 0 && "defect is added defect");

        // find an operational point on the contour starting from the randomly determined starting point
        const auto contour_starting_point = find_non_operational_defect_position(*non_operational_starting_point);

        assert(layout.num_defects() == 0 && "more than one defect");

        auto current_contour_point = contour_starting_point;
        auto backtrack_point       = current_contour_point.x == nw_cell.x ?
                                         current_contour_point :
                                         typename Lyt::cell{current_contour_point.x - 1, current_contour_point.y};

        auto current_neighborhood = moore_neighborhood(current_contour_point);
        auto next_point           = current_contour_point == backtrack_point ?
                                        current_neighborhood.front() :
                                        next_clockwise_point(current_neighborhood, backtrack_point);

        while (next_point != contour_starting_point)
        {
            const auto operational_status = is_defect_position_operational(next_point);

            assert(layout.num_defects() == 0 && "more than one defect");

            if (operational_status == operational_status::NON_OPERATIONAL)
            {
                backtrack_point       = current_contour_point;
                current_contour_point = next_point;
            }
            else
            {
                backtrack_point = next_point;
            }

            current_neighborhood = moore_neighborhood(current_contour_point);
            next_point           = next_clockwise_point(current_neighborhood, backtrack_point);
        }

        log_stats();

        return defect_domain;
    }

  private:
    void determine_edge_cells() noexcept
    {
        // bounding box around the given layout to have north-west and south-east cells.
        const auto                              siqad_layout = convert_to_siqad_coordinates(layout);
        bounding_box_2d<decltype(siqad_layout)> bb{siqad_layout};

        auto nw = fiction::siqad::to_fiction_coord<cube::coord_t>(bb.get_min());  // north-west cell
        auto se = fiction::siqad::to_fiction_coord<cube::coord_t>(bb.get_max());  // south-east cell

        // shift nw and se cell by the additional scanning area to cover an area that is larger than the gate area.
        nw.x = nw.x - params.defect_influence_params.additional_scanning_area.first;
        nw.y = nw.y - params.defect_influence_params.additional_scanning_area.second;

        se.x = se.x + params.defect_influence_params.additional_scanning_area.first;
        se.y = se.y + params.defect_influence_params.additional_scanning_area.second;

        nw_cell = nw;
        se_cell = se;
        // defect_cells = all_coordinates_in_spanned_area(nw, se);
    }

    [[nodiscard]] std::optional<typename Lyt::cell> find_non_operational_defect_position_via_sidbs() noexcept
    {
        layout.foreach_cell(
            [this](const auto& c)
            {
                auto c_copy = c;
                c_copy.x    = c.x - 1;
                if (layout.is_empty_cell(c_copy))
                {
                    layout.assign_sidb_defect(c_copy, params.defect_influence_params.defect);
                    // determine the operational status
                    const auto operational_value = is_defect_position_operational(c_copy);

                    layout.assign_sidb_defect(c_copy, sidb_defect{sidb_defect_type::NONE});

                    // if the parameter combination is operational, return its step values in x and y dimension
                    if (operational_value == operational_status::NON_OPERATIONAL)
                    {
                        return c_copy;
                    }
                }
            });
        return std::nullopt;
    }

    [[nodiscard]] std::optional<typename Lyt::cell>
    find_non_operational_defect_position_via_random_sampling(const std::size_t samples) noexcept
    {
        std::set<typename Lyt::cell> random_cells{};
        while (random_cells.size() < samples)
        {
            const auto random = random_coordinate(nw_cell, se_cell);
            if (layout.is_empty_cell(random))
            {
                random_cells.insert(random);
            }
        }

        for (const auto& random_cell : random_cells)
        {
            layout.assign_sidb_defect(random_cell, params.defect_influence_params.defect);
            // determine the operational status
            const auto operational_value = is_defect_position_operational(random_cell);

            layout.assign_sidb_defect(random_cell, sidb_defect{sidb_defect_type::NONE});

            // if the parameter combination is operational, return its step values in x and y dimension
            if (operational_value == operational_status::NON_OPERATIONAL)
            {
                return random_cell;
            }
        }

        return std::nullopt;
    }

    operational_status is_defect_position_operational(const typename Lyt::cell& c) noexcept
    {
        // if the point has already been sampled, return the stored operational status
        if (const auto op_value = has_already_been_sampled(c); op_value.has_value())
        {
            return *op_value;
        }

        const auto operational = [this, &c]()
        {
            defect_domain.operational_values[c] = operational_status::OPERATIONAL;

            return operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, &c]()
        {
            defect_domain.operational_values[c] = operational_status::NON_OPERATIONAL;

            return operational_status::NON_OPERATIONAL;
        };

        // increment the number of evaluated parameter combinations
        ++num_evaluated_defect_positions;

        layout.assign_sidb_defect(c, params.defect_influence_params.defect);
        const auto& [status, sim_calls] = is_operational(layout, truth_table, params.is_operational_params);
        layout.assign_sidb_defect(c, sidb_defect{sidb_defect_type::NONE});
        num_simulator_invocations += sim_calls;

        if (status == operational_status::NON_OPERATIONAL)
        {
            return non_operational();
        }

        // if we made it here, the layout is operational
        return operational();
    }

    [[nodiscard]] inline std::optional<operational_status>
    has_already_been_sampled(const typename Lyt::cell& c) const noexcept
    {
        if (const auto it = defect_domain.operational_values.find(c); it != defect_domain.operational_values.cend())
        {
            return it->second;
        }

        return std::nullopt;
    }

    [[nodiscard]] typename Lyt::cell
    find_non_operational_defect_position(const typename Lyt::cell& starting_defect_position) noexcept
    {
        auto latest_non_operational_defect_position = starting_defect_position;
        // layout.assign_sidb_defect(starting_defect_position, params.defect_influence_params.defect);
        previous_defect_position = starting_defect_position;

        // move towards the left border of the parameter range
        for (int64_t x = starting_defect_position.x; x >= nw_cell.x; --x)
        {
            current_defect_position = {x, starting_defect_position.y};

            layout.assign_sidb_defect(current_defect_position, params.defect_influence_params.defect);

            const auto operational_status = is_defect_position_operational(current_defect_position);

            layout.assign_sidb_defect(current_defect_position, sidb_defect{sidb_defect_type::NONE});
            ;

            if (operational_status == operational_status::NON_OPERATIONAL)
            {
                latest_non_operational_defect_position = current_defect_position;
            }
            else
            {
                return latest_non_operational_defect_position;
            }
        }

        // if no boundary point was found, the operational area extends outside the parameter range
        // return the latest operational point
        return latest_non_operational_defect_position;
    }

    /**
     * Helper function that writes the the statistics of the operational domain computation to the statistics object.
     * Due to data races that can occur during the computation, each value is temporarily held in an atomic variable and
     * written to the statistics object only after the computation has finished.
     */
    void log_stats() const noexcept
    {
        stats.num_simulator_invocations      = num_simulator_invocations;
        stats.num_evaluated_defect_positions = num_evaluated_defect_positions;

        for (const auto& [param_point, status] : defect_domain.operational_values)
        {
            if (status == operational_status::OPERATIONAL)
            {
                ++stats.num_operational_defect_positions;
            }
            else
            {
                ++stats.num_non_operational_defect_positions;
            }
        }
    }

    [[nodiscard]] std::vector<typename Lyt::cell> moore_neighborhood(const typename Lyt::cell& c) const noexcept
    {
        std::vector<typename Lyt::cell> neighbors{};
        neighbors.reserve(8);

        const auto& moore = c;

        const auto decr_x = (moore.x - 1 >= nw_cell.x) ? moore.x - 1 : moore.x;
        const auto incr_x = (moore.x + 1 <= se_cell.x) ? moore.x + 1 : moore.x;
        const auto decr_y = (moore.y - 1 >= nw_cell.y) ? moore.y - 1 : moore.y;
        const auto incr_y = (moore.y + 1 <= se_cell.y) ? moore.y + 1 : moore.y;

        // add neighbors in clockwise direction

        // right
        if (moore.x != incr_x)
        {
            neighbors.emplace_back(incr_x, moore.y);
        }
        // lower-right
        if (moore.x != incr_x && moore.y != decr_y)
        {
            neighbors.emplace_back(incr_x, decr_y);
        }
        // down
        if (moore.y != decr_y)
        {
            neighbors.emplace_back(moore.x, decr_y);
        }
        // lower-left
        if (moore.x != decr_x && moore.y != decr_y)
        {
            neighbors.emplace_back(decr_x, decr_y);
        }
        // left
        if (moore.x != decr_x)
        {
            neighbors.emplace_back(decr_x, moore.y);
        }
        // upper-left
        if (moore.x != decr_x && moore.y != incr_y)
        {
            neighbors.emplace_back(decr_x, incr_y);
        }
        // up
        if (moore.y != incr_y)
        {
            neighbors.emplace_back(moore.x, incr_y);
        }
        // upper-right
        if (moore.x != incr_x && moore.y != incr_y)
        {
            neighbors.emplace_back(incr_x, incr_y);
        }

        return neighbors;
    };

    /**
     * The SiDB cell-level layout to investigate.
     */
    sidb_surface<Lyt> layout{};
    /**
     * The specification of the layout.
     */
    const std::vector<TT>& truth_table;
    /**
     * The parameters for the operational domain computation.
     */
    const defect_influence_params& params;

    typename Lyt::cell nw_cell{};

    typename Lyt::cell se_cell{};

    typename Lyt::cell current_defect_position{};

    typename Lyt::cell previous_defect_position{};

    /**
     * The operational domain of the layout.
     */
    defect_operational_domain<Lyt> defect_domain{};

    /**
     * The statistics of the operational domain computation.
     */
    defect_influence_stats& stats;

    std::atomic<std::size_t> num_simulator_invocations{0};
    /**
     * Number of evaluated parameter combinations.
     */
    std::atomic<std::size_t> num_evaluated_defect_positions{0};
};
}  // namespace detail

template <typename Lyt, typename TT>
defect_operational_domain<Lyt>
defect_influence_of_sidb_gate_contour_tracing(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                                              const defect_influence_params& params = {},
                                              defect_influence_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(has_cube_coord_v<Lyt>, "Lyt is not based on cube coordinates");

    defect_influence_stats                 st{};
    detail::defect_influence_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.contour_tracing(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}

// template <typename Lyt, typename TT>
//[[nodiscard]] std::pair<typename Lyt::cell, double> maximum_defect_influence_position_and_distance_of_sidb_gate(
//     const Lyt& lyt, const std::vector<TT>& spec,
//     const maximum_defect_influence_position_and_distance_of_sidb_gate_params& params = {}) noexcept
//{
//     static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
//     static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
//     static_assert(!has_offset_ucoord_v<Lyt>, "Lyt should not be based on offset coordinates");
//     static_assert(!is_charge_distribution_surface_v<Lyt>, "Lyt cannot be a charge distribution surface");
//
//     assert(lyt.num_pis() > 0 && "skeleton needs input cells");
//     assert(lyt.num_pos() > 0 && "skeleton needs output cells");
//
//     assert(!spec.empty());
//     // all elements in tts must have the same number of variables
//     assert(std::adjacent_find(spec.begin(), spec.end(),
//                               [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) ==
//                               spec.end());
//
//     bdl_input_iterator<Lyt> bii{lyt, params.bdl_pairs_params};
//     double                  maximum_defect_influence_distance = 0.0;
//     cell<Lyt>               defect_cell{};
//     // number of different input combinations
//     for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bii)
//     {
//         maximum_defect_influence_distance_stats stats_defect{};
//         const auto                              influence_cell_distance =
//             maximum_defect_influence_position_and_distance(lyt, params.defect_influence_params, &stats_defect);
//         std::cout << mockturtle::to_seconds(stats_defect.time_total) << std::endl;
//         if (influence_cell_distance.second > maximum_defect_influence_distance)
//         {
//             maximum_defect_influence_distance = influence_cell_distance.second;
//             defect_cell                       = influence_cell_distance.first;
//         }
//     }
//     return {defect_cell, maximum_defect_influence_distance};
// }

}  // namespace fiction

#endif  // FICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_OF_SIDB_GATE_HPP
