//
// Created by marcel on 21.07.23.
//

#ifndef FICTION_DETECT_BDL_PAIRS_HPP
#define FICTION_DETECT_BDL_PAIRS_HPP

#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/traits.hpp"

#include <units.h>

#include <algorithm>
#include <unordered_set>
#include <vector>

namespace fiction
{

template <typename Lyt>
struct bdl_pair
{
    const sidb_technology::cell_type type{};

    const cell<Lyt> top_sidb{};
    const cell<Lyt> bottom_sidb{};

    bdl_pair() = default;
    bdl_pair(const sidb_technology::cell_type t, const cell<Lyt> top, const cell<Lyt> bottom) noexcept :
            type{t},
            top_sidb{top},
            bottom_sidb{bottom}
    {}
};

struct detect_bdl_pairs_params
{
    /**
     * The maximum distance between two dots to be considered a BDL pair.
     */
    units::length::nanometer_t threshold{2_nm};
};

template <typename Lyt>
std::vector<bdl_pair<Lyt>> detect_io_bdl_pairs(const Lyt& lyt, const detect_bdl_pairs_params params = {}) noexcept
{
    std::vector<bdl_pair<Lyt>> bdl_pairs{};
    bdl_pairs.reserve(lyt.num_pis() / 2 + lyt.num_pos() / 2);

    const auto pair_up_dots = [&lyt, &params, &bdl_pairs](const std::vector<cell<Lyt>>&    io_dots,
                                                          const sidb_technology::cell_type type) noexcept
    {
        struct pairwise_dot_distance
        {
            cell<Lyt> sidb1{};
            cell<Lyt> sidb2{};

            units::length::nanometer_t distance{};

            pairwise_dot_distance() = default;
            pairwise_dot_distance(const cell<Lyt> s1, const cell<Lyt> s2, const units::length::nanometer_t d) noexcept :
                    sidb1{s1},
                    sidb2{s2},
                    distance{d}
            {}
        };

        const auto compute_pairwise_dot_distances = [&lyt](const std::vector<cell<Lyt>>& dots) noexcept
        {
            std::vector<pairwise_dot_distance> pairwise_distances{};
            pairwise_distances.reserve(dots.size() * (dots.size() - 1) / 2);

            for (auto i = 0u; i < dots.size(); ++i)
            {
                for (auto j = i + 1; j < dots.size(); ++j)
                {
                    pairwise_distances.emplace_back(dots[i], dots[j], sidb_nanometer_distance(lyt, dots[i], dots[j]));
                }
            }

            return pairwise_distances;
        };

        const auto dot_distance_comparator = [](const auto& lhs, const auto& rhs) noexcept
        { return lhs.distance < rhs.distance; };

        // compute pairwise distances
        auto input_pairwise_distances = compute_pairwise_dot_distances(io_dots);
        // sort pairwise distances
        std::sort(input_pairwise_distances.begin(), input_pairwise_distances.end(), dot_distance_comparator);
        // pair unique dots with the smallest distance
        std::unordered_set<cell<Lyt>> paired_dots{};
        paired_dots.reserve(io_dots.size());
        const auto already_paired_up = [&paired_dots](const auto& dot) noexcept
        { return paired_dots.find(dot) != paired_dots.cend(); };

        for (auto& potential_bdl_pair : input_pairwise_distances)
        {
            // if the distance is larger than the threshold, we can break the loop because the remaining distances
            // must be larger as well due to the prior sorting; this prevents unlikely pairings and helps performance
            if (potential_bdl_pair.distance > params.threshold)
            {
                break;
            }

            // if either dot has already been matched, skip
            if (already_paired_up(potential_bdl_pair.sidb1) || already_paired_up(potential_bdl_pair.sidb2))
            {
                continue;
            }

            // a BDL pair has been detected (swap SiDBs if necessary)
            if (potential_bdl_pair.sidb1 > potential_bdl_pair.sidb2)
            {
                bdl_pairs.emplace_back(type, potential_bdl_pair.sidb2, potential_bdl_pair.sidb1);
            }
            else
            {
                bdl_pairs.emplace_back(type, potential_bdl_pair.sidb1, potential_bdl_pair.sidb2);
            }

            // mark the dots as paired
            paired_dots.insert(potential_bdl_pair.sidb1);
            paired_dots.insert(potential_bdl_pair.sidb2);
        }
    };

    // collect all input dots
    std::vector<cell<Lyt>> input_dots{};
    input_dots.reserve(lyt.num_pis());
    lyt.foreach_pi([&input_dots](const auto& pi) { input_dots.push_back(pi); });

    // pair up input dots
    pair_up_dots(input_dots, sidb_technology::cell_type::INPUT);

    // collect all output dots
    std::vector<cell<Lyt>> output_dots{};
    output_dots.reserve(lyt.num_pos());
    lyt.foreach_po([&output_dots](const auto& po) { output_dots.push_back(po); });

    // pair up output dots
    pair_up_dots(output_dots, sidb_technology::cell_type::OUTPUT);

    // return the detected I/O BDL pairs
    return bdl_pairs;
}

};      // namespace fiction

#endif  // FICTION_DETECT_BDL_PAIRS_HPP
