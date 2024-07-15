//
// Created by Jan Drewniok on 26.06.24.
//

#ifndef FICTION_ON_THE_FLY_CIRCUIT_DESIGN_ON_DEFECTIVE_SURFACE_HPP
#define FICTION_ON_THE_FLY_CIRCUIT_DESIGN_ON_DEFECTIVE_SURFACE_HPP

#include "fiction/algorithms/physical_design/apply_gate_library.hpp"
#include "fiction/algorithms/physical_design/design_sidb_gates.hpp"
#include "fiction/algorithms/physical_design/exact.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/technology/parameterized_gate_library.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_skeleton_bestagon_library.hpp"
#include "fiction/technology/sidb_surface_analysis.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <exception>
#include <iostream>
#include <optional>
#include <utility>

namespace fiction
{

/**
 * This struct stores the parameters to design SiDB circuit on a defective surface.
 *
 * @tparam CellLyt Cell-level layout type.
 */
template <typename CellLyt>
struct on_the_fly_circuit_design_params
{
    /**
     * Parameters for the parameterized gate library.
     */
    parameterized_gate_library_params<CellLyt> parameterized_gate_library_parameter = {};
    /**
     * Parameters for the *exact* placement&routing algorithm.
     */
    exact_physical_design_params exact_design_parameter = {};
};

/**
 * Statistics for the on-the-fly defect-aware circuit design.
 */
struct on_the_fly_circuit_design_stats
{
    /**
     * The total runtime of the operational domain computation.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * The `stats` of the *exact* algorithm.
     */
    exact_physical_design_stats exact_stats{};
};

namespace detail
{

template <typename Ntk, typename CellLyt, typename GateLyt>
class on_the_fly_circuit_design_impl
{
  public:
    on_the_fly_circuit_design_impl(const Ntk& network, const on_the_fly_circuit_design_params<CellLyt>& design_params,
                                   const GateLyt& tiling, on_the_fly_circuit_design_stats& st) :
            lattice_tiling{tiling},
            mapped_network{network},
            params{design_params},
            stats{st}
    {}

    [[nodiscard]] sidb_defect_surface<CellLyt> design_circuit_on_defective_surface()
    {
        std::optional<GateLyt> gate_level_layout = std::nullopt;
        CellLyt                cell_level_layout{};

        // generating the blacklist based on neutral defects. The long-range electrostatic influence of charged defects
        // is not considered as gates are designed on-the-fly.
        auto black_list = sidb_surface_analysis<sidb_skeleton_bestagon_library>(
            lattice_tiling, params.parameterized_gate_library_parameter.defect_surface, std::make_pair(0, 0));

        const mockturtle::stopwatch stop{stats.time_total};

        while (!gate_level_layout.has_value())
        {
            // P&R with *exact* and the pre-determined blacklist
            gate_level_layout = exact_with_blacklist<GateLyt>(mapped_network, black_list, params.exact_design_parameter,
                                                              &stats.exact_stats);
            if (gate_level_layout.has_value())
            {
                try
                {
                    cell_level_layout = apply_parameterized_gate_library<CellLyt, parameterized_gate_library, GateLyt,
                                                                         parameterized_gate_library_params<CellLyt>>(
                        *gate_level_layout, params.parameterized_gate_library_parameter);
                }

                // on-the-fly gate design was unsuccessful at a certain tile. Hence, this tile-gate pair is added to the
                // blacklist.
                catch (const gate_design_exception<tt, GateLyt>& e)
                {
                    black_list[e.which_tile()][e.which_truth_table()].push_back(e.which_port_list());
                }

                catch (const std::exception& e)
                {
                    std::cerr << "Caught std::exception: " << e.what() << '\n';
                }
            }

            // P&R was unsuccessful
            else
            {
                break;
            }
        }

        // empty defect-surface is returned when no P&R solution is found
        if (!gate_level_layout.has_value())
        {
            return sidb_defect_surface<CellLyt>{};
        }

        // check equivalence
        const auto miter = mockturtle::miter<technology_network>(mapped_network, *gate_level_layout);
        const auto eq    = mockturtle::equivalence_checking(*miter);

        // empty defect-surface is returned when equivalence check is empty
        if (!eq.has_value())
        {
            return sidb_defect_surface<CellLyt>{};
        }

        // in case of equality, defect-surface with the SiDBs of the circuit is returned
        if (*eq)
        {
            sidb_defect_surface<CellLyt> sidbs_and_defects{cell_level_layout};

            // add defects to the circuit.
            params.parameterized_gate_library_parameter.defect_surface.foreach_sidb_defect(
                [&sidbs_and_defects](const auto& defect)
                { sidbs_and_defects.assign_sidb_defect(defect.first, defect.second); });

            return sidbs_and_defects;
        }

        // in case of no equality, empty defect-surface is returned
        return sidb_defect_surface<CellLyt>{};
    }

  private:
    /**
     * Gate-level layout.
     */
    GateLyt lattice_tiling;
    /**
     * Mapped network.
     */
    Ntk mapped_network{};
    /**
     * Parameters for the on-the-fly circuit design.
     */
    on_the_fly_circuit_design_params<CellLyt> params{};
    /**
     * Statistics for the on-the-fly circuit design.
     */
    on_the_fly_circuit_design_stats& stats;
};

}  // namespace detail

/**
 * This function applies the on-the-fly circuit design algorithm to a specified defective surface, automatically
 * creating customized SiDB gates while integrating atomic defects as essential components of the design. It was
 * proposed in \"On-the-fly Defect-Aware Design of Circuits based on Silicon Dangling Bond Logic\" by J. Drewniok, M.
 * Walter, S. S. H. Ng, K. Walus, and R. Wille in IEEE NANO 2024.
 *
 * @tparam Ntk      The type of the input network.
 * @tparam CellLyt  The type of the cell layout.
 * @tparam GateLyt  The type of the gate layout.
 * @param mapped_network  The input network to be mapped onto the defective surface.
 * @param design_params   The parameters used for designing the circuit, encapsulated in an
 * `on_the_fly_circuit_design_params` object.
 * @param lattice_tiling  The lattice tiling used for the circuit design.
 * @param defect_surface  The defective surface on which the circuit is to be designed.
 * @param stats           Pointer to a structure for collecting statistics. If nullptr, statistics are not collected.
 *
 * @return A `sidb_defect_surface<CellLyt>` representing the designed circuit on the defective surface.
 */
template <typename Ntk, typename CellLyt, typename GateLyt>
[[nodiscard]] sidb_defect_surface<CellLyt> on_the_fly_circuit_design_on_defective_surface(
    const Ntk& mapped_network, const on_the_fly_circuit_design_params<CellLyt>& design_params,
    const GateLyt& lattice_tiling, on_the_fly_circuit_design_stats* stats = nullptr)
{
    on_the_fly_circuit_design_stats                               st{};
    detail::on_the_fly_circuit_design_impl<Ntk, CellLyt, GateLyt> p{mapped_network, design_params, lattice_tiling, st};

    const auto result = p.design_circuit_on_defective_surface();

    if (stats)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_ON_THE_FLY_CIRCUIT_DESIGN_ON_DEFECTIVE_SURFACE_HPP
