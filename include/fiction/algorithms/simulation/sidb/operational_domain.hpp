//
// Created by marcel on 21.07.23.
//

#ifndef FICTION_OPERATIONAL_DOMAIN_HPP
#define FICTION_OPERATIONAL_DOMAIN_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/traits.hpp"

#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <cassert>
#include <utility>
#include <vector>

#include <btree.h>

namespace fiction
{

struct operational_domain
{
    /**
     * The sweep parameters for the operational domain computation.
     */
    enum class sweep_parameter
    {
        /**
         * The relative permittivity of the dielectric material.
         */
        EPSILON_R,
        /**
         * The Thomas-Fermi screening length.
         */
        LAMBDA_TF,
        /**
         * The energy transition level.
         */
        MU_MINUS
    };

    sweep_parameter x_dimension{operational_domain::sweep_parameter::EPSILON_R};
    sweep_parameter y_dimension{operational_domain::sweep_parameter::LAMBDA_TF};

    enum class operational_status
    {
        /**
         * The layout is operational.
         */
        OPERATIONAL,
        /**
         * The layout is non-operational.
         */
        NON_OPERATIONAL
    };

    phmap::btree_map<std::pair<double, double>, operational_status> operational_values{};
};

struct operational_domain_params
{
    /**
     * The simulation parameters for the operational domain computation. The sweep parameters are overwritten in each
     * simulation step.
     */
    sidb_simulation_parameters sim_params{};

    operational_domain::sweep_parameter x_dimension{operational_domain::sweep_parameter::EPSILON_R};

    double x_min{0.0};
    double x_max{10.0};
    double x_step{0.1};

    operational_domain::sweep_parameter y_dimension{operational_domain::sweep_parameter::LAMBDA_TF};

    double y_min{0.0};
    double y_max{10.0};
    double y_step{0.1};

    detect_bdl_pairs_params bdl_params{};
};

struct operational_domain_stats
{
    /**
     * The total runtime of the operational domain computation.
     */
    mockturtle::stopwatch<>::duration time_total{0};
};

namespace detail
{

template <typename Lyt, typename TT>
class operational_domain_impl
{
  public:
    operational_domain_impl(Lyt& lyt, const TT& tt, const operational_domain_params& ps,
                            operational_domain_stats& st) noexcept :
            layout{lyt},
            truth_table{tt},
            params{ps},
            pst{st},
            output_bdl_pairs{detect_bdl_pairs<Lyt>(layout, sidb_technology::cell_type::OUTPUT, params.bdl_params)}
    {
        assert(output_bdl_pairs.size() == 1 && "The layout must have exactly one output BDL pair");
    }

    operational_domain grid_search() noexcept
    {
        mockturtle::stopwatch stop{pst.time_total};

        operational_domain opdomain{};

        opdomain.x_dimension = params.x_dimension;
        opdomain.y_dimension = params.y_dimension;

        sidb_simulation_parameters sim_params = params.sim_params;

        // for x and y counters, we pre-compute the number of steps to avoid using floating-point numbers in the loop

        for (auto y = 0; y < (params.y_max - params.y_min) / params.y_step; ++y)
        {
            set_y_dimension_value(sim_params, params.y_min + y * params.y_step);

            for (auto x = 0; x < (params.x_max - params.x_min) / params.x_step; ++x)
            {
                set_x_dimension_value(sim_params, params.x_min + x * params.x_step);

                opdomain.operational_values[{x, y}] = is_operational(sim_params);
            }
        }

        return opdomain;
    }

  private:
    /**
     * The layout to investigate.
     */
    Lyt& layout;
    /**
     * The specification of the layout.
     */
    const TT& truth_table;
    /**
     * The parameters for the operational domain computation.
     */
    const operational_domain_params& params;
    /**
     * The statistics of the operational domain computation.
     */
    operational_domain_stats& pst;
    /**
     * The output BDL pair of the layout.
     */
    const std::vector<bdl_pair<Lyt>> output_bdl_pairs;
    /**
     * The BDL input iterator for the layout.
     */
    bdl_input_iterator<Lyt> bii{layout, params.bdl_params};

    inline void set_x_dimension_value(sidb_simulation_parameters& sim_params, const double val) const noexcept
    {
        switch (params.x_dimension)
        {
            case operational_domain::sweep_parameter::EPSILON_R:
            {
                sim_params.epsilon_r = val;
                break;
            }
            case operational_domain::sweep_parameter::LAMBDA_TF:
            {
                sim_params.lambda_tf = units::length::nanometer_t{val};
                break;
            }
            case operational_domain::sweep_parameter::MU_MINUS:
            {
                sim_params.mu = units::energy::electron_volt_t{val};
                break;
            }
            default:
            {
                assert(false && "Unknown sweep parameter");
            }
        }
    }

    inline void set_y_dimension_value(sidb_simulation_parameters& sim_params, const double val) const noexcept
    {
        switch (params.y_dimension)
        {
            case operational_domain::sweep_parameter::EPSILON_R:
            {
                sim_params.epsilon_r = val;
                break;
            }
            case operational_domain::sweep_parameter::LAMBDA_TF:
            {
                sim_params.lambda_tf = units::length::nanometer_t{val};
                break;
            }
            case operational_domain::sweep_parameter::MU_MINUS:
            {
                sim_params.mu = units::energy::electron_volt_t{val};
                break;
            }
            default:
            {
                assert(false && "Unknown sweep parameter");
            }
        }
    }

    [[nodiscard]] operational_domain::operational_status
    is_operational(const sidb_simulation_parameters& sim_params) noexcept
    {
        const auto& output_bdl_pair = output_bdl_pairs.front();

        // reset the BDL input iterator
        bii = 0ull;

        // for each input combination
        for (auto i = 0u; i < truth_table.num_bits(); ++i, ++bii)
        {
            const auto expected_output = kitty::get_bit(truth_table, i);

            const auto sim_result = exhaustive_ground_state_simulation(*bii, sim_params);

            // find the ground state, which is the charge distribution with the lowest energy
            const auto ground_state =
                std::min_element(sim_result.charge_distributions.cbegin(), sim_result.charge_distributions.cend(),
                                 [](const auto& lhs, const auto& rhs)
                                 { return lhs.get_system_energy().value() < rhs.get_system_energy().value(); });

            const auto charge_state_output_upper = ground_state->get_charge_state(output_bdl_pair.upper);
            const auto charge_state_output_lower = ground_state->get_charge_state(output_bdl_pair.lower);

            // if the output charge states are equal, the layout is not operational
            if (charge_state_output_lower == charge_state_output_upper)
            {
                return operational_domain::operational_status::NON_OPERATIONAL;
            }

            // if the expected output is 1, the expected charge states are (upper, lower) = (0, -1)
            if (expected_output)
            {
                if (charge_state_output_upper != sidb_charge_state::NEUTRAL ||
                    charge_state_output_lower != sidb_charge_state::NEGATIVE)
                {
                    return operational_domain::operational_status::NON_OPERATIONAL;
                }
            }
            // if the expected output is 0, the expected charge states are (upper, lower) = (-1, 0)
            else
            {
                if (charge_state_output_upper != sidb_charge_state::NEGATIVE ||
                    charge_state_output_lower != sidb_charge_state::NEUTRAL)
                {
                    return operational_domain::operational_status::NON_OPERATIONAL;
                }
            }
        }

        // if we made it here, the layout is operational
        return operational_domain::operational_status::OPERATIONAL;
    }
};

}  // namespace detail

template <typename Lyt, typename TT>
operational_domain operational_domain_grid_search(Lyt& lyt, const TT& spec,
                                                  const operational_domain_params& params = {},
                                                  operational_domain_stats*        pst    = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    operational_domain_stats                 st{};
    detail::operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.grid_search();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_OPERATIONAL_DOMAIN_HPP
