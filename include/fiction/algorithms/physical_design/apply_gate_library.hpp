//
// Created by marcel on 28.06.21.
//

#ifndef FICTION_APPLY_GATE_LIBRARY_HPP
#define FICTION_APPLY_GATE_LIBRARY_HPP

#include "fiction/technology/inml_topolinano_library.hpp"
#include "fiction/technology/qca_one_library.hpp"
#include "fiction/technology/sidb_bestagon_library.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/traits.hpp>

#include <cmath>
#include <type_traits>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wconversion"

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
            cell_lyt{aspect_ratio<CellLyt>{((gate_lyt.x() + 1) * GateLibrary::gate_x_size()) - 1,
                                           ((gate_lyt.y() + 1) * GateLibrary::gate_y_size()) - 1, gate_lyt.z()},
                     gate_lyt.get_clocking_scheme(), "", GateLibrary::gate_x_size(), GateLibrary::gate_y_size()}
    {}

    CellLyt run()
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

                    cell<CellLyt> c{};

                    // Cartesian layouts
                    if constexpr (is_cartesian_layout_v<GateLyt>)
                    {
                        c = {t.x * GateLibrary::gate_x_size(), t.y * GateLibrary::gate_y_size(), t.z};
                    }
                    // Shifted Cartesian layouts
                    else if constexpr (is_shifted_cartesian_layout_v<GateLyt>)
                    {
                        if constexpr (has_horizontally_shifted_cartesian_orientation_v<GateLyt>)
                        {
                            c = {t.x * GateLibrary::gate_x_size(),
                                 static_cast<decltype(c.y)>(t.y * (GateLibrary::gate_y_size())), t.z};
                        }
                        else if constexpr (has_vertically_shifted_cartesian_orientation_v<GateLyt>)
                        {
                            c = {static_cast<decltype(c.x)>(t.x * (GateLibrary::gate_x_size())),
                                 t.y * (GateLibrary::gate_y_size()), t.z};
                        }

                        if constexpr (has_odd_row_cartesian_arrangement_v<GateLyt>)
                        {
                            if (gate_lyt.is_in_odd_row(t))
                            {
                                // odd rows are shifted in by width / 2
                                c.x +=
                                    static_cast<decltype(c.x)>(static_cast<double>(GateLibrary::gate_x_size()) / 2.0);
                            }
                        }
                        else if constexpr (has_even_row_cartesian_arrangement_v<GateLyt>)
                        {
                            if (gate_lyt.is_in_even_row(t))
                            {
                                // even rows are shifted in by width / 2
                                c.x +=
                                    static_cast<decltype(c.x)>(static_cast<double>(GateLibrary::gate_x_size()) / 2.0);
                            }
                        }
                        else if constexpr (has_odd_column_cartesian_arrangement_v<GateLyt>)
                        {
                            if (gate_lyt.is_in_odd_column(t))
                            {
                                // odd columns are shifted in by height / 2
                                c.y +=
                                    static_cast<decltype(c.y)>(static_cast<double>(GateLibrary::gate_y_size()) / 2.0);
                            }
                        }
                        else if constexpr (has_even_column_cartesian_arrangement_v<GateLyt>)
                        {
                            if (gate_lyt.is_in_even_column(t))
                            {
                                // even columns are shifted in by height / 2
                                c.y +=
                                    static_cast<decltype(c.y)>(static_cast<double>(GateLibrary::gate_y_size()) / 2.0);
                            }
                        }
                    }
                    // hexagonal layouts
                    else if constexpr (is_hexagonal_layout_v<GateLyt>)
                    {
                        if constexpr (has_pointy_top_hex_orientation_v<GateLyt>)
                        {
                            // vertical distance between pointy top hexagons is height * 3/4
                            c = {t.x * GateLibrary::gate_x_size(),
                                 static_cast<decltype(c.y)>(t.y * (GateLibrary::gate_y_size() * 3 / 4)), t.z};
                        }
                        else if constexpr (has_flat_top_hex_orientation_v<GateLyt>)
                        {
                            // horizontal distance between flat top hexagons is width * 3/4
                            c = {static_cast<decltype(c.x)>(t.x * (GateLibrary::gate_x_size() * 3 / 4)),
                                 t.y * (GateLibrary::gate_y_size()), t.z};
                        }

                        if constexpr (has_odd_row_hex_arrangement_v<GateLyt>)
                        {
                            if (gate_lyt.is_in_odd_row(t))
                            {
                                // odd rows are shifted in by width / 2
                                c.x +=
                                    static_cast<decltype(c.x)>(static_cast<double>(GateLibrary::gate_x_size()) / 2.0);
                            }
                        }
                        else if constexpr (has_even_row_hex_arrangement_v<GateLyt>)
                        {
                            if (gate_lyt.is_in_even_row(t))
                            {
                                // even rows are shifted in by width / 2
                                c.x +=
                                    static_cast<decltype(c.x)>(static_cast<double>(GateLibrary::gate_x_size()) / 2.0);
                            }
                        }
                        else if constexpr (has_odd_column_hex_arrangement_v<GateLyt>)
                        {
                            if (gate_lyt.is_in_odd_column(t))
                            {
                                // odd columns are shifted in by height / 2
                                c.y +=
                                    static_cast<decltype(c.y)>(static_cast<double>(GateLibrary::gate_y_size()) / 2.0);
                            }
                        }
                        else if constexpr (has_even_column_hex_arrangement_v<GateLyt>)
                        {
                            if (gate_lyt.is_in_even_column(t))
                            {
                                // even columns are shifted in by height / 2
                                c.y +=
                                    static_cast<decltype(c.y)>(static_cast<double>(GateLibrary::gate_y_size()) / 2.0);
                            }
                        }
                    }

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

        return cell_lyt;
    }

  private:
    GateLyt gate_lyt;
    CellLyt cell_lyt;

    void assign_gate(const typename CellLyt::cell& c, const typename GateLibrary::fcn_gate& g,
                     const mockturtle::node<GateLyt>& n)
    {
        auto start_x = c.x;
        auto start_y = c.y;
        auto layer   = c.z;

        for (auto y = 0ul; y < g.size(); ++y)
        {
            for (auto x = 0ul; x < g[y].size(); ++x)
            {
                const typename CellLyt::cell      pos{start_x + x, start_y + y, layer};
                const typename CellLyt::cell_type type{g[y][x]};

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
};

}  // namespace detail

/**
 * Applies a gate library to a given gate-level layout and, thereby, creates and returns a cell-level layout. The gate
 * library type should provide all functions specified in fcn_gate_library. It is, thus, easiest to extend
 * fcn_gate_library to implement a new gate library. Examples are qca_one_library, inml_topolinano_library, and
 * sidb_bestagon_library.
 *
 * May pass through, and thereby throw, an 'unsupported_gate_type_exception' or an
 * 'unsupported_gate_orientation_exception'.
 *
 * @tparam CellLyt Type of the returned cell-level layout.
 * @tparam GateLibrary Type of the gate library to apply.
 * @tparam GateLyt Type of the gate-level layout to apply the library to.
 * @param lyt The gate-level layout.
 * @return A cell-level layout that implements lyt's gate types with building blocks defined in GateLibrary.
 */
template <typename CellLyt, typename GateLibrary, typename GateLyt>
CellLyt apply_gate_library(const GateLyt& lyt)
{
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(mockturtle::has_is_constant_v<GateLyt>, "GateLyt does not implement the is_constant function");
    static_assert(mockturtle::has_foreach_node_v<GateLyt>, "GateLyt does not implement the foreach_node function");

    static_assert(std::is_same_v<technology<CellLyt>, technology<GateLibrary>>,
                  "CellLyt and GateLibrary must implement the same technology");

    detail::apply_gate_library_impl<CellLyt, GateLibrary, GateLyt> p{lyt};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#pragma GCC diagnostic pop

#endif  // FICTION_APPLY_GATE_LIBRARY_HPP
