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
#include "pyfiction/progress.hpp"
#include "pyfiction/types.hpp"

#include <fiction/networks/name_utils.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/node_duplication_planarization.hpp>
#include <fiction/synthesis/planarization.hpp>

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

namespace pyfiction
{

namespace detail
{

/**
 * Copies a planar `virtual_pi_network` into a plain network whose node order encodes the rank order: real primary
 * inputs first, then the virtual ones, then the gates level by level in rank order. The returned vector maps every
 * virtual input, in the order they appear after the real ones, to the index of its real input.
 *
 * @tparam Planar Planar network type.
 * @param planar Planar network.
 * @return The plain network and the virtual-to-real input map.
 */
template <typename Planar>
std::pair<py_tec_network, std::vector<uint32_t>> flatten(const Planar& planar)
{
    py_tec_network                                                   dest{};
    mockturtle::node_map<mockturtle::signal<py_tec_network>, Planar> old2new{planar};
    std::vector<uint32_t>                                            virtual_to_real{};

    old2new[planar.get_constant(false)] = dest.get_constant(false);
    old2new[planar.get_constant(true)]  = dest.get_constant(true);

    // real inputs keep their order; their index is their position among the real inputs
    mockturtle::node_map<uint32_t, Planar> real_index{planar};
    uint32_t                               next_real = 0;

    planar.foreach_pi_unranked(
        [&](const auto& n)
        {
            if (planar.is_real_pi(n))
            {
                old2new[n]    = dest.create_pi();
                real_index[n] = next_real++;
            }
        });
    planar.foreach_pi_unranked(
        [&](const auto& n)
        {
            if (!planar.is_real_pi(n))
            {
                old2new[n] = dest.create_pi();
                virtual_to_real.push_back(real_index[planar.get_real_pi(n)]);
            }
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

    return {dest, virtual_to_real};
}

}  // namespace detail

void planarization(nanobind::module_& m)
{
    namespace py = nanobind;

    using params = fiction::synthesis::node_duplication_planarization_params;

    py::enum_<params::planarization_strategy>(
        m, "planarization_strategy",
        DOC(fiction_synthesis_node_duplication_planarization_params_planarization_strategy))
        .value("DUPLICATION", params::planarization_strategy::DUPLICATION,
               DOC(fiction_synthesis_node_duplication_planarization_params_planarization_strategy_DUPLICATION))
        .value("HYBRID", params::planarization_strategy::HYBRID,
               DOC(fiction_synthesis_node_duplication_planarization_params_planarization_strategy_HYBRID));

    py::enum_<params::decision_criterion>(
        m, "decision_criterion", DOC(fiction_synthesis_node_duplication_planarization_params_decision_criterion))
        .value("WEIGHTED_CONE", params::decision_criterion::WEIGHTED_CONE,
               DOC(fiction_synthesis_node_duplication_planarization_params_decision_criterion_WEIGHTED_CONE))
        .value("LOOKAHEAD", params::decision_criterion::LOOKAHEAD,
               DOC(fiction_synthesis_node_duplication_planarization_params_decision_criterion_LOOKAHEAD));

    py::class_<params::duplication_cost_model>(
        m, "duplication_cost_model",
        DOC(fiction_synthesis_node_duplication_planarization_params_duplication_cost_model))
        .def(py::init<>(), "Default constructor.")
        .def_rw("base", &params::duplication_cost_model::base,
                DOC(fiction_synthesis_node_duplication_planarization_params_duplication_cost_model_base))
        .def_rw("amplitude", &params::duplication_cost_model::amplitude,
                DOC(fiction_synthesis_node_duplication_planarization_params_duplication_cost_model_amplitude))
        .def_rw("level_growth", &params::duplication_cost_model::level_growth,
                DOC(fiction_synthesis_node_duplication_planarization_params_duplication_cost_model_level_growth))
        .def_rw("depth_growth", &params::duplication_cost_model::depth_growth,
                DOC(fiction_synthesis_node_duplication_planarization_params_duplication_cost_model_depth_growth))
        .def_rw("buffer_weight", &params::duplication_cost_model::buffer_weight,
                DOC(fiction_synthesis_node_duplication_planarization_params_duplication_cost_model_buffer_weight));

    py::class_<params>(m, "node_duplication_planarization_params",
                       DOC(fiction_synthesis_node_duplication_planarization_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("strategy", &params::strategy, DOC(fiction_synthesis_node_duplication_planarization_params_strategy))
        .def_rw("criterion", &params::criterion, DOC(fiction_synthesis_node_duplication_planarization_params_criterion))
        .def_rw("xor_gates", &params::xor_gates, DOC(fiction_synthesis_node_duplication_planarization_params_xor_gates))
        .def_rw("max_crossings_per_rank", &params::max_crossings_per_rank,
                DOC(fiction_synthesis_node_duplication_planarization_params_max_crossings_per_rank))
        .def_rw("duplication_cost", &params::duplication_cost,
                DOC(fiction_synthesis_node_duplication_planarization_params_duplication_cost))
        .def_rw("lookahead_budget", &params::lookahead_budget,
                DOC(fiction_synthesis_node_duplication_planarization_params_lookahead_budget))
        .def_rw("max_duplications", &params::max_duplications,
                DOC(fiction_synthesis_node_duplication_planarization_params_max_duplications))
        .def_rw("seed", &params::seed, DOC(fiction_synthesis_node_duplication_planarization_params_seed));

    py::class_<fiction::synthesis::planarization_params>(m, "planarization_params",
                                                         DOC(fiction_synthesis_planarization_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("on_progress", &fiction::synthesis::planarization_params::on_progress, pyfiction::ON_PROGRESS_GETTER,
                pyfiction::CALLBACK_SETTER, "Receives completed work and the phase total.")
        .def_rw("duplication", &fiction::synthesis::planarization_params::duplication,
                DOC(fiction_synthesis_planarization_params_duplication))
        .def_rw("fanout_degree", &fiction::synthesis::planarization_params::fanout_degree,
                DOC(fiction_synthesis_planarization_params_fanout_degree));

    py::class_<fiction::synthesis::planarization_stats>(m, "planarization_stats",
                                                        DOC(fiction_synthesis_planarization_stats))
        .def(py::init<>(), "Default constructor.")
        .def_ro("time_total", &fiction::synthesis::planarization_stats::time_total,
                DOC(fiction_synthesis_planarization_stats_time_total))
        .def_prop_ro(
            "num_duplications",
            [](const fiction::synthesis::planarization_stats& st) { return st.duplication.num_duplications; },
            DOC(fiction_synthesis_node_duplication_planarization_stats_num_duplications))
        .def_prop_ro(
            "num_crossing_levels",
            [](const fiction::synthesis::planarization_stats& st) { return st.duplication.num_crossing_levels; },
            DOC(fiction_synthesis_node_duplication_planarization_stats_num_crossing_levels))
        .def_prop_ro(
            "num_crossings",
            [](const fiction::synthesis::planarization_stats& st) { return st.crossing_gates.num_crossings; },
            DOC(fiction_synthesis_crossing_gate_planarization_stats_num_crossings))
        .def_ro("num_nodes", &fiction::synthesis::planarization_stats::num_nodes,
                DOC(fiction_synthesis_planarization_stats_num_nodes))
        .def("__repr__",
             [](const fiction::synthesis::planarization_stats& st)
             {
                 std::ostringstream os{};
                 st.report(os);
                 return os.str();
             });

    m.def(
        "planarization",
        [](const py_tec_network& network, const fiction::synthesis::planarization_params& ps,
           fiction::synthesis::planarization_stats* pst)
        {
            // the pipeline needs a balanced network with unified outputs and ranks; the ranks follow the node order
            if (!fiction::synthesis::is_balanced(network, {.unify_outputs = true}))
            {
                throw std::invalid_argument("The network must be balanced with unified outputs; see network_balancing");
            }

            const fiction::networks::views::mutable_rank_view ranked{network};

            return detail::flatten(fiction::synthesis::planarization(ranked, ps, pst));
        },
        py::arg("network"), py::arg("params") = fiction::synthesis::planarization_params{},
        py::arg("statistics") = nullptr, DOC(fiction_synthesis_planarization),
        py::call_guard<py::gil_scoped_release>());
}

}  // namespace pyfiction
