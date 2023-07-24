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
    bdl_pair(const sidb_technology::cell_type t, const cell<Lyt> u, const cell<Lyt> l) noexcept :
            type{t},
            upper{u},
            lower{l}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
        static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
    }
};

/**
 * Parameters for the BDL pair detection algorithms.
 */
struct detect_bdl_pairs_params
{
    /**
     * The maximum distance between two dots to be considered a BDL pair.
     */
    units::length::nanometer_t threshold{2_nm};
};

/**
 * This algorithm detects input or output BDL pairs in an SiDB layout. It does so by first collecting all input and
 * output dots and then uniquely pairing them up based on their distance. A threshold can be defined (default = 2 nm),
 * after which SiDBs are no longer considered a pair. The distance between two dots is computed using the
 * `sidb_nanometer_distance` function. The algorithm returns a vector of I/O BDL pairs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to detect I/O BDL pairs in.
 * @param params Parameters for the BDL pair detection algorithm.
 * @return A vector of I/O BDL pairs.
 */
template <typename Lyt>
std::vector<bdl_pair<Lyt>> detect_io_bdl_pairs(const Lyt& lyt, const detect_bdl_pairs_params params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    std::vector<bdl_pair<Lyt>> bdl_pairs{};
    bdl_pairs.reserve(lyt.num_pis() / 2 + lyt.num_pos() / 2);

    /**
     * Pairs up dots based on their distance. It does so by first computing the pairwise distances between all dots and
     * then sorting them. The smallest distances are then used to pair up the dots. The function takes a vector of dots
     * and a cell type as input. It fills the vector of BDL pairs declared above.
     */
    const auto pair_up_dots = [&lyt, &params,
                               &bdl_pairs](const std::vector<cell<Lyt>>&    io_dots,
                                           const sidb_technology::cell_type type) noexcept -> void
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
             * Distance between the two dots.
             */
            units::length::nanometer_t distance{};
            /**
             * Standard constructor for empty pairwise dot distances.
             */
            pairwise_dot_distance() = default;
            /**
             * Constructor for pairwise dot distances.
             *
             * @param s1 The first dot.
             * @param s2 The second dot.
             * @param d The distance between the two dots.
             */
            pairwise_dot_distance(const cell<Lyt> s1, const cell<Lyt> s2, const units::length::nanometer_t d) noexcept :
                    sidb1{s1},
                    sidb2{s2},
                    distance{d}
            {}
        };
        /**
         * Computes the pairwise distances between all dots in the input vector.
         */
        const auto compute_pairwise_dot_distances =
            [&lyt](const std::vector<cell<Lyt>>& dots) noexcept -> std::vector<pairwise_dot_distance>
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
        /**
         * Comparator for pairwise dot distances. Used in the sorting algorithm.
         */
        const auto dot_distance_comparator = [](const auto& lhs, const auto& rhs) noexcept -> bool
        { return lhs.distance < rhs.distance; };

        // compute pairwise distances
        auto input_pairwise_distances = compute_pairwise_dot_distances(io_dots);
        // sort pairwise distances
        std::sort(input_pairwise_distances.begin(), input_pairwise_distances.end(), dot_distance_comparator);
        // pair unique dots with the smallest distance
        std::unordered_set<cell<Lyt>> paired_dots{};
        paired_dots.reserve(io_dots.size());
        /**
         * Checks whether a dot has already been paired up.
         */
        const auto already_paired_up = [&paired_dots](const auto& dot) noexcept -> bool
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
