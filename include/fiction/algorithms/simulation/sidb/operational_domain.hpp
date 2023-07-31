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

#include <btree.h>
#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <phmap.h>

#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * An operational domain is a set of simulation parameter values for which a given SiDB layout is logically operational.
 * This means that a layout is deemed operational if the layout's ground state corresponds with a given Boolean function
 * at the layout's outputs for all possible input combinations. In this implementation, \f$ n \f$ BDL input wires and a
 * single BDL output wire are assumed for a given layout. Any operational domain computation algorithm toggles through
 * all \f$ 2^n \f$ input combinations and evaluates the layout's output behavior in accordance with the given Boolean
 * function. The layout is only considered operational for a certain parameter combination, if the output behavior is
 * correct for all input combinations. The operational domain can be computed by sweeping over specified simulation
 * parameters and checking the operational status of the layout for each parameter combination. The operational domain
 * is then defined as the set of all parameter combinations for which the layout is operational. Different techniques
 * for performing these sweep are implemented.
 */
struct operational_domain
{
    /**
     * Possible sweep parameters for the operational domain computation.
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
    /**
     * X dimension sweep parameter.
     */
    sweep_parameter x_dimension{operational_domain::sweep_parameter::EPSILON_R};
    /**
     * Y dimension sweep parameter.
     */
    sweep_parameter y_dimension{operational_domain::sweep_parameter::LAMBDA_TF};
    /**
     * Possible operational status of a layout.
     */
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
    /**
     * The operational status of the layout for each specified parameter combination. This constitutes the operational
     * domain. The first element of the pair is the x dimension value, the second element is the y dimension value.
     * The operational status is stored as the value of the map.
     */
    phmap::btree_map<std::pair<double, double>, operational_status> operational_values{};
};

/**
 * Parameters for the operational domain computation. The parameters are used across the different operational domain
 * computation algorithms.
 */
struct operational_domain_params
{
    /**
     * The simulation parameters for the operational domain computation. Most parameters will be kept constant across
     * sweeps, but the sweep parameters are adjusted in each simulation step and thus overwritten in this object.
     */
    sidb_simulation_parameters sim_params{};
    /**
     * The sweep parameter for the x dimension.
     */
    operational_domain::sweep_parameter x_dimension{operational_domain::sweep_parameter::EPSILON_R};
    /**
     * The minimum value of the x dimension sweep.
     */
    double x_min{0.0};
    /**
     * The maximum value of the x dimension sweep.
     */
    double x_max{10.0};
    /**
     * The step size of the x dimension sweep.
     */
    double x_step{0.1};
    /**
     * The sweep parameter for the y dimension.
     */
    operational_domain::sweep_parameter y_dimension{operational_domain::sweep_parameter::LAMBDA_TF};
    /**
     * The minimum value of the y dimension sweep.
     */
    double y_min{0.0};
    /**
     * The maximum value of the y dimension sweep.
     */
    double y_max{10.0};
    /**
     * The step size of the y dimension sweep.
     */
    double y_step{0.1};
    /**
     * The parameters for the BDL pair detection, which is necessary during the operational domain computation to
     * detect input and output BDL pairs.
     */
    detect_bdl_pairs_params bdl_params{};
};
/**
 * Statistics for the operational domain computation. The statistics are used across the different operational domain
 * computation algorithms.
 */
struct operational_domain_stats
{
    /**
     * The total runtime of the operational domain computation.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Number of evaluated parameter combinations.
     */
    std::size_t num_evaluated_parameter_combinations{0};
    /**
     * Number of parameter combinations, for which the layout is operational.
     */
    std::size_t num_operational_parameter_combinations{0};
    /**
     * Number of parameter combinations, for which the layout is non-operational.
     */
    std::size_t num_non_operational_parameter_combinations{0};
};

namespace detail
{

template <typename Lyt, typename TT>
class operational_domain_impl
{
  public:
    /**
     * Standard constructor. Initializes the layout, the truth table, the parameters and the statistics. Also detects
     * the output BDL pair, which is necessary for the operational domain computation. The layout must have exactly
     * one output BDL pair.
     *
     * @param lyt SiDB cell-level layout to be evaluated.
     * @param tt Truth table of the Boolean function, which the layout should implement.
     * @param ps Parameters for the operational domain computation.
     * @param st Statistics of the process.
     */
    operational_domain_impl(Lyt& lyt, const TT& tt, const operational_domain_params& ps,
                            operational_domain_stats& st) noexcept :
            layout{lyt},
            truth_table{tt},
            params{ps},
            stats{st},
            output_bdl_pairs{detect_bdl_pairs<Lyt>(layout, sidb_technology::cell_type::OUTPUT, params.bdl_params)}
    {
        assert(output_bdl_pairs.size() == 1 && "The layout must have exactly one output BDL pair");
    }
    /**
     * Performs a grid search over the specified parameter ranges with the specified step sizes. The grid search always
     * has quadratic complexity. The operational domain is computed for each parameter combination.
     *
     * @return The operational domain of the layout.
     */
    [[nodiscard]] operational_domain grid_search() noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        operational_domain opdomain{};

        opdomain.x_dimension = params.x_dimension;
        opdomain.y_dimension = params.y_dimension;

        sidb_simulation_parameters sim_params = params.sim_params;

        // for x and y counters, we pre-compute the number of steps to avoid using floating-point numbers in the loop

        for (auto y = 0; y < (params.y_max - params.y_min) / params.y_step; ++y)
        {
            const double y_val = params.y_min + y * params.y_step;

            set_y_dimension_value(sim_params, y_val);

            for (auto x = 0; x < (params.x_max - params.x_min) / params.x_step; ++x)
            {
                const double x_val = params.x_min + x * params.x_step;

                set_x_dimension_value(sim_params, x_val);

                opdomain.operational_values[{x_val, y_val}] = is_operational(sim_params);
            }
        }

        return opdomain;
    }
    /**
     * Performs a random sampling of the specified number of samples within the specified parameter range. The
     * operational domain is computed for each sample point.
     *
     * @param samples Number of samples to be taken.
     * @return The (partial) operational domain of the layout.
     */
    [[nodiscard]] operational_domain random_sampling(const std::size_t samples) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        operational_domain opdomain{};

        opdomain.x_dimension = params.x_dimension;
        opdomain.y_dimension = params.y_dimension;

        sidb_simulation_parameters sim_params = params.sim_params;

        phmap::flat_hash_set<std::pair<std::size_t, std::size_t>> sampled_points{};
        sampled_points.reserve(samples);

        const auto already_sampled = [&sampled_points](const std::size_t x, const std::size_t y) {
            return sampled_points.find({x, y}) != sampled_points.end();
        };

        static std::mt19937_64 generator{std::random_device{}()};

        // calculate the number of steps in x and y dimension to instantiate distributions
        const auto num_x_steps = static_cast<std::size_t>((params.x_max - params.x_min) / params.x_step);
        const auto num_y_steps = static_cast<std::size_t>((params.y_max - params.y_min) / params.y_step);

        // instantiate distributions
        std::uniform_int_distribution<std::size_t> x_distribution{0, num_x_steps - 1};
        std::uniform_int_distribution<std::size_t> y_distribution{0, num_y_steps - 1};

        for (std::size_t i = 0; i < samples; ++i)
        {
            // sample x and y dimension
            const auto x_sample = x_distribution(generator);
            const auto y_sample = y_distribution(generator);

            // check if the point has already been sampled
            if (already_sampled(x_sample, y_sample))
            {
                // --i;  // TODO do we want to have a fixed number of sample attempts or a fixed number of samples?
                continue;
            }

            // add the point to the set of sampled points
            sampled_points.insert({x_sample, y_sample});

            // convert the sample to the actual value
            const auto x_val = params.x_min + static_cast<double>(x_sample) * params.x_step;
            const auto y_val = params.y_min + static_cast<double>(y_sample) * params.y_step;

            set_x_dimension_value(sim_params, x_val);
            set_y_dimension_value(sim_params, y_val);

            opdomain.operational_values[{x_val, y_val}] = is_operational(sim_params);
        }

        return opdomain;
    }

  private:
    /**
     * The SiDB cell-level layout to investigate.
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
    operational_domain_stats& stats;
    /**
     * The output BDL pair of the layout.
     */
    const std::vector<bdl_pair<Lyt>> output_bdl_pairs;
    /**
     * The BDL input iterator for the layout.
     */
    bdl_input_iterator<Lyt> bii{layout, params.bdl_params};

    /**
     * Potential sweep dimensions.
     */
    enum class sweep_dimension : uint8_t
    {
        X,
        Y
    };
    /**
     * Helper function that sets the value of a sweep dimension in the simulation parameters.
     *
     * @param sim_parameters Simulation parameter object to set the sweep dimension `dim` to value `val`.
     * @param val Value to set the dimension `dim` to.
     * @param dim Sweep dimension to set the value `val` to.
     */
    inline void set_dimension_value(sidb_simulation_parameters& sim_parameters, double val,
                                    sweep_dimension dim) const noexcept
    {
        operational_domain::sweep_parameter sweep_parameter =
            dim == sweep_dimension::X ? params.x_dimension : params.y_dimension;

        switch (sweep_parameter)
        {
            case operational_domain::sweep_parameter::EPSILON_R:
            {
                sim_parameters.epsilon_r = val;
                break;
            }
            case operational_domain::sweep_parameter::LAMBDA_TF:
            {
                sim_parameters.lambda_tf = val;
                break;
            }
            case operational_domain::sweep_parameter::MU_MINUS:
            {
                sim_parameters.mu = val;
                break;
            }
            default:
            {
                assert(false && "Unknown sweep parameter");
            }
        }
    }
    /**
     * Helper function that sets the value of the x dimension in the simulation parameters.
     *
     * @param sim_params Simulation parameter object to set the x dimension value of.
     * @param val Value to set the x dimension to.
     */
    inline void set_x_dimension_value(sidb_simulation_parameters& sim_params, const double val) const noexcept
    {
        set_dimension_value(sim_params, val, sweep_dimension::X);
    }
    /**
     * Helper function that sets the value of the y dimension in the simulation parameters.
     *
     * @param sim_params Simulation parameter object to set the y dimension value of.
     * @param val Value to set the y dimension to.
     */
    inline void set_y_dimension_value(sidb_simulation_parameters& sim_params, const double val) const noexcept
    {
        set_dimension_value(sim_params, val, sweep_dimension::Y);
    }
    /**
     * Performs an exhaustive ground state simulation for all input combinations of the stored layout using the given
     * simulation parameters. This function is used by all operational domain computation techniques. The function
     * terminates as soon as a non-operational state is found. In the worst case, the function performs \f$ 2^n \f$
     * simulations, where \f$ n \f$ is the number of inputs of the layout.
     *
     * @param sim_params Simulation parameters to use for the simulation.
     * @return The operational status of the layout under the given simulation parameters.
     */
    [[nodiscard]] operational_domain::operational_status
    is_operational(const sidb_simulation_parameters& sim_params) noexcept
    {
        // increment the number of evaluated parameter combinations
        ++stats.num_evaluated_parameter_combinations;

        // take the first (and only) output BDL pair
        const auto& output_bdl_pair = output_bdl_pairs.front();

        // reset the BDL input iterator
        bii = 0ull;

        // for each input combination
        for (auto i = 0u; i < truth_table.num_bits(); ++i, ++bii)
        {
            // the expected output of the layout is the i-th bit of the truth table
            const auto expected_output = kitty::get_bit(truth_table, i);

            // TODO replace with QuickExact
            // perform an exhaustive ground state simulation
            const auto sim_result = exhaustive_ground_state_simulation(*bii, sim_params);

            // TODO is this necessary or is it guaranteed that the ground state is always the first element?
            // find the ground state, which is the charge distribution with the lowest energy
            const auto ground_state = std::min_element(
                sim_result.charge_distributions.cbegin(), sim_result.charge_distributions.cend(),
                [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

            // fetch the charge states of the output BDL pair
            const auto charge_state_output_upper = ground_state->get_charge_state(output_bdl_pair.upper);
            const auto charge_state_output_lower = ground_state->get_charge_state(output_bdl_pair.lower);

            // if the output charge states are equal, the layout is not operational
            if (charge_state_output_lower == charge_state_output_upper)
            {
                ++stats.num_non_operational_parameter_combinations;

                return operational_domain::operational_status::NON_OPERATIONAL;
            }

            // if the expected output is 1, the expected charge states are (upper, lower) = (0, -1)
            if (expected_output)
            {
                if (charge_state_output_upper != sidb_charge_state::NEUTRAL ||
                    charge_state_output_lower != sidb_charge_state::NEGATIVE)
                {
                    ++stats.num_non_operational_parameter_combinations;

                    return operational_domain::operational_status::NON_OPERATIONAL;
                }
            }
            // if the expected output is 0, the expected charge states are (upper, lower) = (-1, 0)
            else
            {
                if (charge_state_output_upper != sidb_charge_state::NEGATIVE ||
                    charge_state_output_lower != sidb_charge_state::NEUTRAL)
                {
                    ++stats.num_non_operational_parameter_combinations;

                    return operational_domain::operational_status::NON_OPERATIONAL;
                }
            }
        }

        // if we made it here, the layout is operational
        ++stats.num_operational_parameter_combinations;

        return operational_domain::operational_status::OPERATIONAL;
    }
};

}  // namespace detail

/**
 * Computes the operational domain of the given SiDB cell-level layout. The operational domain is the set of all
 * parameter combinations for which the layout is logically operational. Logical operation is defined as the layout
 * implementing the given truth table. The input BDL pairs of the layout are assumed to be in the same order as the
 * inputs of the truth table. // TODO implement the matching of truth table inputs and BDL pair ordering
 *
 * This algorithm uses a grid search to find the operational domain. The grid search is performed by exhaustively
 * sweeping the parameter space in the x and y dimensions. Since grid search is exhaustive, the algorithm is guaranteed
 * to find the operational domain, if it exists within the parameter range. However, the algorithm performs a quadratic
 * number of operational checks on the layout, where each operational check consists of up to \f$ 2^n \f$ exact ground
 * state simulations, where \f$ n \f$ is the number of inputs of the layout. Each exact ground state simulation has
 * exponential complexity in of itself. Therefore, the algorithm is only feasible for small layouts with few inputs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected truth table of the layout.
 * @param params Operational domain computation parameters.
 * @param pst Operational domain computation statistics.
 * @return The operational domain of the layout.
 */
template <typename Lyt, typename TT>
operational_domain operational_domain_grid_search(Lyt& lyt, const TT& spec,
                                                  const operational_domain_params& params = {},
                                                  operational_domain_stats*        pst    = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
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
/**
 * Computes the operational domain of the given SiDB cell-level layout. The operational domain is the set of all
 * parameter combinations for which the layout is logically operational. Logical operation is defined as the layout
 * implementing the given truth table. The input BDL pairs of the layout are assumed to be in the same order as the
 * inputs of the truth table. // TODO implement the matching of truth table inputs and BDL pair ordering
 *
 * This algorithm uses random sampling to find a part of the operational domain that might not be complete. It performs
 * a total of `samples` uniformly-distributed random samples within the parameter range. For each sample, the algorithm
 * performs one operational checks on the layout, where each operational check consists of up to \f$ 2^n \f$ exact
 * ground state simulations, where \f$ n \f$ is the number of inputs of the layout. Each exact ground state simulation
 * has exponential complexity in of itself. Therefore, the algorithm is only feasible for small layouts with few inputs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected truth table of the layout.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param pst Operational domain computation statistics.
 * @return The (partial) operational domain of the layout.
 */
template <typename Lyt, typename TT>
operational_domain operational_domain_random_sampling(Lyt& lyt, const TT& spec, const std::size_t samples,
                                                      const operational_domain_params& params = {},
                                                      operational_domain_stats*        pst    = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    operational_domain_stats                 st{};
    detail::operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.random_sampling(samples);

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_OPERATIONAL_DOMAIN_HPP
