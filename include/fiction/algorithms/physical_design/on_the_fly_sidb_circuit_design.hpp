//
// Created by Jan Drewniok on 26.06.24.
//

#ifndef FICTION_ON_THE_FLY_SIDB_CIRCUIT_DESIGN_HPP
#define FICTION_ON_THE_FLY_SIDB_CIRCUIT_DESIGN_HPP

#include "fiction/algorithms/physical_design/apply_gate_library.hpp"
#include "fiction/algorithms/physical_design/exact.hpp"
#include "fiction/technology/cell_ports.hpp"
#include "fiction/technology/fcn_gate_library.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_on_the_fly_gate_library.hpp"
#include "fiction/technology/sidb_skeleton_bestagon_library.hpp"
#include "fiction/technology/sidb_surface_analysis.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <cstdio>
#include <optional>
#include <stdexcept>
#include <utility>

namespace fiction
{

/**
 * This struct stores the parameters to design an SiDB circuit on a defective surface.
 *
 * @tparam CellLyt SiDB cell-level layout type.
 */
template <typename CellLyt>
struct on_the_fly_sidb_circuit_design_on_defective_surface_params
{
    /**
     * Parameters for the SiDB on-the-fly gate library.
     */
    sidb_on_the_fly_gate_library_params<CellLyt> sidb_on_the_fly_gate_library_parameters = {};
    /**
     * Parameters for the *exact* placement and routing algorithm.
     */
    exact_physical_design_params exact_design_parameters = {};
};

/**
 * This struct stores the parameters to design an SiDB circuit.
 *
 * @tparam CellLyt SiDB cell-level layout type.
 */
template <typename CellLyt>
struct on_the_fly_sidb_circuit_design_params
{
    /**
     * Parameters for the SiDB on-the-fly gate library.
     */
    sidb_on_the_fly_gate_library_params<CellLyt> sidb_on_the_fly_gate_library_parameters = {};
};

/**
 * Statistics for the on-the-fly defect-aware circuit design.
 */
template <typename GateLyt>
struct on_the_fly_circuit_design_on_defective_surface_stats
{
    /**
     * The total runtime of the on-the-fly circuit design.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * The `stats` of the *exact* algorithm.
     */
    exact_physical_design_stats exact_stats{};
    /**
     * The gate-level layout after P&R.
     */
    std::optional<GateLyt> gate_layout{};
};

/**
 * This function implements an on-the-fly circuit design algorithm for a defective SiDB surface.
 *
 * The process begins with placement and routing using a blacklist and the `exact` method. The blacklist includes
 * skeleton-tile pairs that are excluded due to collisions between skeleton and neutral defects on specific tiles. After
 * identifying a valid placement and routing, a defect-aware SiDB gate design algorithm is applied. This algorithm
 * designs gates for each tile while accounting for atomic defects. If the gate design is unsuccessful, the blacklist is
 * updated with the problematic skeleton-gate pair, and the placement and routing process is restarted. If the gate
 * design succeeds, the algorithm finalizes the design and returns the SiDB circuit. This approach ensures that the
 * circuit remains functional even in the presence of defects.
 *
 * This methodology is detailed in the paper "On-the-fly Defect-Aware Design of Circuits based on Silicon Dangling Bond
 * Logic" by J. Drewniok, M. Walter, S. S. H. Ng, K. Walus, and R. Wille, IEEE NANO 2024
 * (https://ieeexplore.ieee.org/abstract/document/10628962).
 *
 * @tparam Ntk The type of the input network.
 * @tparam CellLyt SiDB cell-level layout type.
 * @tparam GateLyt Gate-level layout type.
 * @param ntk The input network to be mapped onto the defective surface.
 * @param lattice_tiling The lattice tiling used for the circuit design.
 * @param defective_surface The defective surface on which the SiDB circuit is designed.
 * @param params The parameters used for designing the circuit, encapsulated in an
 * `on_the_fly_sidb_circuit_design_params` object.
 * @param stats Pointer to a structure for collecting statistics. If nullptr, statistics are not collected.
 * @return A `sidb_defect_surface<CellLyt>` representing the designed circuit on the defective surface.
 */
template <typename Ntk, typename CellLyt, typename GateLyt>
[[nodiscard]] sidb_defect_surface<CellLyt> on_the_fly_sidb_circuit_design_on_defective_surface(
    const Ntk& ntk, const GateLyt& lattice_tiling, const sidb_defect_surface<CellLyt>& defective_surface,
    const on_the_fly_sidb_circuit_design_on_defective_surface_params<CellLyt>& params = {},
    on_the_fly_circuit_design_on_defective_surface_stats<GateLyt>*             stats  = nullptr)
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(is_hexagonal_layout_v<GateLyt>, "GateLyt is not a hexagonal");
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<CellLyt>, "CellLyt is not an SiDB layout");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    on_the_fly_circuit_design_on_defective_surface_stats<GateLyt> st{};

    exact_physical_design_stats exact_stats{};

    auto params_copy                                                   = params;
    params_copy.sidb_on_the_fly_gate_library_parameters.defect_surface = defective_surface;

    sidb_defect_surface<CellLyt> result{};

    {
        const mockturtle::stopwatch stop{st.time_total};

        std::optional<GateLyt> gate_level_layout = std::nullopt;

        CellLyt lyt{};

        // generating the blacklist based on neutral defects. The long-range electrostatic influence of charged defects
        // is not considered as gates are designed on-the-fly.
        auto black_list = sidb_surface_analysis<sidb_skeleton_bestagon_library, GateLyt, CellLyt>(
            lattice_tiling, params.sidb_on_the_fly_gate_library_parameters.defect_surface.value(),
            std::make_pair(0, 0));

        while (!gate_level_layout.has_value())
        {
            // P&R with *exact* and the pre-determined blacklist
            gate_level_layout =
                exact_with_blacklist<GateLyt>(ntk, black_list, params.exact_design_parameters, &exact_stats);
            st.exact_stats = exact_stats;

            if (gate_level_layout.has_value())
            {
                st.gate_layout = gate_level_layout;

                try
                {
                    lyt = apply_parameterized_gate_library<CellLyt, sidb_on_the_fly_gate_library, GateLyt,
                                                           sidb_on_the_fly_gate_library_params<CellLyt>>(
                        *gate_level_layout, params.sidb_on_the_fly_gate_library_parameters);
                }

                // on-the-fly gate design was unsuccessful at a certain tile. Hence, this tile-gate pair is added to the
                // blacklist and the process is rerun.
                catch (const gate_design_exception<tt, GateLyt>& e)
                {
                    gate_level_layout = std::nullopt;
                    black_list[e.which_tile()][e.which_truth_table()].push_back(e.which_port_list());
                }

                catch (const unsupported_gate_orientation_exception<cell<CellLyt>, port_direction>& e)
                {
                    fmt::print(stderr, "[e] Unsupported gate orientation encountered at tile: {} and ports: {}\n",
                               e.where(), e.which_ports());
                    fmt::print(stderr, "[e] Terminating loop due to critical error.\n");
                    break;
                }
            }
            // P&R was unsuccessful
            else
            {
                throw std::runtime_error("P&R was unsuccessful");
            }
        }

        sidb_defect_surface<CellLyt> sidbs_and_defects{lyt};

        // add defects to the circuit.
        params.sidb_on_the_fly_gate_library_parameters.defect_surface.value().foreach_sidb_defect(
            [&sidbs_and_defects](const auto& defect)
            { sidbs_and_defects.assign_sidb_defect(defect.first, defect.second); });

        result = sidbs_and_defects;
    }

    if (stats)
    {
        *stats = st;
    }

    return result;
}

/**
 * This function implements an on-the-fly SiDB circuit design algorithm.
 *
 * The process begins with an already placed and routed gate-level layout. For each gate, the corresponding SiDB
 * implementation is designed by using an SiDB gate design algorithm.
 *
 * @tparam CellLyt SiDB cell-level layout type.
 * @tparam GateLyt Gate-level layout type.
 * @param gate_lyt Gate-level layout.
 * @param lattice_tiling The lattice tiling used for the circuit design.
 * @param params The parameters used for designing the circuit, encapsulated in an
 * `on_the_fly_sidb_circuit_design_params` object.
 * @param stats Pointer to a structure for collecting statistics. If `nullptr`, statistics are not collected.
 * @return A `CellLyt` representing the designed SiDB circuit.
 */
template <typename CellLyt, typename GateLyt>
[[nodiscard]] CellLyt on_the_fly_sidb_circuit_design(const GateLyt&                                        gate_lyt,
                                                     const on_the_fly_sidb_circuit_design_params<CellLyt>& params = {})
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(is_hexagonal_layout_v<GateLyt>, "GateLyt is not a hexagonal");
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<CellLyt>, "CellLyt is not an SiDB layout");

    CellLyt result{};

    try
    {
        return apply_parameterized_gate_library<CellLyt, sidb_on_the_fly_gate_library, GateLyt,
                                                sidb_on_the_fly_gate_library_params<CellLyt>>(
            *gate_lyt, params.sidb_on_the_fly_gate_library_parameters);
    }

    // on-the-fly gate design was unsuccessful at a certain tile. Hence, this tile-gate pair is added to the
    // blacklist and the process is rerun.
    catch (const gate_design_exception<tt, GateLyt>& e)
    {
        throw std::runtime_error("Gate design was unsuccessful");
    }
}

}  // namespace fiction

#endif  // FICTION_ON_THE_FLY_SIDB_CIRCUIT_DESIGN_HPP
