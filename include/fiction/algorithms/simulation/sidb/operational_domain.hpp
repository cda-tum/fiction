//
// Created by marcel on 21.07.23.
//

#ifndef FICTION_OPERATIONAL_DOMAIN_HPP
#define FICTION_OPERATIONAL_DOMAIN_HPP

#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/execution_utils.hpp"
#include "fiction/utils/hash.hpp"
#include "fiction/utils/phmap_utils.hpp"

#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <phmap.h>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <tuple>
#include <type_traits>
#include <vector>

namespace fiction
{

/**
 * An operational domain is a set of simulation parameter values for which a given SiDB layout is logically operational.
 * This means that a layout is deemed operational if the layout's ground state corresponds with a given Boolean function
 * at the layout's outputs for all possible input combinations. In this implementation, \f$n\f$ BDL input wires and a
 * single BDL output wire are assumed for a given layout. Any operational domain computation algorithm toggles through
 * all \f$2^n\f$ input combinations and evaluates the layout's output behavior in accordance with the given Boolean
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
     * The parameter point holds parameter values in the x and y dimension.
     */
    struct parameter_point
    {
        /**
         * Standard default constructor.
         */
        parameter_point() = default;
        /**
         * Standard constructor.
         *
         * @param x_val X dimension parameter value.
         * @param y_val Y dimension parameter value.
         */
        parameter_point(const double x_val, const double y_val) : x{x_val}, y{y_val} {}
        /**
         * X dimension parameter value.
         */
        double x;
        /**
         * Y dimension parameter value.
         */
        double y;
        /**
         * Equality operator.
         *
         * @param other Other parameter point to compare with.
         * @return `true` iff the parameter points are equal.
         */
        [[nodiscard]] bool operator==(const parameter_point& other) const noexcept
        {
            return x == other.x && y == other.y;
        }
        /**
         * Inequality operator.
         *
         * @param other Other parameter point to compare with.
         * @return `true` iff the parameter points are not equal.
         */
        [[nodiscard]] bool operator!=(const parameter_point& other) const noexcept
        {
            return !(*this == other);
        }
        /**
         * Support for structured bindings.
         *
         * @tparam I Index of the parameter value to be returned.
         * @return The parameter value at the specified index.
         */
        template <std::size_t I>
        auto get() const noexcept
        {
            static_assert(I < 2, "Index out of bounds for parameter_point");

            if constexpr (I == 0)
            {
                return x;
            }
            else  // I == 1
            {
                return y;
            }
        }
        // Custom comparison function for parameter_point
        bool compare_parameter_point(const parameter_point& p1, const parameter_point& p2)
        {
            constexpr double tolerance = std::numeric_limits<double>::epsilon();  // Tolerance for comparison

            // Compare each coordinate with a tolerance
            return std::abs(p1.x - p2.x) < tolerance && std::abs(p1.y - p2.y) < tolerance;
        }
    };
    /**
     * The operational status of the layout for each specified parameter combination. This constitutes the operational
     * domain. The key of the map is the parameter point, which holds the parameter values in the x and y dimension.
     * The operational status is stored as the value of the map.
     */
    locked_parallel_flat_hash_map<parameter_point, operational_status> operational_values{};
};
/**
 * This struct represents a collection of parameter points along with their associated excited state numbers
 * for a specific charge distribution.
 */
struct valid_physical_parameters_with_excited_state_number
{
    /**
     * This map collects parameter points along with their corresponding excited state numbers for a specific
     * charge distribution. For example, an entry may look like ((5.0, 5.2), 1), indicating that the charge distribution
     * at the parameter point (5.0, 5.2) corresponds to the 1st excited state.
     */
    locked_parallel_flat_hash_map<operational_domain::parameter_point, uint64_t> suitable_physical_parameters{};
};
/**
 * This function compares two parameter points, `p1` and `p2`, for equality with tolerance.
 * It checks if the difference between each coordinate of the parameter points is within a specified tolerance.
 *
 * @param p1 The first parameter point to compare.
 * @param p2 The second parameter point to compare.
 * @return true if the parameter points are considered equal within the specified tolerance, false otherwise.
 */
static bool compare_parameter_point(const operational_domain::parameter_point& p1,
                                    const operational_domain::parameter_point& p2)
{
    constexpr double tolerance = fiction::physical_constants::POP_STABILITY_ERR;
    return std::abs(p1.x - p2.x) < tolerance && std::abs(p1.y - p2.y) < tolerance;
}
/**
 * This function searches for a parameter point, specified by the `parameter`,
 * in the provided locked_parallel_flat_hash_map `suitable_parameters` with tolerance.
 *
 * @param suitable_parameters The map containing parameter points as keys and associated values.
 * @param pp The parameter point to search for in the map.
 * @return An iterator to the found parameter point in the map, or end() if not found.
 */
static locked_parallel_flat_hash_map<operational_domain::parameter_point, uint64_t>::const_iterator
find_parameter_point_with_tolerance(
    const locked_parallel_flat_hash_map<operational_domain::parameter_point, uint64_t>& suitable_parameters,
    const operational_domain::parameter_point&                                          pp)
{
    auto compare_keys = [&pp](const auto& pair) { return compare_parameter_point(pair.first, pp); };
    return std::find_if(suitable_parameters.begin(), suitable_parameters.end(), compare_keys);
}

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
    sidb_simulation_parameters simulation_parameters{};
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
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
     * Standard constructor. Initializes the layout, the truth table, the parameters and the statistics. Also
     * detects the output BDL pair, which is necessary for the operational domain computation. The layout must
     * have exactly one output BDL pair.
     *
     * @param lyt SiDB cell-level layout to be evaluated.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     * @param ps Parameters for the operational domain computation.
     * @param st Statistics of the process.
     */
    operational_domain_impl(const Lyt& lyt, const std::vector<TT>& tt, const operational_domain_params& ps,
                            operational_domain_stats& st, bool consider_logic = true) noexcept :
            layout{lyt},
            truth_table{tt},
            params{ps},
            stats{st},
            output_bdl_pairs{consider_logic ?
                                 detect_bdl_pairs<Lyt>(lyt, sidb_technology::cell_type::OUTPUT, ps.bdl_params) :
                                 std::vector<bdl_pair<Lyt>>{}},
            x_indices(num_x_steps() + 1),  // pre-allocate the x dimension indices
            y_indices(num_y_steps() + 1)   // pre-allocate the y dimension indices
    {
        x_values.reserve(num_x_steps() + 1);
        y_values.reserve(num_y_steps() + 1);

        op_domain.x_dimension = params.x_dimension;
        op_domain.y_dimension = params.y_dimension;

        std::iota(x_indices.begin(), x_indices.end(), 0ul);
        std::iota(y_indices.begin(), y_indices.end(), 0ul);

        // if the value of the x-parameter is greater than params.x_max after num_x_steps() steps, this value is
        // ignored in the operational domain calculation.
        if ((params.x_min + (x_indices.size() - 1) * params.x_step) - params.x_max >
            physical_constants::POP_STABILITY_ERR)
        {
            x_indices.pop_back();
        }
        // if the value of the y-parameter is greater than params.y_max after num_y_steps() steps, this value is
        // ignored in the operational domain calculation.
        if (((params.y_min + (y_indices.size() - 1) * params.y_step) - params.y_max) >
            physical_constants::POP_STABILITY_ERR)
        {
            y_indices.pop_back();
        }

        // generate the x dimension values
        for (std::size_t i = 0; i < x_indices.size(); ++i)
        {
            x_values.push_back(params.x_min + static_cast<double>(i) * params.x_step);
        }

        // generate the y dimension values
        for (std::size_t i = 0; i < y_indices.size(); ++i)
        {
            y_values.push_back(params.y_min + static_cast<double>(i) * params.y_step);
        }
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

        // for each x value in parallel
        std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ x_indices.cbegin(), x_indices.cend(),
                      [this](const auto x)
                      {
                          // for each y value in parallel
                          std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ y_indices.cbegin(), y_indices.cend(),
                                        [this, x](const auto y) {
                                            is_step_point_operational({x, y});
                                        });
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

        const auto step_point_samples = generate_random_step_points(samples);

        // for each sample point in parallel
        std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ step_point_samples.cbegin(), step_point_samples.cend(),
                      [this](const auto& sp) { is_step_point_operational(sp); });

        log_stats();

        return op_domain;
    }
    /**
     * Performs flood fill to determine the operational domain. The algorithm first performs a random sampling of the
     * specified number of samples. From each operational point found in this way, it starts the flood fill. The
     * operational domain will finally only contain up to `samples` random non-operational points as well as all
     * operational points that are reachable via flood fill from the found operational points plus a one pixel wide
     * border around the domain.
     *
     * @param samples Maximum number of random samples to be taken before flood fill.
     * @return The (partial) operational domain of the layout.
     */
    [[nodiscard]] operational_domain flood_fill(const std::size_t samples) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        const auto step_point_samples = generate_random_step_points(samples);

        // for each sample point in parallel
        std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ step_point_samples.cbegin(), step_point_samples.cend(),
                      [this](const auto& sp) { is_step_point_operational(sp); });

        // a queue of (x, y) dimension step points to be evaluated
        std::queue<step_point> queue{};

        // a utility function that adds the adjacent points to the queue for further evaluation
        const auto queue_next_points = [this, &queue](const step_point& sp)
        {
            for (const auto& m : moore_neighborhood(sp))
            {
                if (!has_already_been_sampled(m))
                {
                    queue.push(m);
                }
            }
        };

        // add the neighbors of each operational point to the queue
        for (const auto& [param_point, status] : op_domain.operational_values)
        {
            if (status == operational_status::OPERATIONAL)
            {
                queue_next_points(to_step_point(param_point));
            }
        }

        // for each point in the queue
        while (!queue.empty())
        {
            // fetch the step point and remove it from the queue
            const auto sp = queue.front();
            queue.pop();

            // if the point has already been sampled, continue with the next
            if (has_already_been_sampled(sp))
            {
                continue;
            }

            // check if the point is operational
            const auto operational_status = is_step_point_operational(sp);

            // if the point is operational, add its eight neighbors to the queue
            if (operational_status == operational_status::OPERATIONAL)
            {
                queue_next_points(sp);
            }
        }

        log_stats();

        return op_domain;
    }
    /**
     * Performs contour tracing to determine the operational domain. The algorithm first performs a random sampling of
     * up to the specified number of samples. It stops random sampling once it finds a single operational point, from
     * which it moves straight outwards until it encounters the counter of the operational domain. From this point, it
     * traces the contour until it reaches the initial contour point again. The operational domain will finally only
     * contain up to `samples` random non-operational points as well as the contour of the found operational domain plus
     * a one pixel wide border around it.
     *
     * @param samples Maximum number of random samples to be taken before contour tracing.
     * @return The (partial) operational domain of the layout.
     */
    [[nodiscard]] operational_domain contour_tracing(const std::size_t samples) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        // first, perform random sampling to find an operational starting point
        const auto starting_point = find_operational_step_point_via_random_sampling(samples);

        // if no operational point was found within the specified number of samples, return
        if (!starting_point.has_value())
        {
            return op_domain;
        }

        const auto next_clockwise_point = [](std::vector<step_point>& neighborhood,
                                             const step_point&        backtrack) noexcept -> step_point
        {
            assert(std::find(neighborhood.cbegin(), neighborhood.cend(), backtrack) != neighborhood.cend() &&
                   "The backtrack point must be part of the neighborhood");

            while (neighborhood.back() != backtrack)
            {
                std::rotate(neighborhood.begin(), neighborhood.begin() + 1, neighborhood.end());
            }

            return neighborhood.front();
        };

        // find an operational point on the contour starting from the randomly determined starting point
        const auto contour_starting_point = find_operational_contour_step_point(*starting_point);

        auto current_contour_point = contour_starting_point;
        auto backtrack_point       = current_contour_point.x == 0 ?
                                         current_contour_point :
                                         step_point{current_contour_point.x - 1, current_contour_point.y};

        auto current_neighborhood = moore_neighborhood(current_contour_point);

        auto next_point = contour_starting_point;

        if (!current_neighborhood.empty())
        {
            next_point = current_contour_point == backtrack_point ?
                             current_neighborhood.front() :
                             next_clockwise_point(current_neighborhood, backtrack_point);
        }

        while (next_point != contour_starting_point)
        {
            const auto operational_status = is_step_point_operational(next_point);

            if (operational_status == operational_status::OPERATIONAL)
            {
                backtrack_point       = current_contour_point;
                current_contour_point = next_point;
            }
            else
            {
                backtrack_point = next_point;
            }

            current_neighborhood = moore_neighborhood(current_contour_point);
            next_point           = next_clockwise_point(current_neighborhood, backtrack_point);
        }

        log_stats();

        return op_domain;
    }

    /**
     * Performs a grid search over the specified parameter ranges with the specified step sizes. The grid search always
     * has quadratic complexity. The operational status is computed for each parameter combination.
     *
     * @return The operational domain of the layout.
     */
    [[nodiscard]] valid_physical_parameters_with_excited_state_number
    grid_search_to_determine_parameter_for_given_cds(Lyt& lyt) noexcept
    {
        valid_physical_parameters_with_excited_state_number suitable_params_domain{};

        mockturtle::stopwatch stop{stats.time_total};

        // for each x value in parallel
        std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ x_indices.cbegin(), x_indices.cend(),
                      [this, &lyt](const auto x)
                      {
                          // for each y value in parallel
                          std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ y_indices.cbegin(), y_indices.cend(),
                                        [this, &lyt, x](const auto y) {
                                            is_step_point_suitable_for_given_cds({x, y}, lyt);
                                        });
                      });

        sidb_simulation_parameters simulation_parameters = params.simulation_parameters;

        for (const auto& [param_point, status] : op_domain.operational_values)
        {
            if (status == operational_status::OPERATIONAL)
            {
                set_x_dimension_value(simulation_parameters, param_point.x);
                set_y_dimension_value(simulation_parameters, param_point.y);
                const auto simulation_results = quickexact(
                    lyt,
                    quickexact_params<cell<Lyt>>{simulation_parameters,
                                                 quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF});
                const auto energy_dist = energy_distribution(simulation_results.charge_distributions);
                lyt.assign_physical_parameters(simulation_parameters);
                const auto position = energy_dist.find(lyt.get_system_energy());
                if (position != energy_dist.cend())
                {
                    const auto excited_state_number = std::distance(energy_dist.begin(), position);
                    suitable_params_domain.suitable_physical_parameters.emplace(param_point, excited_state_number);
                }
            }
        }

        return suitable_params_domain;
    }

  private:
    /**
     * The SiDB cell-level layout to investigate.
     */
    const Lyt& layout;
    /**
     * The specification of the layout.
     */
    const std::vector<TT>& truth_table;
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
     * A step point represents a point in the x and y dimension from 0 to the maximum number of steps. A step point does
     * not hold the actual parameter values, but the step values in the x and y dimension, respectively.
     *
     * See `operational_domain::parameter_point` for a point that holds the actual parameter values.
     */
    struct step_point
    {
        /**
         * Standard default constructor.
         */
        step_point() = default;
        /**
         * Standard constructor.
         *
         * @param x_step X dimension step value.
         * @param y_step Y dimension step value.
         */
        step_point(const std::size_t x_step, const std::size_t y_step) : x{x_step}, y{y_step} {}
        /**
         * X dimension step value.
         */
        std::size_t x;
        /**
         * Y dimension step value.
         */
        std::size_t y;
        /**
         * Equality operator.
         *
         * @param other Other step point to compare with.
         * @return `true` iff the step points are equal.
         */
        [[nodiscard]] bool operator==(const step_point& other) const noexcept
        {
            return x == other.x && y == other.y;
        }
        /**
         * Inequality operator.
         *
         * @param other Other step point to compare with.
         * @return `true` iff the step points are not equal.
         */
        [[nodiscard]] bool operator!=(const step_point& other) const noexcept
        {
            return !(*this == other);
        }
        /**
         * Less than operator.
         *
         * @param other Other step point to compare with.
         * @return `true` if this step point is less than to the other.
         */
        [[nodiscard]] bool operator<(const step_point& other) const noexcept
        {
            if (y != other.y)
            {
                return y < other.y;
            }
            return x < other.x;
        }
    };
    /**
     * Converts a step point to a parameter point.
     *
     * @param sp Step point to convert.
     * @return The parameter point corresponding to the step point `sp`.
     */
    [[nodiscard]] operational_domain::parameter_point to_parameter_point(const step_point& sp) const noexcept
    {
        return {x_values[sp.x], y_values[sp.y]};
    }
    /**
     * Converts a parameter point to a step point.
     *
     * @param pp Parameter point to convert.
     * @return The step point corresponding to the parameter point `pp`.
     */
    [[nodiscard]] step_point to_step_point(const operational_domain::parameter_point& pp) const noexcept
    {
        const auto it_x = std::lower_bound(x_values.cbegin(), x_values.cend(), pp.x);
        const auto it_y = std::lower_bound(y_values.cbegin(), y_values.cend(), pp.y);

        assert(it_x != x_values.cend() && "parameter point is outside of the x range");
        assert(it_y != y_values.cend() && "parameter point is outside of the y range");

        const auto x_dis = std::distance(x_values.cbegin(), it_x);
        const auto y_dis = std::distance(y_values.cbegin(), it_y);

        return {static_cast<std::size_t>(x_dis), static_cast<std::size_t>(y_dis)};
    }
    /**
     * Calculates the number of steps in the x dimension based on the provided parameters.
     *
     * @return The number of steps in the x dimension.
     */
    [[nodiscard]] inline std::size_t num_x_steps() const noexcept
    {
        return static_cast<std::size_t>(std::round((params.x_max - params.x_min) / params.x_step));
    }
    /**
     * Calculates the number of steps in the y dimension based on the provided parameters.
     *
     * @return The number of steps in the y dimension.
     */
    [[nodiscard]] inline std::size_t num_y_steps() const noexcept
    {
        return static_cast<std::size_t>(std::round((params.y_max - params.y_min) / params.y_step));
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
     * Determines whether the point at step position `(x, y)` has already been sampled and returns the operational value
     * at `(x, y)` if it already exists. Here, `x` and `y` represent steps in the x and y dimension, respectively, not
     * the actual values of the parameters.
     *
     * @param sp Step point to check.
     * @return The operational status of the point at step position `sp = (x, y)` or `std::nullopt` if `(x, y)` has not
     * been sampled yet.
     */
    [[nodiscard]] inline std::optional<operational_status> has_already_been_sampled(const step_point& sp) const noexcept
    {
        if (const auto it = op_domain.operational_values.find(to_parameter_point(sp));
            it != op_domain.operational_values.cend())
        {
            return it->second;
        }

        return std::nullopt;
    }
    /**
     * Logs and returns the operational status at the given point `sp = (x, y)`. If the point has already been sampled,
     * it returns the cached value. Otherwise, a ground state simulation is performed for all input combinations of the
     * stored layout using the given simulation parameters. It terminates as soon as a non-operational state is found.
     * In the worst case, the function performs \f$2^n\f$ simulations, where \f$n\f$ is the number of inputs of the
     * layout. This function is used by all operational domain computation techniques.
     *
     * Any investigated point is added to the stored `op_domain`, regardless of its operational status.
     *
     * @param sp Step point to be investigated.
     * @return The operational status of the layout under the given simulation parameters.
     */
    operational_status is_step_point_operational(const step_point& sp) noexcept
    {
        // if the point has already been sampled, return the stored operational status
        if (const auto op_value = has_already_been_sampled(sp); op_value.has_value())
        {
            return *op_value;
        }

        // fetch the x and y dimension values
        const auto param_point = to_parameter_point(sp);

        const auto operational = [this, &param_point]()
        {
            op_domain.operational_values[param_point] = operational_status::OPERATIONAL;

            return operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, &param_point]()
        {
            op_domain.operational_values[param_point] = operational_status::NON_OPERATIONAL;

            return operational_status::NON_OPERATIONAL;
        };

        // increment the number of evaluated parameter combinations
        ++num_evaluated_parameter_combinations;

        sidb_simulation_parameters sim_params = params.simulation_parameters;
        set_x_dimension_value(sim_params, param_point.x);
        set_y_dimension_value(sim_params, param_point.y);

        const auto& [status, sim_calls] =
            is_operational(layout, truth_table, is_operational_params{sim_params, params.sim_engine});
        num_simulator_invocations += sim_calls;

        if (status == operational_status::NON_OPERATIONAL)
        {
            return non_operational();
        }

        // if we made it here, the layout is operational
        return operational();
    }
    /**
     * Logs and returns the operational status at the given point `sp = (x, y)`. If the point has already been sampled,
     * it returns the cached value. Otherwise, a ground state simulation is performed for all input combinations of the
     * stored layout using the given simulation parameters. It terminates as soon as a non-operational state is found.
     * In the worst case, the function performs \f$ 2^n \f$ simulations, where \f$ n \f$ is the number of inputs of the
     * layout. This function is used by all operational domain computation techniques.
     *
     * Any investigated point is added to the stored `op_domain`, regardless of its operational status.
     *
     * @param sp Step point to be investigated.
     * @return The operational status of the layout under the given simulation parameters.
     */
    operational_status is_step_point_suitable_for_given_cds(const step_point& sp, Lyt& lyt) noexcept
    {
        // if the point has already been sampled, return the stored operational status
        if (const auto op_value = has_already_been_sampled(sp); op_value.has_value())
        {
            return *op_value;
        }

        // fetch the x and y dimension values
        const auto param_point = to_parameter_point(sp);

        const auto operational = [this, &param_point]()
        {
            op_domain.operational_values[param_point] = operational_status::OPERATIONAL;

            return operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, &param_point]()
        {
            op_domain.operational_values[param_point] = operational_status::NON_OPERATIONAL;

            return operational_status::NON_OPERATIONAL;
        };

        // increment the number of evaluated parameter combinations
        ++num_evaluated_parameter_combinations;

        sidb_simulation_parameters sim_params = params.simulation_parameters;
        set_x_dimension_value(sim_params, param_point.x);
        set_y_dimension_value(sim_params, param_point.y);

        lyt.assign_physical_parameters(sim_params);

        if (lyt.is_physically_valid())
        {
            return operational();
        }

        // if we made it here, the layout is operational
        return non_operational();
    }
    /**
     * Generates (potentially repeating) random `step_points` in the stored parameter range. The number of generated
     * points is exactly equal to `samples`.
     *
     * @param samples Number of random `step_point`s to generate.
     * @return A set of random `step_point`s in the stored parameter range.
     */
    [[nodiscard]] std::set<step_point> generate_random_step_points(const std::size_t samples) noexcept
    {
        static std::mt19937_64 generator{std::random_device{}()};

        // instantiate distributions
        std::uniform_int_distribution<std::size_t> x_distribution{0, x_indices.size() - 1};
        std::uniform_int_distribution<std::size_t> y_distribution{0, y_indices.size() - 1};

        // container for the random samples
        std::set<step_point> step_point_samples{};

        for (std::size_t i = 0; i < samples; ++i)
        {
            // sample x and y dimension
            step_point_samples.insert(step_point{x_distribution(generator), y_distribution(generator)});
        }

        return step_point_samples;
    }
    /**
     * Performs random sampling to find any operational parameter combination. This function is useful if a single
     * starting point is required within the domain to expand from. This function returns the step in x and y dimension
     * of the first operational point found. If no operational parameter combination can be found within the given
     * number of samples, the function returns `std::nullopt`.
     *
     * This function adds any sampled points to the `op_domain` member variables.
     *
     * @param samples Maximum number of samples to take. Works as a timeout.
     * @return The first operational step point, if any could be found, `std::nullopt` otherwise.
     */
    [[nodiscard]] std::optional<step_point>
    find_operational_step_point_via_random_sampling(const std::size_t samples) noexcept
    {
        for (const auto& sample_step_point : generate_random_step_points(samples))
        {
            // determine the operational status
            const auto operational_value = is_step_point_operational(sample_step_point);

            // if the parameter combination is operational, return its step values in x and y dimension
            if (operational_value == operational_status::OPERATIONAL)
            {
                return sample_step_point;
            }
        }

        return std::nullopt;
    }
    /**
     * Finds a boundary starting point for the contour tracing algorithm. This function starts at the given starting
     * point and moves towards the left edge of the parameter range. It returns the last operational point it
     * encounters before it reaches the edge. If no non-operational point is found, the operational area extends outside
     * the parameter range and the function returns the last operational point that was investigated, i.e., a point at
     * the border of the parameter range.
     *
     * @param starting_point Starting step point for the boundary search.
     * @return An operational step point at the edge of the operational domain `starting_point` is located in.
     */
    [[nodiscard]] step_point find_operational_contour_step_point(const step_point& starting_point) noexcept
    {
        auto latest_operational_point = starting_point;

        // move towards the left border of the parameter range
        for (std::size_t x = starting_point.x; x > 0; --x)
        {
            const auto left_step = step_point{x, starting_point.y};

            const auto operational_status = is_step_point_operational(left_step);

            if (operational_status == operational_status::OPERATIONAL)
            {
                latest_operational_point = left_step;
            }
            else
            {
                return latest_operational_point;
            }
        }

        // if no boundary point was found, the operational area extends outside the parameter range
        // return the latest operational point
        return latest_operational_point;
    }
    /**
     * Returns the Moore neighborhood of the step point at `sp = (x, y)`. The Moore neighborhood is the set of all
     * points that are adjacent to `(x, y)` including the diagonals. Thereby, the Moore neighborhood contains up to 8
     * points as points outside of the parameter range are not gathered. The points are returned in clockwise order
     * starting from the right neighbor.
     *
     * @param sp Step point to get the Moore neighborhood of.
     * @return The Moore neighborhood of the step point at `sp = (x, y)`.
     */
    [[nodiscard]] std::vector<step_point> moore_neighborhood(const step_point& sp) const noexcept
    {
        std::vector<step_point> neighbors{};
        neighbors.reserve(8);

        const auto& [x, y] = sp;

        const auto decr_x = (x > 0) ? x - 1 : x;
        const auto incr_x = (x + 1 < x_indices.size()) ? x + 1 : x;
        const auto decr_y = (y > 0) ? y - 1 : y;
        const auto incr_y = (y + 1 < y_indices.size()) ? y + 1 : y;

        // add neighbors in clockwise direction

        // right
        if (x != incr_x)
        {
            neighbors.emplace_back(incr_x, y);
        }
        // lower-right
        if (x != incr_x && y != decr_y)
        {
            neighbors.emplace_back(incr_x, decr_y);
        }
        // down
        if (y != decr_y)
        {
            neighbors.emplace_back(x, decr_y);
        }
        // lower-left
        if (x != decr_x && y != decr_y)
        {
            neighbors.emplace_back(decr_x, decr_y);
        }
        // left
        if (x != decr_x)
        {
            neighbors.emplace_back(decr_x, y);
        }
        // upper-left
        if (x != decr_x && y != incr_y)
        {
            neighbors.emplace_back(decr_x, incr_y);
        }
        // up
        if (y != incr_y)
        {
            neighbors.emplace_back(x, incr_y);
        }
        // upper-right
        if (x != incr_x && y != incr_y)
        {
            neighbors.emplace_back(incr_x, incr_y);
        }

        return neighbors;
    };
    /**
     * Helper function that writes the the statistics of the operational domain computation to the statistics object.
     * Due to data races that can occur during the computation, each value is temporarily held in an atomic variable and
     * written to the statistics object only after the computation has finished.
     */
    void log_stats() const noexcept
    {
        stats.num_simulator_invocations            = num_simulator_invocations;
        stats.num_evaluated_parameter_combinations = num_evaluated_parameter_combinations;

        for (const auto& [param_point, status] : op_domain.operational_values)
        {
            if (status == operational_status::OPERATIONAL)
            {
                ++stats.num_operational_parameter_combinations;
            }
            else
            {
                ++stats.num_non_operational_parameter_combinations;
            }
        }
    }
};

}  // namespace detail

/**
 * Computes the operational domain of the given SiDB cell-level layout. The operational domain is the set of all
 * parameter combinations for which the layout is logically operational. Logical operation is defined as the layout
 * implementing the given truth table. The input BDL pairs of the layout are assumed to be in the same order as the
 * inputs of the truth table.
 *
 * This algorithm uses a grid search to find the operational domain. The grid search is performed by exhaustively
 * sweeping the parameter space in the x and y dimensions. Since grid search is exhaustive, the algorithm is guaranteed
 * to find the operational domain, if it exists within the parameter range. However, the algorithm performs a quadratic
 * number of operational checks on the layout, where each operational check consists of up to \f$2^n\f$ exact ground
 * state simulations, where \f$n\f$ is the number of inputs of the layout. Each exact ground state simulation has
 * exponential complexity in of itself. Therefore, the algorithm is only feasible for small layouts with few inputs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected vector of truth tables of the layout. Each truth table represents an output of
 * the Boolean function.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The operational domain of the layout.
 */
template <typename Lyt, typename TT>
operational_domain operational_domain_grid_search(const Lyt& lyt, const std::vector<TT>& spec,
                                                  const operational_domain_params& params = {},
                                                  operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
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
 * inputs of the truth table.
 *
 * This algorithm uses random sampling to find a part of the operational domain that might not be complete. It performs
 * a total of `samples` uniformly-distributed random samples within the parameter range. For each sample, the algorithm
 * performs one operational check on the layout, where each operational check consists of up to \f$2^n\f$ exact
 * ground state simulations, where \f$n\f$ is the number of inputs of the layout. Each exact ground state simulation
 * has exponential complexity in of itself. Therefore, the algorithm is only feasible for small layouts with few inputs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The (partial) operational domain of the layout.
 */
template <typename Lyt, typename TT>
operational_domain operational_domain_random_sampling(const Lyt& lyt, const std::vector<TT>& spec,
                                                      const std::size_t                samples,
                                                      const operational_domain_params& params = {},
                                                      operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
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
 * inputs of the truth table.
 *
 * This algorithm first uses random sampling to find several operational points within the parameter range. From there,
 * it employs the "flood fill" algorithm to explore the operational domain. The algorithm is guaranteed to find all
 * operational areas in their entirety if the initial random sampling found at least one operational point within them.
 * Thereby, this algorithm works for disconnected operational domains.
 *
 * It performs `samples` uniformly-distributed random samples within the parameter range. From there, it performs
 * another number of samples equal to the number of points within the operational domain plus the first non-operational
 * point in each direction. For each sample, the algorithm performs one operational check on the layout, where each
 * operational check consists of up to \f$2^n\f$ exact ground state simulations, where \f$n\f$ is the number of
 * inputs of the layout. Each exact ground state simulation has exponential complexity in of itself. Therefore, the
 * algorithm is only feasible for small layouts with few inputs.
 *
 * This flavor of operational domain computation was proposed in \"Reducing the Complexity of Operational Domain
 * Computation in Silicon Dangling Bond Logic\" by M. Walter, J. Drewniok, S. S. H. Ng, K. Walus, and R. Wille in
 * NANOARCH 2023.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The (partial) operational domain of the layout.
 */
template <typename Lyt, typename TT>
operational_domain operational_domain_flood_fill(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                                                 const operational_domain_params& params = {},
                                                 operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
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
/**
 * Computes the operational domain of the given SiDB cell-level layout. The operational domain is the set of all
 * parameter combinations for which the layout is logically operational. Logical operation is defined as the layout
 * implementing the given truth table. The input BDL pairs of the layout are assumed to be in the same order as the
 * inputs of the truth table.
 *
 * This algorithm first uses random sampling to find a single operational point within the parameter range. From there,
 * it traverses outwards to find the edge of the operational area and performs Moore neighborhood contour tracing to
 * explore the contour of the operational domain. If the operational domain is connected, the algorithm is guaranteed to
 * find the contours of the entire operational domain within the parameter range if the initial random sampling found an
 * operational point.
 *
 * It performs up to `samples` uniformly-distributed random samples within the parameter range until an operational
 * point is found. From there, it performs another number of samples equal to the distance to an edge of the operational
 * area. Finally, it performs up to 8 samples for each contour point (however, the actual number is usually much lower).
 * For each sample, the algorithm performs one operational check on the layout, where each operational check consists of
 * up to \f$2^n\f$ exact ground state simulations, where \f$n\f$ is the number of inputs of the layout. Each exact
 * ground state simulation has exponential complexity in of itself. Therefore, the algorithm is only feasible for small
 * layouts with few inputs.
 *
 * This flavor of operational domain computation was proposed in \"Reducing the Complexity of Operational Domain
 * Computation in Silicon Dangling Bond Logic\" by M. Walter, J. Drewniok, S. S. H. Ng, K. Walus, and R. Wille in
 * NANOARCH 2023.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The (partial) operational domain of the layout.
 */
template <typename Lyt, typename TT>
operational_domain operational_domain_contour_tracing(const Lyt& lyt, const std::vector<TT>& spec,
                                                      const std::size_t                samples,
                                                      const operational_domain_params& params = {},
                                                      operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    operational_domain_stats                 st{};
    detail::operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.contour_tracing(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * This function computes the physical parameters necessary for ensuring the physical validity of the given charge
 * distribution surface `cds`. It not only identifies the physical parameters that render the charge
 * distribution physically valid but also determines the excited state number. The ground state corresponds to zero,
 * and each subsequent excited state is numbered accordingly.
 *
 * @tparam Lyt The charge distribution surface type.
 * @param cds The charge distribution surface for which physical parameters are to be determined.
 * @param params Operational domain parameters.
 * @return An instance of valid_physical_parameters_with_excited_state_number containing the determined physical
 * parameters.
 */
template <typename Lyt>
[[nodiscard]] valid_physical_parameters_with_excited_state_number
find_valid_physical_parameters_and_excited_state_number(Lyt& cds, const operational_domain_params& params = {})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(is_charge_distribution_surface_v<Lyt>, "Lyt is not a charge distribution surface");

    operational_domain_stats                 st{};
    detail::operational_domain_impl<Lyt, tt> p{cds, std::vector<tt>{}, params, st, false};

    const auto result = p.grid_search_to_determine_parameter_for_given_cds(cds);

    return result;
}

}  // namespace fiction

namespace std
{
// make `operational_domain::parameter_point` compatible with `std::integral_constant`
template <>
struct tuple_size<fiction::operational_domain::parameter_point> : std::integral_constant<size_t, 2>
{};
// make `operational_domain::parameter_point` compatible with `std::tuple_element`
template <size_t I>
struct tuple_element<I, fiction::operational_domain::parameter_point>
{
    using type = double;
};
// make `operational_domain::parameter_point` compatible with `std::hash`
template <>
struct hash<fiction::operational_domain::parameter_point>
{
    size_t operator()(const fiction::operational_domain::parameter_point& p) const noexcept
    {
        size_t h = 0;
        fiction::hash_combine(h, p.x, p.y);

        return h;
    }
};
}  // namespace std

#endif  // FICTION_OPERATIONAL_DOMAIN_HPP
