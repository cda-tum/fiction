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
 * @brief Planarizes benchmark networks by node duplication and verifies every result.
 * @author Benjamin Hien (hibenj)
 */

#include "fiction_experiments.hpp"

#include <fiction/networks/io/network_reader.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/crossing_gate_planarization.hpp>
#include <fiction/synthesis/fanout_substitution.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/node_duplication_planarization.hpp>
#include <fiction/synthesis/planar_fanout_substitution.hpp>
#include <fiction/synthesis/planar_rebalancing.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/graph/mincross.hpp>
#include <fiction/verification/virtual_miter.hpp>

#include <fmt/format.h>
#include <kitty/partial_truth_table.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <span>
#include <sstream>
#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::io;
using namespace fiction::networks::views;
using namespace fiction::synthesis;
using namespace fiction::utils::graph;
using namespace fiction::verification;

namespace
{

/**
 * Benchmarks whose balanced network has more nodes are skipped, as in the paper.
 */
constexpr uint32_t MAX_BALANCED_NODES = 8000;
/**
 * Planar networks with more nodes are checked by random simulation instead of SAT.
 */
constexpr uint32_t MAX_NODES_FOR_SAT = 50000;
/**
 * Number of random input patterns of the simulation-based check.
 */
constexpr uint32_t SIMULATION_PATTERNS = 4096;
/**
 * Planarizations that duplicate more nodes than this are aborted and reported as failures, since pure node
 * duplication can grow exponentially with the depth of the network.
 */
constexpr uint64_t MAX_DUPLICATIONS = 2000000;

/**
 * Reads the first network of a benchmark file.
 */
tec_nt read_network(const std::string& name)
{
    std::ostringstream      os{};
    network_reader<tec_ptr> reader{fiction_experiments::benchmark_path(name), os};

    return *reader.get_networks().front();
}

/**
 * Lists the IWLS93 benchmarks shipped in `benchmarks/IWLS93`, sorted by name.
 */
std::vector<std::string> iwls93_benchmarks()
{
    namespace fs = std::filesystem;

    std::vector<std::string> benchmarks{};

    const auto dir = fs::path{EXPERIMENTS_PATH} / "../benchmarks/IWLS93";

    if (!fs::is_directory(dir))
    {
        fmt::print("[w] IWLS93 directory not found: {}\n", dir.string());
        return benchmarks;
    }

    for (const auto& entry : fs::directory_iterator{dir})
    {
        if (entry.is_regular_file() && entry.path().extension() == ".v")
        {
            benchmarks.push_back(fmt::format("IWLS93/{}", entry.path().stem().string()));
        }
    }

    std::sort(benchmarks.begin(), benchmarks.end());

    return benchmarks;
}

/**
 * Counts the crossings of a ranked network without reordering it.
 */
template <typename Ntk>
uint64_t count_crossings(const Ntk& ntk)
{
    mincross_params ps{};
    ps.optimize = false;
    mincross_stats st{};

    mincross(ntk, ps, &st);

    return st.num_crossings;
}

/**
 * Result of an equivalence check.
 */
struct equivalence_result
{
    /**
     * Whether the networks were found equivalent.
     */
    bool equivalent{false};
    /**
     * `SAT` for a proof, `simulation` for a random-pattern check, `miter` if no miter could be built.
     */
    std::string method{"miter"};
};

/**
 * Simulates `SIMULATION_PATTERNS` random patterns on the original network and on the planarized one, where every
 * virtual primary input takes the value of its real one, and compares the outputs. Independent of `virtual_miter`,
 * which cannot handle rank views with unused primary inputs.
 */
template <typename Spec, typename Impl>
bool simulate_equivalent(const Spec& spec, const Impl& impl)
{
    using tt_t = kitty::partial_truth_table;

    const mockturtle::partial_simulator sim{spec.num_pis(), SIMULATION_PATTERNS};
    const auto                          reference = mockturtle::simulate<tt_t>(spec, sim);

    mockturtle::node_map<tt_t, Impl> tts{impl};
    tts[impl.get_node(impl.get_constant(false))] = tt_t{SIMULATION_PATTERNS};
    tts[impl.get_node(impl.get_constant(true))]  = ~tt_t{SIMULATION_PATTERNS};

    // real primary inputs are created in the order of the original network's inputs
    uint32_t real_index = 0;
    impl.foreach_pi_unranked(
        [&](const auto& n)
        {
            if (impl.is_real_pi(n))
            {
                tts[n] = sim.compute_pi(real_index++);
            }
            else
            {
                tts[n] = tts[impl.get_real_pi(n)];
            }
        });

    if (real_index != spec.num_pis())
    {
        return false;
    }

    const mockturtle::topo_view topo{impl};
    topo.foreach_gate(
        [&](const auto& n)
        {
            std::vector<tt_t> fanin_tts{};
            impl.foreach_fanin(n, [&](const auto& f) { fanin_tts.push_back(tts[impl.get_node(f)]); });
            tts[n] = impl.compute(n, fanin_tts.begin(), fanin_tts.end());
        });

    bool equal = impl.num_pos() == spec.num_pos();
    impl.foreach_po(
        [&](const auto& po, const auto i)
        {
            const auto tt = impl.is_complemented(po) ? ~tts[impl.get_node(po)] : tts[impl.get_node(po)];
            equal         = equal && i < reference.size() && tt == reference[i];
        });

    return equal;
}

/**
 * Checks equivalence of a planarized network against the original, virtual primary inputs included: by SAT on the
 * `virtual_miter` for results up to `MAX_NODES_FOR_SAT` nodes whose original has no unused primary inputs, by
 * random simulation otherwise.
 */
template <typename Spec, typename Impl>
equivalence_result is_equivalent(const Spec& spec, const Impl& impl)
{
    bool unused_pis = false;
    spec.foreach_pi([&spec, &unused_pis](const auto& n) { unused_pis = unused_pis || spec.fanout_size(n) == 0; });

    if (impl.size() > MAX_NODES_FOR_SAT || unused_pis)
    {
        return {simulate_equivalent(spec, impl), "simulation"};
    }

    const auto miter = virtual_miter<technology_network>(spec, impl);

    if (!miter.has_value())
    {
        return {};
    }

    mockturtle::equivalence_checking_stats st{};
    const auto                             cec = mockturtle::equivalence_checking(*miter, {}, &st);

    return {cec.value_or(false), "SAT"};
}

}  // namespace

int main(const int argc, const char** argv)  // NOLINT
{
    // an optional first argument restricts the run to benchmarks whose name contains it
    const std::span<const char*> args{argv, static_cast<std::size_t>(argc)};
    const std::string            filter = args.size() > 1 ? args[1] : "";

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint64_t, uint32_t,
                            uint32_t, uint64_t, double, bool, std::string, double, bool, uint64_t, uint32_t, uint32_t,
                            double, bool, bool, uint32_t, uint64_t, uint64_t, uint32_t, uint32_t, double, bool, bool,
                            double>
        planarization_exp{"planarization",
                          "benchmark",
                          "inputs",
                          "outputs",
                          "nodes",
                          "balanced nodes",
                          "width",
                          "depth",
                          "crossings",
                          "planar nodes",
                          "virtual PIs",
                          "duplications",
                          "time (s)",
                          "equivalent",
                          "method",
                          "equivalence time (s)",
                          "balanced",
                          "crossings after",
                          "substituted nodes",
                          "rebalanced nodes",
                          "pipeline time (s)",
                          "pipeline ok",
                          "pipeline equivalent",
                          "hybrid nodes",
                          "crossing levels",
                          "kept crossings",
                          "hybrid gadget nodes",
                          "hybrid rebalanced nodes",
                          "hybrid time (s)",
                          "hybrid ok",
                          "hybrid equivalent",
                          "size decrease (%)"};

    auto       benchmarks = fiction_experiments::all_benchmarks();
    const auto iwls93     = iwls93_benchmarks();
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
            b_ps.unify_outputs = true;

            const auto balanced = network_balancing<technology_network>(ntk, b_ps);

            if (balanced.size() > MAX_BALANCED_NODES)
            {
                fmt::print("[w] skipping {} with {} balanced nodes\n", benchmark, balanced.size());
                continue;
            }

            const mutable_rank_view ranked{balanced};

            fmt::print("[i]   counting crossings of {} nodes\n", balanced.size());
            const auto crossings_before = count_crossings(ranked);

            fmt::print("[i]   planarizing\n");
            node_duplication_planarization_params d_ps{};
            d_ps.max_duplications = MAX_DUPLICATIONS;

            node_duplication_planarization_stats st{};
            const auto                           planar = node_duplication_planarization(ranked, d_ps, &st);

            fmt::print("[i]   checking equivalence of {} planar nodes\n", planar.size());
            mockturtle::stopwatch<>::duration eq_time{};
            equivalence_result                eq{};
            {
                const mockturtle::stopwatch stop{eq_time};
                eq = is_equivalent(ntk, planar);
            }

            fmt::print("[i]   checking balance and crossings\n");
            const auto bal             = is_balanced(planar, b_ps);
            const auto crossings_after = count_crossings(planar);

            // the paper's post-processing: fanout trees plus minimal buffering, both rank- and planarity-preserving
            fmt::print("[i]   substituting fanouts and rebalancing\n");
            mockturtle::stopwatch<>::duration pipeline_time{};
            uint32_t                          substituted_nodes = 0;
            uint32_t                          rebalanced_nodes  = 0;
            bool                              pipeline_ok       = false;
            equivalence_result                pipeline_eq{};
            {
                const mockturtle::stopwatch stop{pipeline_time};
                const auto                  substituted = planar_fanout_substitution(planar);
                const auto                  rebalanced  = planar_rebalancing(substituted);
                substituted_nodes                       = substituted.size();
                rebalanced_nodes                        = rebalanced.size();
                pipeline_ok = is_balanced(rebalanced, b_ps) && is_fanout_substituted(rebalanced) &&
                              count_crossings(rebalanced) == 0;
                pipeline_eq = is_equivalent(ntk, rebalanced);
            }

            // the paper's hybrid: per-level choice, crossing gates with XOR gadgets, then the same post-processing
            fmt::print("[i]   hybrid planarization\n");
            mockturtle::stopwatch<>::duration    hybrid_time{};
            node_duplication_planarization_stats hybrid_st{};
            uint32_t                             hybrid_nodes        = 0;
            uint32_t                             hybrid_gadget_nodes = 0;
            uint32_t                             hybrid_rebalanced   = 0;
            bool                                 hybrid_ok           = false;
            equivalence_result                   hybrid_eq{};
            {
                const mockturtle::stopwatch stop{hybrid_time};

                node_duplication_planarization_params h_ps{};
                h_ps.strategy         = node_duplication_planarization_params::planarization_strategy::HYBRID;
                h_ps.xor_gates        = true;
                h_ps.max_duplications = MAX_DUPLICATIONS;

                const auto hybrid = node_duplication_planarization(ranked, h_ps, &hybrid_st);
                hybrid_nodes      = hybrid.size();

                crossing_gate_planarization_params cg_ps{};
                cg_ps.xor_gates = true;

                const auto gadgets  = crossing_gate_planarization(hybrid, cg_ps);
                hybrid_gadget_nodes = gadgets.size();

                const auto rebalanced = planar_rebalancing(planar_fanout_substitution(gadgets));
                hybrid_rebalanced     = rebalanced.size();
                hybrid_ok             = is_balanced(rebalanced, b_ps) && is_fanout_substituted(rebalanced) &&
                                        count_crossings(rebalanced) == 0;
                hybrid_eq             = is_equivalent(ntk, rebalanced);
            }

            const double decrease =
                rebalanced_nodes == 0 ?
                    0.0 :
                    100.0 * (1.0 - static_cast<double>(hybrid_rebalanced) / static_cast<double>(rebalanced_nodes));

            if (!eq.equivalent || !bal || crossings_after != 0 || !pipeline_ok || !pipeline_eq.equivalent ||
                !hybrid_ok || !hybrid_eq.equivalent)
            {
                ++failures;
                fmt::print("[e] {}: equivalent={} ({}) balanced={} crossings={} pipeline ok={} equivalent={} hybrid "
                           "ok={} equivalent={}\n",
                           benchmark, eq.equivalent, eq.method, bal, crossings_after, pipeline_ok,
                           pipeline_eq.equivalent, hybrid_ok, hybrid_eq.equivalent);
            }

            planarization_exp(benchmark, ntk.num_pis(), ntk.num_pos(), ntk.size(), balanced.size(), ranked.width(),
                              ranked.depth(), crossings_before, planar.size(), planar.num_virtual_pis(),
                              st.num_duplications, mockturtle::to_seconds(st.time_total), eq.equivalent, eq.method,
                              mockturtle::to_seconds(eq_time), bal, crossings_after, substituted_nodes,
                              rebalanced_nodes, mockturtle::to_seconds(pipeline_time), pipeline_ok,
                              pipeline_eq.equivalent, hybrid_nodes, hybrid_st.num_crossing_levels,
                              hybrid_st.num_crossings, hybrid_gadget_nodes, hybrid_rebalanced,
                              mockturtle::to_seconds(hybrid_time), hybrid_ok, hybrid_eq.equivalent, decrease);
            planarization_exp.save();
            planarization_exp.table();
        }
        catch (const std::exception& e)
        {
            ++failures;
            fmt::print("[e] {}: {}\n", benchmark, e.what());
        }
    }

    fmt::print("[i] {} of {} benchmarks failed\n", failures, benchmarks.size());

    return failures == 0 ? 0 : 1;
}
