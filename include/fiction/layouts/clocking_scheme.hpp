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
 * @brief Clocking scheme type and the catalog of regular clockings the library ships.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/traits.hpp"

#include <phmap.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fiction::layouts::clocking
{

/**
 * Clocking scheme that assigns a clock number to every tile position \f$(x, y)\f$. Clocking schemes are owned by gate
 * and tile-clocked cell layouts. A clock zone spans every layer, so a scheme has no z-coordinate.
 *
 * A clocking scheme is defined by a rectangular cutout that repeats seamlessly in all directions, including negative
 * coordinates. Clock numbers can be overridden per tile. Many regular clocking schemes have been proposed in the
 * literature; the factories below construct them.
 *
 * Schemes are values: they can be copied, assigned, and compared. Two schemes are equal iff their names, phase counts,
 * degrees, cutouts, regularity, and overridden clock numbers are equal.
 */
class scheme
{
  public:
    /**
     * Clock phase index.
     */
    using clock_number = uint8_t;
    /**
     * Number of inputs or outputs of a clock zone.
     */
    using degree = uint8_t;
    /**
     * Degree of a scheme that imposes no bound beyond the fan-in of the layout topology.
     */
    static constexpr degree UNBOUNDED = std::numeric_limits<degree>::max();
    /**
     * Standard constructor.
     *
     * @param n The clocking scheme's name. The name is the key under which `get_scheme` finds the scheme.
     * @param cutout Rows of clock numbers. Row `y`, column `x` holds the clock number of every tile
     * \f$(x + i \cdot w, y + j \cdot h)\f$ for all integers \f$i, j\f$, where \f$w\f$ and \f$h\f$ are the width and
     * height of `cutout`.
     * @param num_clocks Number of clock phases that make up one clock cycle. Must be 3 or 4.
     * @param max_in_degree Maximum number of inputs the scheme supports per clock zone.
     * @param max_out_degree Maximum number of outputs the scheme supports per clock zone.
     * @param regular Flag to identify the scheme as regular.
     * @throws std::invalid_argument if `cutout` is empty or not rectangular, if `num_clocks` is neither 3 nor 4, or if
     * an entry of `cutout` is not below `num_clocks`.
     */
    scheme(std::string n, const std::vector<std::vector<clock_number>>& cutout, const clock_number num_clocks,
           const degree max_in_degree, const degree max_out_degree, const bool regular = true) :
            scheme_name{std::move(n)},
            width{cutout.empty() ? 0 : static_cast<int64_t>(cutout.front().size())},
            height{static_cast<int64_t>(cutout.size())},
            phases{num_clocks},
            in_degree{max_in_degree},
            out_degree{max_out_degree},
            regular_flag{regular}
    {
        if (width == 0)
        {
            throw std::invalid_argument("clocking scheme cutout must not be empty");
        }
        if (phases != 3u && phases != 4u)
        {
            throw std::invalid_argument("clocking scheme must have 3 or 4 clock phases");
        }

        cells.reserve(static_cast<std::size_t>(width * height));

        for (const auto& row : cutout)
        {
            if (std::cmp_not_equal(row.size(), width))
            {
                throw std::invalid_argument("clocking scheme cutout must be rectangular");
            }
            if (std::ranges::any_of(row, [this](const auto cn) { return cn >= phases; }))
            {
                throw std::invalid_argument("clocking scheme cutout contains a clock number not below num_clocks");
            }

            cells.insert(cells.cend(), row.cbegin(), row.cend());
        }
    }
    /**
     * Returns the clock number of the tile at \f$(x, y)\f$: its overridden clock number if one exists, and the
     * repeated cutout entry otherwise.
     *
     * @param x x-coordinate of the tile.
     * @param y y-coordinate of the tile.
     * @return Clock number of \f$(x, y)\f$.
     */
    [[nodiscard]] clock_number operator()(const int64_t x, const int64_t y) const noexcept
    {
        if (!overrides.empty())
        {
            if (const auto it = overrides.find({x, y}); it != overrides.cend())
            {
                return it->second;
            }
        }

        return cells[static_cast<std::size_t>((floor_mod(y, height) * width) + floor_mod(x, width))];
    }
    /**
     * Overrides the clock number of the tile at \f$(x, y)\f$. An overridden scheme is irregular.
     *
     * @param x x-coordinate of the tile.
     * @param y y-coordinate of the tile.
     * @param cn Clock number to assign. The scheme stores `cn % num_clocks()`.
     */
    void override_clock_number(const int64_t x, const int64_t y, const clock_number cn) noexcept
    {
        overrides[{x, y}] = static_cast<clock_number>(cn % phases);
    }
    /**
     * Checks for the clocking scheme's regularity.
     *
     * @return `true` iff the scheme is regular and no clock number is overridden.
     */
    [[nodiscard]] bool is_regular() const noexcept
    {
        return regular_flag && overrides.empty();
    }
    /**
     * Name of the clocking scheme.
     *
     * @return The canonical name, e.g., `"2DDWAVE"`, without a phase-count suffix.
     */
    [[nodiscard]] const std::string& name() const noexcept
    {
        return scheme_name;
    }
    /**
     * Number of clock phases in this scheme.
     *
     * @return 3 or 4.
     */
    [[nodiscard]] clock_number num_clocks() const noexcept
    {
        return phases;
    }
    /**
     * Maximum number of inputs the scheme supports per clock zone.
     *
     * @return Maximum in-degree, or `UNBOUNDED`.
     */
    [[nodiscard]] degree max_in_degree() const noexcept
    {
        return in_degree;
    }
    /**
     * Maximum number of outputs the scheme supports per clock zone.
     *
     * @return Maximum out-degree, or `UNBOUNDED`.
     */
    [[nodiscard]] degree max_out_degree() const noexcept
    {
        return out_degree;
    }
    /**
     * Compares all properties of two schemes, overridden clock numbers included.
     *
     * @param other Scheme to compare against.
     * @return `true` iff both schemes are equal.
     */
    [[nodiscard]] bool operator==(const scheme& other) const = default;

  private:
    /**
     * Canonical name.
     */
    std::string scheme_name;
    /**
     * Cutout width.
     */
    int64_t width;
    /**
     * Cutout height.
     */
    int64_t height;
    /**
     * Cutout entries in row-major order.
     */
    std::vector<clock_number> cells{};
    /**
     * Number of clock phases.
     */
    clock_number phases;
    /**
     * Maximum in-degree.
     */
    degree in_degree;
    /**
     * Maximum out-degree.
     */
    degree out_degree;
    /**
     * Defines the clocking as regular and well-defined by the cutout.
     */
    bool regular_flag;
    /**
     * Overridden clock numbers by tile position.
     */
    phmap::flat_hash_map<std::pair<int64_t, int64_t>, clock_number> overrides{};
    /**
     * Modulo that rounds toward negative infinity, so that a cutout repeats seamlessly at negative coordinates.
     *
     * @param a Dividend.
     * @param m Positive divisor.
     * @return \f$a \bmod m \in [0, m)\f$.
     */
    [[nodiscard]] static constexpr int64_t floor_mod(const int64_t a, const int64_t m) noexcept
    {
        return ((a % m) + m) % m;
    }
};

// The canonical name of each pre-defined clocking scheme. These are what `get_scheme` and
// `clocking::state::is_clocking_scheme` match against. `get_scheme` additionally accepts the phase-count
// spellings that carry no constant of their own -- `OPEN3`, `ROW4`, `2DDWAVEHEX3`, and the like -- and matches
// case-insensitively, which is why the names stay strings rather than becoming an enum: they cross into FGL
// files, the CLI, and `pyfiction` as free text. Deliberately not a Doxygen block: it describes all twelve, and a
// `/** */` here would be recorded as documentation for `OPEN_NAME` alone.
inline constexpr const char* OPEN_NAME          = "OPEN";
inline constexpr const char* COLUMNAR_NAME      = "COLUMNAR";
inline constexpr const char* ROW_NAME           = "ROW";
inline constexpr const char* TWODDWAVE_NAME     = "2DDWAVE";
inline constexpr const char* TWODDWAVE_HEX_NAME = "2DDWAVEHEX";
inline constexpr const char* USE_NAME           = "USE";
inline constexpr const char* RES_NAME           = "RES";
inline constexpr const char* ESR_NAME           = "ESR";
inline constexpr const char* CFE_NAME           = "CFE";
inline constexpr const char* RIPPLE_NAME        = "RIPPLE";
inline constexpr const char* SRS_NAME           = "SRS";
inline constexpr const char* BANCS_NAME         = "BANCS";

/**
 * Number of clock phases of a clocking scheme.
 */
enum class num_clks : uint8_t
{
    /**
     * Three clocks.
     */
    THREE,
    /**
     * Four clocks.
     */
    FOUR
};
/**
 * Arrangement of the shifted rows or columns of a hexagonal layout.
 */
enum class hex_arrangement : uint8_t
{
    /**
     * Odd rows are shifted.
     */
    ODD_ROW,
    /**
     * Even rows are shifted.
     */
    EVEN_ROW,
    /**
     * Odd columns are shifted.
     */
    ODD_COLUMN,
    /**
     * Even columns are shifted.
     */
    EVEN_COLUMN
};

namespace detail
{

/**
 * Transposes a rectangular cutout.
 *
 * @param cutout Rectangular cutout.
 * @return The cutout with rows and columns swapped.
 */
[[nodiscard]] inline std::vector<std::vector<scheme::clock_number>>
transpose(const std::vector<std::vector<scheme::clock_number>>& cutout)
{
    std::vector<std::vector<scheme::clock_number>> transposed(cutout.front().size(),
                                                              std::vector<scheme::clock_number>(cutout.size()));

    for (std::size_t y = 0; y < cutout.size(); ++y)
    {
        for (std::size_t x = 0; x < cutout[y].size(); ++x)
        {
            transposed[x][y] = cutout[y][x];
        }
    }

    return transposed;
}

/**
 * Converts a phase count to the number of clocks.
 *
 * @param n Phase count.
 * @return 3 or 4.
 */
[[nodiscard]] constexpr scheme::clock_number count(const num_clks n) noexcept
{
    return static_cast<scheme::clock_number>(n == num_clks::THREE ? 3u : 4u);
}

}  // namespace detail

/**
 * Returns an irregular clocking that maps every tile to clock number 0. It is intended to be overridden. Its degrees
 * are `scheme::UNBOUNDED`, i.e., only the layout topology bounds them.
 *
 * @param n Number of clocks.
 * @return Irregular clocking scheme.
 */
[[nodiscard]] inline scheme open(const num_clks n = num_clks::FOUR)
{
    return scheme{OPEN_NAME, {{0}}, detail::count(n), scheme::UNBOUNDED, scheme::UNBOUNDED, false};
}
/**
 * Returns a linear 1D clocking as originally introduced in \"A device architecture for computing with quantum dots\" by
 * C. S. Lent and P. D. Tougaw in the Proceedings of the IEEE 1997.
 *
 * @param n Number of clocks.
 * @return Columnar clocking scheme.
 */
[[nodiscard]] inline scheme columnar(const num_clks n = num_clks::FOUR)
{
    if (n == num_clks::THREE)
    {
        return scheme{COLUMNAR_NAME, {{0, 1, 2}}, 3u, 3u, 2u};
    }

    return scheme{COLUMNAR_NAME, {{0, 1, 2, 3}}, 4u, 3u, 2u};
}
/**
 * Returns a 90° rotated linear 1D clocking based on the one originally introduced in \"A device architecture for
 * computing with quantum dots\" by C. S. Lent and P. D. Tougaw in the Proceedings of the IEEE 1997.
 *
 * @param n Number of clocks.
 * @return Row-based clocking scheme.
 */
[[nodiscard]] inline scheme row(const num_clks n = num_clks::FOUR)
{
    if (n == num_clks::THREE)
    {
        return scheme{ROW_NAME, {{0}, {1}, {2}}, 3u, 3u, 2u};
    }

    return scheme{ROW_NAME, {{0}, {1}, {2}, {3}}, 4u, 3u, 2u};
}
/**
 * Returns the 2DDWave clocking as defined in \"Clocking and Cell Placement for QCA\" by V. Vankamamidi, M. Ottavi,
 * and F. Lombardi in IEEE Conference on Nanotechnology 2006.
 *
 * @param n Number of clocks.
 * @return 2DDWave clocking scheme.
 */
[[nodiscard]] inline scheme twoddwave(const num_clks n = num_clks::FOUR)
{
    if (n == num_clks::THREE)
    {
        return scheme{TWODDWAVE_NAME, {{0, 1, 2}, {1, 2, 0}, {2, 0, 1}}, 3u, 2u, 2u};
    }

    return scheme{TWODDWAVE_NAME, {{0, 1, 2, 3}, {1, 2, 3, 0}, {2, 3, 0, 1}, {3, 0, 1, 2}}, 4u, 2u, 2u};
}
/**
 * Returns a hexagonal variation of the 2DDWave clocking as originally defined in \"Clocking and Cell Placement for
 * QCA\" by V. Vankamamidi, M. Ottavi, and F. Lombardi in IEEE Conference on Nanotechnology 2006.
 *
 * @param a Arrangement of the hexagonal layout's shifted rows or columns.
 * @param n Number of clocks.
 * @return Hexagonal 2DDWave clocking scheme.
 */
[[nodiscard]] inline scheme twoddwave_hex(const hex_arrangement a, const num_clks n = num_clks::FOUR)
{
    using cutout = std::vector<std::vector<scheme::clock_number>>;

    // clang-format off

    const cutout odd_3{{0, 1, 2},
                       {1, 2, 0},
                       {1, 2, 0},
                       {2, 0, 1},
                       {2, 0, 1},
                       {0, 1, 2}};

    const cutout even_3{{0, 1, 2},
                        {0, 1, 2},
                        {1, 2, 0},
                        {1, 2, 0},
                        {2, 0, 1},
                        {2, 0, 1}};

    const cutout odd_4{{0, 1, 2, 3},
                       {1, 2, 3, 0},
                       {1, 2, 3, 0},
                       {2, 3, 0, 1},
                       {2, 3, 0, 1},
                       {3, 0, 1, 2},
                       {3, 0, 1, 2},
                       {0, 1, 2, 3}};

    const cutout even_4{{0, 1, 2, 3},
                        {0, 1, 2, 3},
                        {1, 2, 3, 0},
                        {1, 2, 3, 0},
                        {2, 3, 0, 1},
                        {2, 3, 0, 1},
                        {3, 0, 1, 2},
                        {3, 0, 1, 2}};

    // clang-format on

    const bool  odd  = a == hex_arrangement::ODD_ROW || a == hex_arrangement::ODD_COLUMN;
    const auto& rows = [&]() -> const cutout&
    {
        if (n == num_clks::THREE)
        {
            return odd ? odd_3 : even_3;
        }
        return odd ? odd_4 : even_4;
    }();

    // column arrangements shift columns instead of rows, so their cutouts are the transposed row cutouts
    const bool columns = a == hex_arrangement::ODD_COLUMN || a == hex_arrangement::EVEN_COLUMN;

    return scheme{TWODDWAVE_HEX_NAME, columns ? detail::transpose(rows) : rows, detail::count(n), 2u, 2u};
}
/**
 * Returns the USE clocking as defined in \"USE: A Universal, Scalable, and Efficient Clocking Scheme for
 * QCA\" by Caio Araujo T. Campos, Abner L. Marciano, Omar P. Vilela Neto, and Frank Sill Torres in TCAD 2015.
 *
 * @return USE clocking scheme.
 */
[[nodiscard]] inline scheme use()
{
    return scheme{USE_NAME, {{0, 1, 2, 3}, {3, 2, 1, 0}, {2, 3, 0, 1}, {1, 0, 3, 2}}, 4u, 2u, 2u};
}
/**
 * Returns the RES clocking as defined in \"An efficient clocking scheme for quantum-dot cellular automata\" by
 * Mrinal Goswami, Anindan Mondal, Mahabub Hasan Mahalat, Bibhash Sen, and Biplab K. Sikdar in International Journal
 * of Electronics Letters 2019.
 *
 * @return RES clocking scheme.
 */
[[nodiscard]] inline scheme res()
{
    return scheme{RES_NAME, {{3, 0, 1, 2}, {0, 1, 0, 3}, {1, 2, 3, 0}, {0, 3, 2, 1}}, 4u, 3u, 3u};
}
/**
 * Returns the ESR clocking as defined in \"An efficient, scalable, regular clocking scheme based on quantum dot
 * cellular automata\" by Jayanta Pal, Amit Kumar Pramanik, Jyotirmoy Sil Sharma, Apu Kumar Saha, and Bibhash Sen in
 * Analog Integrated Circuits and Signal Processing 2021.
 *
 * @return ESR clocking scheme.
 */
[[nodiscard]] inline scheme esr()
{
    return scheme{ESR_NAME, {{3, 0, 1, 2}, {0, 1, 2, 3}, {1, 2, 3, 0}, {0, 3, 2, 1}}, 4u, 3u, 3u};
}
/**
 * Returns the CFE clocking as defined in \"CFE: a convenient, flexible, and efficient clocking scheme for
 * quantum-dot cellular automata\" by Feifei Deng, Guang-Jun Xie, Xin Cheng, Zhang Zhang, and Yongqiang Zhang in IET
 * Circuits, Devices & Systems 2020.
 *
 * @return CFE clocking scheme.
 */
[[nodiscard]] inline scheme cfe()
{
    return scheme{CFE_NAME, {{0, 1, 0, 1}, {3, 2, 3, 2}, {0, 1, 0, 1}, {3, 2, 3, 2}}, 4u, 3u, 3u};
}
/**
 * Returns the Ripple clocking as defined in \"Ripple Clock Schemes for Quantum-dot Cellular Automata Circuits\" by
 * Prafull Purohit, Master Thesis, Rochester Institute of Technology, 2012.
 *
 * @return Ripple clocking scheme.
 */
[[nodiscard]] inline scheme ripple()
{
    return scheme{RIPPLE_NAME, {{0, 1, 2, 3}, {3, 2, 1, 0}, {0, 1, 2, 3}, {3, 2, 1, 0}}, 4u, 3u, 3u};
}
/**
 * Returns the SRS clocking as defined in \"Simple, robust and systematic QCA clocking scheme for area-efficient
 * nanocircuits\" by Mrinal Goswami, Tonmoy Jyoti Sharma, and Arpita Nath Boruah in International Journal of Electronics
 * Letters 2025.
 *
 * @return SRS clocking scheme.
 */
[[nodiscard]] inline scheme srs()
{
    return scheme{SRS_NAME, {{1, 0, 3, 2}, {2, 3, 0, 1}, {3, 2, 3, 0}, {0, 1, 2, 1}}, 4u, 3u, 3u};
}
/**
 * Returns the BANCS clocking as defined in \"BANCS: Bidirectional Alternating Nanomagnetic Clocking Scheme\"
 * by Ruan Evangelista Formigoni, Omar P. Vilela Neto, and Jose Augusto M. Nacif in SBCCI 2018.
 *
 * @return BANCS clocking scheme.
 */
[[nodiscard]] inline scheme bancs()
{
    return scheme{BANCS_NAME, {{0, 1, 2}, {2, 1, 0}, {2, 0, 1}, {1, 0, 2}, {1, 2, 0}, {0, 2, 1}}, 3u, 2u, 2u};
}
/**
 * Checks whether a given clocking scheme is registered as a cycle-free one. These currently are
 *
 * - COLUMNAR
 * - ROW
 * - 2DDWAVE
 * - 2DDWAVEHEX
 *
 * @param scm Clocking scheme to check.
 * @return `true` iff `scm` is listed as one of the linear clocking schemes.
 */
[[nodiscard]] inline bool is_linear(const scheme& scm) noexcept
{
    static constexpr std::array<std::string_view, 4> linear_schemes{
        {COLUMNAR_NAME, ROW_NAME, TWODDWAVE_NAME, TWODDWAVE_HEX_NAME}};

    return std::ranges::find(linear_schemes, scm.name()) != linear_schemes.cend();
}
/**
 * Exception to be thrown when an unsupported clocking scheme is requested.
 */
class unsupported_scheme_exception : public std::exception
{
  public:
    explicit unsupported_scheme_exception() noexcept : std::exception() {}

    /**
     * Returns the diagnostic message for an unsupported clocking scheme.
     *
     * @return A null-terminated diagnostic message.
     */
    [[nodiscard]] const char* what() const noexcept override
    {
        return "given clocking scheme is unsupported";
    }
};
/**
 * Returns a clocking scheme by name. The lookup ignores case and accepts a trailing `3` or `4` that selects the phase
 * count of a scheme that supports it. Without a suffix, a scheme has its default phase count: 3 for BANCS and 4 for
 * all others. `2DDWAVEHEX` requires a hexagonal arrangement and falls back to `2DDWAVE` without one.
 *
 * @param scheme_name Name of the desired clocking scheme.
 * @param hex Arrangement of the hexagonal layout the scheme is for, or `std::nullopt` for a non-hexagonal layout.
 * @return Clocking scheme that matches `scheme_name`, or `std::nullopt` if no clocking scheme by that name exists or
 * the scheme does not support the requested phase count.
 */
[[nodiscard]] inline std::optional<scheme> get_scheme(const std::string_view               scheme_name,
                                                      const std::optional<hex_arrangement> hex = std::nullopt)
{
    std::string name{scheme_name};
    std::ranges::transform(name, name.begin(), [](const char ch)
                           { return static_cast<char>(std::toupper(static_cast<unsigned char>(ch))); });

    // no base name ends in a digit, so a trailing 3 or 4 is always a phase count
    std::optional<num_clks> phases{};
    if (!name.empty() && (name.back() == '3' || name.back() == '4'))
    {
        phases = name.back() == '3' ? num_clks::THREE : num_clks::FOUR;
        name.pop_back();
    }

    const auto n = phases.value_or(num_clks::FOUR);

    if (name == OPEN_NAME)
    {
        return open(n);
    }
    if (name == COLUMNAR_NAME)
    {
        return columnar(n);
    }
    if (name == ROW_NAME)
    {
        return row(n);
    }
    if (name == TWODDWAVE_NAME)
    {
        return twoddwave(n);
    }
    if (name == TWODDWAVE_HEX_NAME)
    {
        return hex.has_value() ? twoddwave_hex(*hex, n) : twoddwave(n);
    }
    if (name == BANCS_NAME)
    {
        return phases != num_clks::FOUR ? std::optional{bancs()} : std::nullopt;
    }

    // the remaining schemes have four phases only
    if (phases == num_clks::THREE)
    {
        return std::nullopt;
    }
    if (name == USE_NAME)
    {
        return use();
    }
    if (name == RES_NAME)
    {
        return res();
    }
    if (name == ESR_NAME)
    {
        return esr();
    }
    if (name == CFE_NAME)
    {
        return cfe();
    }
    if (name == RIPPLE_NAME)
    {
        return ripple();
    }
    if (name == SRS_NAME)
    {
        return srs();
    }

    return std::nullopt;
}
/**
 * Returns a clocking scheme by name for layouts of type `Lyt`. `2DDWAVEHEX` takes the hexagonal arrangement of `Lyt`.
 * See the non-template overload for the accepted names.
 *
 * @tparam Lyt Layout type.
 * @param scheme_name Name of the desired clocking scheme.
 * @return Clocking scheme that matches `scheme_name`, or `std::nullopt` if no clocking scheme by that name exists.
 */
template <typename Lyt>
[[nodiscard]] std::optional<scheme> get_scheme(const std::string_view scheme_name)
{
    if constexpr (is_hexagonal_layout_v<Lyt>)
    {
        if constexpr (has_odd_row_hex_arrangement_v<Lyt>)
        {
            return get_scheme(scheme_name, hex_arrangement::ODD_ROW);
        }
        else if constexpr (has_even_row_hex_arrangement_v<Lyt>)
        {
            return get_scheme(scheme_name, hex_arrangement::EVEN_ROW);
        }
        else if constexpr (has_odd_column_hex_arrangement_v<Lyt>)
        {
            return get_scheme(scheme_name, hex_arrangement::ODD_COLUMN);
        }
        else if constexpr (has_even_column_hex_arrangement_v<Lyt>)
        {
            return get_scheme(scheme_name, hex_arrangement::EVEN_COLUMN);
        }
    }

    return get_scheme(scheme_name, std::nullopt);
}

}  // namespace fiction::layouts::clocking
