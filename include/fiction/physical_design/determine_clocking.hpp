/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief SAT-based assignment of clock numbers to an unclocked gate-level layout.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/traits.hpp"

#include <bill/sat/cardinality.hpp>
#include <bill/sat/interface/common.hpp>
#include <bill/sat/interface/types.hpp>
#include <bill/sat/solver.hpp>  // NOLINT(misc-include-cleaner): umbrella header pulling in the solver backends
#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction::physical_design
{

/**
 * Parameters for the `determine_clocking` algorithm.
 */
struct determine_clocking_params
{
    /**
     * The SAT solver to use.
     */
    bill::solvers sat_engine = bill::solvers::bsat2;
};
/**
 * Statistics for the `determine_clocking` algorithm.
 */
struct determine_clocking_stats
{
    /**
     * Total runtime.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Reports the statistics to the given output stream.
     *
     * @param out The output stream to report to.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time = {:.2f} secs\n", mockturtle::to_seconds(time_total));
    }
};

namespace detail
{

/**
 * @brief Encodes declared connections as clock-zone constraints.
 * @tparam Lyt Gate-level layout type.
 * @tparam SolverType SAT backend.
 */
template <typename Lyt, bill::solvers SolverType = bill::solvers::ghack>
class sat_clocking_handler
{
  public:
    /** @brief Creates variables for occupied clock zones. @param lyt Validated layout. */
    explicit sat_clocking_handler(Lyt& lyt) : layout{lyt}, number_of_clocks{layout.num_clocks()}
    {
        layout.foreach_object(
            [this](const auto id)
            {
                const auto zone = clock_zone(id);
                if (variables.contains({zone, 0}))
                {
                    return;
                }
                clock_zones.push_back(zone);
                for (typename Lyt::clock_number_t clk{}; clk < number_of_clocks; ++clk)
                {
                    variables.emplace(tile_clock_number{zone, clk}, solver.add_variable());
                }
            });
    }
    /**
     * @brief Solves clock constraints and commits a complete clocking value on success.
     * @return Whether the constraints are satisfiable.
     */
    bool determine_clocks()
    {
        at_least_one_clock_number_per_tile();
        at_most_one_clock_number_per_tile();
        exclude_clock_assignments_that_violate_information_flow();
        symmetry_breaking();
        if (solver.solve() == bill::result::states::satisfiable)
        {
            assign_clock_numbers(solver.get_model().model());
            return true;
        }
        return false;
    }

  private:
    /** @brief Layout receiving the completed clock assignment. */
    Lyt& layout;
    /** @brief Number of phases in the stored scheme. */
    const typename Lyt::clock_number_t number_of_clocks;
    /** @brief SAT backend. */
    bill::solver<SolverType> solver{};
    /** @brief Clock-zone and phase key. */
    using tile_clock_number = std::pair<tile<Lyt>, typename Lyt::clock_number_t>;
    /** @brief Variables for every occupied clock zone and phase. */
    std::unordered_map<tile_clock_number, bill::var_type> variables{};
    /** @brief Distinct occupied clock zones, shared across all layers. */
    std::vector<tile<Lyt>> clock_zones{};

    /** @brief Returns the zero-layer clock-zone coordinate. @param id Placed object. @return Clock zone. */
    [[nodiscard]] tile<Lyt> clock_zone(const typename Lyt::object_id id) const
    {
        const auto t = layout.get_tile(id);
        return {t.x, t.y, 0};
    }
    /** @brief Requires one phase in each occupied clock zone. */
    void at_least_one_clock_number_per_tile()
    {
        for (const auto& zone : clock_zones)
        {
            std::vector<bill::var_type> phases{};
            phases.reserve(number_of_clocks);
            for (typename Lyt::clock_number_t clk{}; clk < number_of_clocks; ++clk)
            {
                phases.push_back(variables.at({zone, clk}));
            }
            bill::at_least_one(phases, solver);
        }
    }
    /** @brief Excludes multiple phases in one occupied clock zone. */
    void at_most_one_clock_number_per_tile()
    {
        for (const auto& zone : clock_zones)
        {
            for (typename Lyt::clock_number_t first{}; first < number_of_clocks; ++first)
            {
                for (typename Lyt::clock_number_t second = first + 1; second < number_of_clocks; ++second)
                {
                    solver.add_clause({{bill::lit_type{variables.at({zone, first}), bill::negative_polarity},
                                        bill::lit_type{variables.at({zone, second}), bill::negative_polarity}}});
                }
            }
        }
    }
    /** @brief Requires every declared source to precede its destination by one phase. */
    void exclude_clock_assignments_that_violate_information_flow()
    {
        layout.foreach_object(
            [this](const auto id)
            {
                const auto destination = clock_zone(id);
                layout.foreach_fanin(
                    id,
                    [this, &destination](const auto source)
                    {
                        const auto predecessor = clock_zone(source);
                        for (typename Lyt::clock_number_t dst{}; dst < number_of_clocks; ++dst)
                        {
                            for (typename Lyt::clock_number_t src{}; src < number_of_clocks; ++src)
                            {
                                if (static_cast<typename Lyt::clock_number_t>((src + 1) % number_of_clocks) != dst)
                                {
                                    solver.add_clause(
                                        {{bill::lit_type{variables.at({destination, dst}), bill::negative_polarity},
                                          bill::lit_type{variables.at({predecessor, src}), bill::negative_polarity}}});
                                }
                            }
                        }
                    });
            });
    }
    /** @brief Fixes the phase rotation along the first PI's first-sink chain. */
    void symmetry_breaking()
    {
        if (layout.num_pis() == 0)
        {
            return;
        }
        std::optional<typename Lyt::object_id>      current{layout.pi_at(0)};
        std::unordered_set<typename Lyt::object_id> visited{};
        typename Lyt::clock_number_t                clk{};
        while (current && visited.insert(*current).second)
        {
            solver.add_clause(variables.at({clock_zone(*current), clk}));
            std::optional<typename Lyt::object_id> next{};
            layout.foreach_sink(*current,
                                [&next](const auto destination)
                                {
                                    next = destination.object;
                                    return false;
                                });
            current = next;
            clk     = static_cast<typename Lyt::clock_number_t>((clk + 1) % number_of_clocks);
        }
    }
    /** @brief Prepares and commits model phases without partial layout updates. @param model SAT assignment. */
    void assign_clock_numbers(const bill::result::model_type& model)
    {
        auto scheme = layout.get_clocking_scheme();
        for (const auto& zone : clock_zones)
        {
            for (typename Lyt::clock_number_t clk{}; clk < number_of_clocks; ++clk)
            {
                if (model.at(variables.at({zone, clk})) == bill::lbool_type::true_)
                {
                    scheme.override_clock_number(zone.x, zone.y, clk);
                    break;
                }
            }
        }
        layout.replace_clocking_scheme(scheme);
    }
};

/** @brief Validates topology and dispatches clock assignment. @tparam Lyt Gate-level layout type. */
template <typename Lyt>
class determine_clocking_impl
{
  public:
    /** @brief Creates a clock-assignment operation. @param lyt Layout. @param p Parameters. @param st Statistics. */
    determine_clocking_impl(Lyt& lyt, const determine_clocking_params& p, determine_clocking_stats& st) :
            layout{lyt},
            params{p},
            stats{st}
    {}

    /** @brief Validates and solves the layout. @return Whether a clock assignment exists. */
    bool run()
    {
        // measure run time
        mockturtle::stopwatch stop{stats.time_total};

        if (layout.is_empty())
        {
            return true;
        }

        validate_layout();

        switch (params.sat_engine)
        {
            case bill::solvers::ghack:
            {
                return sat_clocking_handler<Lyt, bill::solvers::ghack>{layout}.determine_clocks();
            }
            case bill::solvers::glucose_41:
            {
                return sat_clocking_handler<Lyt, bill::solvers::glucose_41>{layout}.determine_clocks();
            }
            case bill::solvers::bsat2:
            {
                return sat_clocking_handler<Lyt, bill::solvers::bsat2>{layout}.determine_clocks();
            }
#ifndef BILL_WINDOWS_PLATFORM
            case bill::solvers::maple:
            {
                return sat_clocking_handler<Lyt, bill::solvers::maple>{layout}.determine_clocks();
            }
            case bill::solvers::bmcg:
            {
                return sat_clocking_handler<Lyt, bill::solvers::bmcg>{layout}.determine_clocks();
            }
#endif
            default:
            {
                return sat_clocking_handler<Lyt>{layout}.determine_clocks();
            }
        }
    }

  private:
    /** @brief Rejects missing inputs, nonadjacent connections, cycles, and objects outside the frame. */
    void validate_layout() const
    {
        std::unordered_map<typename Lyt::object_id, uint32_t> remaining_inputs{};
        std::vector<typename Lyt::object_id>                  ready{};
        ready.reserve(layout.size());
        layout.foreach_object(
            [&](const auto id)
            {
                const auto t = layout.get_tile(id);
                if (!layout.is_within_bounds(t))
                {
                    throw std::invalid_argument("Clock assignment requires every object inside the layout frame");
                }
                const auto arity = layout.input_count(id);
                remaining_inputs.emplace(id, arity);
                if (arity == 0)
                {
                    ready.push_back(id);
                }
                for (uint32_t input{}; input < arity; ++input)
                {
                    const auto source = layout.source({id, input});
                    if (!source)
                    {
                        throw std::invalid_argument("Clock assignment requires every input to be connected");
                    }
                    if (!layout.is_adjacent_elevation_of(t, layout.get_tile(*source)))
                    {
                        throw std::invalid_argument("Clock assignment requires adjacent connected objects");
                    }
                }
            });
        for (std::size_t next{}; next < ready.size(); ++next)
        {
            layout.foreach_sink(ready[next],
                                [&](const auto sink)
                                {
                                    if (--remaining_inputs.at(sink.object) == 0)
                                    {
                                        ready.push_back(sink.object);
                                    }
                                });
        }
        if (ready.size() != layout.size())
        {
            throw std::invalid_argument("Clock assignment requires acyclic connections");
        }
    }
    /**
     * The layout to assign clock numbers to.
     */
    Lyt& layout;
    /**
     * Parameters.
     */
    determine_clocking_params params;
    /**
     * Statistics.
     */
    determine_clocking_stats& stats;
};

}  // namespace detail

/**
 * Determines clock numbers for the given (unclocked) gate-level layout. This algorithm parses the layout's gate and
 * wire connections, disregarding any existing clocking information, and constructs a SAT instance to find a valid clock
 * number assignment under which the information flow is respected. On success, occupied clock zones use the solved
 * phases. The stored clocking scheme retains its name and phase count.
 *
 * All objects must lie inside the frame, every input must be connected to an adjacent source, and connections must
 * be acyclic. Existing clock assignments need not respect those connections. Clock zones span all layers.
 *
 * If no valid clock number assignment exists for `lyt`, this function returns `false`. Failure preserves the layout
 * and its clocking scheme. A successful assignment preserves synchronization delays and unoccupied clock overrides.
 *
 * This algorithm was proposed in \"Ending the Tyranny of the Clock: SAT-based Clock Number Assignment for Field-coupled
 * Nanotechnologies\" by M. Walter, J. Drewniok, and R. Wille in IEEE NANO 2024
 * (https://ieeexplore.ieee.org/abstract/document/10628908).
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt The gate-level layout to assign clock numbers to.
 * @param params Parameters.
 * @param stats Statistics.
 * @return `true` iff `lyt` could be successfully clocked via a valid clock number assignment.
 * @throws std::invalid_argument If placement or declared connections violate the required topology.
 * @throws std::bad_alloc If allocation fails.
 */
template <typename Lyt>
bool determine_clocking(Lyt& lyt, const determine_clocking_params& params = {},
                        determine_clocking_stats* stats = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    determine_clocking_stats             st{};
    detail::determine_clocking_impl<Lyt> p{lyt, params, st};

    const auto result = p.run();

    if (stats)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction::physical_design
