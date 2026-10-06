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
 * @brief Sweeps the duplication cost model of the hybrid planarization strategy over benchmark networks.
 * @author Benjamin Hien (hibenj)
 */

#include "fiction_experiments.hpp"

#include <fiction/networks/io/network_reader.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/crossing_gate_planarization.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/node_duplication_planarization.hpp>
#include <fiction/synthesis/planar_fanout_substitution.hpp>
#include <fiction/synthesis/planar_rebalancing.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>
#include <mockturtle/algorithms/cleanup.hpp>
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

namespace
{

/**
 * Benchmarks whose balanced network has more nodes are skipped.
 */
constexpr uint32_t MAX_BALANCED_NODES = 8000;
/**
 * Duplication limit per planarization, so that a bad setting cannot run away.
 */
constexpr uint64_t MAX_DUPLICATIONS = 500000;

/**
 * One setting of the cost model.
 */
struct setting
{
    /**
     * Name of the varied parameter.
     */
    std::string parameter;
    /**
     * Parameters of the planarization.
     */
    node_duplication_planarization_params ps;
};

/**
 * Builds the one-factor-at-a-time settings around the defaults, plus the degenerate models.
 *
 * @return Settings to sweep.
 */
std::vector<setting> settings()
{
    using params = node_duplication_planarization_params;

    const auto base = []
    {
        params ps{};
        ps.strategy         = params::planarization_strategy::HYBRID;
        ps.xor_gates        = true;
        ps.max_duplications = MAX_DUPLICATIONS;
        return ps;
    };

    std::vector<setting> result{};

    result.push_back({"default", base()});

    {
        auto ps      = base();
        ps.criterion = params::decision_criterion::WEIGHTED_CONE;
        result.push_back({"weighted cone", ps});
    }
    for (const double v : {1.0, 1.5, 3.0, 4.0})
    {
        auto ps                         = base();
        ps.criterion                    = params::decision_criterion::WEIGHTED_CONE;
        ps.duplication_cost.node_weight = v;
        result.push_back({fmt::format("node_weight={}", v), ps});
    }
    for (const double v : {0.0, 1.0, 2.0})
    {
        auto ps                           = base();
        ps.criterion                      = params::decision_criterion::WEIGHTED_CONE;
        ps.duplication_cost.buffer_weight = v;
        result.push_back({fmt::format("buffer_weight={}", v), ps});
    }
    for (const double v : {1.0, 1.01, 1.05, 1.1})
    {
        auto ps                          = base();
        ps.criterion                     = params::decision_criterion::WEIGHTED_CONE;
        ps.duplication_cost.depth_growth = v;
        result.push_back({fmt::format("depth_growth={}", v), ps});
    }
    for (const uint32_t v : {0u, 8u, 128u})
    {
        auto ps      = base();
        ps.max_swaps = v;
        result.push_back({fmt::format("max_swaps={}", v), ps});
    }
    {
        auto ps             = base();
        ps.lookahead_budget = 100000u;
        result.push_back({"lookahead budget=1e5", ps});
    }
    {
        auto ps                   = base();
        ps.max_crossings_per_rank = 10000u;
        result.push_back({"crossings<=1e4", ps});
    }
    {
        auto ps     = base();
        ps.strategy = params::planarization_strategy::DUPLICATION;
        result.push_back({"duplication only", ps});
    }

    return result;
}

/**
 * Reads the first network of a benchmark file.
 *
 * @param name Benchmark name relative to `benchmarks/`, without extension.
 * @return The network.
 */
tec_nt read_network(const std::string& name)
{
    std::ostringstream      os{};
    network_reader<tec_ptr> reader{fiction_experiments::benchmark_path(name), os};

    return *reader.get_networks().front();
}

}  // namespace

int main(const int argc, const char** argv)  // NOLINT
{
    const std::span<const char*> args{argv, static_cast<std::size_t>(argc)};
    const std::string            filter = args.size() > 1 ? args[1] : "";

    experiments::experiment<std::string, std::string, uint32_t, uint32_t, uint64_t, uint64_t, uint32_t, uint32_t,
                            double>
        sweep_exp{"planarization_cost_model_sweep",
                  "benchmark",
                  "setting",
                  "balanced nodes",
                  "planarized nodes",
                  "crossing levels",
                  "kept crossings",
                  "with gadgets",
                  "final nodes",
                  "time (s)"};

    auto       benchmarks = fiction_experiments::all_benchmarks();
    const auto iwls93     = fiction_experiments::iwls93_benchmarks();
    benchmarks.insert(benchmarks.end(), iwls93.cbegin(), iwls93.cend());

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

            for (const auto& [name, ps] : settings())
            {
                try
                {
                    mockturtle::stopwatch<>::duration    time{};
                    node_duplication_planarization_stats st{};
                    uint32_t                             planarized = 0;
                    uint32_t                             gadgets    = 0;
                    uint32_t                             final_size = 0;
                    {
                        const mockturtle::stopwatch stop{time};

                        const auto hybrid = node_duplication_planarization(ranked, ps, &st);
                        planarized        = hybrid.size();

                        crossing_gate_planarization_params cg_ps{};
                        cg_ps.xor_gates              = true;
                        cg_ps.max_crossings_per_rank = ps.max_crossings_per_rank;

                        const auto with_gadgets = crossing_gate_planarization(hybrid, cg_ps);
                        gadgets                 = with_gadgets.size();

                        final_size = planar_rebalancing(planar_fanout_substitution(with_gadgets)).size();
                    }

                    sweep_exp(benchmark, name, balanced.size(), planarized, st.num_crossing_levels, st.num_crossings,
                              gadgets, final_size, mockturtle::to_seconds(time));
                }
                catch (const std::exception& e)
                {
                    fmt::print("[w] {} / {}: {}\n", benchmark, name, e.what());
                }
            }

            sweep_exp.save();
            sweep_exp.table();
        }
        catch (const std::exception& e)
        {
            fmt::print("[e] {}: {}\n", benchmark, e.what());
        }
    }

    return 0;
}
