//
// Created by marcel on 21.07.23.
//

#ifndef FICTION_OPERATIONAL_DOMAIN_HPP
#define FICTION_OPERATIONAL_DOMAIN_HPP

#include "fiction/algorithms/simulation/sidb/critical_temperature.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/hash.hpp"
#include "fiction/utils/math_utils.hpp"
#include "fiction/utils/phmap_utils.hpp"

#include <btree.h>
#include <fmt/format.h>
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
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

namespace fiction
{

/**
 * The parameter point holds parameter values in the x and y dimension.
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
    std::vector<double> parameters{};
    /**
     * Equality operator. Checks if this parameter point is equal to another point within a specified tolerance.
     * The tolerance is defined by `physical_constants::POP_STABILITY_ERR`.
     *
     * @param other Other parameter point to compare with.
     * @return `true` iff the parameter points are equal.
     */
    [[nodiscard]] bool operator==(const parameter_point& other) const noexcept
    {
        // Check if sizes are equal
        if (parameters.size() != other.parameters.size())
        {
            return false;
        }

        // Define tolerance for comparison
        constexpr auto tolerance = physical_constants::POP_STABILITY_ERR;

        // Compare each element with tolerance
        for (std::size_t i = 0; i < parameters.size(); ++i)
        {
            if (std::fabs(parameters[i] - other.parameters[i]) >= tolerance)
            {
                return false;
            }
        }

        return true;
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
namespace detail
{
/**
 * Forward-declaration for `operational_domain`.
 */
template <typename MapType>
std::optional<typename MapType::mapped_type> contains_key(const MapType& map, const typename MapType::key_type& key);

}  // namespace detail
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
 *
 * @tparam Key The type representing the key. Defaults to `parameter_point`.
 * @tparam Value The type representing the value. Defaults to `operational_status`.
 * @tparam MetricType The type representing the metric value. Defaults to `double`.
 */
template <typename Key = parameter_point, typename Value = operational_status, typename MetricType = double>
struct operational_domain
{
    /**
     * The dimensions to sweep over, ordered by priority. The first dimension is the x dimension, the second dimension
     * is the y dimension, etc.
     */
    std::vector<sweep_parameter> dimensions{};
    /**
     * This can store different information depending on the use case. If the operational domain is simulated for
     * different physical parameters, the parameters are stored with the corresponding operating status.
     */
    locked_parallel_flat_hash_map<Key, Value> operational_values{};
    /**
     * This can store different information depending on the use case. If the critical temperature is simulated on top
     * of the operational domain, it is stored here.
     */
    locked_parallel_flat_hash_map<Key, MetricType> metric_values{};
    /**
     * This function retrieves the value associated with the provided key from the operational domain. If
     * the key is found in the domain, its corresponding value is returned. Otherwise, `std::nullopt`
     * is returned.
     *
     * @param key The key to look up.
     * @return The value associated with the provided key. If the key is not found, `std::nullopt` is returned.
     */
    [[nodiscard]] std::optional<Value> get_value(const Key& key) const
    {
        return detail::contains_key(operational_values, key);
    }
    /**
     * This function retrieves the value associated with the provided key from the metric values. If
     * the key is found, its corresponding value is returned. Otherwise, `std::nullopt`
     * is returned.
     *
     * @param key The key to look up in the metric values.
     * @return The value associated with the provided key. If the key is not found, `std::nullopt` is returned.
     */
    [[nodiscard]] std::optional<MetricType> get_metric_value(const Key& key) const
    {
        return detail::contains_key(metric_values, key);
    }
    /**
     * Adds a value to the operational domain.
     *
     * @key The key to add the value to.
     * @value The value to add.
     */
    void add_value(const Key& key, const Value& value)
    {
        operational_values.try_emplace(key, value);
    }
    /**
     * Adds a value to the metric values.
     *
     * @param key The key to add the value to.
     * @param value The value to add.
     */
    void add_metric_value(const Key& key, const MetricType& value)
    {
        metric_values.try_emplace(key, value);
    }
    /**
     * Returns the operational domain.
     *
     * @return The operational domain.
     */
    [[nodiscard]] locked_parallel_flat_hash_map<Key, Value> get_operational_domain() const
    {
        return operational_values;
    }
    /**
     * Returns the metric values. If no metric values are present, `std::nullopt` is returned.
     *
     * @return The metric values.
     */
    [[nodiscard]] locked_parallel_flat_hash_map<Key, Value> get_metric_values() const
    {
        return metric_values;
    }
};
/**
 * A range of values for a dimension sweep. The range is defined by a minimum value, a maximum value and a step size.
 */
struct operational_domain_value_range
{
    /**
     * The sweep parameter of the dimension.
     */
    sweep_parameter dimension;
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
     * This enumeration defines whether specific metrics, such as critical temperature,
     * are simulated within the operational domain.
     */
    enum class metric_simulation : uint8_t
    {
        /**
         * Simulates the critical temperature within the operational domain.
         */
        CRITICAL_TEMPERATURE_SIM,
        /**
         * Disables metric simulation, meaning no metrics are simulated within the operational domain.
         */
        DISABLED
    };
    /**
     * The parameters used to determine if a layout is operational or non-operational.
     */
    is_operational_params operational_params{};
    /**
     * The dimensions to sweep over together with their value ranges, ordered by priority. The first dimension is the x
     * dimension, the second dimension is the y dimension, etc.
     */
    std::vector<operational_domain_value_range> sweep_dimensions{
        operational_domain_value_range{sweep_parameter::EPSILON_R, 1.0, 10.0, 0.1},
        operational_domain_value_range{sweep_parameter::LAMBDA_TF, 1.0, 10.0, 0.1}};
    /**
     * Specifies the metric to simulate within the operational domain.
     */
    metric_simulation metric_sim{metric_simulation::DISABLED};
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
    /**
     * Total number of parameter points in the parameter space.
     */
    std::size_t num_total_parameter_points{0};
};

namespace detail
{

/**
 * This function validates the given sweep parameters for the operational domain computation. It checks if the minimum
 * value of any sweep dimension is larger than the corresponding maximum value. Additionally, it checks if the step size
 * of any sweep dimension is negative or zero.
 *
 * If any of this is the case, an `std::invalid_argument` is thrown.
 *
 * @param params The operational domain parameters to validate.
 */
void validate_sweep_parameters(const operational_domain_params& params)
{
    for (auto d = 0u; d < params.sweep_dimensions.size(); ++d)
    {
        if (params.sweep_dimensions[d].max < params.sweep_dimensions[d].min)
        {
            throw std::invalid_argument(
                fmt::format("Invalid sweep dimension: 'max' value is smaller than 'min' value for "
                            "dimension {}",
                            d));
        }
        if (params.sweep_dimensions[d].step <= 0.0)
        {
            throw std::invalid_argument(
                fmt::format("Invalid sweep dimension: 'step' size is negative or 0 for dimension {}", d));
        }
    }
}
/**
 * This function checks for the containment of a given key in a given map. If the key is found in the map, the
 * associated `MapType::value_type` is returned. Otherwise, `std::nullopt` is returned.
 *
 * @tparam MapType The type of the map.
 * @param map The map in which to search for `key`.
 * @param key The key to search for in `map`.
 * @return The associated `MapType::value_type` of `key` in `map`, or `std::nullopt` if `key` is not contained in `map`.
 */
template <typename MapType>
std::optional<typename MapType::mapped_type> contains_key(const MapType& map, const typename MapType::key_type& key)
{
    std::optional<typename MapType::mapped_type> result;

    map.if_contains(key, [&result](const typename MapType::value_type& entry) { result = entry.second; });

    return result;
}

/**
 * This function searches for a floating-point value specified by the `key` in the provided map `map`, applying a
 * tolerance specified by `fiction::physical_constants::POP_STABILITY_ERR`. Each key in the map is compared to the
 * specified key within this tolerance.
 *
 * @tparam MapType The type of the map containing parameter points as keys.
 * @param map The map containing parameter points as keys and associated values.
 * @param key The parameter point to search for in the map.
 * @return An iterator to the found parameter point in the map, or `map.cend()` if not found.
 */
template <typename MapType>
typename MapType::const_iterator find_key_with_tolerance(const MapType& map, const typename MapType::key_type& key)
{
    static_assert(std::is_floating_point_v<typename MapType::key_type>, "Map key type must be floating-point");

    constexpr double tolerance = physical_constants::POP_STABILITY_ERR;

    auto compare_keys = [&key, &tolerance](const auto& pair) { return std::abs(pair.first - key) < tolerance; };

    return std::find_if(map.cbegin(), map.cend(), compare_keys);
}

template <typename Lyt, typename TT, typename OpDomain>
class operational_domain_impl
{
  public:
    /**
     * Standard constructor. Initializes the lyt, the truth table, the parameters and the statistics. Also
     * detects the output BDL pair, which is necessary for the operational domain computation. The lyt must
     * have exactly one output BDL pair.
     *
     * @param lyt SiDB cell-level lyt to be evaluated.
     * @param tt Expected Boolean function of the lyt given as a multi-output truth table.
     * @param ps Parameters for the operational domain computation.
     * @param st Statistics of the process.
     */
    operational_domain_impl(const Lyt& lyt, const std::vector<TT>& tt, const operational_domain_params& ps,
                            operational_domain_stats& st) noexcept :
            layout{lyt},
            truth_table{tt},
            params{ps},
            stats{st},
            output_bdl_pairs{detect_bdl_pairs<Lyt>(
                layout, sidb_technology::cell_type::OUTPUT,
                ps.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)},
            num_dimensions{params.sweep_dimensions.size()},
            input_bdl_wires{detect_bdl_wires(lyt, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                             bdl_wire_selection::INPUT)},
            output_bdl_wires{detect_bdl_wires(lyt, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                              bdl_wire_selection::OUTPUT)}
    {
        const auto logic_cells = lyt.get_cells_by_type(technology<Lyt>::cell_type::LOGIC);

        assert(((params.operational_params.strategy_to_analyze_operational_status !=
                 is_operational_params::operational_analysis_strategy::FILTER_ONLY) ||
                (logic_cells.size() > 0)) &&
               "No logic cells found in the layout");

        // the canvas layout is created which is defined by the logic cells.
        for (const auto& c : logic_cells)
        {
            canvas_lyt.assign_cell_type(c, technology<Lyt>::cell_type::NORMAL);
        }

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
            // ignored in the operational domain calculation
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
                                    (static_cast<double>(i) * params.sweep_dimensions[d].step));
            }
        }
    }
    /**
     * Additional Constructor. Initializes the layout, the parameters and the statistics.
     *
     * @param lyt SiDB cell-level layout to be evaluated.
     * @param ps Parameters for the operational domain computation.
     * @param st Statistics of the process.
     */
    operational_domain_impl(const Lyt& lyt, const operational_domain_params& ps, operational_domain_stats& st) noexcept
            :
            layout{lyt},
            truth_table{std::vector<TT>{}},
            params{ps},
            stats{st},
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
            // ignored in the operational domain calculation
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
                                    (static_cast<double>(i) * params.sweep_dimensions[d].step));
            }
        }
    }
    /**
     * Performs a grid search over the specified parameter ranges with the specified step sizes. The grid search always
     * has quadratic complexity. The operational status is computed for each parameter combination.
     *
     * @return The operational domain of the layout.
     */
    [[nodiscard]] operational_domain<parameter_point, operational_status> grid_search() noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        const auto all_index_combinations = cartesian_combinations(indices);

        std::vector<step_point> all_step_points{};
        all_step_points.reserve(all_index_combinations.size());

        std::transform(all_index_combinations.cbegin(), all_index_combinations.cend(),
                       std::back_inserter(all_step_points), [](const auto& comb) noexcept { return step_point{comb}; });

        // shuffle the step points to simulate in random order. This helps with load-balancing since
        // operational/non-operational points are usually clustered. However, non-operational points can be simulated
        // faster on average because of the early termination condition. Thus, threads that mainly simulate
        // non-operational points will finish earlier and will be idle while other threads are still simulating the more
        // expensive operational points
        std::shuffle(all_step_points.begin(), all_step_points.end(), std::mt19937_64{std::random_device{}()});

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
    [[nodiscard]] operational_domain<parameter_point, operational_status>
    random_sampling(const std::size_t samples) noexcept
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
     * @param given_parameter_point Optional parameter point in the parameter space. If it lies within the
     * operational region, it is used as a starting point for flood fill.
     * @return The (partial) operational domain of the layout.
     */
    [[nodiscard]] operational_domain<parameter_point, operational_status>
    flood_fill(const std::size_t                     samples,
               const std::optional<parameter_point>& given_parameter_point = std::nullopt) noexcept
    {
        assert((num_dimensions == 2 || num_dimensions == 3) &&
               "Flood fill is only supported for two and three dimensions");

        mockturtle::stopwatch stop{stats.time_total};

        auto step_point_samples = generate_random_step_points(samples);

        if (given_parameter_point.has_value())
        {
            step_point_samples.push_back(to_step_point(given_parameter_point.value()));
        }

        simulate_operational_status_in_parallel(step_point_samples);

        // a queue of (x, y[, z]) dimension step points to be evaluated
        std::queue<step_point> queue{};

        // a utility function that adds the adjacent points to the queue for further evaluation
        const auto queue_next_points = [this, &queue](const step_point& sp)
        {
            if (num_dimensions == 2)
            {
                for (const auto& m : moore_neighborhood_2d(sp))
                {
                    if (!has_already_been_sampled(m))
                    {
                        queue.push(m);
                    }
                }
            }
            else  // num_dimensions == 3
            {
                for (const auto& m : moore_neighborhood_3d(sp))
                {
                    if (!has_already_been_sampled(m))
                    {
                        queue.push(m);
                    }
                }
            }
        };

        // add the neighbors of each operational point to the queue
        for (const auto& [param_point, status] : op_domain.get_operational_domain())
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
    [[nodiscard]] operational_domain<parameter_point, operational_status>
    contour_tracing(const std::size_t samples) noexcept
    {
        assert(num_dimensions == 2 && "Contour tracing is only supported for two dimensions");

        mockturtle::stopwatch stop{stats.time_total};

        const auto step_point_samples = generate_random_step_points(samples);

        simulate_operational_status_in_parallel(step_point_samples);

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

        // for each sampled point
        for (const auto& starting_point : step_point_samples)
        {
            // if the current starting point is non-operational, skip to the next one
            const auto domain_value = op_domain.get_value(to_parameter_point(starting_point));
            if (domain_value.has_value())
            {
                if (domain_value.value() == operational_status::NON_OPERATIONAL)
                {
                    continue;
                }
            }

            // if the current step point has been inferred as operational, skip to the next one
            if (is_step_point_inferred_operational(starting_point))
            {
                continue;
            }

            // find an operational point on the contour starting from the randomly determined starting point
            const auto contour_starting_point = find_operational_contour_step_point(starting_point);

            auto current_contour_point = contour_starting_point;

            const auto x = current_contour_point.step_values[0];
            const auto y = current_contour_point.step_values[1];

            auto backtrack_point = x == 0 ? current_contour_point : step_point{{x - 1, y}};

            auto current_neighborhood = moore_neighborhood_2d(current_contour_point);

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

                current_neighborhood = moore_neighborhood_2d(current_contour_point);
                next_point           = next_clockwise_point(current_neighborhood, backtrack_point);
            }

            infer_operational_status_in_enclosing_contour(starting_point);
        }

        log_stats();

        return op_domain;
    }
    /**
     * Performs a grid search over the specified parameter ranges. For each physical parameter combination found for
     * which the given CDS is physically valid, it is determined whether the CDS is the ground state or the n-th excited
     * state.
     *
     * @param lyt SiDB cell-level layout that is simulated and compared to the given CDS.
     * @return All physically valid physical parameters and the excited state number.
     */
    [[nodiscard]] operational_domain<parameter_point, uint64_t>
    grid_search_for_physically_valid_parameters(Lyt& lyt) noexcept
    {
        operational_domain<parameter_point, uint64_t> suitable_params_domain{};

        mockturtle::stopwatch stop{stats.time_total};

        // Cartesian product of all step point indices
        const auto all_index_combinations = cartesian_combinations(indices);

        // calculate the size of each slice
        const auto slice_size = (all_index_combinations.size() + num_threads - 1) / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        // launch threads, each with its own slice of random step points
        for (auto i = 0ul; i < num_threads; ++i)
        {
            const auto start = i * slice_size;
            const auto end   = std::min(start + slice_size, all_index_combinations.size());

            if (start >= end)
            {
                break;  // no more work to distribute
            }

            threads.emplace_back(
                [this, &lyt, start, end, &all_index_combinations]
                {
                    for (auto it = all_index_combinations.cbegin() + static_cast<int64_t>(start);
                         it != all_index_combinations.cbegin() + static_cast<int64_t>(end); ++it)
                    {
                        is_step_point_suitable(lyt, step_point{*it});  // construct a step_point
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

        sidb_simulation_parameters simulation_parameters = params.operational_params.simulation_parameters;

        for (const auto& [param_point, status] : op_domain.operational_values)
        {
            if constexpr (std::is_same_v<OpDomain, operational_domain<parameter_point, operational_status>>)
            {
                if (status == operational_status::NON_OPERATIONAL)
                {
                    continue;
                }

                for (auto d = 0u; d < num_dimensions; ++d)
                {
                    set_dimension_value(simulation_parameters, param_point.parameters[d], d);
                }

                auto sim_results = sidb_simulation_result<Lyt>{};

                if (params.operational_params.sim_engine == sidb_simulation_engine::QUICKEXACT)
                {
                    // perform an exact ground state simulation
                    sim_results =
                        quickexact(lyt, quickexact_params<cell<Lyt>>{
                                            simulation_parameters,
                                            quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF});
                }
                else if (params.operational_params.sim_engine == sidb_simulation_engine::EXGS)
                {
                    // perform an exhaustive ground state simulation
                    sim_results = exhaustive_ground_state_simulation(lyt, simulation_parameters);
                }
                else if (params.operational_params.sim_engine == sidb_simulation_engine::QUICKSIM)
                {
                    // perform a heuristic simulation
                    const quicksim_params qs_params{simulation_parameters, 500, 0.6};
                    sim_results = quicksim(lyt, qs_params);
                }
                else
                {
                    assert(false && "unsupported simulation engine");
                }

                const auto energy_dist = energy_distribution(sim_results.charge_distributions);

                lyt.assign_physical_parameters(simulation_parameters);
                const auto position = find_key_with_tolerance(energy_dist, lyt.get_system_energy());

                if (position == energy_dist.cend())
                {
                    continue;
                }

                const auto excited_state_number = std::distance(energy_dist.cbegin(), position);
                suitable_params_domain.operational_values.emplace(param_point, excited_state_number);
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
     * The logical specification of the layout.
     */
    const std::vector<TT> truth_table;
    /**
     * The parameters for the operational domain computation.
     */
    operational_domain_params params;
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
     * This layout consists of the canvas cells of the layout.
     */
    Lyt canvas_lyt{};
    /**
     * The operational domain of the layout.
     */
    OpDomain op_domain{};
    /**
     * Forward-declare step_point.
     */
    struct step_point;
    /**
     * All the points inferred (assumed) to be operational but not actually simulated.
     */
    phmap::btree_set<step_point> inferred_op_domain;
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
     * Input BDL wires.
     */
    const std::vector<bdl_wire<Lyt>> input_bdl_wires;
    /**
     * Output BDL wires.
     */
    const std::vector<bdl_wire<Lyt>> output_bdl_wires;
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
    [[nodiscard]] parameter_point to_parameter_point(const step_point& sp) const noexcept
    {
        std::vector<double> parameter_values{};
        for (auto d = 0u; d < num_dimensions; ++d)
        {
            parameter_values.push_back(values[d][sp.step_values[d]]);
        }

        return parameter_point{parameter_values};
    }
    /**
     * Converts a parameter point to a step point.
     *
     * @param pp Parameter point to convert.
     * @return The step point corresponding to the parameter point `pp`.
     */
    [[nodiscard]] step_point to_step_point(const parameter_point& pp) const noexcept
    {
        std::vector<std::size_t> step_values{};
        step_values.reserve(num_dimensions);

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            // Ensure the parameter is within the valid range
            [[maybe_unused]] const auto min_val = values[d].front();
            [[maybe_unused]] const auto max_val = values[d].back();

            assert(pp.parameters[d] >= min_val && pp.parameters[d] <= max_val &&
                   "Parameter point is outside of the value range");

            const auto it = std::lower_bound(values[d].cbegin(), values[d].cend(), pp.parameters[d]);

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
        const sweep_parameter sweep_parameter = params.sweep_dimensions[dim].dimension;

        switch (sweep_parameter)
        {
            case sweep_parameter::EPSILON_R:
            {
                sim_parameters.epsilon_r = val;
                break;
            }
            case sweep_parameter::LAMBDA_TF:
            {
                sim_parameters.lambda_tf = val;
                break;
            }
            case sweep_parameter::MU_MINUS:
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
        if (const auto v = contains_key(op_domain.operational_values, to_parameter_point(sp)); v.has_value())
        {
            return v.value();
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
        if (const auto op_value = has_already_been_sampled(sp); op_value.has_value())
        {
            return *op_value;
        }

        const auto param_point = to_parameter_point(sp);

        const auto operational = [this, &param_point](const std::optional<double>& ct_value = std::nullopt) noexcept
        {
            op_domain.operational_values.try_emplace(param_point, operational_status::OPERATIONAL);

            if (ct_value.has_value())
            {
                op_domain.metric_values.try_emplace(param_point, ct_value.value());
            }

            return operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, &param_point]() noexcept
        {
            op_domain.operational_values.try_emplace(param_point, operational_status::NON_OPERATIONAL);

            if (params.metric_sim == operational_domain_params::metric_simulation::CRITICAL_TEMPERATURE_SIM)
            {
                op_domain.metric_values.try_emplace(param_point, 0.0);
            }
            return operational_status::NON_OPERATIONAL;
        };

        ++num_evaluated_parameter_combinations;

        sidb_simulation_parameters sim_params = params.operational_params.simulation_parameters;

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            set_dimension_value(sim_params, values[d][sp.step_values[d]], d);
        }

        auto op_params_set_dimension_values                  = params.operational_params;
        op_params_set_dimension_values.simulation_parameters = sim_params;

        const auto& [status, sim_calls] = is_operational(layout, truth_table, op_params_set_dimension_values,
                                                         input_bdl_wires, output_bdl_wires, std::optional{canvas_lyt});

        num_simulator_invocations += sim_calls;

        if (status == operational_status::NON_OPERATIONAL)
        {
            return non_operational();
        }

        if (params.metric_sim == operational_domain_params::metric_simulation::CRITICAL_TEMPERATURE_SIM)
        {
            const auto ct = critical_temperature_gate_based(
                layout, truth_table, critical_temperature_params{op_params_set_dimension_values});

            return operational(ct);
        }

        return operational();
    }
    /**
     * This function checks if the given charge distribution surface (CDS) is physically valid for the parameter point
     * represented by the step point `sp`.
     *
     * @param lyt CDS to check.
     * @param sp Step point to be investigated.
     * @return The operational status of the layout under the given simulation parameters.
     */
    operational_status is_step_point_suitable(Lyt lyt, const step_point& sp) noexcept
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
            op_domain.add_value(param_point, operational_status::OPERATIONAL);

            return operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, &param_point]()
        {
            op_domain.add_value(param_point, operational_status::NON_OPERATIONAL);

            return operational_status::NON_OPERATIONAL;
        };

        // increment the number of evaluated parameter combinations
        ++num_evaluated_parameter_combinations;

        sidb_simulation_parameters sim_params = params.operational_params.simulation_parameters;

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            set_dimension_value(sim_params, param_point.parameters[d], d);
        }

        lyt.assign_physical_parameters(sim_params);

        if (lyt.is_physically_valid())
        {
            return operational();
        }

        // if we made it here, the layout is non-operational
        return non_operational();
    }
    /**
     * Checks whether the given step point is part of the inferred operational domain. If it is, the point is marked as
     * enclosed in the operational domain. No simulation is performed on `sp`. If `sp` is not contained in the inferred
     * operational domain, it does not mean that `sp` is definitely non-operational. It could still appear in the
     * regular operational domain with either status.
     *
     * This function is used by the contour tracing algorithm.
     *
     * @param sp Step point to check for inferred operational status.
     * @return `true` iff `sp` is contained in `inferred_op_domain`.
     */
    [[nodiscard]] inline bool is_step_point_inferred_operational(const step_point& sp) const noexcept
    {
        return inferred_op_domain.count(sp) > 0;
    }
    /**
     * Generates unique random `step_points` in the stored parameter range. The number of generated points is at most
     * equal to `samples`.
     *
     * @param samples Maximum number of random `step_point`s to generate.
     * @return A vector of unique random `step_point`s in the stored parameter range of size at most equal to `samples`.
     */
    [[nodiscard]] std::vector<step_point> generate_random_step_points(const std::size_t samples) const noexcept
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
        phmap::btree_set<step_point> step_point_samples{};

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
     * @note The distribution of the work among threads is a simple slice-based approach. If your step points are
     * ordered, consider shuffling the vector first for better load balancing. Otherwise, some threads might finish
     * early if they got assigned a slice with mainly non-operational samples, which are faster to compute due to the
     * early termination condition.
     *
     * @param step_points A vector of step points for which the operational status is to be simulated.
     */
    void simulate_operational_status_in_parallel(const std::vector<step_point>& step_points) noexcept
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
    [[maybe_unused]] [[nodiscard]] std::optional<step_point>
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

        // if no boundary point was found, the operational area extends outside the parameter range;
        // return the latest operational point
        return latest_operational_point;
    }
    /**
     * Returns the 2D Moore neighborhood of the step point at `sp = (x, y)`. The 2D Moore neighborhood is the set of all
     * points that are adjacent to `(x, y)` in the plane including the diagonals. Thereby, the 2D Moore neighborhood
     * contains up to 8 points as points outside of the parameter range are not gathered. The points are returned in
     * clockwise order starting from the right neighbor.
     *
     * @param sp Step point to get the 2D Moore neighborhood of.
     * @return The 2D Moore neighborhood of the step point at `sp = (x, y)`.
     */
    [[nodiscard]] std::vector<step_point> moore_neighborhood_2d(const step_point& sp) const noexcept
    {
        assert(num_dimensions == 2 && "2D Moore neighborhood is only supported for 2 dimensions");
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
     * Returns the 3D Moore neighborhood of the step point at `sp = (x, y, z)`. The 3D Moore neighborhood is the set of
     * all points that are adjacent to `(x, y, z)` in the 3D space including the diagonals. Thereby, the 3D Moore
     * neighborhood contains up to 26 points as points outside of the parameter range are not gathered. The points are
     * returned in no particular order.
     *
     * @param sp Step point to get the 3D Moore neighborhood of.
     * @return The 3D Moore neighborhood of the step point at `sp = (x, y, z)`.
     */
    [[nodiscard]] std::vector<step_point> moore_neighborhood_3d(const step_point& sp) const noexcept
    {
        assert(num_dimensions == 3 && "3D Moore neighborhood is only supported for 3 dimensions");
        assert(sp.step_values.size() == 3 && "Given step point must have 3 dimensions");

        std::vector<step_point> neighbors{};
        neighbors.reserve(26);

        const auto emplace = [&neighbors](const auto x, const auto y, const auto z) noexcept
        { neighbors.emplace_back(std::vector<std::size_t>{x, y, z}); };

        const auto x = sp.step_values[0];
        const auto y = sp.step_values[1];
        const auto z = sp.step_values[2];

        const auto num_x_indices = indices[0].size();
        const auto num_y_indices = indices[1].size();
        const auto num_z_indices = indices[2].size();

        // add neighbors in no particular order

        // iterate over all combinations of (-1, 0, 1) offsets for x, y, and z
        for (const int64_t x_offset : {-1, 0, 1})
        {
            for (const int64_t y_offset : {-1, 0, 1})
            {
                for (const int64_t z_offset : {-1, 0, 1})
                {
                    // skip the center cell
                    if (x_offset == 0 && y_offset == 0 && z_offset == 0)
                    {
                        continue;
                    }

                    // calculate new coordinate
                    const int64_t dx = static_cast<int64_t>(x) + x_offset;
                    const int64_t dy = static_cast<int64_t>(y) + y_offset;
                    const int64_t dz = static_cast<int64_t>(z) + z_offset;

                    // check if the new coordinate is within the bounds
                    if ((dx >= 0 && dx < static_cast<int64_t>(num_x_indices)) &&
                        (dy >= 0 && dy < static_cast<int64_t>(num_y_indices)) &&
                        (dz >= 0 && dz < static_cast<int64_t>(num_z_indices)))
                    {
                        emplace(static_cast<uint64_t>(dx), static_cast<uint64_t>(dy), static_cast<uint64_t>(dz));
                    }
                }
            }
        }

        return neighbors;
    }
    /**
     * Given a starting point, this function marks all points that are enclosed by the operational domain contour as
     * 'inferred operational'. This assumes that the operational domain does not have holes. To the best of the author's
     * knowledge, at the time of writing this code, there exists no proof that operational domains are always
     * continuous, i.e., hole-free. Marking points as 'inferred operational' can be useful to avoid recomputation in,
     * e.g., contour tracing if an operational domain with multiple islands is investigated.
     *
     * The function starts at the given starting point and performs flood fill to mark all points that are reachable
     * from the starting point until it encounters the non-operational edges.
     *
     * Note that no physical simulation is conducted by this function!
     *
     * @param starting_point Step point at which to start the inference. If `starting_point` is non-operational, this
     * function might invoke undefined behavior.
     */
    void infer_operational_status_in_enclosing_contour(const step_point& starting_point) noexcept
    {
        assert(num_dimensions == 2 && "This function is only supported for two dimensions");
        assert(is_step_point_operational(starting_point) == operational_status::OPERATIONAL &&
               "starting_point must be within the operational domain");

        // a queue of (x, y) dimension step points to be marked as inferred operational
        std::queue<step_point> queue{};

        // a utility function that adds the adjacent points to the queue for further evaluation
        const auto queue_next_points = [this, &queue](const step_point& sp) noexcept
        {
            for (const auto& m : moore_neighborhood_2d(sp))
            {
                // if the point has already been inferred as operational, continue with the next
                if (is_step_point_inferred_operational(m))
                {
                    continue;
                }

                // if the point has already been sampled
                if (const auto operational_status = has_already_been_sampled(m); operational_status.has_value())
                {
                    // and found to be non-operational, continue with the next
                    if (operational_status.value() == operational_status::NON_OPERATIONAL)
                    {
                        continue;
                    }
                }

                // otherwise, it is either found operational or can be inferred as such
                queue.push(m);
                inferred_op_domain.insert(m);
            }
        };

        // if the starting point has not already been inferred as operational
        if (is_step_point_inferred_operational(starting_point))
        {
            // mark the starting point as inferred operational
            inferred_op_domain.insert(starting_point);

            // add the starting point's neighbors to the queue
            queue_next_points(starting_point);
        }

        // for each point in the queue
        while (!queue.empty())
        {
            // fetch the step point and remove it from the queue
            const auto sp = queue.front();
            queue.pop();

            // if the point is known to be non-operational, continue with the next
            if (const auto operational_status = has_already_been_sampled(sp); operational_status.has_value())
            {
                if (operational_status.value() == operational_status::NON_OPERATIONAL)
                {
                    continue;
                }
            }

            // otherwise (operational or unknown), queue its neighbors
            queue_next_points(sp);
        }
    }
    /**
     * Helper function that writes the the statistics of the operational domain computation to the statistics object.
     * Due to data races that can occur during the computation, each value is temporarily held in an atomic variable and
     * written to the statistics object only after the computation has finished.
     */
    void log_stats() const noexcept
    {
        stats.num_simulator_invocations            = num_simulator_invocations.load();
        stats.num_evaluated_parameter_combinations = num_evaluated_parameter_combinations.load();

        for (const auto& [param_point, status] : op_domain.get_operational_domain())
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

        stats.num_total_parameter_points =
            std::accumulate(values.cbegin(), values.cend(), static_cast<std::size_t>(1),
                            [](std::size_t product, const auto& val) { return product * val.size(); });
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
 * This function may throw an `std::invalid_argument` exception if the given sweep parameters are invalid.
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
operational_domain<parameter_point, operational_status>
operational_domain_grid_search(const Lyt& lyt, const std::vector<TT>& spec,
                               const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    // this may throw an `std::invalid_argument` exception
    detail::validate_sweep_parameters(params);

    operational_domain_stats                                                                          st{};
    detail::operational_domain_impl<Lyt, TT, operational_domain<parameter_point, operational_status>> p{lyt, spec,
                                                                                                        params, st};

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
 * This function may throw an `std::invalid_argument` exception if the given sweep parameters are invalid.
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
operational_domain<parameter_point, operational_status>
operational_domain_random_sampling(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                                   const operational_domain_params& params = {},
                                   operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    // this may throw an `std::invalid_argument` exception
    detail::validate_sweep_parameters(params);

    operational_domain_stats                                                                          st{};
    detail::operational_domain_impl<Lyt, TT, operational_domain<parameter_point, operational_status>> p{lyt, spec,
                                                                                                        params, st};

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
 * it employs the "flood fill" algorithm to explore the operational domain. The algorithm is guaranteed to determine all
 * operational "islands" in their entirety if the initial random sampling found at least one operational point within
 * them. Thereby, this algorithm works for disconnected operational domains.
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
 * This function may throw an `std::invalid_argument` exception if the given sweep parameters are invalid.
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
operational_domain<parameter_point, operational_status>
operational_domain_flood_fill(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                              const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (params.sweep_dimensions.size() != 2 && params.sweep_dimensions.size() != 3)
    {
        throw std::invalid_argument("Flood fill is only applicable to 2 or 3 dimensions");
    }

    // this may throw an `std::invalid_argument` exception
    detail::validate_sweep_parameters(params);

    operational_domain_stats                                                                          st{};
    detail::operational_domain_impl<Lyt, TT, operational_domain<parameter_point, operational_status>> p{lyt, spec,
                                                                                                        params, st};

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
 * This algorithm first uses random sampling to find a set of operational point within the parameter range. From there,
 * it traverses outwards to find the edge of the operational area and performs Moore neighborhood contour tracing to
 * explore the contour of the operational domain. This is repeated for all initially sampled points that do not lie
 * within a contour. The algorithm is guaranteed to determine the contours of all operational "islands" if the initial
 * random sampling found at least one operational point within them. Thereby, this algorithm works for disconnected
 * operational domains.
 *
 * It performs `samples` uniformly-distributed random samples within the parameter range. For each thusly discovered
 * operational island, it performs another number of samples equal to the distance to an edge of each operational
 * area. Finally, it performs up to 8 samples for each contour point (however, the actual number is usually lower).
 * For each sample, the algorithm performs one operational check on the layout, where each operational check consists of
 * up to \f$2^n\f$ exact ground state simulations, where \f$n\f$ is the number of inputs of the layout. Each exact
 * ground state simulation has exponential complexity in of itself. Therefore, the algorithm is only feasible for small
 * layouts with few inputs.
 *
 * This flavor of operational domain computation was proposed in \"Reducing the Complexity of Operational Domain
 * Computation in Silicon Dangling Bond Logic\" by M. Walter, J. Drewniok, S. S. H. Ng, K. Walus, and R. Wille in
 * NANOARCH 2023.
 *
 * This function may throw an `std::invalid_argument` exception if the given sweep parameters are invalid.
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
operational_domain<parameter_point, operational_status>
operational_domain_contour_tracing(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                                   const operational_domain_params& params = {},
                                   operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (params.sweep_dimensions.size() != 2)
    {
        throw std::invalid_argument("Contour tracing is only applicable to exactly 2 dimensions");
    }

    // this may throw an `std::invalid_argument` exception
    detail::validate_sweep_parameters(params);

    operational_domain_stats                                                                          st{};
    detail::operational_domain_impl<Lyt, TT, operational_domain<parameter_point, operational_status>> p{lyt, spec,
                                                                                                        params, st};
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

// make `operational_domain::parameter_point` compatible with `std::integral_constant`
template <>
struct tuple_size<fiction::parameter_point> : std::integral_constant<size_t, 2>
{};

// make `operational_domain::parameter_point` compatible with `std::tuple_element`
template <size_t I>
struct tuple_element<I, fiction::parameter_point>
{
    using type = double;
};

// make `operational_domain::parameter_point` compatible with `std::hash`
template <>
struct hash<fiction::parameter_point>
{
    // tolerance for double hashing
    static constexpr auto tolerance = fiction::physical_constants::POP_STABILITY_ERR;

    size_t operator()(const fiction::parameter_point& p) const noexcept
    {
        size_t h = 0;
        for (const auto& d : p.parameters)
        {
            // hash the double values with tolerance
            fiction::hash_combine(h, static_cast<size_t>(d / tolerance));
        }

        return h;
    }
};

}  // namespace std

#endif  // FICTION_OPERATIONAL_DOMAIN_HPP
