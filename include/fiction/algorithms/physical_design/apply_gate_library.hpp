//
// Created by marcel on 28.06.21.
//

#ifndef FICTION_APPLY_GATE_LIBRARY_HPP
#define FICTION_APPLY_GATE_LIBRARY_HPP

#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/name_utils.hpp"

#include <optional>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>

#include <cstdint>
#endif
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <functional>
#include <type_traits>

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

namespace fiction
{

namespace detail
{

template <typename CellLyt, typename GateLibrary, typename GateLyt>
class apply_gate_library_impl
{
  public:
    explicit apply_gate_library_impl(const GateLyt& lyt) :
            gate_lyt{lyt},
            cell_lyt{determine_aspect_ratio_for_cell_level_layout(gate_lyt)}
    {
        cell_lyt.resize(aspect_ratio_t<CellLyt>{((gate_lyt.x() + 1) * GateLibrary::gate_x_size()) - 1,
                                                ((gate_lyt.y() + 1) * GateLibrary::gate_y_size()) - 1, gate_lyt.z()});
        cell_lyt.set_tile_size_x(GateLibrary::gate_x_size());
        cell_lyt.set_tile_size_y(GateLibrary::gate_y_size());

        // if GateLyt and CellLyt are based on the same coordinate type, copy the clocking scheme over
        if constexpr (std::is_same_v<coordinate<CellLyt>, coordinate<GateLyt>>)
        {
            cell_lyt.replace_clocking_scheme(gate_lyt.get_clocking_scheme());
        }
        // otherwise, try to find a matching clocking scheme (this will discard overwritten clock numbers)
        else
        {
            if (const auto clk_scheme = get_clocking_scheme<CellLyt>(gate_lyt.get_clocking_scheme().name);
                clk_scheme.has_value())
            {
                cell_lyt.replace_clocking_scheme(clk_scheme.value());
            }
        }
    }

    /**
     * Run the cell layout generation process.
     *
     * This function performs the cell layout generation process based on the gate library and the gate-level layout
     * information provided by `GateLibrary` and `gate_lyt`. It iterates through the nodes in the gate-level layout and
     * maps gates to cell implementations based on their corresponding positions and types. Optionally, it performs
     * post-layout optimization and sets the layout name if certain conditions are met.
     *
     * @param defect_lyt Optional defect surface.
     * @return A `CellLyt` object representing the generated cell layout.
     */
    [[nodiscard]] CellLyt run_static_gate_library(const std::optional<CellLyt>& defect_surface = std::nullopt)
    {
#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(gate_lyt.size()), "[i] applying gate library: |{0}|"};
#endif

        gate_lyt.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] auto i)
            {
                if (!gate_lyt.is_constant(n))
                {
                    const auto t = gate_lyt.get_tile(n);

                    // retrieve the top-leftmost cell in tile t
                    const auto c =
                        relative_to_absolute_cell_position<GateLibrary::gate_x_size(), GateLibrary::gate_y_size(),
                                                           GateLyt, CellLyt>(gate_lyt, t, cell<CellLyt>{0, 0});

                    assign_gate(c, GateLibrary::set_up_gate(gate_lyt, t), n);
                }
#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif
            });

        // perform post-layout optimization if necessary
        if constexpr (has_post_layout_optimization_v<GateLibrary, CellLyt>)
        {
            GateLibrary::post_layout_optimization(cell_lyt);
        }
        // if available, recover layout name
        if constexpr (has_get_layout_name_v<GateLyt> && has_set_layout_name_v<CellLyt>)
        {
            cell_lyt.set_layout_name(gate_lyt.get_layout_name());
        }

        if constexpr (is_sidb_defect_surface_v<CellLyt>)
        {
            if (defect_surface.has_value())
            {
                // due to issue with windows-2019 Visual Studio 16 2019 and v142. It doesn't compile without using
                // "copy_lyt". When using "cell_lyt.assign_sidb_defect(...)" inside the lambda function, it results in
                // the error: "error C2059: syntax error: '.'".
                auto copy_lyt = cell_lyt.clone();
                // copy the original defects over to the circuit since they are gone when converting the gate-level
                // layout to the cell-level layout.
                defect_surface.value().foreach_sidb_defect([this, &copy_lyt](const auto& def)
                                                           { copy_lyt.assign_sidb_defect(def.first, def.second); });
                return copy_lyt;
            }
        }

        return cell_lyt;
    }
    /**
     * Run the cell layout generation process.
     *
     * This function performs the cell layout generation process based on the SiDB on-the-fly gate library and the
     * gate-level layout information provided by `GateLibrary` and `gate_lyt`. It iterates through the nodes in the
     * gate-level layout and maps gates to cell implementations based on their corresponding positions and types.
     * Optionally, it performs post-layout optimization and sets the layout name if certain conditions are met.
     *
     * @tparam Params Type of the Parameters used for the SiDB on-the-fly gate library.
     * @param params Parameters used for the SiDB on-the-fly gate library.
     * @param defect_surface Optional defect surface.
     * @return A `CellLyt` object representing the generated cell layout.
     */
    template <typename Params>
    [[nodiscard]] auto run_parameterized_gate_library(const Params&                 params,
                                                      const std::optional<CellLyt>& defect_surface = std::nullopt)
    {
#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(gate_lyt.size()), "[i] applying gate library: |{0}|"};
#endif
        // perform post-layout optimization if necessary
        if constexpr (has_post_layout_optimization_v<GateLibrary, CellLyt>)
        {
            GateLibrary::post_layout_optimization(gate_lyt);
        }

        gate_lyt.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] auto i)
            {
                if (!gate_lyt.is_constant(n))
                {
                    const auto t = gate_lyt.get_tile(n);

                    // retrieve the top-leftmost cell in tile t
                    const auto c =
                        relative_to_absolute_cell_position<GateLibrary::gate_x_size(), GateLibrary::gate_y_size(),
                                                           GateLyt, CellLyt>(gate_lyt, t, cell<CellLyt>{0, 0});

                    assign_gate(c,
                                GateLibrary::template set_up_gate<GateLyt, CellLyt, Params>(gate_lyt, t, params,
                                                                                            defect_surface),
                                n);
                }
#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif
            });

        // if available, recover layout name
        cell_lyt.set_layout_name(get_name(gate_lyt));

        if constexpr (is_sidb_defect_surface_v<CellLyt>)
        {
            if (defect_surface.has_value())
            {
                // due to issue with windows-2019 Visual Studio 16 2019 and v142. It doesn't compile without using
                // "copy_lyt". When using "cell_lyt.assign_sidb_defect(...)" inside the lambda function, it results in
                // the error: "error C2059: syntax error: '.'".
                auto copy_lyt = cell_lyt.clone();
                // copy the original defects over to the circuit since they are gone when converting the gate-level
                // layout to the cell-level layout.
                defect_surface.value().foreach_sidb_defect([this, &copy_lyt](const auto& def)
                                                           { copy_lyt.assign_sidb_defect(def.first, def.second); });
                return copy_lyt;
            }
        }

        return cell_lyt;
    }

  private:
    /**
     * Gate-level layout.
     */
    GateLyt gate_lyt;
    /**
     * Cell-level layout.
     */
    CellLyt cell_lyt;
    /**
     * This function assigns a given FCN gate implementation to the total cell layout.
     *
     * @param c Top-left cell of the tile where the gate is placed.
     * @param g Gate implementation.
     * @param n Corresponding node in the gate-level layout.
     */
    void assign_gate(const cell<CellLyt>& c, const typename GateLibrary::fcn_gate& g,
                     const mockturtle::node<GateLyt>& n)
    {
        const auto start_x = c.x;
        const auto start_y = c.y;
        const auto layer   = c.z;

        for (auto y = 0ul; y < g.size(); ++y)
        {
            for (auto x = 0ul; x < g[y].size(); ++x)
            {
                const cell<CellLyt> pos{start_x + x, start_y + y, layer};
                const auto          type{g[y][x]};

                if (!technology<CellLyt>::is_empty_cell(type))
                {
                    cell_lyt.assign_cell_type(pos, type);
                }

                // set IO names
                if (technology<CellLyt>::is_input_cell(type) || technology<CellLyt>::is_output_cell(type))
                {
                    cell_lyt.assign_cell_name(pos, gate_lyt.get_name(n));
                }
            }
        }
    }
    /**
     * Computes the (inclusively) bounding coordinate for a cell-level layout that is derived from the dimensions of the
     * given gate-level layout, while respecting tiling geometry in which even and odd rows/columns do not line up.
     *
     * @param gate_lyt Gate-level layout of which the dimensions are read.
     * @return Aspect ratio for a cell-level layout that corresponds to the dimensions of the given gate-level layout.
     */
    static aspect_ratio_t<CellLyt> determine_aspect_ratio_for_cell_level_layout(const GateLyt& gate_lyt) noexcept
    {
        const std::function<cell<CellLyt>(GateLyt, tile<GateLyt>, cell<CellLyt>)> rel_to_abs_cell_pos =
            relative_to_absolute_cell_position<GateLibrary::gate_x_size(), GateLibrary::gate_y_size(), GateLyt,
                                               CellLyt>;

        const cell<CellLyt> max_rel_coord = {GateLibrary::gate_x_size() - 1, GateLibrary::gate_y_size() - 1};

        const cell<CellLyt> first_odd_tile = {gate_lyt.x() != 0 ? 1 : 0, gate_lyt.y() != 0 ? 1 : 0};

        const auto max_coord_even_x = rel_to_abs_cell_pos(gate_lyt, {0, gate_lyt.y()}, max_rel_coord);
        const auto max_coord_odd_x  = rel_to_abs_cell_pos(gate_lyt, {first_odd_tile.x, gate_lyt.y()}, max_rel_coord);
        const auto max_coord_even_y = rel_to_abs_cell_pos(gate_lyt, {gate_lyt.x(), 0}, max_rel_coord);
        const auto max_coord_odd_y  = rel_to_abs_cell_pos(gate_lyt, {gate_lyt.x(), first_odd_tile.y}, max_rel_coord);

        return aspect_ratio_t<CellLyt>{coordinate<CellLyt>{std::max(max_coord_even_y.x, max_coord_odd_y.x),
                                                           std::max(max_coord_even_x.y, max_coord_odd_x.y)}};
    }
};

}  // namespace detail

/**
 * Applies a gate library to a given gate-level layout and, thereby, creates and returns a cell-level layout. The gate
 * library type should provide all functions specified in fcn_gate_library. It is, thus, easiest to extend
 * fcn_gate_library to implement a new gate library. Examples are `qca_one_library`, `inml_topolinano_library`, and
 * `sidb_bestagon_library`.
 *
 * May pass through, and thereby throw, an `unsupported_gate_type_exception` or an
 * `unsupported_gate_orientation_exception`.
 *
 * @tparam CellLyt Type of the returned cell-level layout.
 * @tparam GateLibrary Type of the gate library to apply.
 * @tparam GateLyt Type of the gate-level layout to apply the library to.
 * @param lyt The gate-level layout.
 * @return A cell-level layout that implements `lyt`'s gate types with building blocks defined in `GateLibrary`.
 */
template <typename CellLyt, typename GateLibrary, typename GateLyt>
[[nodiscard]] CellLyt apply_gate_library(const GateLyt& lyt)
{
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(!has_siqad_coord_v<CellLyt>, "CellLyt cannot have SiQAD coordinates");
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(mockturtle::has_is_constant_v<GateLyt>, "GateLyt does not implement the is_constant function");
    static_assert(mockturtle::has_foreach_node_v<GateLyt>, "GateLyt does not implement the foreach_node function");

    static_assert(std::is_same_v<technology<CellLyt>, technology<GateLibrary>>,
                  "CellLyt and GateLibrary must implement the same technology");

    detail::apply_gate_library_impl<CellLyt, GateLibrary, GateLyt> p{lyt};

    return p.run_static_gate_library();
}

/**
 * Applies a gate library to a given gate-level layout and maps the SiDB and defect locations onto a defect surface. The
 * gate library type should provide all functions specified in fcn_gate_library. It is, thus, easiest to extend
 * fcn_gate_library to implement a new gate library. Examples are `qca_one_library`, `inml_topolinano_library`, and
 * `sidb_bestagon_library`.
 *
 * May pass through, and thereby throw, an `unsupported_gate_type_exception` or an
 * `unsupported_gate_orientation_exception`.
 *
 * @tparam CellLyt Type of the returned cell-level layout.
 * @tparam GateLibrary Type of the gate library to apply.
 * @tparam GateLyt Type of the gate-level layout to apply the library to.
 * @param lyt The gate-level layout.
 * @return A cell-level layout that implements `lyt`'s gate types with building blocks defined in `GateLibrary`.
 */
template <typename DefectLyt, typename GateLibrary, typename GateLyt>
[[nodiscard]] DefectLyt apply_gate_library_to_defective_surface(const GateLyt& lyt, const DefectLyt& defect_surface)
{
    static_assert(is_cell_level_layout_v<DefectLyt>, "DefectLyt is not a cell-level layout");
    static_assert(is_sidb_defect_surface_v<DefectLyt>, "DefectLyt is not an SiDB defect surface");
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(mockturtle::has_is_constant_v<GateLyt>, "GateLyt does not implement the is_constant function");
    static_assert(mockturtle::has_foreach_node_v<GateLyt>, "GateLyt does not implement the foreach_node function");

    static_assert(std::is_same_v<technology<DefectLyt>, technology<GateLibrary>>,
                  "DefectLyt and GateLibrary must implement the same technology");

    detail::apply_gate_library_impl<DefectLyt, GateLibrary, GateLyt> p{lyt};

    return p.run_static_gate_library(defect_surface);
}
/**
 * Applies a parameterized gate library to a given
 * gate-level layout and, thereby, creates and returns a cell-level layout.
 *
 * May pass through, and thereby throw, an `unsupported_gate_type_exception`, an
 * `unsupported_gate_orientation_exception` and any further custom exceptions of the gate libraries.
 *
 * @tparam CellLyt Type of the returned cell-level layout.
 * @tparam GateLibrary Type of the gate library to apply.
 * @tparam GateLyt Type of the gate-level layout to apply the library to.
 * @tparam Params Type of the parameter used for SiDB on-the-fly gate library.
 * @param lyt The gate-level layout.
 * @param params Parameter for the gate library.
 * @return A cell-level layout that implements `lyt`'s gate types with building blocks defined in `GateLibrary`.
 */
template <typename CellLyt, typename GateLibrary, typename GateLyt, typename Params>
[[nodiscard]] CellLyt apply_parameterized_gate_library(const GateLyt& lyt, const Params& params)
{
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(has_cube_coord_v<CellLyt>, "CellLyt must be based on cube coordinates");
    static_assert(mockturtle::has_is_constant_v<GateLyt>, "GateLyt does not implement the is_constant function");
    static_assert(mockturtle::has_foreach_node_v<GateLyt>, "GateLyt does not implement the foreach_node function");

    static_assert(std::is_same_v<technology<CellLyt>, technology<GateLibrary>>,
                  "CellLyt and GateLibrary must implement the same technology");

    detail::apply_gate_library_impl<CellLyt, GateLibrary, GateLyt> p{lyt};

    // Run the gate library with the parameters
    const CellLyt result = p.template run_parameterized_gate_library<Params>(params);

    return result;
}

/**
 * Applies a defect-aware parameterized gate library to a given
 * gate-level layout and, thereby, creates and returns a cell-level layout.
 *
 * May pass through, and thereby throw, an `unsupported_gate_type_exception`, an
 * `unsupported_gate_orientation_exception` and any further custom exceptions of the gate libraries.
 *
 * @tparam DefectLyt Type of the returned cell-level layout.
 * @tparam GateLibrary Type of the gate library to apply.
 * @tparam GateLyt Type of the gate-level layout to apply the library to.
 * @tparam Params Type of the parameter used for SiDB on-the-fly gate library.
 * @param lyt The gate-level layout.
 * @param params Parameter for the gate library.
 * @param defect_lyt Defect surface.
 * @return A cell-level layout that implements `lyt`'s gate types with building blocks defined in `GateLibrary`.
 */
template <typename DefectLyt, typename GateLibrary, typename GateLyt, typename Params>
[[nodiscard]] DefectLyt apply_parameterized_gate_library_to_defective_surface(const GateLyt& lyt, const Params& params,
                                                                              const DefectLyt& defect_surface)
{
    static_assert(is_cell_level_layout_v<DefectLyt>, "DefectLyt is not a cell-level layout");
    static_assert(is_sidb_defect_surface_v<DefectLyt>, "DefectLyt is not an SiDB defect surface");
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(has_cube_coord_v<DefectLyt>, "DefectLyt must be based on cube coordinates");
    static_assert(mockturtle::has_is_constant_v<GateLyt>, "GateLyt does not implement the is_constant function");
    static_assert(mockturtle::has_foreach_node_v<GateLyt>, "GateLyt does not implement the foreach_node function");

    static_assert(std::is_same_v<technology<DefectLyt>, technology<GateLibrary>>,
                  "DefectLyt and GateLibrary must implement the same technology");

    detail::apply_gate_library_impl<DefectLyt, GateLibrary, GateLyt> p{lyt};

    // Run the gate library with the parameters
    const DefectLyt result = p.template run_parameterized_gate_library<Params>(params, defect_surface);

    return result;
}

}  // namespace fiction

#pragma GCC diagnostic pop

#endif  // FICTION_APPLY_GATE_LIBRARY_HPP
