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
#include "fiction/algorithms/simulation/sidb/sidb_simulation_domain.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/constants.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/hash.hpp"
#include "fiction/utils/math_utils.hpp"

#include <btree.h>
#include <fmt/format.h>
#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <functional>
#include <iterator>
#include <limits>
#include <mutex>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * The parameter point holds one parameter value per sweep dimension.
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
     * Equality operator. Checks if this parameter point is equal to another point within a specified tolerance.
     * The tolerance is defined by `constants::ERROR_MARGIN`.
     *
     * @param other Other parameter point to compare with.
     * @return `true` iff the parameter points are equal.
     */
    [[nodiscard]] bool operator==(const parameter_point& other) const noexcept
    {
        return std::ranges::equal(parameters, other.parameters, [](const auto lhs, const auto rhs) noexcept
                                  { return quantize(lhs) == quantize(rhs); });
    }
    /**
     * Maps a parameter value onto the grid of `constants::ERROR_MARGIN`-wide cells that both this type's equality and
     * its `std::hash` specialization are defined on.
     *
     * Comparing parameter values with a tolerance, as in `std::fabs(lhs - rhs) < constants::ERROR_MARGIN`, does not
     * yield an equivalence relation: it is not transitive, and two values that compare equal can still fall on
     * opposite sides of a cell boundary and therefore hash differently. That breaks the invariant every hash-based
     * container relies on, namely that equal keys hash equally. Deciding both on the same quantized value restores it.
     *
     * @param value Parameter value to quantize.
     * @return The index of the cell `value` falls into.
     */
    [[nodiscard]] static int64_t quantize(const double value) noexcept
    {
        return static_cast<int64_t>(std::llround(value / constants::ERROR_MARGIN));
    }
    /**
     * Support for structured bindings.
     *
     * @tparam I Index of the parameter value to be returned.
     * @return The parameter value at the specified index.
     * @throws std::out_of_range if the index is out of bounds.
     */
    template <std::size_t I>
    [[nodiscard]] auto get() const
    {
        return parameters.at(I);
    }
    /**
     * Returns the parameter values for each dimension.
     *
     * @return The parameter values for each dimension.
     */
    [[nodiscard]] const std::vector<double>& get_parameters() const noexcept
    {
        return parameters;
    }

  private:
    /**
     * Parameter values for each dimension.
     */
    std::vector<double> parameters;
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
class operational_domain : public sidb_simulation_domain<parameter_point, operational_status>
{
  public:
    /**
     * Default constructor.
     */
    operational_domain() = default;
    /**
     * Standard constructor.
     *
     * @param dims Dimensions.
     */
    explicit operational_domain(const std::vector<sweep_parameter>& dims) : dimensions(dims) {}
    /**
     * Adds a dimension to sweep over. The first dimension is the x dimension, the second dimension is the y dimension,
     * etc.
     *
     * @param dim The dimension to add.
     */
    void add_dimension(const sweep_parameter& dim)
    {
        dimensions.push_back(dim);
    }
    /**
     * Returns a specific dimension by index.
     *
     * @param index The index of the dimension to return.
     * @return The dimension at the specified index.
     * @throws std::out_of_range if the index is out of range.
     */
    [[nodiscard]] const sweep_parameter& get_dimension(const std::size_t index) const
    {
        return dimensions.at(index);
    }
    /**
     * Returns the number of dimensions to sweep over.
     *
     * @return The number of dimensions to sweep over.
     */
    [[nodiscard]]
    std::size_t get_number_of_dimensions() const noexcept
    {
        return dimensions.size();
    }

  private:
    /**
     * The dimensions to sweep over. The first dimension is the x dimension, the second dimension is the y dimension,
     * etc.
     */
    std::vector<sweep_parameter> dimensions;
};
/**
 * The `critical_temperature_domain` class collects the critical temperature and the operational status for a range of
 * different physical parameters of a given SiDB layout. It allows for the evaluation of how the critical temperature
 * depends on variations in the underlying parameter points. This enables simulations to explore the critical
 * temperature's behavior across different conditions and configurations.
 *
 * It was proposed in \"The Operational Domain Explorer: A Comprehensive Framework to Unveil the Thermal Landscape of
 * Silicon Dangling Bond Logic Beyond Conventional Operability\" by M. Walter, J. Drewniok, and R. Wille in IEEE-NANO
 * 2025 (https://ieeexplore.ieee.org/abstract/document/11113672).
 */
class critical_temperature_domain : public sidb_simulation_domain<parameter_point, operational_status, double>
{
  public:
    /**
     * Default constructor.
     */
    critical_temperature_domain() = default;
    /**
     * Standard constructor.
     *
     * @param dims Dimensions.
     */
    explicit critical_temperature_domain(const std::vector<sweep_parameter>& dims) : dimensions(dims) {}
    /**
     * Adds a dimension to sweep over. The first dimension is the x dimension, the second dimension is the y dimension,
     * etc.
     *
     * @param param The dimension to add.
     */
    void add_dimension(const sweep_parameter& param)
    {
        dimensions.push_back(param);
    }
    /**
     * Returns a specific dimension by index.
     *
     * @param index The index of the dimension to return.
     * @return The dimension at the specified index.
     * @throws std::out_of_range if the index is out of range.
     */
    [[nodiscard]] const sweep_parameter& get_dimension(const std::size_t index) const
    {
        return dimensions.at(index);
    }
    /**
     * Returns the number of dimensions to sweep over.
     *
     * @return The number of dimensions to sweep over.
     */
    [[nodiscard]]
    std::size_t get_number_of_dimensions() const noexcept
    {
        return dimensions.size();
    }
    /**
     * Finds the minimum critical temperature in the domain.
     *
     * @return The minimum critical temperature.
     */
    [[nodiscard]] double minimum_ct() const noexcept
    {
        double min_ct = std::numeric_limits<double>::infinity();

        this->for_each(
            [&min_ct](const auto&, const auto& op_value)
            {
                if (std::get<0>(op_value) == operational_status::OPERATIONAL)
                {
                    min_ct = std::min(min_ct, std::get<1>(op_value));
                }
            });

        return min_ct;
    }
    /**
     * Finds the maximum critical temperature in the domain.
     *
     * @return The maximum critical temperature.
     */
    [[nodiscard]] double maximum_ct() const noexcept
    {
        double max_ct = 0.0;

        this->for_each(
            [&max_ct](const auto&, const auto& op_value)
            {
                if (std::get<0>(op_value) == operational_status::OPERATIONAL)
                {
                    max_ct = std::max(max_ct, std::get<1>(op_value));
                }
            });

        return max_ct;
    }

  private:
    /**
     * The dimensions to sweep over, ordered by priority. The first dimension is the x dimension, the second dimension
     * is the y dimension, etc.
     */
    std::vector<sweep_parameter> dimensions;
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
     * The parameters used to determine if a layout is operational or non-operational.
     */
    is_operational_params operational_params{};
    /**
     * The dimensions to sweep over together with their value ranges, ordered by priority. The first dimension is the x
     * dimension, the second dimension is the y dimension, etc.
     */
    std::vector<operational_domain_value_range> sweep_dimensions{
        operational_domain_value_range{.dimension = sweep_parameter::EPSILON_R, .min = 1.0, .max = 10.0, .step = 0.1},
        operational_domain_value_range{.dimension = sweep_parameter::LAMBDA_TF, .min = 1.0, .max = 10.0, .step = 0.1}};
    /**
     * Number of worker threads to distribute the parameter points over. Defaults to the number of hardware threads,
     * which is the behavior this setting replaces. Values below `1` are treated as `1`.
     *
     * Pinning it makes wall-clock comparisons reproducible across runs and machines, and allows an operational domain
     * computation to leave cores free for other work.
     */
    std::size_t number_of_threads{std::thread::hardware_concurrency()};
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
 * This function validates the given parameters for the operational domain computation. It checks if the minimum
 * value of any sweep dimension is larger than the corresponding maximum value, and if the step size of any sweep
 * dimension is negative or zero. Additionally, it checks the preconditions of the operational domain sketch.
 *
 * The sketch, i.e., `operational_analysis_strategy::FILTER_ONLY`, determines the operational status by filtering
 * alone. It has two preconditions: the filtering steps are only defined when kinks are rejected, and they enumerate
 * the charge configurations of the canvas, which the layout's `LOGIC` cells define. If either is unmet, the sketch
 * evaluates nothing and silently falls back to a full simulation of the entire parameter space. Since that is the
 * exhaustive cost the sketch exists to avoid, an unmet precondition is rejected instead of being absorbed.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout the operational domain is computed for.
 * @param params The operational domain parameters to validate.
 * @throws std::invalid_argument if the parameters are invalid.
 */
template <typename Lyt>
void validate_operational_domain_params(const Lyt& lyt, const operational_domain_params& params)
{
    if (params.operational_params.strategy_to_analyze_operational_status ==
        is_operational_params::operational_analysis_strategy::FILTER_ONLY)
    {
        if (params.operational_params.op_condition != is_operational_params::operational_condition::REJECT_KINKS)
        {
            throw std::invalid_argument("The operational domain sketch requires that kinks are rejected: the "
                                        "filtering steps are only defined for 'REJECT_KINKS'");
        }
        if (lyt.num_cells_of_given_type(technology<Lyt>::cell_type::LOGIC) == 0)
        {
            throw std::invalid_argument("The operational domain sketch requires a canvas: the layout has no 'LOGIC' "
                                        "cells for the filtering steps to enumerate");
        }
    }

    for (auto d = 0u; d < params.sweep_dimensions.size(); ++d)
    {
        if (params.sweep_dimensions.at(d).max < params.sweep_dimensions.at(d).min)
        {
            throw std::invalid_argument(
                fmt::format("Invalid sweep dimension: 'max' value is smaller than 'min' value for "
                            "dimension {}",
                            d));
        }
        if (params.sweep_dimensions.at(d).step <= 0.0)
        {
            throw std::invalid_argument(
                fmt::format("Invalid sweep dimension: 'step' size is negative or 0 for dimension {}", d));
        }
    }
}

template <typename Lyt, typename TT, typename OpDomain = operational_domain>
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
                                              bdl_wire_selection::OUTPUT)},
            input_pattern_layouts{generate_bdl_input_pattern_layouts(
                lyt, params.operational_params.input_bdl_iterator_params, input_bdl_wires)}
    {
        // the public entry points reject a `FILTER_ONLY` request on a layout without `LOGIC` cells, so this may only
        // be empty for the strategies that do not need a canvas
        const auto logic_cells = lyt.get_cells_by_type(technology<Lyt>::cell_type::LOGIC);

        // the canvas layout is created which is defined by the logic cells. The cell type matches the one the
        // `is_operational` entry points assign to the canvases they build themselves; the canvas is only ever used to
        // construct a `charge_distribution_surface`, which reads positions and charges, so the two behave identically
        for (const auto& c : logic_cells)
        {
            canvas_lyt.assign_cell_type(c, technology<Lyt>::cell_type::LOGIC);
        }

        indices.reserve(num_dimensions);
        values.reserve(num_dimensions);

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            op_domain.add_dimension(params.sweep_dimensions.at(d).dimension);

            // generate the step points for the dimension
            indices.push_back(std::vector<std::size_t>(num_steps(d) + 1));
            std::iota(indices.at(d).begin(), indices.at(d).end(), 0ul);

            // if the value of the parameter is greater than params.max after num_x_steps() steps, this value is
            // ignored in the operational domain calculation
            if ((params.sweep_dimensions.at(d).min +
                 (static_cast<double>(indices.at(d).size() - 1) * params.sweep_dimensions.at(d).step)) -
                    params.sweep_dimensions.at(d).max >
                constants::ERROR_MARGIN)
            {
                indices.at(d).pop_back();
            }

            values.emplace_back();

            // generate the values for the dimension
            for (const auto i : indices.at(d))
            {
                values.at(d).push_back(params.sweep_dimensions.at(d).min +
                                       (static_cast<double>(i) * params.sweep_dimensions.at(d).step));
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
    // NOLINTNEXTLINE(modernize-pass-by-value)
    operational_domain_impl(const Lyt& lyt, const operational_domain_params& ps, operational_domain_stats& st) noexcept
            :
            layout{lyt},
            truth_table{std::vector<TT>{}},
            params{ps},
            stats{st},
            num_dimensions{params.sweep_dimensions.size()}
    {

        indices.reserve(num_dimensions);
        values.reserve(num_dimensions);

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            op_domain.add_dimension(params.sweep_dimensions.at(d).dimension);

            // generate the step points for the dimension
            indices.push_back(std::vector<std::size_t>(num_steps(d) + 1));
            std::iota(indices.at(d).begin(), indices.at(d).end(), 0ul);

            // if the value of the parameter is greater than params.max after num_x_steps() steps, this value is
            // ignored in the operational domain calculation
            if ((params.sweep_dimensions.at(d).min +
                 (static_cast<double>(indices.at(d).size() - 1) * params.sweep_dimensions.at(d).step)) -
                    params.sweep_dimensions.at(d).max >
                constants::ERROR_MARGIN)
            {
                indices.at(d).pop_back();
            }

            values.emplace_back();

            // generate the values for the dimension
            for (const auto i : indices.at(d))
            {
                values.at(d).push_back(params.sweep_dimensions.at(d).min +
                                       (static_cast<double>(i) * params.sweep_dimensions.at(d).step));
            }
        }
    }
    /**
     * Performs a grid search over the specified parameter ranges with the specified step sizes. The grid search
     * evaluates the product of the step counts of all sweep dimensions. The operational status is computed for each
     * parameter combination.
     *
     * @return The operational domain of the layout.
     */
    [[nodiscard]] OpDomain grid_search() noexcept
    {
        const mockturtle::stopwatch stop{stats.time_total};

        const auto all_index_combinations = cartesian_combinations(indices);

        std::vector<step_point> all_step_points{};
        all_step_points.reserve(all_index_combinations.size());

        std::ranges::transform(all_index_combinations, std::back_inserter(all_step_points),
                               [](const auto& comb) noexcept { return step_point{comb}; });

        // shuffle the step points to simulate in random order. This helps with load-balancing since
        // operational/non-operational points are usually clustered. However, non-operational points can be simulated
        // faster on average because of the early termination condition. Thus, threads that mainly simulate
        // non-operational points will finish earlier and will be idle while other threads are still simulating the more
        // expensive operational points
        std::ranges::shuffle(all_step_points, std::mt19937_64{std::random_device{}()});

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
    [[nodiscard]] OpDomain random_sampling(const std::size_t samples) noexcept
    {
        const mockturtle::stopwatch stop{stats.time_total};

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
     * Both phases are parallelized. The flood fill itself uses a pool of worker threads that share a single work
     * queue, since the points to explore are only discovered as the exploration proceeds. Each step point is scheduled
     * at most once, so no parameter point is simulated twice. The result does not depend on the order in which the
     * points are explored and is, therefore, independent of the thread scheduling.
     *
     * @param samples Maximum number of random samples to be taken before flood fill.
     * @param given_parameter_point Optional parameter point in the parameter space. If it lies within the
     * operational region, it is used as a starting point for flood fill.
     * @return The (partial) operational domain of the layout.
     */
    // NOLINTBEGIN(bugprone-exception-escape): only allocation can throw, which is fatal to the algorithm anyway
    [[nodiscard]] OpDomain
    flood_fill(const std::size_t                     samples,
               const std::optional<parameter_point>& given_parameter_point = std::nullopt) noexcept
    {
        assert(num_dimensions >= 2 && "Flood fill is only supported for two or more dimensions");

        const mockturtle::stopwatch stop{stats.time_total};

        auto step_point_samples = generate_random_step_points(samples);

        if (given_parameter_point.has_value())
        {
            step_point_samples.push_back(to_step_point(given_parameter_point.value()));
        }

        // the samples are generated in lexicographic order, which distributes poorly over the slice-based thread
        // assignment; shuffle them for better load balancing. The order is irrelevant for the flood fill itself
        std::ranges::shuffle(step_point_samples, std::mt19937_64{std::random_device{}()});

        simulate_operational_status_in_parallel(step_point_samples);

        // a queue of (x, y[, z]) dimension step points to be evaluated, and the set of step points that have already
        // been scheduled for evaluation. Both are guarded by `queue_mutex` together with `active_workers` below
        std::deque<step_point>       queue{};
        phmap::btree_set<step_point> scheduled{};

        // this termination logic is hand-rolled rather than built on `std::jthread` with
        // `std::stop_source`/`std::stop_token` and the C++20 `std::condition_variable_any::wait` stop-token overload.
        // Apple's libc++ still gates `<stop_token>` and `std::jthread` behind `-fexperimental-library`, so both macOS
        // CI jobs fail to compile them. A single portable implementation was preferred over a feature-detected second
        // copy. Revisit once the macOS runners ship a libc++ that enables them by default
        std::mutex              queue_mutex{};
        std::condition_variable queue_cv{};

        // the number of workers that popped a step point but have not yet reported back their discovered neighbors
        std::size_t active_workers = 0;

        // set once the flood fill is complete, which lets all waiting workers terminate
        bool finished = false;

        // a utility function that gathers the neighbors of `sp` that are not already known. This is the expensive part
        // of the discovery, so it is deliberately called without holding `queue_mutex`
        const auto unknown_neighborhood = [this](const step_point& sp) noexcept
        {
            std::vector<step_point> unknown{};

            const auto neighborhood = moore_neighborhood(sp);

            std::ranges::copy_if(neighborhood, std::back_inserter(unknown),
                                 [this](const auto& m) noexcept { return !op_domain.contains(to_parameter_point(m)); });

            return unknown;
        };

        // a utility function that adds the given step points to the queue for further evaluation. Each step point is
        // scheduled at most once, which ensures that no parameter point is simulated twice. Must be called under lock
        const auto schedule_points = [&queue, &scheduled](const std::vector<step_point>& step_points) noexcept
        {
            for (const auto& sp : step_points)
            {
                if (scheduled.insert(sp).second)
                {
                    queue.push_back(sp);
                }
            }
        };

        // seed the queue with the neighbors of each operational sample
        op_domain.for_each(
            [this, &schedule_points, &unknown_neighborhood](const auto& param_point, const auto& status) noexcept
            {
                if (std::get<0>(status) == operational_status::OPERATIONAL)
                {
                    schedule_points(unknown_neighborhood(to_step_point(param_point)));
                }
            });

        // if random sampling did not find a single operational point, there is nothing to flood fill
        if (!queue.empty())
        {
            const auto worker = [&]() noexcept
            {
                while (true)
                {
                    std::unique_lock lock{queue_mutex};

                    queue_cv.wait(lock, [&queue, &finished]() noexcept { return !queue.empty() || finished; });

                    // the queue can only be empty here once the flood fill is complete
                    if (queue.empty())
                    {
                        return;
                    }

                    // fetch the step point and remove it from the queue
                    const auto sp = queue.front();
                    queue.pop_front();

                    ++active_workers;

                    lock.unlock();

                    // determine the operational status and, if the point is operational, its yet unknown neighbors.
                    // No lock is held here, which is what enables the parallelism in the first place
                    const auto discovered = is_step_point_operational(sp) == operational_status::OPERATIONAL ?
                                                unknown_neighborhood(sp) :
                                                std::vector<step_point>{};

                    lock.lock();

                    schedule_points(discovered);

                    --active_workers;

                    // the flood fill is complete only once the queue has run dry and no worker is left that could
                    // still discover new points
                    finished = queue.empty() && active_workers == 0;

                    if (finished || !queue.empty())
                    {
                        queue_cv.notify_all();
                    }
                }
            };

            const auto num_workers = number_of_threads;

            std::vector<std::thread> workers{};
            workers.reserve(num_workers);

            for (std::size_t i = 0; i < num_workers; ++i)
            {
                workers.emplace_back(worker);
            }

            for (auto& w : workers)
            {
                w.join();
            }
        }

        log_stats();

        return op_domain;
    }
    // NOLINTEND(bugprone-exception-escape)
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
    // NOLINTNEXTLINE(bugprone-exception-escape): only allocation can throw, which is fatal to the algorithm anyway
    [[nodiscard]] OpDomain contour_tracing(const std::size_t samples) noexcept
    {
        assert(num_dimensions >= 2 && "Contour tracing is only supported for two or more dimensions");

        // Moore contour tracing walks a closed curve by repeatedly taking the next neighbor in clockwise order. That
        // ordering exists only in the plane, so three or more dimensions, where the boundary is a surface rather than
        // a curve, need the boundary-collecting variant instead
        return num_dimensions == 2 ? trace_contour_curve(samples) : trace_boundary_surface(samples);
    }
    /**
     * Traces the contour of the operational domain in two dimensions by Moore contour tracing.
     *
     * @param samples Maximum number of random samples to be taken before contour tracing.
     * @return The (partial) operational domain of the layout.
     */
    // NOLINTNEXTLINE(bugprone-exception-escape): only allocation can throw, which is fatal to the algorithm anyway
    [[nodiscard]] OpDomain trace_contour_curve(const std::size_t samples) noexcept
    {
        assert(num_dimensions == 2 && "Moore contour tracing is only supported for two dimensions");

        const mockturtle::stopwatch stop{stats.time_total};

        const auto step_point_samples = generate_random_step_points(samples);

        simulate_operational_status_in_parallel(step_point_samples);

        const auto next_clockwise_point = [](std::vector<step_point>& neighborhood,
                                             const step_point&        backtrack) noexcept -> step_point
        {
            assert(std::ranges::find(neighborhood, backtrack) != neighborhood.cend() &&
                   "The backtrack point must be part of the neighborhood");

            while (neighborhood.back() != backtrack)
            {
                std::ranges::rotate(neighborhood, neighborhood.begin() + 1);
            }

            return neighborhood.front();
        };

        // for each sampled point
        for (const auto& starting_point : step_point_samples)
        {
            // if the current starting point is non-operational, skip to the next one
            const auto domain_value = op_domain.contains(to_parameter_point(starting_point));
            if (domain_value.has_value())
            {
                if (std::get<0>(domain_value.value()) == operational_status::NON_OPERATIONAL)
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

            // all step points visited by the contour trace; they form a closed 8-connected curve that encloses the
            // operational area `starting_point` is located in
            phmap::btree_set<step_point> contour{contour_starting_point};

            const auto x = current_contour_point.step_values.at(0);
            const auto y = current_contour_point.step_values.at(1);

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

                    contour.insert(current_contour_point);
                }
                else
                {
                    backtrack_point = next_point;
                }

                current_neighborhood = moore_neighborhood_2d(current_contour_point);
                next_point           = next_clockwise_point(current_neighborhood, backtrack_point);
            }

            infer_operational_status_in_enclosing_contour(starting_point, contour);
        }

        log_stats();

        return op_domain;
    }
    /**
     * Traces the boundary surface of the operational domain in three or more dimensions.
     *
     * This serves the same purpose as the two-dimensional Moore contour trace — sample only the boundary of an
     * operational region and infer its interior — but collects the boundary instead of walking it. A closed curve can
     * be walked because its neighbors admit a cyclic order; a closed surface cannot, so the boundary is gathered by a
     * breadth-first search over the operational points that have at least one non-operational Moore neighbor. The
     * resulting set is closed under the Moore neighborhood, which is what the interior inference requires.
     *
     * @param samples Maximum number of random samples to be taken before tracing.
     * @return The (partial) operational domain of the layout.
     */
    // NOLINTNEXTLINE(bugprone-exception-escape): only allocation can throw, which is fatal to the algorithm anyway
    [[nodiscard]] OpDomain trace_boundary_surface(const std::size_t samples) noexcept
    {
        assert(num_dimensions >= 3 && "Boundary surface tracing is intended for three or more dimensions");

        const mockturtle::stopwatch stop{stats.time_total};

        const auto step_point_samples = generate_random_step_points(samples);

        simulate_operational_status_in_parallel(step_point_samples);

        // a step point is on the boundary if it is operational and borders a non-operational point or the edge of the
        // parameter range. The latter is implied: `moore_neighborhood` does not gather points outside the range, so a
        // point at the edge has fewer than `3^n - 1` neighbors
        const auto is_boundary_point = [this](const step_point& sp) noexcept
        {
            const auto neighborhood = moore_neighborhood(sp);

            if (neighborhood.size() < static_cast<std::size_t>(std::pow(3, num_dimensions)) - 1)
            {
                return true;
            }

            return std::ranges::any_of(neighborhood, [this](const auto& m) noexcept
                                       { return is_step_point_operational(m) == operational_status::NON_OPERATIONAL; });
        };

        for (const auto& starting_point : step_point_samples)
        {
            // if the current starting point is non-operational, skip to the next one
            const auto domain_value = op_domain.contains(to_parameter_point(starting_point));
            if (domain_value.has_value())
            {
                if (std::get<0>(domain_value.value()) == operational_status::NON_OPERATIONAL)
                {
                    continue;
                }
            }

            // if the current step point has been inferred as operational, skip to the next one
            if (is_step_point_inferred_operational(starting_point))
            {
                continue;
            }

            // find an operational point on the boundary starting from the randomly determined starting point
            const auto boundary_starting_point = find_operational_contour_step_point(starting_point);

            // all step points visited by the boundary trace; they form a closed surface that encloses the operational
            // region `starting_point` is located in
            phmap::btree_set<step_point> contour{};

            std::queue<step_point>       queue{};
            phmap::btree_set<step_point> visited{boundary_starting_point};

            queue.push(boundary_starting_point);

            while (!queue.empty())
            {
                const auto sp = queue.front();
                queue.pop();

                if (!is_boundary_point(sp))
                {
                    continue;
                }

                contour.insert(sp);

                for (const auto& m : moore_neighborhood(sp))
                {
                    if (!visited.insert(m).second)
                    {
                        continue;
                    }

                    if (is_step_point_operational(m) == operational_status::OPERATIONAL)
                    {
                        queue.push(m);
                    }
                }
            }

            infer_operational_status_in_enclosing_contour(starting_point, contour);
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
    [[nodiscard]] sidb_simulation_domain<parameter_point, uint64_t>
    grid_search_for_physically_valid_parameters(Lyt& lyt) noexcept
    {
        sidb_simulation_domain<parameter_point, uint64_t> suitable_params_domain{};

        const mockturtle::stopwatch stop{stats.time_total};

        // Cartesian product of all step point indices
        const auto all_index_combinations = cartesian_combinations(indices);

        // number of threads. Floored at `1` so that the slice arithmetic below stays well-defined when there is
        // nothing to distribute; the `start >= end` guard in the loop then keeps the worker from being launched
        const auto num_threads = std::max(std::min(number_of_threads, all_index_combinations.size()), std::size_t{1});

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

        op_domain.for_each(
            [&simulation_parameters, &lyt, this, &suitable_params_domain](const auto& param_point, const auto& status)
            {
                if constexpr (std::is_same_v<OpDomain, operational_domain>)
                {
                    if (std::get<0>(status) == operational_status::NON_OPERATIONAL)
                    {
                        return;
                    }

                    for (auto d = 0u; d < num_dimensions; ++d)
                    {
                        set_dimension_value(simulation_parameters, param_point.get_parameters().at(d), d);
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
                        const quicksim_params qs_params{.simulation_parameters = simulation_parameters,
                                                        .iteration_steps       = 500,
                                                        .alpha                 = 0.6};

                        if (const auto result = quicksim(lyt, qs_params); result.has_value())
                        {
                            sim_results = result.value();
                        }
                        else
                        {
                            return;
                        }
                    }
                    else
                    {
                        assert(false && "unsupported simulation engine");
                    }

                    const auto energy_dist = calculate_energy_distribution(sim_results.charge_distributions);

                    lyt.assign_physical_parameters(simulation_parameters);
                    const auto degeneracy_of_layout_energy =
                        energy_dist.degeneracy(lyt.get_electrostatic_potential_energy());

                    if (!degeneracy_of_layout_energy.has_value())
                    {
                        return;
                    }

                    const auto excited_state_number = degeneracy_of_layout_energy.value();
                    suitable_params_domain.add_value(param_point, std::make_tuple(excited_state_number));
                }
            });

        return suitable_params_domain;
    }
    /**
     * Returns the parameter points that were inferred (assumed) to be operational because they are enclosed by a
     * contour traced by `contour_tracing`. These points have not been simulated and are, therefore, not part of the
     * returned operational domain. They are exposed to enable inspection of the enclosure inference.
     *
     * @return The parameter points that have been inferred to be operational.
     */
    [[nodiscard]] std::vector<parameter_point> inferred_operational_parameter_points() const noexcept
    {
        std::vector<parameter_point> parameter_points{};
        parameter_points.reserve(inferred_op_domain.size());

        std::ranges::transform(inferred_op_domain, std::back_inserter(parameter_points),
                               [this](const auto& sp) { return to_parameter_point(sp); });

        return parameter_points;
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
     * Number of worker threads to distribute the parameter points over, taken from the parameters and floored at `1`.
     */
    const std::size_t number_of_threads{std::max(params.number_of_threads, std::size_t{1})};
    /**
     * Input BDL wires.
     */
    const std::vector<bdl_wire<Lyt>> input_bdl_wires;
    /**
     * Output BDL wires.
     */
    const std::vector<bdl_wire<Lyt>> output_bdl_wires;
    /**
     * The layout with each input pattern applied, indexed by input pattern. The input configuration does not depend on
     * the swept parameters, so these layouts are generated once and read by every sample point evaluation. Empty if
     * the layout-only constructor was used, which never evaluates operational status.
     */
    const std::vector<Lyt> input_pattern_layouts;
    /**
     * A step point holds one step value per sweep dimension, each from 0 to the maximum number of steps in that
     * dimension. A step point does not hold the actual parameter values, but the step values.
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
         * Three-way comparison operator. Compares the step values lexicographically, which also yields the equality,
         * inequality, and relational operators via C++20's rewritten candidates.
         *
         * @param other Other step point to compare with.
         * @return The lexicographical ordering of the two step points' step values.
         */
        [[nodiscard]] auto operator<=>(const step_point& other) const = default;
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
        parameter_values.reserve(num_dimensions);

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            parameter_values.push_back(values.at(d).at(sp.step_values.at(d)));
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
            [[maybe_unused]] const auto min_val = values.at(d).front();
            [[maybe_unused]] const auto max_val = values.at(d).back();

            assert(pp.get_parameters().at(d) >= min_val && pp.get_parameters().at(d) <= max_val &&
                   "Parameter point is outside of the value range");

            const auto it = std::ranges::lower_bound(values.at(d), pp.get_parameters().at(d));

            const auto dis = std::distance(values.at(d).cbegin(), it);

            step_values.push_back(static_cast<std::size_t>(dis));
        }

        return step_point{step_values};
    }
    /**
     * Calculates the number of steps in the given dimension based on the provided parameters.
     *
     * @return The number of steps in the given dimension.
     */
    [[nodiscard]] std::size_t num_steps(const std::size_t dimension) const noexcept
    {
        assert(dimension < num_dimensions && "Invalid dimension");

        return static_cast<std::size_t>(
            std::round((params.sweep_dimensions.at(dimension).max - params.sweep_dimensions.at(dimension).min) /
                       params.sweep_dimensions.at(dimension).step));
    }
    /**
     * Helper function that sets the value of a sweep dimension in the simulation parameters.
     *
     * @param sim_parameters Simulation parameter object to set the sweep dimension `dim` to value `val`.
     * @param val Value to set the dimension `dim` to.
     * @param dim Sweep dimension to set the value `val` to.
     */
    void set_dimension_value(sidb_simulation_parameters& sim_parameters, const double val,
                             const std::size_t dim) const noexcept
    {
        switch (params.sweep_dimensions.at(dim).dimension)
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
        if (const auto op_value = op_domain.contains(to_parameter_point(sp)); op_value.has_value())
        {
            return std::get<0>(*op_value);
        }

        const auto param_point = to_parameter_point(sp);

        // NOLINTNEXTLINE(bugprone-exception-escape): only allocation can throw, as in the enclosing algorithms
        const auto operational = [this, &param_point](const std::optional<double>& ct_value = std::nullopt) noexcept
        {
            if constexpr (std::is_same_v<OpDomain, critical_temperature_domain>)
            {
                if (ct_value.has_value())
                {
                    op_domain.add_value(param_point, std::tuple{operational_status::OPERATIONAL, ct_value.value()});
                }
            }
            else
            {
                op_domain.add_value(param_point, std::make_tuple(operational_status::OPERATIONAL));
            }

            return operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, &param_point]() noexcept
        {
            if constexpr (std::is_same_v<OpDomain, critical_temperature_domain>)
            {
                op_domain.add_value(param_point, std::tuple{operational_status::NON_OPERATIONAL, 0.0});
            }
            else
            {
                op_domain.add_value(param_point, std::make_tuple(operational_status::NON_OPERATIONAL));
            }

            return operational_status::NON_OPERATIONAL;
        };

        ++num_evaluated_parameter_combinations;

        sidb_simulation_parameters sim_params = params.operational_params.simulation_parameters;

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            set_dimension_value(sim_params, values.at(d).at(sp.step_values.at(d)), d);
        }

        auto op_params_set_dimension_values                  = params.operational_params;
        op_params_set_dimension_values.simulation_parameters = sim_params;

        const auto& [status, sim_calls] =
            is_operational(input_pattern_layouts, truth_table, op_params_set_dimension_values, input_bdl_wires,
                           output_bdl_wires, std::optional{canvas_lyt});

        num_simulator_invocations += sim_calls;

        if (status == operational_status::NON_OPERATIONAL)
        {
            return non_operational();
        }

        if constexpr (std::is_same_v<OpDomain, critical_temperature_domain>)
        {
            // the input pattern layouts and the BDL detection results do not depend on the swept parameters, so the
            // ones generated once in the constructor are handed to every sample point instead of being re-derived here
            const auto ct = critical_temperature_gate_based(
                input_pattern_layouts, truth_table,
                critical_temperature_params{.operational_params = op_params_set_dimension_values}, output_bdl_pairs,
                input_bdl_wires, output_bdl_wires);

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
        if (const auto op_value = op_domain.contains(to_parameter_point(sp)); op_value.has_value())
        {
            return std::get<0>(*op_value);
        }

        // fetch the parameter values of all sweep dimensions
        const auto param_point = to_parameter_point(sp);

        const auto operational = [this, &param_point]()
        {
            op_domain.add_value(param_point, std::make_tuple(operational_status::OPERATIONAL));

            return operational_status::OPERATIONAL;
        };

        const auto non_operational = [this, &param_point]()
        {
            op_domain.add_value(param_point, std::make_tuple(operational_status::NON_OPERATIONAL));

            return operational_status::NON_OPERATIONAL;
        };

        // increment the number of evaluated parameter combinations
        ++num_evaluated_parameter_combinations;

        sidb_simulation_parameters sim_params = params.operational_params.simulation_parameters;

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            set_dimension_value(sim_params, param_point.get_parameters().at(d), d);
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
    [[nodiscard]] bool is_step_point_inferred_operational(const step_point& sp) const noexcept
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
            distributions.emplace_back(0, indices.at(d).size() - 1);
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
                dimension_samples.push_back(distributions.at(d)(generator));
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
        // number of threads. Floored at `1` so that the slice arithmetic below stays well-defined when there is
        // nothing to distribute; the `start >= end` guard in the loop then keeps the worker from being launched
        const std::size_t num_threads = std::max(std::min(number_of_threads, step_points.size()), std::size_t{1});

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
                    std::ranges::for_each(std::ranges::subrange{step_points.cbegin() + static_cast<int64_t>(start),
                                                                step_points.cbegin() + static_cast<int64_t>(end)},
                                          [this](const auto& sp) { is_step_point_operational(sp); });
                });
        }

        // wait for all threads to complete
        for (auto& thread : threads)
        {
            thread.join();
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
        assert(starting_point.step_values.size() == num_dimensions &&
               "Given step point must match the number of dimensions");

        auto latest_operational_point = starting_point;

        // move towards the lower border of the first dimension, holding all other dimensions fixed
        for (std::size_t x = starting_point.step_values.at(0); x > 0; --x)
        {
            auto left_step_values  = starting_point.step_values;
            left_step_values.at(0) = x;

            const auto left_step = step_point{left_step_values};

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

        const auto x = sp.step_values.at(0);
        const auto y = sp.step_values.at(1);

        const auto num_x_indices = indices.at(0).size();
        const auto num_y_indices = indices.at(1).size();

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
    }
    /**
     * Returns the von Neumann neighborhood of the given step point. The von Neumann neighborhood is the set of all
     * points that differ from `sp` by one step in exactly one dimension, i.e., the axis-aligned neighbors excluding
     * the diagonals. It contains up to `2n` points for `n` sweep dimensions, as points outside of the parameter range
     * are not gathered. The points are returned in no particular order.
     *
     * @param sp Step point to get the von Neumann neighborhood of.
     * @return The von Neumann neighborhood of `sp`.
     */
    [[nodiscard]] std::vector<step_point> von_neumann_neighborhood(const step_point& sp) const noexcept
    {
        assert(sp.step_values.size() == num_dimensions && "Given step point must match the number of dimensions");

        std::vector<step_point> neighbors{};
        neighbors.reserve(2 * num_dimensions);

        for (auto d = 0u; d < num_dimensions; ++d)
        {
            const auto step = sp.step_values.at(d);

            if (step > 0)
            {
                auto decremented  = sp.step_values;
                decremented.at(d) = step - 1;
                neighbors.emplace_back(decremented);
            }
            if (step + 1 < indices.at(d).size())
            {
                auto incremented  = sp.step_values;
                incremented.at(d) = step + 1;
                neighbors.emplace_back(incremented);
            }
        }

        return neighbors;
    }
    /**
     * Returns the Moore neighborhood of the given step point. The Moore neighborhood is the set of all points that
     * differ from `sp` by at most one step in every dimension, i.e., the adjacent points including the diagonals. It
     * contains up to `3^n - 1` points for `n` sweep dimensions, as points outside of the parameter range are not
     * gathered. The points are returned in no particular order.
     *
     * `moore_neighborhood_2d` returns the same set for two dimensions, but in clockwise order, which the 2D contour
     * trace depends on. This function cannot replace it: there is no canonical cyclic ordering of the neighbors in
     * three or more dimensions.
     *
     * @param sp Step point to get the Moore neighborhood of.
     * @return The Moore neighborhood of `sp`.
     */
    [[nodiscard]] std::vector<step_point> moore_neighborhood(const step_point& sp) const noexcept
    {
        assert(sp.step_values.size() == num_dimensions && "Given step point must match the number of dimensions");

        // 3^n offsets, one of which is the center point itself
        const auto num_offsets = static_cast<std::size_t>(std::pow(3, num_dimensions));

        std::vector<step_point> neighbors{};
        neighbors.reserve(num_offsets - 1);

        // enumerate the offset vectors in {-1, 0, 1}^n as a mixed-radix counter over base 3
        for (std::size_t offset_index = 0; offset_index < num_offsets; ++offset_index)
        {
            auto neighbor    = sp.step_values;
            auto remainder   = offset_index;
            bool is_center   = true;
            bool is_in_range = true;

            for (auto d = 0u; d < num_dimensions; ++d)
            {
                const auto offset = static_cast<int64_t>(remainder % 3) - 1;
                remainder /= 3;

                if (offset != 0)
                {
                    is_center = false;
                }

                const auto step = static_cast<int64_t>(sp.step_values.at(d)) + offset;

                if (step < 0 || std::cmp_greater_equal(step, indices.at(d).size()))
                {
                    is_in_range = false;
                    break;
                }

                neighbor.at(d) = static_cast<std::size_t>(step);
            }

            if (!is_center && is_in_range)
            {
                neighbors.emplace_back(neighbor);
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
     * from the starting point until it encounters the traced contour.
     *
     * The flood fill expands over the von Neumann neighborhood, which connects `2n` points for `n` sweep dimensions,
     * while the given contour is closed under the Moore neighborhood, which connects `3^n - 1`. A `2n`-connected path
     * cannot cross a `(3^n - 1)`-connected closed boundary, so the inference is guaranteed to stay within the region
     * the contour encloses. In two dimensions this is the familiar pairing of a 4-connected path against an
     * 8-connected closed curve. Points on the contour itself are marked, but not expanded from.
     *
     * Note that no physical simulation is conducted by this function!
     *
     * @param starting_point Step point at which to start the inference. If `starting_point` is non-operational, this
     * function might invoke undefined behavior.
     * @param contour The step points visited by the contour trace that encloses `starting_point`.
     */
    // NOLINTNEXTLINE(bugprone-exception-escape): only allocation can throw, as in the calling `contour_tracing`
    void infer_operational_status_in_enclosing_contour(const step_point&                   starting_point,
                                                       const phmap::btree_set<step_point>& contour) noexcept
    {
        assert(is_step_point_operational(starting_point) == operational_status::OPERATIONAL &&
               "starting_point must be within the operational domain");

        // if the starting point has already been inferred as operational, this area has been covered before
        if (is_step_point_inferred_operational(starting_point))
        {
            return;
        }

        // a queue of step points to be marked as inferred operational
        std::queue<step_point> queue{};

        // mark the starting point as inferred operational and use it to seed the flood fill
        inferred_op_domain.insert(starting_point);
        queue.push(starting_point);

        // for each point in the queue
        while (!queue.empty())
        {
            // fetch the step point and remove it from the queue
            const auto sp = queue.front();
            queue.pop();

            // the contour is the boundary of the enclosed area; do not expand beyond it
            if (contour.count(sp) > 0)
            {
                continue;
            }

            for (const auto& m : von_neumann_neighborhood(sp))
            {
                // if the point has already been inferred as operational, continue with the next
                if (is_step_point_inferred_operational(m))
                {
                    continue;
                }

                // if the point has already been sampled
                if (const auto operational_status = op_domain.contains(to_parameter_point(m));
                    operational_status.has_value())
                {
                    // and found to be non-operational, continue with the next
                    if (std::get<0>(operational_status.value()) == operational_status::NON_OPERATIONAL)
                    {
                        continue;
                    }
                }

                // otherwise, it is either found operational or can be inferred as such
                inferred_op_domain.insert(m);
                queue.push(m);
            }
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

        op_domain.for_each(
            [this](const auto& param_point [[maybe_unused]], const auto& status)
            {
                if (std::get<0>(status) == operational_status::OPERATIONAL)
                {
                    ++stats.num_operational_parameter_combinations;
                }
                else
                {
                    ++stats.num_non_operational_parameter_combinations;
                }
            });

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
 * sweeping all sweep dimensions. Since grid search is exhaustive, the algorithm is guaranteed
 * to find the operational domain, if it exists within the parameter range. However, the algorithm performs one
 * operational check per parameter combination, i.e., the product of the step counts of all sweep dimensions, where
 * each operational check consists of up to \f$2^n\f$ exact ground
 * state simulations, where \f$n\f$ is the number of inputs of the layout. Each exact ground state simulation has
 * exponential complexity in of itself. Therefore, the algorithm is only feasible for small layouts with few inputs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param tt Expected Boolean function of the lyt given as a multi-output truth table.
 * @param ps Parameters for the operational domain computation.
 * @param st Statistics of the process.
 * @return The operational domain of the layout.
 * @throws std::invalid_argument if the given sweep parameters are invalid, or if the operational domain sketch
 * is requested without rejecting kinks or on a layout without `LOGIC` cells. Any number of sweep
 * dimensions is accepted.
 */
template <typename Lyt, typename TT>
[[nodiscard]] operational_domain operational_domain_grid_search(const Lyt& lyt, const std::vector<TT>& spec,
                                                                const operational_domain_params& params = {},
                                                                operational_domain_stats*        stats  = nullptr)
{
    // do not convert the `static_assert` type checks in this file's public entry points into `requires` clauses: the
    // pyfiction docstring generator parses this header as C++11 (see the note on `manhattan_distance` in
    // `algorithms/path_finding/distance.hpp`), where the clause is a syntax error that drops the Doxygen comments of
    // the declarations that follow. `kitty` also provides no `is_truth_table_v` alias that would simplify the third
    // check
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    // this may throw an `std::invalid_argument` exception
    detail::validate_operational_domain_params(lyt, params);

    operational_domain_stats                                     st{};
    detail::operational_domain_impl<Lyt, TT, operational_domain> p{lyt, spec, params, st};

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
 * @return The operational domain of the layout.
 * @throws std::invalid_argument if the given sweep parameters are invalid, or if the operational domain sketch
 * is requested without rejecting kinks or on a layout without `LOGIC` cells. Any number of sweep
 * dimensions is accepted.
 */
template <typename Lyt, typename TT>
[[nodiscard]] operational_domain operational_domain_random_sampling(const Lyt& lyt, const std::vector<TT>& spec,
                                                                    const std::size_t                samples,
                                                                    const operational_domain_params& params = {},
                                                                    operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    // this may throw an `std::invalid_argument` exception
    detail::validate_operational_domain_params(lyt, params);

    operational_domain_stats                                     st{};
    detail::operational_domain_impl<Lyt, TT, operational_domain> p{lyt, spec, params, st};

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
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The operational domain of the layout.
 * @throws std::invalid_argument if the given sweep parameters are invalid, or if the operational domain sketch
 * is requested without rejecting kinks or on a layout without `LOGIC` cells. Flood fill and contour
 * tracing additionally require at least two sweep dimensions; grid search and random sampling accept
 * any number.
 */
template <typename Lyt, typename TT>
[[nodiscard]] operational_domain
operational_domain_flood_fill(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                              const operational_domain_params& params = {}, operational_domain_stats* stats = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (params.sweep_dimensions.size() < 2)
    {
        throw std::invalid_argument("Flood fill is only applicable to 2 or more dimensions");
    }

    // this may throw an `std::invalid_argument` exception
    detail::validate_operational_domain_params(lyt, params);

    operational_domain_stats                                     st{};
    detail::operational_domain_impl<Lyt, TT, operational_domain> p{lyt, spec, params, st};

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
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The operational domain of the layout.
 * @throws std::invalid_argument if the given sweep parameters are invalid, or if the operational domain sketch
 * is requested without rejecting kinks or on a layout without `LOGIC` cells. Flood fill and contour
 * tracing additionally require at least two sweep dimensions; grid search and random sampling accept
 * any number.
 */
template <typename Lyt, typename TT>
[[nodiscard]] operational_domain operational_domain_contour_tracing(const Lyt& lyt, const std::vector<TT>& spec,
                                                                    const std::size_t                samples,
                                                                    const operational_domain_params& params = {},
                                                                    operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (params.sweep_dimensions.size() < 2)
    {
        throw std::invalid_argument("Contour tracing is only applicable to 2 or more dimensions");
    }

    // this may throw an `std::invalid_argument` exception
    detail::validate_operational_domain_params(lyt, params);

    operational_domain_stats                                     st{};
    detail::operational_domain_impl<Lyt, TT, operational_domain> p{lyt, spec, params, st};
    const auto                                                   result = p.contour_tracing(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * Computes the critical temperature domain of the given SiDB cell-level layout. The critical temperature domain
 * consists of all parameter combinations for which the layout is logically operational, along with the critical
 * temperature for each specific parameter point.
 *
 * This algorithm uses a grid search to find the operational domain. The grid search is performed by exhaustively
 * sweeping all sweep dimensions. Since grid search is exhaustive, the algorithm is guaranteed
 * to find the operational domain, if it exists within the parameter range. However, the algorithm performs one
 * operational check per parameter combination, i.e., the product of the step counts of all sweep dimensions, where
 * each operational check consists of up to \f$2^n\f$ exact ground
 * state simulations, where \f$n\f$ is the number of inputs of the layout. Each exact ground state simulation has
 * exponential complexity in of itself. Therefore, the algorithm is only feasible for small layouts with few inputs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected vector of truth tables of the layout. Each truth table represents an output of the Boolean
 * function.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The critical temperature domain of the layout.
 * @throws std::invalid_argument if the given sweep parameters are invalid, or if the operational domain sketch
 * is requested without rejecting kinks or on a layout without `LOGIC` cells. Any number of sweep
 * dimensions is accepted.
 */
template <typename Lyt, typename TT>
[[nodiscard]] critical_temperature_domain
critical_temperature_domain_grid_search(const Lyt& lyt, const std::vector<TT>& spec,
                                        const operational_domain_params& params = {},
                                        operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    // this may throw an `std::invalid_argument` exception
    detail::validate_operational_domain_params(lyt, params);

    operational_domain_stats                                              st{};
    detail::operational_domain_impl<Lyt, TT, critical_temperature_domain> p{lyt, spec, params, st};

    const auto result = p.grid_search();

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * Computes the critical temperature domain of the given SiDB cell-level layout. The critical temperature domain
 * consists of all parameter combinations for which the layout is logically operational, along with the critical
 * temperature for each specific parameter point.
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
 * @return The critical temperature domain of the layout.
 * @throws std::invalid_argument if the given sweep parameters are invalid, or if the operational domain sketch
 * is requested without rejecting kinks or on a layout without `LOGIC` cells. Any number of sweep
 * dimensions is accepted.
 */
template <typename Lyt, typename TT>
[[nodiscard]] critical_temperature_domain
critical_temperature_domain_random_sampling(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                                            const operational_domain_params& params = {},
                                            operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    // this may throw an `std::invalid_argument` exception
    detail::validate_operational_domain_params(lyt, params);

    operational_domain_stats                                              st{};
    detail::operational_domain_impl<Lyt, TT, critical_temperature_domain> p{lyt, spec, params, st};

    const auto result = p.random_sampling(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * Computes the critical temperature domain of the given SiDB cell-level layout. The critical temperature domain
 * consists of all parameter combinations for which the layout is logically operational, along with the critical
 * temperature for each specific parameter point.
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
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The critical temperature domain of the layout.
 * @throws std::invalid_argument if the given sweep parameters are invalid, or if the operational domain sketch
 * is requested without rejecting kinks or on a layout without `LOGIC` cells. Flood fill and contour
 * tracing additionally require at least two sweep dimensions; grid search and random sampling accept
 * any number.
 */
template <typename Lyt, typename TT>
[[nodiscard]] critical_temperature_domain
critical_temperature_domain_flood_fill(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                                       const operational_domain_params& params = {},
                                       operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (params.sweep_dimensions.size() < 2)
    {
        throw std::invalid_argument("Flood fill is only applicable to 2 or more dimensions");
    }

    // this may throw an `std::invalid_argument` exception
    detail::validate_operational_domain_params(lyt, params);

    operational_domain_stats                                              st{};
    detail::operational_domain_impl<Lyt, TT, critical_temperature_domain> p{lyt, spec, params, st};

    const auto result = p.flood_fill(samples);

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * Computes the critical temperature domain of the given SiDB cell-level layout. The critical temperature domain
 * consists of all parameter combinations for which the layout is logically operational, along with the critical
 * temperature for each specific parameter point.nt.
 *
 * This algorithm first uses random sampling to find a set of operational point within the parameter range. From there,
 * it traverses outwards to find the edge of the operational area and performs Moore neighborhood contour tracing to
 * explore the contour of the operational domain. This is repeated for all initially sampled points that do not lie
 * within a contour. The algorithm is guaranteed to determine the contours of all operational "islands" if the initial
 * random sampling found at least one operational point within them. Thereby, this algorithm works for disconnected
 * operational domains. The critical temperature is computed for each operational point.
 *
 * It performs `samples` uniformly-distributed random samples within the parameter range. For each thusly discovered
 * operational island, it performs another number of samples equal to the distance to an edge of each operational
 * area. Finally, it performs up to 8 samples for each contour point (however, the actual number is usually lower).
 * For each sample, the algorithm performs one operational check on the layout, where each operational check consists of
 * up to \f$2^n\f$ exact ground state simulations, where \f$n\f$ is the number of inputs of the layout. Each exact
 * ground state simulation has exponential complexity in of itself. Therefore, the algorithm is only feasible for small
 * layouts with few inputs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout to compute the operational domain for.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param samples Number of samples to perform.
 * @param params Operational domain computation parameters.
 * @param stats Operational domain computation statistics.
 * @return The critical temperature domain of the layout.
 * @throws std::invalid_argument if the given sweep parameters are invalid, or if the operational domain sketch
 * is requested without rejecting kinks or on a layout without `LOGIC` cells. Flood fill and contour
 * tracing additionally require at least two sweep dimensions; grid search and random sampling accept
 * any number.
 */
template <typename Lyt, typename TT>
[[nodiscard]] critical_temperature_domain
critical_temperature_domain_contour_tracing(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                                            const operational_domain_params& params = {},
                                            operational_domain_stats*        stats  = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (params.sweep_dimensions.size() < 2)
    {
        throw std::invalid_argument("Contour tracing is only applicable to 2 or more dimensions");
    }

    // this may throw an `std::invalid_argument` exception
    detail::validate_operational_domain_params(lyt, params);

    operational_domain_stats                                              st{};
    detail::operational_domain_impl<Lyt, TT, critical_temperature_domain> p{lyt, spec, params, st};
    const auto                                                            result = p.contour_tracing(samples);

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
    size_t operator()(const fiction::parameter_point& pp) const noexcept
    {
        size_t hash_value = 0;
        for (const auto& parameter : pp.get_parameters())
        {
            // hash the cell the parameter value falls into, which is what `parameter_point::operator==` compares.
            // Casting the quotient straight to `size_t` would be undefined for the negative values that a `MU_MINUS`
            // sweep produces
            fiction::hash_combine(hash_value, fiction::parameter_point::quantize(parameter));
        }

        return hash_value;
    }
};

}  // namespace std

#endif  // FICTION_OPERATIONAL_DOMAIN_HPP
