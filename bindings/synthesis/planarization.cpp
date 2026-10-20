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
 * @brief Python bindings for `fiction/synthesis/planarization.hpp`.
 * @author Benjamin Hien (hibenj)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/networks/name_utils.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/node_duplication_planarization.hpp>
#include <fiction/synthesis/planarization.hpp>

#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/chrono.h>       // NOLINT(misc-include-cleaner): converts the runtime of the statistics
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner): converts the statistics report
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * Copies a planar `virtual_pi_network` into a plain network whose node order encodes the rank order: the primary
 * inputs in the order of level 0, then the gates level by level in rank order. The returned vector holds, for every
 * primary input of the result in that order, the index of the input of the original network it stands for; a virtual
 * input maps to the index of its real input.
 *
 * @tparam Planar Planar network type.
 * @param planar Planar network.
 * @return The plain network and the original input index of every input.
 */
template <typename Planar>
std::pair<py_tec_network, std::vector<uint32_t>> flatten(const Planar& planar)
{
    py_tec_network                                                   dest{};
    mockturtle::node_map<mockturtle::signal<py_tec_network>, Planar> old2new{planar};
    std::vector<uint32_t>                                            original_input{};

    old2new[planar.get_constant(false)] = dest.get_constant(false);
    old2new[planar.get_constant(true)]  = dest.get_constant(true);

    // the index of every real input in the original network is its creation order
    mockturtle::node_map<uint32_t, Planar> real_index{planar};
    uint32_t                               next_real = 0;

    planar.foreach_pi_unranked(
        [&](const auto& n)
        {
            if (planar.is_real_pi(n))
            {
                real_index[n] = next_real++;
            }
        });

    // inputs in rank order
    planar.foreach_pi(
        [&](const auto& n)
        {
            old2new[n] = dest.create_pi();
            original_input.push_back(real_index[planar.is_real_pi(n) ? n : planar.get_real_pi(n)]);
        });

    for (uint32_t level = 1; level <= planar.depth(); ++level)
    {
        planar.foreach_node_in_rank(level,
                                    [&](const auto& n)
                                    {
                                        if (planar.is_constant(n) || planar.is_pi(n))
                                        {
                                            return;
                                        }

                                        std::vector<mockturtle::signal<py_tec_network>> children{};
                                        planar.foreach_fanin(n, [&](const auto& f)
                                                             { children.push_back(old2new[planar.get_node(f)]); });

                                        old2new[n] = dest.create_node(children, planar.node_function(n));
                                    });
    }

    planar.foreach_po([&](const auto& po) { dest.create_po(old2new[planar.get_node(po)]); });

    fiction::networks::restore_names(planar, dest, old2new);

    return {dest, original_input};
}

}  // namespace detail

/**
 * @brief Binds the planarization pipeline.
 * @param m Python module.
 */
void register_planarization(nb::module_& m)
{
    using params = fiction::synthesis::node_duplication_planarization_params;
    using stats  = fiction::synthesis::planarization_stats;

    // the keyword defaults are those of the pipeline, which differ from the duplication stage's own (strategy)
    const auto defaults = fiction::synthesis::planarization_params{};

    nb::enum_<params::planarization_strategy>(
        m, "planarization_strategy",
        DOC(fiction_synthesis_node_duplication_planarization_params_planarization_strategy))
        .value("DUPLICATION", params::planarization_strategy::DUPLICATION,
               DOC(fiction_synthesis_node_duplication_planarization_params_planarization_strategy_DUPLICATION))
        .value("HYBRID", params::planarization_strategy::HYBRID,
               DOC(fiction_synthesis_node_duplication_planarization_params_planarization_strategy_HYBRID));

    nb::enum_<params::decision_criterion>(
        m, "decision_criterion", DOC(fiction_synthesis_node_duplication_planarization_params_decision_criterion))
        .value("WEIGHTED_CONE", params::decision_criterion::WEIGHTED_CONE,
               DOC(fiction_synthesis_node_duplication_planarization_params_decision_criterion_WEIGHTED_CONE))
        .value("LOOKAHEAD", params::decision_criterion::LOOKAHEAD,
               DOC(fiction_synthesis_node_duplication_planarization_params_decision_criterion_LOOKAHEAD));

    nb::enum_<params::output_order>(m, "output_order",
                                    DOC(fiction_synthesis_node_duplication_planarization_params_output_order))
        .value("KEEP_PO_ORDER", params::output_order::KEEP_PO_ORDER,
               DOC(fiction_synthesis_node_duplication_planarization_params_output_order_KEEP_PO_ORDER))
        .value("RANDOM_PO_ORDER", params::output_order::RANDOM_PO_ORDER,
               DOC(fiction_synthesis_node_duplication_planarization_params_output_order_RANDOM_PO_ORDER));

    nb::class_<stats>(m, "planarization_stats", DOC(fiction_synthesis_planarization_stats))
        .def(nb::init<>(), "Default constructor.")
        .def_ro("time_total", &stats::time_total, DOC(fiction_synthesis_planarization_stats_time_total))
        .def_prop_ro(
            "num_duplications", [](const stats& st) { return st.duplication.num_duplications; },
            DOC(fiction_synthesis_node_duplication_planarization_stats_num_duplications))
        .def_prop_ro(
            "num_crossing_levels", [](const stats& st) { return st.duplication.num_crossing_levels; },
            DOC(fiction_synthesis_node_duplication_planarization_stats_num_crossing_levels))
        .def_prop_ro(
            "num_crossings", [](const stats& st) { return st.crossing_gates.num_crossings; },
            DOC(fiction_synthesis_crossing_gate_planarization_stats_num_crossings))
        .def_ro("num_nodes", &stats::num_nodes, DOC(fiction_synthesis_planarization_stats_num_nodes))
        .def("__repr__",
             [](const stats& st)
             {
                 std::ostringstream os{};
                 st.report(os);
                 return os.str();
             });

    m.def(
        "planarization",
        [](const py_tec_network& network, const decltype(params{}.on_progress)& on_progress,
           const decltype(params{}.strategy) strategy, const decltype(params{}.criterion) criterion,
           const decltype(params{}.crossing_gates.xor_gates) xor_gates, const decltype(params{}.max_swaps) max_swaps,
           const decltype(params{}.crossing_gates.max_crossings_per_level) max_crossings_per_level,
           const decltype(params{}.duplication_cost.node_weight)           node_weight,
           const decltype(params{}.duplication_cost.buffer_weight)         buffer_weight,
           const decltype(params{}.duplication_cost.depth_growth)          depth_growth,
           const decltype(params{}.lookahead_budget)                       lookahead_budget,
           const decltype(params{}.max_duplications) max_duplications, const decltype(params{}.po_order) po_order,
           const decltype(params{}.seed)&                                           seed,
           const decltype(fiction::synthesis::planarization_params{}.fanout_degree) fanout_degree, stats* statistics)
        {
            fiction::synthesis::planarization_params ps{};
            ps.on_progress                                        = on_progress;
            ps.duplication.strategy                               = strategy;
            ps.duplication.criterion                              = criterion;
            ps.duplication.crossing_gates.xor_gates               = xor_gates;
            ps.duplication.max_swaps                              = max_swaps;
            ps.duplication.crossing_gates.max_crossings_per_level = max_crossings_per_level;
            ps.duplication.duplication_cost.node_weight           = node_weight;
            ps.duplication.duplication_cost.buffer_weight         = buffer_weight;
            ps.duplication.duplication_cost.depth_growth          = depth_growth;
            ps.duplication.lookahead_budget                       = lookahead_budget;
            ps.duplication.max_duplications                       = max_duplications;
            ps.duplication.po_order                               = po_order;
            ps.duplication.seed                                   = seed;
            ps.fanout_degree                                      = fanout_degree;

            // gates that no output reaches have no level and cannot be ranked, so they are dropped
            const auto cleaned = mockturtle::cleanup_dangling(network);

            // the pipeline needs a balanced network with unified outputs and ranks; the ranks follow the node order.
            // The result's node order is its rank order, and every input is reported with the index of the original
            // input it stands for, so that virtual inputs can be tied to their real ones.
            if (!fiction::synthesis::is_balanced(cleaned, {.unify_outputs = true, .buffer_constant_outputs = false}))
            {
                throw std::invalid_argument("The network must be balanced with unified outputs; see network_balancing");
            }

            const nb::gil_scoped_release release{};

            const fiction::networks::views::mutable_rank_view ranked{cleaned};

            return detail::flatten(fiction::synthesis::planarization(ranked, ps, statistics));
        },
        nb::arg("network"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("strategy") = defaults.duplication.strategy, nb::arg("criterion") = defaults.duplication.criterion,
        nb::arg("xor_gates")               = defaults.duplication.crossing_gates.xor_gates,
        nb::arg("max_swaps")               = defaults.duplication.max_swaps,
        nb::arg("max_crossings_per_level") = defaults.duplication.crossing_gates.max_crossings_per_level,
        nb::arg("node_weight")             = defaults.duplication.duplication_cost.node_weight,
        nb::arg("buffer_weight")           = defaults.duplication.duplication_cost.buffer_weight,
        nb::arg("depth_growth")            = defaults.duplication.duplication_cost.depth_growth,
        nb::arg("lookahead_budget")        = defaults.duplication.lookahead_budget,
        nb::arg("max_duplications")        = defaults.duplication.max_duplications,
        nb::arg("po_order") = defaults.duplication.po_order, nb::arg("seed") = defaults.duplication.seed,
        nb::arg("fanout_degree") = defaults.fanout_degree, nb::arg("statistics") = nullptr,
        R"doc(Planarizes a balanced technology network with unified outputs: node duplication with the chosen
strategy, crossing gates for the levels the hybrid strategy left crossed, planar fanout substitution, and planar
rebalancing. See `network_balancing` for the precondition. Gates that no output reaches are dropped. The function
checks the result for crossings and for balance with unified outputs; `is_fanout_substituted` checks its fanout degree.

Args:
    network: Balanced technology network with unified outputs.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    strategy: Planarization strategy: duplicate every level, or decide per level (hybrid).
    criterion: How the hybrid strategy estimates the cost of duplicating a level.
    xor_gates: Whether the crossing gates consist of XOR gates, which are smaller and shallower than the
        AND-OR-NOT crossing gates.
    max_swaps: Maximum number of adjacent swaps per level that the hybrid strategy tries to reduce crossings.
    max_crossings_per_level: Levels with more crossings are always duplicated; the crossing gates reject such levels.
    node_weight: Weight of a gate in the duplication cost model of the hybrid strategy.
    buffer_weight: Weight of a buffer or inverter chain node in the duplication cost model.
    depth_growth: Growth of the duplication cost per level in the duplication cost model.
    lookahead_budget: Nodes a lookahead may create before it is cut off; must be positive.
    max_duplications: Abort once more than this many nodes are duplicated at once; 0 lifts the limit.
    po_order: Primary output order used to seed the first level.
    seed: Seed of the random primary output order; None draws a random seed.
    fanout_degree: Maximum output degree of the fanout nodes in the result; at least 2.
    statistics: Statistics object filled by the pipeline; None discards statistics.

Returns:
    A pair of the planar, balanced, fanout-substituted network in rank order and, for every input of that
    network, the index of the input of `network` it stands for. A duplicated input appears several times; tie
    those inputs together when simulating or checking equivalence.

Raises:
    ValueError: If `network` is not balanced with unified outputs, if a gate has only constant fanins, if
        `fanout_degree` is below 2, or if the duplication options are invalid (a lookahead budget of zero, a
        negative or non-finite cost weight).
    RuntimeError: If more than `max_duplications` nodes were duplicated, if two adjacent
        levels have more than `max_crossings_per_level` crossings, or if a stage cannot keep its contract.
        `max_duplications = 0` lifts the limit, and node duplication can grow exponentially with the depth of the
        network.
)doc");
}

}  // namespace pyfiction
