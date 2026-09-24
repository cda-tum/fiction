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
 * @brief Turns a gate-level layout into a cell-level one by applying a gate library.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/layouts/layout_utils.hpp"
#include "fiction/networks/name_utils.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/progress.hpp"

#include <mockturtle/traits.hpp>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

namespace fiction::physical_design
{

/**
 * The highest cell position of the Cartesian cell grid that a gate library's tiles span when applied to a gate-level
 * layout, respecting tilings in which even and odd rows or columns do not line up. `apply_gate_library` sizes QCA,
 * molQCA, and iNML layouts this way.
 *
 * @tparam GateLibrary Gate library whose tile size is used.
 * @tparam GateLyt Gate-level layout type.
 * @param gate_lyt Gate-level layout.
 * @return Highest cell position of the grid, including the layer count of `gate_lyt`.
 */
template <typename GateLibrary, typename GateLyt>
[[nodiscard]] layouts::coords::offset cell_grid_extent(const GateLyt& gate_lyt) noexcept
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");

    using cell_t = layouts::coords::offset;

    const auto rel_to_abs_cell_pos = [](const GateLyt& g, const tile<GateLyt>& t, const cell_t& rel)
    {
        return layouts::relative_to_absolute_cell_position<GateLibrary::gate_x_size(), GateLibrary::gate_y_size()>(g, t,
                                                                                                                   rel);
    };

    const cell_t max_rel_coord = {GateLibrary::gate_x_size() - 1, GateLibrary::gate_y_size() - 1};

    const cell_t first_odd_tile = {gate_lyt.x() != 0 ? 1 : 0, gate_lyt.y() != 0 ? 1 : 0};

    const auto max_coord_even_x = rel_to_abs_cell_pos(gate_lyt, {0, gate_lyt.y()}, max_rel_coord);
    const auto max_coord_odd_x  = rel_to_abs_cell_pos(gate_lyt, {first_odd_tile.x, gate_lyt.y()}, max_rel_coord);
    const auto max_coord_even_y = rel_to_abs_cell_pos(gate_lyt, {gate_lyt.x(), 0}, max_rel_coord);
    const auto max_coord_odd_y  = rel_to_abs_cell_pos(gate_lyt, {gate_lyt.x(), first_odd_tile.y}, max_rel_coord);

    return {std::max(max_coord_even_y.x, max_coord_odd_y.x), std::max(max_coord_even_x.y, max_coord_odd_x.y),
            gate_lyt.z()};
}

namespace detail
{

template <typename GateLibrary, typename GateLyt>
class apply_gate_library_impl
{
  public:
    /** @brief Layout type that the gate library produces. */
    using cell_lyt_t = typename GateLibrary::layout;
    /** @brief Whether the library places SiDBs on lattice sites instead of cells on a Cartesian grid. */
    // NOLINTNEXTLINE(readability-identifier-naming): static class member, not a global constant
    static constexpr bool places_sidbs = std::is_same_v<cell_lyt_t, sidb::layout>;
    /**
     * @brief Grid coordinate of a gate cell. SiDB gates on hexagonal tiles reach negative rows, so they are placed via
     * cube coordinates.
     */
    using cell_t = std::conditional_t<places_sidbs, layouts::coords::cube, layouts::coords::offset>;
    /**
     * @brief Prepares cell mapping with optional gate counts.
     * @param lyt Gate-level source layout.
     * @param callback Receives completed gate mappings.
     * @param lat Lattice of the produced layout if the library places SiDBs.
     */
    explicit apply_gate_library_impl(const GateLyt& lyt, utils::progress_callback callback = {},
                                     const sidb::lattice& lat = sidb::lattice::si_100_2x1()) :
            gate_lyt{lyt},
            cell_lyt{create_cell_layout(gate_lyt, lat)},
            on_progress{std::move(callback)}
    {}
    /**
     * Maps every gate of the gate-level layout to its implementation from the static gate library, runs the library's
     * post-layout optimization if it provides one, and recovers the layout name.
     *
     * @return The produced layout.
     */
    [[nodiscard]] cell_lyt_t run_static_gate_library()
    {
        map_gates([](const GateLyt& g, const tile<GateLyt>& t) { return GateLibrary::set_up_gate(g, t); });

        if constexpr (requires(cell_lyt_t& l) { GateLibrary::post_layout_optimization(l); })
        {
            GateLibrary::post_layout_optimization(cell_lyt);
        }
        if constexpr (has_get_layout_name_v<GateLyt>)
        {
            cell_lyt.set_layout_name(gate_lyt.get_layout_name());
        }

        return cell_lyt;
    }
    /**
     * Maps every gate of the gate-level layout to its implementation from a parameterized gate library such as the
     * SiDB on-the-fly gate library, and recovers the layout name.
     *
     * @tparam Params Type of the parameters of the gate library.
     * @param params Parameters of the gate library.
     * @param defect_surface Optional defect surface.
     * @return The produced layout.
     */
    template <typename Params>
    [[nodiscard]] cell_lyt_t
    run_parameterized_gate_library(const Params&                      params,
                                   const std::optional<sidb::layout>& defect_surface = std::nullopt)
    {
        map_gates([&params, &defect_surface](const GateLyt& g, const tile<GateLyt>& t)
                  { return GateLibrary::set_up_gate(g, t, params, defect_surface); });

        cell_lyt.set_layout_name(networks::get_name(gate_lyt));

        return cell_lyt;
    }

  private:
    /**
     * Gate-level layout.
     */
    GateLyt gate_lyt;
    /**
     * Produced layout.
     */
    cell_lyt_t cell_lyt;
    /** @brief Receives completed gate mappings. */
    utils::progress_callback on_progress;
    /**
     * @brief Creates the empty layout that receives the gates: an SiDB layout on the given lattice, or a cell grid
     * spanning the gate-level layout whose clock zones are the library's tiles and follow the gate-level clocking.
     * @param src Gate-level source layout.
     * @param lat Lattice of an SiDB layout.
     * @return Empty layout.
     */
    [[nodiscard]] static cell_lyt_t create_cell_layout(const GateLyt& src, const sidb::lattice& lat)
    {
        if constexpr (places_sidbs)
        {
            return cell_lyt_t{lat};
        }
        else
        {
            cell_lyt_t lyt{cell_grid_extent<GateLibrary>(src)};

            if constexpr (requires(cell_lyt_t& l) { l.set_tile_size_x(GateLibrary::gate_x_size()); })
            {
                lyt.set_tile_size_x(GateLibrary::gate_x_size());
                lyt.set_tile_size_y(GateLibrary::gate_y_size());

                // if GateLyt and the produced layout share the coordinate type, copy the clocking scheme over
                if constexpr (std::is_same_v<coordinate<cell_lyt_t>, coordinate<GateLyt>>)
                {
                    lyt.replace_clocking_scheme(src.get_clocking_scheme());
                }
                // otherwise, try to find a matching clocking scheme (this discards overwritten clock numbers)
                else if (const auto clk_scheme =
                             layouts::clocking::get_scheme<cell_lyt_t>(src.get_clocking_scheme().name);
                         clk_scheme.has_value())
                {
                    lyt.replace_clocking_scheme(clk_scheme.value());
                }
            }

            return lyt;
        }
    }
    /**
     * @brief Counts nonconstant nodes using the mapping traversal, or skips the scan without a callback.
     * @return Number of nodes mapped to cell implementations.
     */
    [[nodiscard]] std::size_t mapping_count() const
    {
        std::size_t count{};
        if (on_progress)
        {
            gate_lyt.foreach_node([&](const auto& n) { count += !gate_lyt.is_constant(n); });
        }
        return count;
    }
    /**
     * @brief Places the implementation of every nonconstant node in its tile.
     * @tparam SetUpGate Callable on `(const GateLyt&, const tile<GateLyt>&)` returning a gate.
     * @param set_up_gate Returns the implementation of a tile.
     */
    template <typename SetUpGate>
    void map_gates(const SetUpGate& set_up_gate)
    {
        utils::progress_reporter progress{on_progress, "mapping gates", mapping_count()};
        gate_lyt.foreach_node(
            [&, this](const auto& n)
            {
                if (!gate_lyt.is_constant(n))
                {
                    const auto t = gate_lyt.get_tile(n);

                    // retrieve the top-leftmost cell in tile t
                    const auto c = layouts::relative_to_absolute_cell_position<GateLibrary::gate_x_size(),
                                                                               GateLibrary::gate_y_size()>(
                        gate_lyt, t, cell_t{0, 0});

                    assign_gate(c, set_up_gate(gate_lyt, t), n);
                    progress.advance();
                }
            });
    }
    /**
     * Assigns a gate implementation to the cells of its tile. Input and output cells of a cell grid receive the node
     * name. If the layout has synchronization elements, the tile's synchronization delay goes to the clock zone that
     * contains the tile; a ground wire and a crossing wire share one clock zone, which keeps the larger delay.
     *
     * @param c Top-left cell of the tile where the gate is placed.
     * @param g Gate implementation.
     * @param n Corresponding node in the gate-level layout.
     */
    void assign_gate(const cell_t& c, const typename GateLibrary::gate& g, const mockturtle::node<GateLyt>& n)
    {
        if constexpr (requires(cell_lyt_t& l, const cell_t& p) {
                          l.assign_synchronization_element(l.get_clock_zone(p), 1u);
                      })
        {
            if (const auto delay = gate_lyt.get_synchronization_element(gate_lyt.get_tile(n));
                delay > cell_lyt.get_synchronization_element(c))
            {
                cell_lyt.assign_synchronization_element(cell_lyt.get_clock_zone(c), delay);
            }
        }

        using cell_type = typename GateLibrary::cell_type;

        for (auto y = 0ul; y < g.size(); ++y)
        {
            for (auto x = 0ul; x < g[y].size(); ++x)
            {
                const cell_t pos{c.x + x, c.y + y, c.z};
                const auto   type{g[y][x]};

                if (type == cell_type::EMPTY)
                {
                    continue;
                }

                if constexpr (places_sidbs)
                {
                    cell_lyt.assign_sidb(sidb::to_lattice_site(pos), type);
                }
                else
                {
                    cell_lyt.assign_cell_type(pos, type);

                    if (type == cell_type::INPUT || type == cell_type::OUTPUT)
                    {
                        cell_lyt.assign_cell_name(pos, gate_lyt.get_name(n));
                    }
                }
            }
        }
    }
};

}  // namespace detail

/**
 * Applies a gate library to a given gate-level layout and, thereby, creates and returns the layout type the library
 * produces, e.g., `qca::layout` for `qca::qca_one_library` or `sidb::layout` for `sidb::bestagon_library`. The gate
 * library type should provide all functions specified in `fcn::gate_library`. It is, thus, easiest to extend
 * `fcn::gate_library` to implement a new gate library.
 *
 * A cell grid spans the gate-level layout; if it has tile-based clocking, its clock zones are the library's tiles and
 * follow the gate-level clocking, and, if it has synchronization elements, each clock zone receives the
 * synchronization delay of its gate tile. The delay therefore also covers cells that are added to the zone later,
 * e.g., via cells. Input and output cells carry the names of their nodes. An SiDB layout lies on the H-Si(100)-2x1
 * lattice.
 *
 * May pass through, and thereby throw, an `unsupported_gate_type_exception` or an
 * `unsupported_gate_orientation_exception`.
 *
 * @tparam GateLibrary Type of the gate library to apply.
 * @tparam GateLyt Type of the gate-level layout to apply the library to.
 * @param lyt The gate-level layout.
 * @param on_progress Optional callback reporting completed nonconstant gate mappings.
 * @return A layout that implements `lyt`'s gate types with building blocks defined in `GateLibrary`.
 */
template <typename GateLibrary, typename GateLyt>
[[nodiscard]] typename GateLibrary::layout apply_gate_library(const GateLyt&           lyt,
                                                              utils::progress_callback on_progress = {})
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(mockturtle::has_is_constant_v<GateLyt>, "GateLyt does not implement the is_constant function");
    static_assert(mockturtle::has_foreach_node_v<GateLyt>, "GateLyt does not implement the foreach_node function");

    detail::apply_gate_library_impl<GateLibrary, GateLyt> p{lyt, std::move(on_progress)};

    return p.run_static_gate_library();
}

/**
 * @brief Applies a static SiDB gate library to a gate-level layout on a defective surface: the gates are placed on the
 * surface's lattice as with `apply_gate_library`, and the surface's defects are copied into the result.
 *
 * @tparam GateLibrary Gate library type.
 * @tparam GateLyt Gate-level layout type.
 * @param lyt The gate-level layout.
 * @param defect_surface The surface with the defects.
 * @return The SiDB layout with the gates and the defects.
 */
template <typename GateLibrary, typename GateLyt>
[[nodiscard]] sidb::layout apply_gate_library_to_defective_surface(const GateLyt&      lyt,
                                                                   const sidb::layout& defect_surface)
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(mockturtle::has_is_constant_v<GateLyt>, "GateLyt does not implement the is_constant function");
    static_assert(mockturtle::has_foreach_node_v<GateLyt>, "GateLyt does not implement the foreach_node function");
    static_assert(std::is_same_v<typename GateLibrary::layout, sidb::layout>, "GateLibrary must produce SiDB layouts");

    detail::apply_gate_library_impl<GateLibrary, GateLyt> p{lyt, {}, defect_surface.get_lattice()};

    auto result = p.run_static_gate_library();

    defect_surface.foreach_defect([&result](const auto& sd) { result.assign_defect(sd.first, sd.second); });

    return result;
}
/**
 * Applies a parameterized gate library to a given gate-level layout and, thereby, creates and returns the layout type
 * the library produces.
 *
 * May pass through, and thereby throw, an `unsupported_gate_type_exception`, an
 * `unsupported_gate_orientation_exception` and any further custom exceptions of the gate libraries.
 *
 * @tparam GateLibrary Type of the gate library to apply.
 * @tparam GateLyt Type of the gate-level layout to apply the library to.
 * @tparam Params Type of the parameter used for SiDB on-the-fly gate library.
 * @param lyt The gate-level layout.
 * @param params Parameter for the gate library.
 * @return A layout that implements `lyt`'s gate types with building blocks defined in `GateLibrary`.
 */
template <typename GateLibrary, typename GateLyt, typename Params>
[[nodiscard]] typename GateLibrary::layout apply_parameterized_gate_library(const GateLyt& lyt, const Params& params)
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(mockturtle::has_is_constant_v<GateLyt>, "GateLyt does not implement the is_constant function");
    static_assert(mockturtle::has_foreach_node_v<GateLyt>, "GateLyt does not implement the foreach_node function");

    detail::apply_gate_library_impl<GateLibrary, GateLyt> p{lyt};

    return p.template run_parameterized_gate_library<Params>(params);
}

/**
 * @brief Applies a parameterized SiDB gate library to a gate-level layout on a defective surface: the library designs
 * every gate with the surface's defects near its tile in place, the gates are placed on the surface's lattice, and the
 * surface's defects are copied into the result.
 *
 * @tparam GateLibrary Gate library type.
 * @tparam GateLyt Gate-level layout type.
 * @tparam Params Parameter type of the gate library.
 * @param lyt The gate-level layout.
 * @param params Parameters of the gate library.
 * @param defect_surface The surface with the defects.
 * @return The SiDB layout with the gates and the defects.
 */
template <typename GateLibrary, typename GateLyt, typename Params>
[[nodiscard]] sidb::layout apply_parameterized_gate_library_to_defective_surface(const GateLyt&      lyt,
                                                                                 const Params&       params,
                                                                                 const sidb::layout& defect_surface)
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(mockturtle::has_is_constant_v<GateLyt>, "GateLyt does not implement the is_constant function");
    static_assert(mockturtle::has_foreach_node_v<GateLyt>, "GateLyt does not implement the foreach_node function");
    static_assert(std::is_same_v<typename GateLibrary::layout, sidb::layout>, "GateLibrary must produce SiDB layouts");

    detail::apply_gate_library_impl<GateLibrary, GateLyt> p{lyt, {}, defect_surface.get_lattice()};

    auto result = p.template run_parameterized_gate_library<Params>(params, defect_surface);

    defect_surface.foreach_defect([&result](const auto& sd) { result.assign_defect(sd.first, sd.second); });

    return result;
}

}  // namespace fiction::physical_design
#pragma GCC diagnostic pop
