//
// Created by marcel on 21.07.23.
//

#ifndef FICTION_OPERATIONAL_DOMAIN_HPP
#define FICTION_OPERATIONAL_DOMAIN_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/execution_utils.hpp"
#include "fiction/utils/phmap_utils.hpp"

#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <phmap.h>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <optional>
#include <queue>
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
    locked_parallel_flat_hash_map<std::pair<double, double>, operational_status> operational_values{};
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
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::EXGS};
    /**
     * The sweep parameter for the x dimension.
     */
    operational_domain::sweep_parameter x_dimension{operational_domain::sweep_parameter::EPSILON_R};
    /**
     * The minimum value of the x dimension sweep.
     */
    double x_min{1.0};
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
    double y_min{1.0};
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
     * Number of simulator invocations.
     */
    std::size_t num_simulator_invocations{0};
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
    operational_domain_impl(const Lyt& lyt, const TT& tt, const operational_domain_params& ps,
                            operational_domain_stats& st) noexcept :
            layout{lyt},
            truth_table{tt},
            params{ps},
            stats{st},
            output_bdl_pairs{detect_bdl_pairs<Lyt>(layout, sidb_technology::cell_type::OUTPUT, params.bdl_params)},
            x_indices(num_x_steps()),  // pre-allocate the x dimension indices
            y_indices(num_y_steps()),  // pre-allocate the y dimension indices
            x_values(num_x_steps()),   // pre-allocate the x dimension values
            y_values(num_y_steps())    // pre-allocate the y dimension values
    {
        assert(output_bdl_pairs.size() == 1 && "The layout must have exactly one output BDL pair");

        op_domain.x_dimension = params.x_dimension;
        op_domain.y_dimension = params.y_dimension;

        std::iota(x_indices.begin(), x_indices.end(), 0ul);
        std::iota(y_indices.begin(), y_indices.end(), 0ul);

        // generate the x dimension values
        std::generate(x_values.begin(), x_values.end(),
                      [x = 0, this]() mutable
                      {
                          const double x_val = params.x_min + x * params.x_step;
                          ++x;
                          return x_val;
                      });

        // generate the y dimension values
        std::generate(y_values.begin(), y_values.end(),
                      [y = 0, this]() mutable
                      {
                          const double y_val = params.y_min + y * params.y_step;
                          ++y;
                          return y_val;
                      });
    }
    /**
     * Performs a grid search over the specified parameter ranges with the specified step sizes. The grid search always
     * has quadratic complexity. The operational status is computed for each parameter combination.
     *
     * @return The operational domain of the layout.
     */
    [[nodiscard]] operational_domain grid_search() noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        // TODO replace vectors with ranges for memory efficiency

        // for each x value in parallel
        std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ x_indices.cbegin(), x_indices.cend(),
                      [this](const auto x)
                      {
                          // for each y value in parallel
                          std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ y_indices.cbegin(), y_indices.cend(),
                                        [this, x](const auto y) { is_operational(x, y); });
                      });

        log_stats();

        return op_domain;
    }
    /**
     * Performs a random sampling of the specified number of samples within the specified parameter range. The
     * operational status is computed for each sample point.
     *
     * @param samples Number of random samples to be taken.
     * @return The (partial) operational domain of the layout.
     */
    [[nodiscard]] operational_domain random_sampling(const std::size_t samples) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        static std::mt19937_64 generator{std::random_device{}()};

        std::vector<std::size_t> x_samples{};
        std::vector<std::size_t> y_samples{};

        x_samples.reserve(samples);
        y_samples.reserve(samples);

        // sample x and y indices
        std::sample(x_indices.begin(), x_indices.end(), std::back_inserter(x_samples), samples, generator);
        std::sample(y_indices.begin(), y_indices.end(), std::back_inserter(y_samples), samples, generator);

        std::vector<std::pair<std::size_t, std::size_t>> xy_samples{};
        xy_samples.reserve(samples);

        // tie x and y indices together
        std::transform(x_samples.cbegin(), x_samples.cend(), y_samples.cbegin(), std::back_inserter(xy_samples),
                       [](const auto x, const auto y) { return std::make_pair(x, y); });

        // for each sample point in parallel
        std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ xy_samples.cbegin(), xy_samples.cend(),
                      [this](const auto xy) { is_operational(xy.first, xy.second); });

        log_stats();

        return op_domain;
    }
    /**
     * Performs flood fill to determine the operational domain. The algorithm first performs a random sampling of up to
     * the specified number of samples. It stops random sampling once it finds a single operational point, from which it
     * starts the flood fill. The operational domain will finally only contain up to `samples` random non-operational
     * points as well as all operational points that are reachable via flood fill from the first found operational
     * point.
     *
     * @param samples Maximum number of random samples to be taken before flood fill.
     * @return The (partial) operational domain of the layout.
     */
    [[nodiscard]] operational_domain flood_fill(const std::size_t samples) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        // first, perform random sampling to find an operational starting point
        const auto starting_point = find_operational_parameters_via_random_sampling(samples);

        // if no operational point was found within the specified number of samples, return
        if (!starting_point.has_value())
        {
            return op_domain;
        }

        // a queue of (x, y) dimension steps to be evaluated
        using parameter_queue = std::queue<std::pair<std::size_t, std::size_t>>;
        parameter_queue queue{};

        // a utility function that adds the adjacent points to the queue for further evaluation
        const auto queue_next_points = [this, &queue](const auto x, const auto y)
        {
            // increase in x dimension
            if (const auto incr_x = x + 1; incr_x < x_values.size())
            {
                if (!has_already_been_sampled(incr_x, y))
                {
                    queue.emplace(incr_x, y);
                }
            }
            // decrease in x dimension
            if (const auto decr_x = x - 1; x > 0)
            {
                if (!has_already_been_sampled(decr_x, y))
                {
                    queue.emplace(decr_x, y);
                }
            }
            // increase in y dimension
            if (const auto incr_y = y + 1; incr_y < y_values.size())
            {
                if (!has_already_been_sampled(x, incr_y))
                {
                    queue.emplace(x, incr_y);
                }
            }
            // decrease in y dimension
            if (const auto decr_y = y - 1; y > 0)
            {
                if (!has_already_been_sampled(x, decr_y))
                {
                    queue.emplace(x, decr_y);
                }
            }
        };

        // add the neighbors of the starting point to the queue
        queue_next_points(starting_point->first, starting_point->second);

        // for each point in the queue
        while (!queue.empty())
        {
            // get the parameter combination and remove it from the queue
            const auto [x, y] = queue.front();
            queue.pop();

            // if the point has already been sampled, continue with the next
            if (has_already_been_sampled(x, y))
            {
                continue;
            }

            // check if the point is operational
            const auto operational_status = is_operational(x, y);

            // if the point is operational, add its four neighbors to the queue
            if (operational_status == operational_domain::operational_status::OPERATIONAL)
            {
                queue_next_points(x, y);
            }
        }

        log_stats();

        return op_domain;
    }

  private:
    /**
     * The SiDB cell-level layout to investigate.
     */
    const Lyt& layout;
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
     * X dimension steps.
     */
    std::vector<std::size_t> x_indices;
    /**
     * Y dimension steps.
     */
    std::vector<std::size_t> y_indices;
    /**
     * All x dimension values.
     */
    std::vector<double> x_values;
    /**
     * All y dimension values.
     */
    std::vector<double> y_values;
    /**
     * The operational domain of the layout.
     */
    operational_domain op_domain{};
    /**
     * Number of simulator invocations.
     */
    std::atomic<std::size_t> num_simulator_invocations{0};
    /**
     * Number of evaluated parameter combinations.
     */
    std::atomic<std::size_t> num_evaluated_parameter_combinations{0};
    /**
     * Number of parameter combinations, for which the layout is operational.
     */
    std::atomic<std::size_t> num_operational_parameter_combinations{0};
    /**
     * Number of parameter combinations, for which the layout is non-operational.
     */
    std::atomic<std::size_t> num_non_operational_parameter_combinations{0};
    /**
     * Determines whether the point at step position `(x, y)` has already been sampled and returns the operational value
     * at `(x, y)` if it already exists. Here, `x` and `y` represent steps in the x and y dimension, respectively, not
     * the actual values of the parameters.
     *
     * @param x X dimension step value.
     * @param y Y dimension step value.
     * @return The operational status of the point at step position `(x, y)` or `std::nullopt` if `(x, y)` has not been
     * sampled yet.
     */
    [[nodiscard]] inline std::optional<operational_domain::operational_status>
    has_already_been_sampled(const std::size_t x, const std::size_t y) const noexcept
    {
        if (const auto it = op_domain.operational_values.find({x_values[x], y_values[y]});
            it != op_domain.operational_values.cend())
        {
            return it->second;
        }

        return std::nullopt;
    }
    /**
     * Calculates the number of steps in the x dimension based on the provided parameters.
     *
     * @return The number of steps in the x dimension.
     */
    [[nodiscard]] inline std::size_t num_x_steps() const noexcept
    {
        return static_cast<std::size_t>((params.x_max - params.x_min) / params.x_step);
    }
    /**
     * Calculates the number of steps in the y dimension based on the provided parameters.
     *
     * @return The number of steps in the y dimension.
     */
    [[nodiscard]] inline std::size_t num_y_steps() const noexcept
    {
        return static_cast<std::size_t>((params.y_max - params.y_min) / params.y_step);
    }
    /**
     * Potential sweep dimensions.
     */
    enum class sweep_dimension : uint8_t
    {
        /**
         * Sweep dimension X.
         */
        X,
        /**
         * Sweep dimension Y.
         */
        Y
    };
    /**
     * Helper function that sets the value of a sweep dimension in the simulation parameters.
     *
     * @param sim_parameters Simulation parameter object to set the sweep dimension `dim` to value `val`.
     * @param val Value to set the dimension `dim` to.
     * @param dim Sweep dimension to set the value `val` to.
     */
    inline void set_dimension_value(sidb_simulation_parameters& sim_parameters, const double val,
                                    const sweep_dimension dim) const noexcept
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
                sim_parameters.mu_minus = val;
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
     * If the point `(x, y)` has already been investigated, the stored operational status is returned without conducting
     * another simulation.
     *
     * @param x X dimension step value.
     * @param y Y dimension step value.
     * @return The operational status of the layout under the given simulation parameters.
     */
    operational_domain::operational_status is_operational(const std::size_t x, const std::size_t y) noexcept
    {
        // if the point has already been sampled, return the stored operational status
        if (const auto op_value = has_already_been_sampled(x, y); op_value.has_value())
        {
            return *op_value;
        }

        // fetch the x and y dimension values
        const auto x_val = x_values[x];
        const auto y_val = y_values[y];

        const auto operational = [this, x_val, y_val]()
        {
            ++num_operational_parameter_combinations;

            op_domain.operational_values[{x_val, y_val}] = operational_domain::operational_status::OPERATIONAL;

            return operational_domain::operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, x_val, y_val]()
        {
            ++num_non_operational_parameter_combinations;

            op_domain.operational_values[{x_val, y_val}] = operational_domain::operational_status::NON_OPERATIONAL;

            return operational_domain::operational_status::NON_OPERATIONAL;
        };

        // increment the number of evaluated parameter combinations
        ++num_evaluated_parameter_combinations;

        // take the first (and only) output BDL pair
        const auto& output_bdl_pair = output_bdl_pairs.front();

        // initialize a BDL input iterator
        bdl_input_iterator<Lyt> bii{layout, params.bdl_params};

        sidb_simulation_parameters sim_params = params.sim_params;
        set_x_dimension_value(sim_params, x_val);
        set_y_dimension_value(sim_params, y_val);

        // for each input combination
        for (auto i = 0u; i < truth_table.num_bits(); ++i, ++bii)
        {
            // the expected output of the layout is the i-th bit of the truth table
            const auto expected_output = kitty::get_bit(truth_table, i);

            if (can_positive_charges_occur(*bii, sim_params))
            {
                return non_operational();
            }

            sidb_simulation_result<Lyt> sim_result{};

            if (params.sim_engine == sidb_simulation_engine::EXGS)
            {
                // perform an exhaustive ground state simulation
                sim_result = exhaustive_ground_state_simulation(*bii, sim_params);
            }
            else if (params.sim_engine == sidb_simulation_engine::QUICKSIM)
            {
                // perform a heuristic simulation
                const quicksim_params qs_params{sim_params, 500, 0.6, 1};
                sim_result = quicksim(*bii, qs_params);
            }
            else
            {
                assert(false && "unsupported simulation engine");
            }

            ++num_simulator_invocations;

            // if no physically-valid charge distributions were found, the layout is non-operational
            if (sim_result.charge_distributions.empty())
            {
                return non_operational();
            }

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
                return non_operational();
            }
            // if the expected output is 1, the expected charge states are (upper, lower) = (0, -1)
            if (expected_output)
            {
                if (charge_state_output_upper != sidb_charge_state::NEUTRAL ||
                    charge_state_output_lower != sidb_charge_state::NEGATIVE)
                {
                    return non_operational();
                }
            }
            // if the expected output is 0, the expected charge states are (upper, lower) = (-1, 0)
            else
            {
                if (charge_state_output_upper != sidb_charge_state::NEGATIVE ||
                    charge_state_output_lower != sidb_charge_state::NEUTRAL)
                {
                    return non_operational();
                }
            }
        }

        // if we made it here, the layout is operational
        return operational();
    }
    /**
     * Performs random sampling to find any operational parameter combination. This function is useful if a single
     * starting point is required within the domain to expand from. This function returns the step in x and y dimension
     * of the first operational point found. If no operational parameter combination can be found within the given
     * number of samples, the function returns std::nullopt.
     *
     * This function adds any sampled points to the `op_domain` member variables.
     *
     * // TODO this function contains a lot of code duplication from `random_sampling` as of right now.
     * // TODO This must be refactored.
     *
     * @param samples Maximum number of samples to take. Works as a timeout.
     * @return The first pair of operational parameters given as x and y steps, if any could be found, `std::nullopt`
     * otherwise.
     */
    [[nodiscard]] std::optional<std::pair<std::size_t, std::size_t>>
    find_operational_parameters_via_random_sampling(const std::size_t samples) noexcept
    {
        static std::mt19937_64 generator{std::random_device{}()};

        // instantiate distributions
        std::uniform_int_distribution<std::size_t> x_distribution{0, x_values.size() - 1};
        std::uniform_int_distribution<std::size_t> y_distribution{0, y_values.size() - 1};

        for (std::size_t i = 0; i < samples; ++i)
        {
            // sample x and y dimension
            const auto x_sample = x_distribution(generator);
            const auto y_sample = y_distribution(generator);

            // determine the operational status
            const auto operational_value = is_operational(x_sample, y_sample);

            // if the parameter combination is operational, return its step values in x and y dimension
            if (operational_value == operational_domain::operational_status::OPERATIONAL)
            {
                return std::make_pair(x_sample, y_sample);
            }
        }

        return std::nullopt;
    }
    /**
     * Helper function that writes the the statistics of the operational domain computation to the statistics object.
     * Due to data races that can occur during the computation, each value is temporarily held in an atomic variable and
     * written to the statistics object only after the computation has finished.
     */
    void log_stats() const noexcept
    {
        stats.num_simulator_invocations                  = num_simulator_invocations;
        stats.num_evaluated_parameter_combinations       = num_evaluated_parameter_combinations;
        stats.num_operational_parameter_combinations     = num_operational_parameter_combinations;
        stats.num_non_operational_parameter_combinations = num_non_operational_parameter_combinations;
    }

    [[nodiscard]] bool can_positive_charges_occur(const Lyt&                        lyt,
                                                  const sidb_simulation_parameters& sim_params) const noexcept
    {
        const charge_distribution_surface charge_lyt{lyt, sim_params, sidb_charge_state::NEGATIVE};

        bool result = false;

        charge_lyt.foreach_cell(
            [&result, &sim_params, charge_lyt](const auto& c)
            {
                if (const auto local_pot = charge_lyt.get_local_potential(c); local_pot.has_value())
                {
                    if ((-(*local_pot) + sim_params.mu_plus()) > -physical_constants::POP_STABILITY_ERR)
                    {
                        result = true;

                        return false;  // break
                    }
                }

                return true;  // continue
            });

        return result;
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
 * @param stats Operational domain computation statistics.
 * @return The operational domain of the layout.
 */
template <typename Lyt, typename TT>
operational_domain operational_domain_grid_search(const Lyt& lyt, const TT& spec,
                                                  const operational_domain_params& params = {},
                                                  operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    operational_domain_stats                 st{};
    detail::operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.grid_search();

    if (stats)
    {
        *stats = st;
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
 * performs one operational check on the layout, where each operational check consists of up to \f$ 2^n \f$ exact
 * ground state simulations, where \f$ n \f$ is the number of inputs of the layout. Each exact ground state simulation
 * has exponential complexity in of itself. Therefore, the algorithm is only feasible for small layouts with few inputs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected truth table of the layout.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The (partial) operational domain of the layout.
 */
template <typename Lyt, typename TT>
operational_domain operational_domain_random_sampling(const Lyt& lyt, const TT& spec, const std::size_t samples,
                                                      const operational_domain_params& params = {},
                                                      operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    operational_domain_stats                 st{};
    detail::operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.random_sampling(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * Computes the operational domain of the given SiDB cell-level layout. The operational domain is the set of all
 * parameter combinations for which the layout is logically operational. Logical operation is defined as the layout
 * implementing the given truth table. The input BDL pairs of the layout are assumed to be in the same order as the
 * inputs of the truth table. // TODO implement the matching of truth table inputs and BDL pair ordering
 *
 * This algorithm first uses random sampling to find a single operational point within the parameter range. From there,
 * it employs the "flood fill" algorithm to explore the operational domain. If the operational domain is connected, the
 * algorithm is guaranteed to find the entire operational domain within the parameter range if the initial random
 * sampling found an operational point.
 *
 * It performs up to `samples` uniformly-distributed random samples within the parameter range until an operational
 * point is found. From there, it performs another number of samples equal to the number of points within the
 * operational domain plus the first non-operational point in each direction. For each sample, the algorithm performs
 * one operational check on the layout, where each operational check consists of up to \f$ 2^n \f$ exact ground state
 * simulations, where \f$ n \f$ is the number of inputs of the layout. Each exact ground state simulation has
 * exponential complexity in of itself. Therefore, the algorithm is only feasible for small layouts with few inputs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected truth table of the layout.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The (partial) operational domain of the layout.
 */
template <typename Lyt, typename TT>
operational_domain operational_domain_flood_fill(const Lyt& lyt, const TT& spec, const std::size_t samples,
                                                 const operational_domain_params& params = {},
                                                 operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    operational_domain_stats                 st{};
    detail::operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.flood_fill(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_OPERATIONAL_DOMAIN_HPP
