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
 * @brief The H-Si surface lattice as lattice vectors plus a two-site basis, and the lattice sites SiDBs occupy.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace fiction::sidb
{

/**
 * A site of the H-Si surface lattice. Site `(x, y, z)` is the `z`-th basis site of the unit cell reached by `x` steps
 * along the first and `y` steps along the second lattice vector of a `lattice`. This is the `(n, m, l)` lattice
 * coordinate of SiQAD's SQD files. Both supported reconstructions have a two-site basis, so `z` is either 0 or 1.
 *
 * Sites order in raster order: by `y`, then by `z`, then by `x`.
 */
struct lattice_site
{
    /**
     * Steps along the first lattice vector.
     */
    int32_t x{0};
    /**
     * Steps along the second lattice vector.
     */
    int32_t y{0};
    /**
     * Basis site within the unit cell, 0 or 1.
     */
    uint8_t z{0};
    /**
     * Default constructor. Creates the site `(0, 0, 0)`.
     */
    constexpr lattice_site() noexcept = default;
    /**
     * Creates the site `(x, y, z)`.
     *
     * @tparam X Integral type of `x`.
     * @tparam Y Integral type of `y`.
     * @tparam Z Integral type of `z`.
     * @param x Steps along the first lattice vector.
     * @param y Steps along the second lattice vector.
     * @param z Basis site, 0 or 1.
     */
    template <std::integral X, std::integral Y, std::integral Z>
    constexpr lattice_site(const X x, const Y y, const Z z) noexcept :
            x{static_cast<int32_t>(x)},
            y{static_cast<int32_t>(y)},
            z{static_cast<uint8_t>(static_cast<uint8_t>(z) & 1u)}
    {}
    /**
     * Creates the site `(x, y, 0)`.
     *
     * @tparam X Integral type of `x`.
     * @tparam Y Integral type of `y`.
     * @param x Steps along the first lattice vector.
     * @param y Steps along the second lattice vector.
     */
    template <std::integral X, std::integral Y>
    constexpr lattice_site(const X x, const Y y) noexcept : x{static_cast<int32_t>(x)}, y{static_cast<int32_t>(y)}
    {}
    /**
     * Compares two sites for equality.
     *
     * @param other Right-hand side site.
     * @return `true` iff both sites are identical.
     */
    [[nodiscard]] constexpr bool operator==(const lattice_site& other) const noexcept = default;
    /**
     * Orders two sites in raster order: by `y`, then by `z`, then by `x`.
     *
     * @param other Right-hand side site.
     * @return The ordering of this site relative to `other`.
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const lattice_site& other) const noexcept
    {
        if (const auto c = y <=> other.y; c != std::strong_ordering::equal)
        {
            return c;
        }
        if (const auto c = z <=> other.z; c != std::strong_ordering::equal)
        {
            return c;
        }
        return x <=> other.x;
    }
    /**
     * Adds another site to this one, carrying an overflowing basis index into the next unit cell along the second
     * lattice vector. Does not modify this site.
     *
     * @param other Site to add.
     * @return Sum of both sites.
     */
    [[nodiscard]] constexpr lattice_site operator+(const lattice_site& other) const noexcept
    {
        return {x + other.x, y + other.y + (z & other.z), z ^ other.z};
    }
    /**
     * Subtracts another site from this one, borrowing from the previous unit cell along the second lattice vector when
     * the basis index underflows. Does not modify this site.
     *
     * @param other Site to subtract.
     * @return Difference of both sites.
     */
    [[nodiscard]] constexpr lattice_site operator-(const lattice_site& other) const noexcept
    {
        return {x - other.x, y - other.y - static_cast<int32_t>(z == 0u && other.z != 0u), z ^ other.z};
    }
    /**
     * Returns a string representation of the form `"(x,y,z)"`.
     *
     * @return String representation of the form `"(x,y,z)"`.
     */
    [[nodiscard]] std::string str() const
    {
        return fmt::format("({},{},{})", x, y, z);
    }
};
/**
 * The row of a site counted in single SiDB rows: `2 * y + z`. This is the y-coordinate of the Cartesian cell-level
 * layouts that `physical_design::apply_gate_library` produces.
 *
 * @param s Site.
 * @return Row of `s`.
 */
[[nodiscard]] constexpr int32_t row_of(const lattice_site& s) noexcept
{
    return (2 * s.y) + s.z;
}
/**
 * The site at a given column and single-SiDB row, the inverse of `row_of`. Negative rows map to the unit cell below
 * them, so `(x, -1)` is `(x, -1, 1)`.
 *
 * @param x Column.
 * @param row Row counted in single SiDB rows.
 * @return The site `(x, floor(row / 2), row mod 2)`.
 */
[[nodiscard]] constexpr lattice_site site_at_row(const int32_t x, const int32_t row) noexcept
{
    const int32_t y = row >= 0 ? row / 2 : -((-row + 1) / 2);
    return {x, y, row - (2 * y)};
}
/**
 * All sites in the rectangle spanned by two corner sites, in raster order (top to bottom, left to right), both corners
 * included. The corners may be given in any order.
 *
 * @param first_corner One corner of the rectangle.
 * @param second_corner The opposite corner.
 * @return The sites in the rectangle in raster order.
 */
[[nodiscard]] inline std::vector<lattice_site> sites_in_area(const lattice_site& first_corner,
                                                             const lattice_site& second_corner)
{
    const auto min_x   = std::min(first_corner.x, second_corner.x);
    const auto max_x   = std::max(first_corner.x, second_corner.x);
    const auto min_row = std::min(row_of(first_corner), row_of(second_corner));
    const auto max_row = std::max(row_of(first_corner), row_of(second_corner));

    std::vector<lattice_site> sites{};
    sites.reserve(static_cast<std::size_t>(max_x - min_x + 1) * static_cast<std::size_t>(max_row - min_row + 1));

    for (auto row = min_row; row <= max_row; ++row)
    {
        for (auto x = min_x; x <= max_x; ++x)
        {
            sites.push_back(site_at_row(x, row));
        }
    }

    return sites;
}
/**
 * A uniformly random site in the rectangle spanned by two corner sites, both corners included.
 *
 * @tparam Rng Uniform random bit generator type.
 * @param first_corner One corner of the rectangle.
 * @param second_corner The opposite corner.
 * @param rng Random number generator to draw from.
 * @return A random site in the rectangle.
 */
template <std::uniform_random_bit_generator Rng>
[[nodiscard]] lattice_site random_site_in_area(const lattice_site& first_corner, const lattice_site& second_corner,
                                               Rng& rng)
{
    const auto min_x   = std::min(first_corner.x, second_corner.x);
    const auto max_x   = std::max(first_corner.x, second_corner.x);
    const auto min_row = std::min(row_of(first_corner), row_of(second_corner));
    const auto max_row = std::max(row_of(first_corner), row_of(second_corner));

    return site_at_row(std::uniform_int_distribution<int32_t>{min_x, max_x}(rng),
                       std::uniform_int_distribution<int32_t>{min_row, max_row}(rng));
}

/**
 * A hydrogen-passivated silicon surface lattice: a Bravais lattice given by two lattice vectors plus a two-site basis.
 * Site `(x, y, z)` lies at `x * a1 + y * a2 + basis[z]`. The vectors are stored in ångström, as SiQAD stores them;
 * positions are reported in nanometers. The two reconstructions SiQAD defines are available as `si_100_2x1()` and
 * `si_111_1x1()`; any other reconstruction with a two-site basis can be described by setting the vectors directly.
 */
struct lattice
{
    /**
     * A vector in the surface plane.
     */
    using vector = std::pair<double, double>;
    /**
     * Name of the reconstruction as SiQAD spells it in SQD files, e.g., `"Si(100) 2x1"`.
     */
    std::string name{};
    /**
     * First lattice vector (unit: Å).
     */
    vector a1{};
    /**
     * Second lattice vector (unit: Å).
     */
    vector a2{};
    /**
     * The two basis sites of the unit cell (unit: Å). The first one is the origin.
     */
    std::array<vector, 2> basis{};
    /**
     * The H-Si(100)-2x1 surface: dimer rows 7.68 Å apart, dimer atoms 3.84 Å apart along the row and 2.25 Å apart
     * across it.
     *
     * @return The H-Si(100)-2x1 lattice.
     */
    [[nodiscard]] static lattice si_100_2x1()
    {
        return {.name = "Si(100) 2x1", .a1 = {3.84, 0.0}, .a2 = {0.0, 7.68}, .basis = {{{0.0, 0.0}, {0.0, 2.25}}}};
    }
    /**
     * The H-Si(111)-1x1 surface: a hexagonal arrangement with 3.84 Å between neighboring sites, described by a
     * rectangular 6.65 Å × 3.84 Å cell with a centered second site.
     *
     * @return The H-Si(111)-1x1 lattice.
     */
    [[nodiscard]] static lattice si_111_1x1()
    {
        return {.name = "Si(111) 1x1", .a1 = {6.65, 0.0}, .a2 = {0.0, 3.84}, .basis = {{{0.0, 0.0}, {3.3255, 1.92}}}};
    }
    /**
     * The position of a site relative to the site `(0, 0, 0)`.
     *
     * @param s Site.
     * @return The `(x, y)` position of `s` (unit: nm).
     */
    [[nodiscard]] vector nm_position(const lattice_site& s) const noexcept
    {
        const auto b = basis.at(s.z);

        return {((s.x * a1.first) + (s.y * a2.first) + b.first) * 0.1,
                ((s.x * a1.second) + (s.y * a2.second) + b.second) * 0.1};
    }
    /**
     * The Euclidean distance between two sites.
     *
     * @param source First site.
     * @param target Second site.
     * @return Distance between `source` and `target` (unit: nm).
     */
    [[nodiscard]] double nm_distance(const lattice_site& source, const lattice_site& target) const noexcept
    {
        if (source == target)
        {
            return 0.0;
        }

        const auto [x1, y1] = nm_position(source);
        const auto [x2, y2] = nm_position(target);

        return std::hypot(x1 - x2, y1 - y2);
    }
    /**
     * Compares two lattices for equality.
     *
     * @param other Right-hand side lattice.
     * @return `true` iff name, vectors, and basis are identical.
     */
    [[nodiscard]] bool operator==(const lattice& other) const noexcept = default;
};

}  // namespace fiction::sidb

namespace std
{
/**
 * Hash for lattice sites.
 */
template <>
struct hash<fiction::sidb::lattice_site>
{
    /**
     * @param s Site to hash.
     * @return Hash of the lattice coordinates.
     */
    std::size_t operator()(const fiction::sidb::lattice_site& s) const noexcept
    {
        const auto h = (static_cast<uint64_t>(static_cast<uint32_t>(s.x)) << 32u) |
                       static_cast<uint64_t>(static_cast<uint32_t>(s.y));

        return std::hash<uint64_t>{}(h ^ (static_cast<uint64_t>(s.z) << 63u));
    }
};
}  // namespace std

/**
 * `fmt` formatter for lattice sites, printing `(x,y,z)`.
 */
template <>
struct fmt::formatter<fiction::sidb::lattice_site>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::sidb::lattice_site& s, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "({},{},{})", s.x, s.y, s.z);
    }
};
