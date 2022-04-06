//
// Created by marcel on 01.04.22.
//

#ifndef FICTION_SIDB_SURFACE_ANALYSIS_HPP
#define FICTION_SIDB_SURFACE_ANALYSIS_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_surface.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <kitty/dynamic_truth_table.hpp>

#include <algorithm>
#include <cstdint>
#include <type_traits>

namespace fiction
{

template <typename GateLibrary, typename GateLyt, typename CellLyt>
std::unordered_map<tile<GateLyt>, std::vector<kitty::dynamic_truth_table>>
sidb_surface_analysis(const GateLyt& gate_lyt, const sidb_surface<CellLyt>& surface) noexcept
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(std::is_same_v<technology<CellLyt>, sidb_technology>, "CellLyt is not an SiDB layout");

    static_assert(has_get_functional_implementations_v<GateLibrary>,
                  "GateLibrary does not implement the get_functional_implementations function");
    static_assert(std::is_same_v<technology<CellLyt>, technology<GateLibrary>>,
                  "CellLyt and GateLibrary must implement the same technology");

    std::unordered_map<tile<GateLyt>, std::vector<kitty::dynamic_truth_table>> black_list{};

    const auto sidbs_affected_by_defects = surface.all_affected_sidbs();
    const auto gate_implementations      = GateLibrary::get_functional_implementations();

    gate_lyt.foreach_tile(
        [&gate_lyt, &black_list, &sidbs_affected_by_defects, &gate_implementations](const auto& t)
        {
            // for each gate in the library
            mockturtle::detail::foreach_element(
                gate_implementations.cbegin(), gate_implementations.cend(),
                [&gate_lyt, &t, &black_list, &sidbs_affected_by_defects](const auto& it)
                {
                    const auto& [fun, gate] = it;

                    // for each cell position in the gate
                    for (auto y = 0u; y < GateLibrary::gate_y_size(); ++y)
                    {
                        for (auto x = 0u; x < GateLibrary::gate_x_size(); ++x)
                        {
                            // if the cell type at position (x, y) in the gate is non-empty
                            if (const auto cell_type = gate[y][x]; cell_type != technology<CellLyt>::cell_type::EMPTY)
                            {
                                // cell position within the gate
                                const auto relative_cell_pos = cell<CellLyt>{x, y, t.z};

                                const auto sidb_pos =
                                    relative_to_absolute_cell_position<GateLibrary::gate_x_size(),
                                                                       GateLibrary::gate_y_size(), GateLyt, CellLyt>(
                                        gate_lyt, t, relative_cell_pos);

                                // if any SiDB position of the current gate is compromised
                                if (sidbs_affected_by_defects.count(sidb_pos) > 0)
                                {
                                    // add this gate's function to the black list of tile t
                                    black_list[t].push_back(fun);
                                    return true;  // skip to next gate
                                }
                            }
                        }
                    }

                    return true;  // keep iterating
                });
        });

    return black_list;
}

}  // namespace fiction

#endif  // FICTION_SIDB_SURFACE_ANALYSIS_HPP
