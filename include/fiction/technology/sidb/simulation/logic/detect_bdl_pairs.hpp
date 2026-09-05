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
 * @brief Detects the SiDB pairs that jointly encode one Binary-dot Logic value.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <optional>
#include <unordered_set>
#include <vector>

namespace fiction::sidb::simulation::logic
{

/**
 * A Binary-dot Logic (BDL) pair is a pair of SiDBs that are close to each other and, thus, most likely share a charge.
 *
 * @tparam CellType Cell type.
 */
template <typename CellType>
struct bdl_pair
{
    /**
     * The type of the SiDBs in the pair. BDL SiDBs must be of the same type. They can either be normal, input, or
     * output SiDBs.
     */
    sidb::sidb_technology::cell_type type{};
    /**
     * The upper SiDB of the pair. Upper and lower are defined relative to each other via the `operator<` overload.
     */
    CellType upper{};
    /**
     * The lower SiDB of the pair. Upper and lower are defined relative to each other via the `operator<` overload.
     */
    CellType lower{};
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
    bdl_pair(const sidb::sidb_technology::cell_type t, const CellType& u, const CellType& l) noexcept :
            type{t},
            upper{u},
            lower{l}
    {}

    /**
     * Equality operator. Also provides `operator!=` via `= default`.
     *
     * @param other The other BDL pair to compare with.
     * @return `true` if this BDL pair is equal to the other, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator==(const bdl_pair<CellType>& other) const noexcept = default;
    /**
     * Less than operator.
     *
     * @param other The other BDL pair to compare with.
     * @return `true` if this BDL pair is less than the other, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator<(const bdl_pair<CellType>& other) const noexcept
    {
        if (upper != other.upper)
        {
            return upper < other.upper;
        }
        return lower < other.lower;
    }

    /**
     * Less than or equal to operator.
     *
     * @param other The other BDL pair to compare with.
     * @return `true` if this BDL pair is less than or equal to the other, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator<=(const bdl_pair<CellType>& other) const noexcept
    {
        return (*this < other) || (*this == other);
    }
    /**
     * Greater than operator.
     *
     * @param other The other BDL pair to compare with.
     * @return `true` if this BDL pair is greater than the other, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator>(const bdl_pair<CellType>& other) const
    {
        return !(*this <= other);
    }
    /**
     * Greater-than-or-equal-to operator.
     *
     * @param other The other BDL pair to compare with.
     * @return `true` if this BDL pair is greater than or equal to the other, otherwise `false`.
     */
    [[nodiscard]] constexpr bool operator>=(const bdl_pair<CellType>& other) const noexcept
    {
        return !(*this < other);
    }
    /**
     * Equality check ignoring type.
     *
     * @param other The other BDL pair to compare with.
     * @return `true` if the `upper` and `lower` attributes are equal, otherwise `false`.
     */
    [[nodiscard]] constexpr bool equal_ignore_type(const bdl_pair<CellType>& other) const noexcept
    {
        return upper == other.upper && lower == other.lower;
    }

    /**
     * Inequality check ignoring type.
     *
     * @param other The other BDL pair to compare with.
     * @return `true` if the `upper` and `lower` attributes are not equal, otherwise `false`.
     */
    [[nodiscard]] constexpr bool not_equal_ignore_type(const bdl_pair<CellType>& other) const noexcept
    {
        return !equal_ignore_type(other);
    }

    /**
     * Checks if the `upper` and `lower` SiDBs in this BDL pair have the same y-coordinate
     * as the corresponding SiDBs in the other BDL pair.
     *
     * @param other The other BDL pair to compare with.
     * @return `true` if both the `upper` and `lower` SiDBs in this pair have the same y-coordinate
     *         as the corresponding SiDBs in the other pair, otherwise `false`.
     */
    [[nodiscard]] constexpr bool has_same_y_coordinate(const bdl_pair<CellType>& other) const noexcept
    {
        return upper.y == other.upper.y && lower.y == other.lower.y;
    }

    /**
     * Checks if the `upper` and `lower` SiDBs in this BDL pair have the same x-coordinate
     * as the corresponding SiDBs in the other BDL pair.
     *
     * @param other The other BDL pair to compare with.
     * @return `true` if both the `upper` and `lower` SiDBs in this pair have the same x-coordinate
     *         as the corresponding SiDBs in the other pair, otherwise `false`.
     */
    [[nodiscard]] constexpr bool has_same_x_coordinate(const bdl_pair<CellType>& other) const noexcept
    {
        return upper.x == other.upper.x && lower.x == other.lower.x;
    }
};

/**
 * Parameters for the BDL pair detection algorithms.
 */
struct detect_bdl_pairs_params
{
    /**
     * The minimum distance between two SiDBs to be considered a BDL pair. This is useful to prevent, e.g., SiDBs of
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
 * Detects the BDL pairs of an SiDB layout. All SiDBs of the given type are collected and uniquely paired up by
 * distance: the pairwise distances are sorted, and the closest unpaired SiDBs within the distance window
 * `[params.minimum_distance, params.maximum_distance]` (defaults: 0.75 nm and 1.5 nm) form a pair. The lower bound
 * keeps, e.g., the SiDBs of an atomic wire from being paired; the upper bound prevents unlikely pairings and bounds
 * the work. Distances follow the layout's lattice. The pairs are returned sorted.
 *
 * @param lyt The layout to detect BDL pairs in.
 * @param type Optional SiDB type to restrict the detection to (`INPUT`, `OUTPUT`, `NORMAL`, ...). If omitted, the
 * BDL pairs of the input, output, and normal SiDBs are detected and returned in that order.
 * @param params Parameters for the BDL pair detection.
 * @return The detected BDL pairs.
 */
[[nodiscard]] inline std::vector<bdl_pair<lattice_site>>
detect_bdl_pairs(const layout& lyt, const std::optional<sidb_technology::cell_type>& type = std::nullopt,
                 const detect_bdl_pairs_params& params = {}) noexcept
{
    assert(params.minimum_distance <= params.maximum_distance);

    // in case no type is given, detect BDL pairs for all types
    if (!type.has_value())
    {
        const auto input_bdls  = detect_bdl_pairs(lyt, sidb_technology::cell_type::INPUT, params);
        const auto output_bdls = detect_bdl_pairs(lyt, sidb_technology::cell_type::OUTPUT, params);
        const auto normal_bdls = detect_bdl_pairs(lyt, sidb_technology::cell_type::NORMAL, params);

        std::vector<bdl_pair<lattice_site>> all_bdls{};
        all_bdls.reserve(input_bdls.size() + output_bdls.size() + normal_bdls.size());

        std::ranges::copy(input_bdls, std::back_inserter(all_bdls));
        std::ranges::copy(output_bdls, std::back_inserter(all_bdls));
        std::ranges::copy(normal_bdls, std::back_inserter(all_bdls));

        return all_bdls;
    }

    const auto dots = lyt.cells_of_type(*type);

    /**
     * A pair of SiDBs together with their distance (unit: nm).
     */
    struct pairwise_dot_distance
    {
        lattice_site sidb1{};
        lattice_site sidb2{};
        double       distance{};
    };

    const auto& lat = lyt.get_lattice();

    // compute pairwise distances
    std::vector<pairwise_dot_distance> pairwise_distances{};
    pairwise_distances.reserve((dots.size() * (dots.size() - 1)) / 2);

    for (std::size_t i = 0; i < dots.size(); ++i)
    {
        for (std::size_t j = i + 1; j < dots.size(); ++j)
        {
            pairwise_distances.push_back({dots[i], dots[j], lat.nm_distance(dots[i], dots[j])});
        }
    }

    // sort pairwise distances
    std::ranges::sort(pairwise_distances,
                      [](const auto& lhs, const auto& rhs) noexcept { return lhs.distance < rhs.distance; });

    std::vector<bdl_pair<lattice_site>> bdl_pairs{};
    bdl_pairs.reserve(dots.size() / 2);

    std::unordered_set<lattice_site> paired_dots{};
    paired_dots.reserve(dots.size());

    // pair unique dots with the smallest distance
    for (const auto& candidate : pairwise_distances)
    {
        // too close, e.g., the SiDBs of an atomic wire
        if (candidate.distance < params.minimum_distance)
        {
            continue;
        }
        // too far apart; every following candidate is farther apart still
        if (candidate.distance > params.maximum_distance)
        {
            break;
        }
        // if either dot has already been matched, skip
        if (paired_dots.contains(candidate.sidb1) || paired_dots.contains(candidate.sidb2))
        {
            continue;
        }

        // a BDL pair has been detected (swap SiDBs if necessary)
        if (candidate.sidb1 > candidate.sidb2)
        {
            bdl_pairs.emplace_back(lyt.get_cell_type(candidate.sidb1), candidate.sidb2, candidate.sidb1);
        }
        else
        {
            bdl_pairs.emplace_back(lyt.get_cell_type(candidate.sidb1), candidate.sidb1, candidate.sidb2);
        }

        paired_dots.insert(candidate.sidb1);
        paired_dots.insert(candidate.sidb2);
    }

    std::ranges::sort(bdl_pairs);

    return bdl_pairs;
}
}  // namespace fiction::sidb::simulation::logic
