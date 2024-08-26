//
// Created by benjamin on 8/26/24.
//

#include "experiments.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/properties/check_planarity.hpp"
#include "fiction/utils/debug/network_writer.hpp"
#include "fiction_experiments.hpp"

#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>        // scalable heuristic for physical design
#include <fiction/algorithms/physical_design/wiring_reduction.hpp>  // wiring reduction algorithm
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/algorithms/verification/virtual_miter.hpp>
#include <fiction/io/dot_drawers.hpp>
#include <fiction/io/network_reader.hpp>  // read networks from files
#include <fiction/networks/technology_network.hpp>
#include <fiction/types.hpp>

#include <fmt/core.h>
#include <fmt/format.h>  // output formatting
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/rank_view.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>

template <typename Ntk>
Ntk read_ntk(const std::string& name)
{
    fmt::print("[i] processing {}\n", name);

    std::ostringstream os{};

    fiction::network_reader<fiction::tec_ptr> reader{fiction_experiments::benchmark_path(name), os};

    const auto nets = reader.get_networks();

    return *nets.front();
}

int main()  // NOLINT
{
    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, bool, bool> wiring_reduction_exp{
        "planarization", "benchmark", "inputs", "outputs", "initial nodes", "nodes_after", "is_planar", "equivalent"};

    static constexpr const uint64_t bench_select =
        (fiction_experiments::trindade16 | fiction_experiments::fontes18) & ~fiction_experiments::clpl;
    // static constexpr const uint64_t bench_select = (fiction_experiments::one_bit_add_maj);

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto benchmark_network = read_ntk<fiction::tec_nt>(benchmark);

        fiction::network_balancing_params ps;
        ps.unify_outputs = true;

        const auto _b = fiction::network_balancing<fiction::technology_network>(
            fiction::fanout_substitution<fiction::technology_network>(benchmark_network), ps);

        const auto planarized_b = fiction::node_duplication_planarization<fiction::technology_network>(_b);

        const auto is_planar = fiction::check_planarity(planarized_b);

        // fiction::debug::write_dot_network(_b);

        // check equivalence
        mockturtle::equivalence_checking_stats st;
        const auto                             cec_m = mockturtle::equivalence_checking(
            *fiction::virtual_miter<fiction::technology_network>(benchmark_network, planarized_b), {}, &st);
        assert(cec_m.has_value());

        // log results
        wiring_reduction_exp(benchmark, benchmark_network.num_pis(), benchmark_network.num_pos(),
                             benchmark_network.num_gates(), _b.num_gates(), is_planar, static_cast<bool>(cec_m));

        wiring_reduction_exp.save();
        wiring_reduction_exp.table();
    }

    return EXIT_SUCCESS;
}