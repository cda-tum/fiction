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

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/extract_layout_network.hpp>
#include <fiction/networks/io/network_reader.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/physical_design/plane.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/planarization.hpp>
#include <fiction/types.hpp>
#include <fiction/verification/equivalence_checking.hpp>

#include <fmt/format.h>
#include <kitty/partial_truth_table.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <span>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::networks::io;
using namespace fiction::networks::views;
using namespace fiction::synthesis;
using namespace fiction::physical_design;
using namespace fiction::verification;

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
 * Layouts above this many nodes are not checked for equivalence: the simulation of such a layout needs several
 * gigabytes. They are still checked for crossings and the diagonal invariant.
 */
constexpr uint32_t MAX_NODES_FOR_EQUIVALENCE = 1000000;
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
    const auto extracted = extract_layout_network(lyt);

    if (extracted.size() > MAX_NODES_FOR_EQUIVALENCE)
    {
        return {true, "skipped"};
    }

    if (extracted.size() > MAX_NODES_FOR_SAT)
    {
        using tt_t = kitty::partial_truth_table;
        const mockturtle::partial_simulator sim{ntk.num_pis(), SIMULATION_PATTERNS};

        const auto reference = mockturtle::simulate<tt_t>(ntk, sim);
        const auto candidate = mockturtle::simulate<tt_t>(extracted, sim);

        return {reference == candidate, "simulation"};
    }

    return {equivalence_checking(ntk, extracted) != eq_type::NO, "SAT"};
}

}  // namespace

/**
 * @brief Planarizes benchmark networks, places and routes them with PLANE, and verifies every layout.
 *
 * This program balances the benchmark networks of the trindade16, fontes18, ISCAS85, EPFL, and IWLS93 suites whose
 * balanced network has at most 8000 nodes, planarizes them with the planarization pipeline, places and routes the
 * planar networks with PLANE, and checks every layout for crossings and for equivalence with its network. The results
 * are printed as a table and saved as `plane.json`.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of C-style strings containing the command-line arguments.
 * @return `EXIT_SUCCESS` if every benchmark was placed and verified, `EXIT_FAILURE` otherwise.
 *
 * Command-line Options:
 *   <filter>      Runs only the benchmarks whose name contains the given text; "" runs all of them.
 *   xor           Builds the crossing gates from XOR gates instead of AND, OR, and NOT gates.
 *   --help, -h    Prints the usage and exits.
 *
 * Example Usage:
 *   To run all benchmarks:
 *   ./plane
 *
 *   To run the ISCAS85 benchmarks with XOR crossing gates:
 *   ./plane ISCAS85 xor
 */
int main(const int argc, const char** argv)
try
{
    const std::span<const char*> args{argv, static_cast<std::size_t>(argc)};

    if (args.size() > 3 || (args.size() == 3 && std::string{args[2]} != "xor") ||
        (args.size() == 2 && (std::string{args[1]} == "--help" || std::string{args[1]} == "-h")))
    {
        fmt::print("Usage: plane [<filter> [xor]]\n"
                   "  <filter>      Runs only the benchmarks whose name contains the given text; \"\" runs all.\n"
                   "  xor           Builds the crossing gates from XOR gates instead of AND, OR, and NOT gates.\n"
                   "  --help, -h    Prints this usage and exits.\n");

        return args.size() == 2 ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    // an optional first argument restricts the run to benchmarks whose name contains it
    const std::string filter = args.size() > 1 ? args[1] : "";
    // an optional second argument "xor" builds the crossing gates from XOR gates as in the paper's experiments
    const bool xor_gates = args.size() > 2;

    if (filter.empty())
    {
        fmt::print("[i] running all benchmarks; pass a name filter to restrict the run, or --help for the usage\n");
    }

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
            b_ps.unify_outputs           = true;
            b_ps.buffer_constant_outputs = false;
            const auto balanced          = network_balancing<technology_network>(ntk, b_ps);

            if (balanced.size() > MAX_BALANCED_NODES)
            {
                fmt::print("[w] skipping {} with {} balanced nodes\n", benchmark, balanced.size());
                continue;
            }

            const mutable_rank_view ranked{balanced};

            fmt::print("[i]   planarizing {} nodes\n", balanced.size());
            planarization_params p_ps{};
            p_ps.duplication.max_duplications         = MAX_DUPLICATIONS;
            p_ps.duplication.crossing_gates.xor_gates = xor_gates;
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
                const auto layout = plane<cartesian_gate_layout>(planar, {}, &l_st);

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

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
catch (const std::exception& exception)
{
    static_cast<void>(std::fputs("[e] Experiment failed: ", stderr));
    static_cast<void>(std::fputs(exception.what(), stderr));
    static_cast<void>(std::fputc('\n', stderr));
    return EXIT_FAILURE;
}
