//
// Created by marcel on 01.04.22.
//

#ifndef FICTION_SIDB_SURFACE_ANALYSIS_HPP
#define FICTION_SIDB_SURFACE_ANALYSIS_HPP

#include "fiction/technology/cell_ports.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/hash.hpp>

#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace fiction
{

/**
 * This alias represents a black list of gates that cannot be placed on certain positions on a (layout) surface in
 * certain rotations. The type is just a map of tile positions to another map that associates gate functions with port
 * lists. The second map ensures that each gate function stays unique while the port lists represent the black listed
 * gate rotations.
 *
 * An empty port list vector means that the gate cannot be placed on the associated tile position AT ALL. This notion is
 * to be used preferably as it, e.g., helps the exact physical design algorithm to convert these assertions into unit
 * clauses which significantly helps runtime.
 */
template <typename Lyt, typename PortType>
using surface_black_list =
    std::unordered_map<tile<Lyt>, std::unordered_map<kitty::dynamic_truth_table, std::vector<port_list<PortType>>,
                                                     kitty::hash<kitty::dynamic_truth_table>>>;
/**
 * Analyzes a given defective SiDB surface and matches it against gate tiles provided by a library. Any gate type that
 * cannot be realized on a certain tile due to disturbances caused by defects gets blacklisted on said tile. The black
 * list is then returned by this function.
 *
 * @note The given gate library must implement both the `get_functional_implementations()` and `get_gate_ports()`
 * functions.
 *
 * @tparam GateLibrary FCN gate library type to fetch the gate descriptions from.
 * @tparam GateLyt Gate-level layout type that specifies the tiling of the SiDB surface.
 * @tparam CellLyt Cell-level layout type that is underlying to the SiDB surface.
 * @param gate_lyt Gate-level layout instance that specifies the aspect ratio.
 * @param surface SiDB surface that instantiates the defects.
 * @return A black list of gate functions associated with tiles.
 */
template <typename GateLibrary, typename GateLyt, typename CellLyt>
[[nodiscard]] auto sidb_surface_analysis(const GateLyt& gate_lyt, const sidb_defect_surface<CellLyt>& surface) noexcept
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(std::is_same_v<technology<CellLyt>, sidb_technology>, "CellLyt is not an SiDB layout");

    static_assert(has_get_functional_implementations_v<GateLibrary>,
                  "GateLibrary does not implement the get_functional_implementations function");
    static_assert(has_get_gate_ports_v<GateLibrary>, "GateLibrary does not implement the get_gate_ports function");
    static_assert(std::is_same_v<technology<CellLyt>, technology<GateLibrary>>,
                  "CellLyt and GateLibrary must implement the same technology");

    // fetch the port type used by the gate library
    using port_type = typename decltype(GateLibrary::get_gate_ports())::mapped_type::value_type::port_type;

    surface_black_list<GateLyt, port_type> black_list{};

    const auto sidbs_affected_by_defects = surface.all_affected_sidbs();
    const auto gate_implementations      = GateLibrary::get_functional_implementations();
    const auto gate_ports                = GateLibrary::get_gate_ports();

    // a lambda that analyzes defect impact on a gate at a given layout tile
    // it had to be extracted from the foreach_tile lambda because its nesting caused an C1001: internal compiler error
    // on Visual Studio 17 (2022) as it could not access GateLibrary::gate_x_size() and GateLibrary::gate_y_size()
    // even though that should be possible and is perfectly valid C++ code... either way, this workaround fixes it
    const auto analyze_gate = [&](const auto& it, const auto& t) noexcept
    {
        const auto& [fun, impls] = it;

        // for each gate in the list of possible implementations
        for (const auto& gate : impls)
        {
            // flag to indicate that the current gate is exhaustively checked and can be skipped
            auto continue_with_next_gate = false;

            // for each cell position in the gate
            for (uint16_t y = 0u; y < GateLibrary::gate_y_size(); ++y)
            {
                for (uint16_t x = 0u; x < GateLibrary::gate_x_size(); ++x)
                {
                    // if the cell type at position (x, y) in the gate is non-empty
                    if (const auto cell_type = gate[y][x]; cell_type != technology<CellLyt>::cell_type::EMPTY)
                    {
                        // cell position within the gate
                        const cell<CellLyt> relative_cell_pos{x, y, t.z};

                        const auto sidb_pos =
                            relative_to_absolute_cell_position<GateLibrary::gate_x_size(), GateLibrary::gate_y_size(),
                                                               GateLyt, CellLyt>(gate_lyt, t, relative_cell_pos);

                        // if any SiDB position of the current gate is compromised
                        if (sidbs_affected_by_defects.count(sidb_pos) > 0)
                        {
                            // add this gate's function to the black list of tile t using the ports specified by
                            // get_gate_ports in GateLibrary
                            for (const auto& port : gate_ports.at(gate))
                            {
                                black_list[t][fun].push_back(port);
                            }

                            continue_with_next_gate = true;

                            break;
                        }
                    }
                }
                // break if any SiDB position of the current gate is found compromised in the inner loop
                if (continue_with_next_gate)
                {
                    break;
                }
            }
        }
    };

    // for each tile in the layout
    gate_lyt.foreach_tile(
        [&](const auto& t) noexcept
        {
            // for each gate in the library
            for (const auto& impl : gate_implementations)
            {
                // analyze the gate by matching its cell positions against the affected SiDBs on the surface
                analyze_gate(impl, t);
            }
        });

    return black_list;
}

}  // namespace fiction

#endif  // FICTION_SIDB_SURFACE_ANALYSIS_HPP
