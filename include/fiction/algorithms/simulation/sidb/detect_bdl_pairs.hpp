//
// Created by marcel on 21.07.23.
//

#ifndef FICTION_DETECT_BDL_PAIRS_HPP
#define FICTION_DETECT_BDL_PAIRS_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_nanometer_distance.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <vector>

namespace fiction
{

/**
 * A Binary-dot Logic (BDL) pair is a pair of SiDBs that are close to each other and, thus, most likely share a charge.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
struct bdl_pair
{
    /**
     * The type of the SiDBs in the pair. BDL SiDBs must be of the same type. They can either be normal, input, or
     * output SiDBs.
     */
    const sidb_technology::cell_type type{};
    /**
     * The upper SiDB of the pair. Upper and lower are defined relative to each other via the `operator<` overload.
     */
    const cell<Lyt> upper{};
    /**
     * The lower SiDB of the pair. Upper and lower are defined relative to each other via the `operator<` overload.
     */
    const cell<Lyt> lower{};
    /**
     * Standard constructor for empty BDL pairs.
     */
    bdl_pair() = default;
    /**
     * Constructor for BDL pairs.
     *
     * @param t Type of the SiDBs in the pair.
     * @param u The upper SiDB of the pair.
     * @param l The lower SiDB of the pair.
     */
    bdl_pair(const sidb_technology::cell_type t, const cell<Lyt>& u, const cell<Lyt>& l) noexcept :
            type{t},
            upper{u},
            lower{l}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }
};

/**
 * Parameters for the BDL pair detection algorithms.
 */
struct detect_bdl_pairs_params
{
    /**
     * The minimum distance between two dots to be considered a BDL pair. This is useful to prevent, e.g., SiDBs of
     * atomic wires to be considered BDL pairs. (unit: nm).
     */
    double minimum_distance{0.75};
    /**
     * The maximum distance between two dots to be considered a BDL pair. This is useful to prevent unlikely pairings
     * of SiDBs that are far apart and to improve performance of the matching algorithm. (unit: nm).
     */
    double maximum_distance{1.5};
};

/**
 * This algorithm detects BDL pairs in an SiDB layout. It does so by first collecting all dots of the given type and
 * then uniquely pairing them up based on their distance. Lower and upper distance thresholds can be defined (defaults =
 * 0.75 nm and 1.5 nm, respectively) to narrow down the range in which SiDBs could be considered a BDL pair. The
 * distance between two dots is computed using the `sidb_nanometer_distance` function. The algorithm returns a vector of
 * BDL pairs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to detect BDL pairs in.
 * @param type The type of the SiDBs to detect BDL pairs for, e.g., `INPUT`, `OUTPUT`, `NORMAL`.
 * @param params Parameters for the BDL pair detection algorithm.
 * @return A vector of BDL pairs.
 */
template <typename Lyt>
std::vector<bdl_pair<Lyt>> detect_bdl_pairs(const Lyt& lyt, const typename technology<Lyt>::cell_type type,
                                            const detect_bdl_pairs_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    // sanity check for parameter settings
    assert(params.minimum_distance <= params.maximum_distance);

    /**
     * Pairs up dots based on their distance. It does so by first computing the pairwise distances between all dots and
     * then sorting them. The smallest distances are then used to pair up the dots. The function takes a vector of dots
     * as input.
     */
    const auto pair_up_dots = [&lyt, &type,
                               &params](const std::vector<cell<Lyt>>& dots) noexcept -> std::vector<bdl_pair<Lyt>>
    {
        /**
         * Container for pairwise dot distances used in the pairing algorithm.
         */
        struct pairwise_dot_distance
        {
            /**
             * First dot.
             */
            cell<Lyt> sidb1{};
            /**
             * Second dot.
             */
            cell<Lyt> sidb2{};
            /**
             * Distance between the two dots. (unit: nm).
             */
            double distance{};
            /**
             * Standard constructor for empty pairwise dot distances.
             */
            pairwise_dot_distance() = default;
            /**
             * Constructor for pairwise dot distances.
             *
             * @param s1 The first dot.
             * @param s2 The second dot.
             * @param d The distance between the two dots. (unit: nm).
             */
            pairwise_dot_distance(const cell<Lyt>& s1, const cell<Lyt>& s2, const double d) noexcept :
                    sidb1{s1},
                    sidb2{s2},
                    distance{d}
            {}
        };
        /**
         * Computes the pairwise distances between all dots in the input vector.
         */
        const auto compute_pairwise_dot_distances = [&dots]() noexcept -> std::vector<pairwise_dot_distance>
        {
            std::vector<pairwise_dot_distance> pairwise_distances{};
            pairwise_distances.reserve((dots.size() * (dots.size() - 1)) / 2);

            for (auto i = 0u; i < dots.size(); ++i)
            {
                for (auto j = i + 1; j < dots.size(); ++j)
                {
                    pairwise_distances.emplace_back(dots[i], dots[j], sidb_nanometer_distance<Lyt>(dots[i], dots[j]));
                }
            }

            return pairwise_distances;
        };
        /**
         * Comparator for pairwise dot distances. Used in the sorting algorithm.
         */
        const auto dot_distance_comparator = [](const auto& lhs, const auto& rhs) noexcept -> bool
        { return lhs.distance < rhs.distance; };

        // container for the detected BDL pairs
        std::vector<bdl_pair<Lyt>> bdl_pairs{};
        bdl_pairs.reserve(dots.size() / 2);

        // compute pairwise distances
        auto pairwise_distances = compute_pairwise_dot_distances();
        // sort pairwise distances
        std::sort(pairwise_distances.begin(), pairwise_distances.end(), dot_distance_comparator);
        // pair unique dots with the smallest distance
        std::unordered_set<cell<Lyt>> paired_dots{};
        paired_dots.reserve(dots.size());
        /**
         * Checks whether a dot has already been paired up.
         */
        const auto already_paired_up = [&paired_dots](const auto& dot) noexcept -> bool
        { return paired_dots.find(dot) != paired_dots.cend(); };

        for (const auto& potential_bdl_pair : pairwise_distances)
        {
            // if the distance is smaller than the lower bound threshold, we can continue to the next pairing; this
            // prevents the pairing of dots that are too close to each other, e.g., in an atomic wire
            if (potential_bdl_pair.distance < params.minimum_distance)
            {
                continue;
            }
            // if the distance is larger than the upper bound threshold, we can break the loop because the remaining
            // distances must be larger as well due to the prior sorting; this prevents unlikely pairings and helps
            // performance
            if (potential_bdl_pair.distance > params.maximum_distance)
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

        return bdl_pairs;
    };

    // collect all dots of the given type
    std::vector<cell<Lyt>> dots_of_given_type{};
    switch (type)
    {
        case (technology<Lyt>::cell_type::INPUT):
        {
            dots_of_given_type.reserve(lyt.num_pis());
            lyt.foreach_pi([&dots_of_given_type](const auto& pi) { dots_of_given_type.push_back(pi); });

            break;
        }
        case (technology<Lyt>::cell_type::OUTPUT):
        {
            dots_of_given_type.reserve(lyt.num_pos());
            lyt.foreach_po([&dots_of_given_type](const auto& po) { dots_of_given_type.push_back(po); });

            break;
        }
        default:
        {
            dots_of_given_type.reserve(lyt.num_cells());
            lyt.foreach_cell(
                [&lyt, &type, &dots_of_given_type](const auto& c)
                {
                    if (lyt.get_cell_type(c) == type)
                    {
                        dots_of_given_type.push_back(c);
                    }
                });

            break;
        }
    }

    // pair up dots and return the detected BDL pairs
    return pair_up_dots(dots_of_given_type);
}

}  // namespace fiction

#endif  // FICTION_DETECT_BDL_PAIRS_HPP
