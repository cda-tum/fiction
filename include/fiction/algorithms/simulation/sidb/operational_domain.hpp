//
// Created by marcel on 21.07.23.
//

#ifndef FICTION_OPERATIONAL_DOMAIN_HPP
#define FICTION_OPERATIONAL_DOMAIN_HPP

#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/traits.hpp"
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
#include <stdexcept>
#include <thread>
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
    enum class sweep_parameter : uint8_t
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
     * The dimensions to sweep over, ordered by priority. The first dimension is the x dimension, the second dimension
     * is the y dimension, etc.
     */
    std::vector<sweep_parameter> dimensions{};
    /**
     * The parameter point holds parameter values in an arbitrary number of dimensions.
     */
    struct parameter_point
    {
        /**
         * Default constructor.
         */
        parameter_point() = default;
        /**
         * Standard constructor.
         *
         * @param values Parameter values for each dimension.
         */
        explicit parameter_point(const std::vector<double>& values) : parameters(values) {}
        /**
         * Parameter values for each dimension.
         */
        std::vector<double> parameters;
        /**
         * Equality operator.
         *
         * @param other Other parameter point to compare with.
         * @return `true` if the parameter points are equal.
         */
        bool operator==(const parameter_point& other) const noexcept
        {
            return parameters == other.parameters;
        }
        /**
         * Inequality operator.
         *
         * @param other Other parameter point to compare with.
         * @return `true` if the parameter points are not equal.
         */
        bool operator!=(const parameter_point& other) const noexcept
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
        auto get() const
        {
            if (I >= parameters.size())
            {
                throw std::out_of_range("Index out of bounds for parameter_point");
            }

            return parameters[I];
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
 * A range of values for a dimension sweep. The range is defined by a minimum value, a maximum value and a step size.
 */
struct operational_domain_value_range
{
    /**
     * The sweep parameter of the dimension.
     */
    operational_domain::sweep_parameter dimension;
    /**
     * The minimum value of the dimension sweep.
     */
    double min{1.0};
    /**
     * The maximum value of the dimension sweep.
     */
    double max{10.0};
    /**
     * The step size of the dimension sweep.
     */
    double step{0.1};
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
    sidb_simulation_parameters simulation_parameters{};
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
    /**
     * The dimensions to sweep over together with their value ranges, ordered by priority. The first dimension is the x
     * dimension, the second dimension is the y dimension, etc.
     */
    std::vector<operational_domain_value_range> sweep_dimensions{
        operational_domain_value_range{operational_domain::sweep_parameter::EPSILON_R, 1.0, 10.0, 0.1},
        operational_domain_value_range{operational_domain::sweep_parameter::LAMBDA_TF, 1.0, 10.0, 0.1}};
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
                            operational_domain_stats& st) noexcept :
            layout{lyt},
            truth_table{tt},
            params{ps},
            stats{st},
            output_bdl_pairs{detect_bdl_pairs<Lyt>(layout, sidb_technology::cell_type::OUTPUT, params.bdl_params)},
            num_dimensions{params.sweep_dimensions.size()}
    {
        op_domain.dimensions.reserve(num_dimensions);

        indices.reserve(num_dimensions);
        values.reserve(num_dimensions);

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            op_domain.dimensions.push_back(params.sweep_dimensions[d].dimension);

            // generate the step points for the dimension
            indices.push_back(std::vector<std::size_t>(num_steps(d) + 1));
            std::iota(indices[d].begin(), indices[d].end(), 0ul);

            // if the value of the parameter is greater than params.max after num_x_steps() steps, this value is
            // ignored in the operational domain calculation.
            if ((params.sweep_dimensions[d].min +
                 static_cast<double>(indices[d].size() - 1) * params.sweep_dimensions[d].step) -
                    params.sweep_dimensions[d].max >
                physical_constants::POP_STABILITY_ERR)
            {
                indices[d].pop_back();
            }

            values.emplace_back();

            // generate the values for the dimension
            for (const auto i : indices[d])
            {
                values[d].push_back(params.sweep_dimensions[d].min +
                                    static_cast<double>(i) * params.sweep_dimensions[d].step);
            }
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

        // generate all possible step point combinations via the cartesian product
        std::vector<step_point> all_step_points{step_point{}};
        for (const auto& dimension : indices)
        {
            std::vector<step_point> expanded_products{};
            for (const auto& product : all_step_points)
            {
                for (const auto& element : dimension)
                {
                    step_point new_product = product;
                    new_product.step_values.push_back(element);
                    expanded_products.push_back(new_product);
                }
            }
            all_step_points = expanded_products;
        }

        simulate_operational_status_in_parallel(all_step_points);

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

        simulate_operational_status_in_parallel(step_point_samples);

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
        assert(num_dimensions == 2 && "Flood fill is only supported for two dimensions");

        mockturtle::stopwatch stop{stats.time_total};

        const auto step_point_samples = generate_random_step_points(samples);

        simulate_operational_status_in_parallel(step_point_samples);

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
        assert(num_dimensions == 2 && "Contour tracing is only supported for two dimensions");

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

        const auto x = current_contour_point.step_values[0];
        const auto y = current_contour_point.step_values[1];

        auto backtrack_point = x == 0 ? current_contour_point : step_point{{x - 1, y}};

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
     * The output BDL pairs of the layout.
     */
    const std::vector<bdl_pair<cell<Lyt>>> output_bdl_pairs;
    /**
     * The number of dimensions.
     */
    const std::size_t num_dimensions;
    /**
     * Dimension steps.
     */
    std::vector<std::vector<std::size_t>> indices;
    /**
     * All dimension values.
     */
    std::vector<std::vector<double>> values;
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
     * Number of available hardware threads.
     */
    const std::size_t num_threads{std::thread::hardware_concurrency()};
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
         * @param steps All dimension step values.
         */
        explicit step_point(const std::vector<std::size_t>& steps) : step_values(steps) {}
        /**
         * All dimension step values.
         */
        std::vector<std::size_t> step_values;
        /**
         * Equality operator.
         *
         * @param other Other step point to compare with.
         * @return `true` iff the step points are equal.
         */
        [[nodiscard]] bool operator==(const step_point& other) const noexcept
        {
            return step_values == other.step_values;
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
            return step_values < other.step_values;
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
        std::vector<double> parameter_values{};
        for (auto d = 0u; d < num_dimensions; ++d)
        {
            parameter_values.push_back(values[d][sp.step_values[d]]);
        }

        return operational_domain::parameter_point{parameter_values};
    }
    /**
     * Converts a parameter point to a step point.
     *
     * @param pp Parameter point to convert.
     * @return The step point corresponding to the parameter point `pp`.
     */
    [[nodiscard]] step_point to_step_point(const operational_domain::parameter_point& pp) const noexcept
    {
        std::vector<std::size_t> step_values;
        for (auto d = 0u; d < num_dimensions; ++d)
        {
            const auto it = std::lower_bound(values[d].cbegin(), values[d].cend(), pp.parameters[d]);

            assert(it != values[d].cend() && "parameter point is outside of the value range");

            const auto dis = std::distance(values[d].cbegin(), it);

            step_values.push_back(static_cast<std::size_t>(dis));
        }

        return step_point{step_values};
    }
    /**
     * Calculates the number of steps in the given dimension based on the provided parameters.
     *
     * @return The number of steps in the given dimension.
     */
    [[nodiscard]] inline std::size_t num_steps(const std::size_t dimension) const noexcept
    {
        assert(dimension < num_dimensions && "Invalid dimension");

        return static_cast<std::size_t>(
            std::round((params.sweep_dimensions[dimension].max - params.sweep_dimensions[dimension].min) /
                       params.sweep_dimensions[dimension].step));
    }
    /**
     * Helper function that sets the value of a sweep dimension in the simulation parameters.
     *
     * @param sim_parameters Simulation parameter object to set the sweep dimension `dim` to value `val`.
     * @param val Value to set the dimension `dim` to.
     * @param dim Sweep dimension to set the value `val` to.
     */
    inline void set_dimension_value(sidb_simulation_parameters& sim_parameters, const double val,
                                    const std::size_t dim) const noexcept
    {
        const operational_domain::sweep_parameter sweep_parameter = op_domain.dimensions[dim];

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
     * Determines whether the point at step position `(d1, ..., dn)` has already been sampled and returns the
     * operational value at `(d1, ..., dn)` if it already exists. Here, `di` represents steps in the i-th dimension, not
     * the actual values of the parameters.
     *
     * @param sp Step point to check.
     * @return The operational status of the point at step position `sp = (d1, ..., dn)` or `std::nullopt` if the point
     * `(d1, ..., dn)` has not been sampled yet.
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
     * Logs and returns the operational status at the given point `sp = (d1, ..., dn)`. If the point has already been
     * sampled, it returns the cached value. Otherwise, a ground state simulation is performed for all input
     * combinations of the stored layout using the given simulation parameters. It terminates as soon as a
     * non-operational state is found. In the worst case, the function performs \f$2^i\f$ simulations, where \f$i\f$ is
     * the number of inputs of the layout. This function is used by all operational domain computation techniques.
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
        for (auto d = 0u; d < num_dimensions; ++d)
        {
            set_dimension_value(sim_params, values[d][sp.step_values[d]], d);
        }

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
     * Generates unique random `step_points` in the stored parameter range. The number of generated points is at most
     * equal to `samples`.
     *
     * @param samples Maximum number of random `step_point`s to generate.
     * @return A vector of unique random `step_point`s in the stored parameter range of size at most equal to `samples`.
     */
    [[nodiscard]] std::vector<step_point> generate_random_step_points(const std::size_t samples) noexcept
    {
        static std::mt19937_64 generator{std::random_device{}()};

        // instantiate distributions
        std::vector<std::uniform_int_distribution<std::size_t>> distributions{};
        distributions.reserve(num_dimensions);

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            distributions.emplace_back(0, indices[d].size() - 1);
        }

        // container for the random samples
        std::set<step_point> step_point_samples{};

        for (std::size_t i = 0; i < samples; ++i)
        {
            std::vector<std::size_t> dimension_samples{};
            dimension_samples.reserve(num_dimensions);

            // sample all dimensions
            for (auto d = 0u; d < num_dimensions; ++d)
            {
                dimension_samples.push_back(distributions[d](generator));
            }

            step_point_samples.insert(step_point{dimension_samples});
        }

        return std::vector<step_point>(step_point_samples.cbegin(), step_point_samples.cend());
    }
    /**
     * Simulates the operational status of the given points in parallel. It divides the work among multiple threads to
     * speed up the computation.
     *
     * @param step_points A vector of step points for which the operational status is to be simulated.
     */
    void simulate_operational_status_in_parallel(const std::vector<step_point>& step_points)
    {
        // calculate the size of each slice
        const auto slice_size = (step_points.size() + num_threads - 1) / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        // launch threads, each with its own slice of random step points
        for (auto i = 0ul; i < num_threads; ++i)
        {
            const auto start = i * slice_size;
            const auto end   = std::min(start + slice_size, step_points.size());

            if (start >= end)
            {
                break;  // no more work to distribute
            }

            threads.emplace_back(
                [this, start, end, &step_points]
                {
                    for (auto it = step_points.cbegin() + static_cast<int64_t>(start);
                         it != step_points.cbegin() + static_cast<int64_t>(end); ++it)
                    {
                        is_step_point_operational(*it);
                    }
                });
        }

        // wait for all threads to complete
        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }
    /**
     * Performs random sampling to find any operational parameter combination. This function is useful if a single
     * starting point is required within the domain to expand from. This function returns the step in all dimensions
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

            // if the parameter combination is operational, return its step values in all dimensions
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
        assert(num_dimensions == 2 && "Contour tracing is only supported for two dimensions");
        assert(starting_point.step_values.size() == 2 && "Given step point must have 2 dimensions");

        auto latest_operational_point = starting_point;

        // move towards the left border of the parameter range
        for (std::size_t x = starting_point.step_values[0]; x > 0; --x)
        {
            const auto y = starting_point.step_values[1];

            const auto left_step = step_point{{x, y}};

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
        assert(num_dimensions == 2 && "Moore neighborhood is only supported for 2 dimensions");
        assert(sp.step_values.size() == 2 && "Given step point must have 2 dimensions");

        std::vector<step_point> neighbors{};
        neighbors.reserve(8);

        const auto emplace = [&neighbors](const auto x, const auto y) noexcept
        { neighbors.emplace_back(std::vector<std::size_t>{x, y}); };

        const auto x = sp.step_values[0];
        const auto y = sp.step_values[1];

        const auto num_x_indices = indices[0].size();
        const auto num_y_indices = indices[1].size();

        const auto decr_x = (x > 0) ? x - 1 : x;
        const auto incr_x = (x + 1 < num_x_indices) ? x + 1 : x;
        const auto decr_y = (y > 0) ? y - 1 : y;
        const auto incr_y = (y + 1 < num_y_indices) ? y + 1 : y;

        // add neighbors in clockwise direction

        // right
        if (x != incr_x)
        {
            emplace(incr_x, y);
        }
        // lower-right
        if (x != incr_x && y != decr_y)
        {
            emplace(incr_x, decr_y);
        }
        // down
        if (y != decr_y)
        {
            emplace(x, decr_y);
        }
        // lower-left
        if (x != decr_x && y != decr_y)
        {
            emplace(decr_x, decr_y);
        }
        // left
        if (x != decr_x)
        {
            emplace(decr_x, y);
        }
        // upper-left
        if (x != decr_x && y != incr_y)
        {
            emplace(decr_x, incr_y);
        }
        // up
        if (y != incr_y)
        {
            emplace(x, incr_y);
        }
        // upper-right
        if (x != incr_x && y != incr_y)
        {
            emplace(incr_x, incr_y);
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

    if (params.sweep_dimensions.size() != 2 && params.sweep_dimensions.size() != 3)
    {
        throw std::runtime_error("Flood fill is only applicable to 2 or 3 dimensions");
    }

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

    if (params.sweep_dimensions.size() != 2)
    {
        throw std::runtime_error("Contour tracing is only applicable to exactly 2 dimensions");
    }

    operational_domain_stats                 st{};
    detail::operational_domain_impl<Lyt, TT> p{lyt, spec, params, st};

    const auto result = p.contour_tracing(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction

namespace std
{
// make `operational_domain::parameter_point` compatible with `std::hash`
template <>
struct hash<fiction::operational_domain::parameter_point>
{
    size_t operator()(const fiction::operational_domain::parameter_point& p) const noexcept
    {
        size_t h = 0;
        for (const auto& d : p.parameters)
        {
            fiction::hash_combine(h, d);
        }

        return h;
    }
};
}  // namespace std

#endif  // FICTION_OPERATIONAL_DOMAIN_HPP
