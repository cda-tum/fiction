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
 * @brief Runs the planarization pipeline and PLANE over benchmark networks and verifies every layout.
 * @author Benjamin Hien (hibenj)
 */

#include "fiction_experiments.hpp"

#include <fiction/networks/io/network_reader.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/physical_design/plane.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/planarization.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>
#include <kitty/partial_truth_table.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <span>
#include <sstream>
#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::io;
using namespace fiction::networks::views;
using namespace fiction::synthesis;
using namespace fiction::physical_design;

namespace
{

/**
 * Balanced networks above this size are skipped.
 */
constexpr uint32_t MAX_BALANCED_NODES = 8000;
/**
 * Layouts up to this many nodes are checked with SAT, larger ones by simulation.
 */
constexpr uint32_t MAX_NODES_FOR_SAT = 200000;
/**
 * Random patterns of the simulation fallback.
 */
constexpr uint32_t SIMULATION_PATTERNS = 4096;
/**
 * Duplication cap of the pipeline.
 */
constexpr uint64_t MAX_DUPLICATIONS = 2000000;

tec_nt read_network(const std::string& name)
{
    std::ostringstream      os{};
    network_reader<tec_ptr> reader{fiction_experiments::benchmark_path(name), os};

    return *reader.get_networks().front();
}

/**
 * Compares the layout with the planar network it was built from. Both have the same inputs in the same order (the
 * layout creates its inputs in the input order of the network), so a plain miter suffices.
 */
template <typename Ntk, typename Lyt>
std::pair<bool, std::string> layout_equivalent(const Ntk& ntk, const Lyt& lyt)
{
    if (lyt.size() > MAX_NODES_FOR_SAT)
    {
        using tt_t = kitty::partial_truth_table;
        const mockturtle::partial_simulator sim{ntk.num_pis(), SIMULATION_PATTERNS};

        const auto reference = mockturtle::simulate<tt_t>(ntk, sim);
        const auto candidate = mockturtle::simulate<tt_t>(lyt, sim);

        return {reference == candidate, "simulation"};
    }

    const auto miter = mockturtle::miter<technology_network>(ntk, lyt);

    if (!miter.has_value())
    {
        return {false, "miter failed"};
    }

    mockturtle::equivalence_checking_stats st{};
    const auto                             cec = mockturtle::equivalence_checking(*miter, {}, &st);

    return {cec.value_or(false), "SAT"};
}

}  // namespace

int main(const int argc, const char** argv)  // NOLINT
{
    const std::span<const char*> args{argv, static_cast<std::size_t>(argc)};
    const std::string            filter = args.size() > 1 ? args[1] : "";
    // optional second argument "xor": build the crossing gadgets from XOR gates as in the paper's experiments
    const bool xor_gates = args.size() > 2 && std::string{args[2]} == "xor";

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, double, uint64_t, uint64_t, uint64_t, uint64_t,
                            uint64_t, double, bool, std::string, double, std::string>
        plane_exp{"plane",      "benchmark", "balanced nodes",  "planar nodes", "virtual PIs", "planarization (s)",
                  "layout x",   "layout y",  "gates",           "wires",        "crossings",   "plane (s)",
                  "equivalent", "method",    "equivalence (s)", "error"};

    auto       benchmarks = fiction_experiments::all_benchmarks();
    const auto iwls93     = fiction_experiments::iwls93_benchmarks();
    benchmarks.insert(benchmarks.end(), iwls93.cbegin(), iwls93.cend());

    uint32_t failures = 0;

    for (const auto& benchmark : benchmarks)
    {
        if (!filter.empty() && benchmark.find(filter) == std::string::npos)
        {
            continue;
        }

        fmt::print("[i] processing {}\n", benchmark);

        try
        {
            const auto ntk = mockturtle::cleanup_dangling(read_network(benchmark));

            network_balancing_params b_ps{};
            b_ps.unify_outputs  = true;
            const auto balanced = network_balancing<technology_network>(ntk, b_ps);

            if (balanced.size() > MAX_BALANCED_NODES)
            {
                fmt::print("[w] skipping {} with {} balanced nodes\n", benchmark, balanced.size());
                continue;
            }

            const mutable_rank_view ranked{balanced};

            fmt::print("[i]   planarizing {} nodes\n", balanced.size());
            planarization_params p_ps{};
            p_ps.duplication.max_duplications = MAX_DUPLICATIONS;
            p_ps.duplication.xor_gates        = xor_gates;
            planarization_stats p_st{};
            const auto          planar = planarization(ranked, p_ps, &p_st);

            fmt::print("[i]   placing and routing {} planar nodes\n", planar.size());
            plane_stats                       l_st{};
            std::string                       error{};
            uint64_t                          crossings  = 0;
            bool                              equivalent = false;
            std::string                       method{"-"};
            mockturtle::stopwatch<>::duration eq_time{};

            try
            {
                const auto layout = plane<cart_gate_clk_lyt>(planar, {}, &l_st);

                crossings = layout.num_crossings();

                fmt::print("[i]   checking equivalence of a {} x {} layout\n", l_st.x_size, l_st.y_size);
                const mockturtle::stopwatch stop{eq_time};
                const auto                  eq = layout_equivalent(planar, layout);
                equivalent                     = eq.first;
                method                         = eq.second;
            }
            catch (const std::exception& e)
            {
                error = e.what();
            }

            if (!error.empty() || crossings != 0 || !equivalent)
            {
                ++failures;
            }

            plane_exp(benchmark, balanced.size(), planar.size(), planar.num_virtual_pis(),
                      mockturtle::to_seconds(p_st.time_total), l_st.x_size, l_st.y_size, l_st.num_gates, l_st.num_wires,
                      crossings, mockturtle::to_seconds(l_st.time_total), equivalent, method,
                      mockturtle::to_seconds(eq_time), error);
            plane_exp.save();
            plane_exp.table();
        }
        catch (const std::exception& e)
        {
            fmt::print("[e] {} failed: {}\n", benchmark, e.what());
            ++failures;
        }
    }

    fmt::print("[i] {} benchmark(s) with failures\n", failures);

    return failures == 0 ? 0 : 1;
}
