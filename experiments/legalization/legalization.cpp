//
// Created by benjamin on 8/26/24.
//

#include "experiments.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/properties/check_planarity_balanced.hpp"
#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/clocked_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/layouts/gate_level_layout.hpp"
#include "fiction/layouts/tile_based_layout.hpp"
#include "fiction_experiments.hpp"

#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>  // scalable heuristic for physical design
#include <fiction/algorithms/verification/design_rule_violations.hpp>
#include <fiction/algorithms/verification/virtual_miter.hpp>
#include <fiction/io/network_reader.hpp>  // read networks from files
#include <fiction/networks/technology_network.hpp>
#include <fiction/types.hpp>

#include <fmt/core.h>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/networks/klut.hpp>

#include <cassert>
#include <cstdint>
#include <filesystem>
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
    experiments::experiment<std::string, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t,
                            std::uint32_t, bool, bool, bool>
        wiring_reduction_exp{"planarization", "benchmark",       "inputs",         "virt_inputs",
                             "outputs",       "initial nodes",   "nodes buffered", "nodes planarized",
                             "is_planar",     "equivalent(ntk)", "equivalent(lyt)"};

    for (const auto& entry :
         std::filesystem::directory_iterator("/home/benjamin/Documents/Repositories/working/fiction/benchmarks/IWLS93"))
    {
        continue;
        fmt::print("[i] processing {}\n", entry.path().filename().string());

        /*if ( "sqrt8ml.v" != entry.path().filename().string())
        {
            continue;
        }*/
        /*if ( "C1355.v" != entry.path().filename().string())
        {
            continue;
        }*/
        if ("C432.v" == entry.path().filename().string())
        {
            continue;
        }
        if ("ex4p.v" == entry.path().filename().string())
        {
            continue;
        }
        if ("apex1.v" == entry.path().filename().string())
        {
            continue;
        }

        std::ostringstream os{};

        fiction::network_reader<fiction::tec_ptr> reader{entry.path().string(), os};

        const auto nets = reader.get_networks();

        auto benchmark_network = *nets.front();

        fiction::network_balancing_params ps;
        ps.unify_outputs = true;

        const auto b = fiction::network_balancing<fiction::technology_network>(
            fiction::fanout_substitution<fiction::technology_network>(benchmark_network), ps);

        // happens with exp4.v
        /*const auto fc = fanins(_b, 9213);

        std::cout << "Node 9213: " << fc.fanin_nodes.size() << std::endl;
        std::cout << "Node 9213: " << _b.is_buf(9213) << std::endl;*/

        // happens with apex1.v
        /*const auto fc = fanins(_b, 8771);

        std::cout << "Node 9213: " << fc.fanin_nodes.size() << std::endl;
        std::cout << "Node 9213: " << _b.is_buf(8771) << std::endl;*/

        if (b.size() > 10000)
        {
            continue;
        }

        const auto planarized_b = fiction::node_duplication_planarization<fiction::technology_network>(b);
        const auto is_planar    = fiction::check_planarity(planarized_b);
        if (planarized_b.size() > 20000)
        {
            continue;
        }
        mockturtle::equivalence_checking_stats st;
        const auto                             cec_m = mockturtle::equivalence_checking(
            *fiction::virtual_miter<fiction::technology_network>(benchmark_network, planarized_b), {}, &st);
        assert(cec_m.has_value());

        fiction::orthogonal_physical_design_stats stats{};
        using gate_layout = fiction::gate_level_layout<
            fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;
        auto layout = fiction::orthogonal<gate_layout>(planarized_b, {}, &stats);

        const fiction::gate_level_drv_params ps_d{};
        fiction::gate_level_drv_stats        st_d{};

        fiction::gate_level_drvs(layout, ps_d, &st_d);

        const auto miter = mockturtle::miter<mockturtle::klut_network>(planarized_b, layout);
        bool       eq    = false;
        if (miter)
        {
            mockturtle::equivalence_checking_stats st_o;

            const auto ce = mockturtle::equivalence_checking(*miter, {}, &st_o);
            eq            = ce.value();
        }
        wiring_reduction_exp(entry.path().filename().string(), benchmark_network.num_pis(),
                             planarized_b.num_virtual_pis(), benchmark_network.num_pos(), benchmark_network.num_gates(),
                             b.num_gates(), planarized_b.num_gates(), is_planar, cec_m.value(), eq);
        wiring_reduction_exp.save();
        wiring_reduction_exp.table();
    }

    static constexpr const std::uint64_t bench_select =
        (fiction_experiments::trindade16 | fiction_experiments::fontes18);
    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto benchmark_network = read_ntk<fiction::tec_nt>(benchmark);

        fiction::network_balancing_params ps;
        ps.unify_outputs = true;

        const auto b = fiction::network_balancing<fiction::technology_network>(
            fiction::fanout_substitution<fiction::technology_network>(benchmark_network), ps);

        const auto planarized_b = fiction::node_duplication_planarization<fiction::technology_network>(b);

        const auto is_planar = fiction::check_planarity(planarized_b);

        if (planarized_b.size() > 20000)
        {
            wiring_reduction_exp(benchmark, benchmark_network.num_pis(), planarized_b.num_virtual_pis(),
                                 benchmark_network.num_pos(), benchmark_network.num_gates(), b.num_gates(),
                                 planarized_b.num_gates(), is_planar, false, false);
            wiring_reduction_exp.save();
            wiring_reduction_exp.table();
            continue;
        }

        // check equivalence
        mockturtle::equivalence_checking_stats st;
        const auto                             cec_m = mockturtle::equivalence_checking(
            *fiction::virtual_miter<fiction::technology_network>(benchmark_network, planarized_b), {}, &st);
        assert(cec_m.has_value());

        fiction::orthogonal_physical_design_stats stats{};
        using gate_layout = fiction::gate_level_layout<
            fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;
        auto layout = fiction::orthogonal<gate_layout>(planarized_b, {}, &stats);

        fiction::gate_level_drv_params ps_d{};
        fiction::gate_level_drv_stats  st_d{};

        fiction::gate_level_drvs(layout, ps_d, &st_d);

        const auto miter = mockturtle::miter<mockturtle::klut_network>(planarized_b, layout);
        bool       eq    = false;
        if (miter)
        {
            mockturtle::equivalence_checking_stats st_o;

            const auto ce = mockturtle::equivalence_checking(*miter, {}, &st_o);
            eq            = ce.value();
        }

        // log results
        wiring_reduction_exp(benchmark, benchmark_network.num_pis(), planarized_b.num_virtual_pis(),
                             benchmark_network.num_pos(), benchmark_network.num_gates(), b.num_gates(),
                             planarized_b.num_gates(), is_planar, cec_m.value(), eq);

        wiring_reduction_exp.save();
        wiring_reduction_exp.table();
    }

    return EXIT_SUCCESS;
}
