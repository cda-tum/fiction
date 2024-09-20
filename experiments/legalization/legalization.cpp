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
    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool, bool,
                            bool>
        wiring_reduction_exp{"planarization", "benchmark",       "inputs",         "virt_inputs",
                             "outputs",       "initial nodes",   "nodes buffered", "nodes planarized",
                             "is_planar",     "equivalent(ntk)", "equivalent(lyt)"};

    int x = 0;
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
        if ( "C432.v" == entry.path().filename().string())
        {
            continue;
        }
        if ( "ex4p.v" == entry.path().filename().string())
        {
            continue;
        }
        if ( "apex1.v" == entry.path().filename().string())
        {
            continue;
        }

        std::ostringstream os{};

        fiction::network_reader<fiction::tec_ptr> reader{entry.path().string(), os};

        const auto nets = reader.get_networks();

        auto benchmark_network = *nets.front();

        bool cont = false;
        benchmark_network.foreach_pi(
            [&benchmark_network, &cont](auto pi)
            {
                if (benchmark_network.is_po(pi))
                {
                    cont = true;
                    // std::cout << "Pi is Po\n";
                }
            });
        if (cont)
        {
            continue;
        }

        // fiction::debug::write_dot_network(benchmark_network);

        // std::cout << benchmark_network.num_gates() << std::endl;

        fiction::network_balancing_params ps;
        ps.unify_outputs = true;

        const auto _b = fiction::network_balancing<fiction::technology_network>(
            fiction::fanout_substitution<fiction::technology_network>(benchmark_network), ps);

        // happens with exp4.v
        /*const auto fc = fanins(_b, 9213);

        std::cout << "Node 9213: " << fc.fanin_nodes.size() << std::endl;
        std::cout << "Node 9213: " << _b.is_buf(9213) << std::endl;*/

        // happens with apex1.v
        /*const auto fc = fanins(_b, 8771);

        std::cout << "Node 9213: " << fc.fanin_nodes.size() << std::endl;
        std::cout << "Node 9213: " << _b.is_buf(8771) << std::endl;*/

        // std::cout << _b.num_gates() << std::endl;

        // fiction::debug::write_dot_network(_b);

        if (_b.size() > 10000)
        {
            /*wiring_reduction_exp(entry.path().filename().string(), benchmark_network.num_pis(), 0,
                                 benchmark_network.num_pos(), benchmark_network.num_gates(), _b.num_gates(),
                                 _b.num_gates(), 0, 0, 0);
            wiring_reduction_exp.save();
            wiring_reduction_exp.table();
            std::cout << ++x << "\n";*/
            continue;
        }

        const auto planarized_b = fiction::node_duplication_planarization<fiction::technology_network>(_b);
        const auto is_planar = fiction::check_planarity(planarized_b);
        if (planarized_b.size() > 20000)
        {
            continue;
        }
        mockturtle::equivalence_checking_stats st;
        const auto                             cec_m = mockturtle::equivalence_checking(
            *fiction::virtual_miter<fiction::technology_network>(benchmark_network, planarized_b), {}, &st);
        assert(cec_m.has_value());
        std::cout << cec_m.value() << std::endl;

        using gate_lyt = fiction::gate_level_layout<
            fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;

        fiction::orthogonal_physical_design_stats stats{};
        using gate_layout = fiction::gate_level_layout<
            fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;
        auto layout = fiction::orthogonal<gate_layout>(planarized_b, {}, &stats);

        fiction::gate_level_drv_params ps_d{};
        fiction::gate_level_drv_stats  st_d{};

        fiction::gate_level_drvs(layout, ps_d, &st_d);

        const auto miter = mockturtle::miter<mockturtle::klut_network>(planarized_b, layout);
        bool eq;
        if (miter)
        {
            mockturtle::equivalence_checking_stats st;

            const auto ce = mockturtle::equivalence_checking(*miter, {}, &st);
            eq = ce.value();
        }
        wiring_reduction_exp(entry.path().filename().string(), benchmark_network.num_pis(),
                             planarized_b.num_virtual_pis(), benchmark_network.num_pos(), benchmark_network.num_gates(),
                             _b.num_gates(), planarized_b.num_gates(), is_planar, cec_m.value(), eq);
        wiring_reduction_exp.save();
        wiring_reduction_exp.table();
        std::cout << ++x << "\n";
    }

    /*fiction::technology_network tec;
    const auto pi0 = tec.create_pi();
    const auto pi1 = tec.create_pi();
    const auto pi2 = tec.create_pi();
    const auto pi3 = tec.create_pi();
    const auto pi4 = tec.create_pi();

    const auto a0 = tec.create_and(pi0, pi1);
    const auto o0 = tec.create_or(pi0, pi1);
    const auto a1 = tec.create_and(pi3, pi4);
    const auto o1 = tec.create_or(pi3, pi4);

    const auto a2 = tec.create_and(o0, pi2);
    const auto a3 = tec.create_and(o1, pi2);
    const auto a4 = tec.create_and(a2, a3);

    tec.create_po(a0);
    tec.create_po(a4);
    tec.create_po(a1);

    fiction::network_balancing_params ps;
    ps.unify_outputs = true;

    const auto _b = fiction::network_balancing<fiction::technology_network>(
        fiction::fanout_substitution<fiction::technology_network>(tec), ps);


    const auto _b_topo = mockturtle::topo_view(_b);
    const auto _b_rank = mockturtle::rank_view(_b);

    fiction::node_duplication_planarization_params dps;
    dps.random_output_order = false;

    const auto planarized_b = fiction::node_duplication_planarization<fiction::technology_network>(_b, dps);

    fiction::debug::write_dot_network(tec, "tec_test");
    fiction::debug::write_dot_network(_b, "t_b");
    fiction::debug::write_dot_network(planarized_b, "network_b");*/

    static constexpr const uint64_t bench_select = (fiction_experiments::iscas85);  // fiction_experiments::iscas85 & ~
    // fiction_experiments::trindade16 | fiction_experiments::fontes18 | fiction_experiments::epfl |
    // fiction_experiments::iscas85 static constexpr const uint64_t bench_select =
    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto benchmark_network = read_ntk<fiction::tec_nt>(benchmark);

        fiction::network_balancing_params ps;
        ps.unify_outputs = true;

        bool cont = false;
        benchmark_network.foreach_pi(
            [&benchmark_network, &cont](auto pi)
            {
                if (benchmark_network.is_po(pi))
                {
                    cont = true;
                    std::cout << "Pi is Po\n";

                }
            });
        if(cont)
        {
            continue;
        }

        const auto _b = fiction::network_balancing<fiction::technology_network>(
            fiction::fanout_substitution<fiction::technology_network>(benchmark_network), ps);

        if (_b.size() > 10000)
        {
            /*wiring_reduction_exp(benchmark, benchmark_network.num_pis(), 0, benchmark_network.num_pos(),
                                 benchmark_network.num_gates(), _b.num_gates(), _b.num_gates(), 0, 0, "TEST");
            wiring_reduction_exp.save();
            wiring_reduction_exp.table();*/
            continue;
        }

        const auto planarized_b = fiction::node_duplication_planarization<fiction::technology_network>(_b);

        const auto is_planar = fiction::check_planarity(planarized_b);

        if (planarized_b.size() > 20000)
        {
            wiring_reduction_exp(benchmark, benchmark_network.num_pis(), planarized_b.num_virtual_pis(), benchmark_network.num_pos(),
                                 benchmark_network.num_gates(), _b.num_gates(), planarized_b.num_gates(), is_planar, 0, 0);
            wiring_reduction_exp.save();
            wiring_reduction_exp.table();
            continue;
        }

        // check equivalence
        mockturtle::equivalence_checking_stats st;
        const auto                             cec_m = mockturtle::equivalence_checking(
            *fiction::virtual_miter<fiction::technology_network>(benchmark_network, planarized_b), {}, &st);
        assert(cec_m.has_value());

        auto name = mockturtle::names_view(planarized_b);

        fiction::restore_names(benchmark_network, name);

        using gate_lyt =
            fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;

        fiction::orthogonal_physical_design_stats stats{};
        using gate_layout = fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;
        auto start = std::chrono::high_resolution_clock::now();
        auto layout = fiction::orthogonal<gate_layout>(planarized_b, {}, &stats);
        auto stop = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = stop - start;

        std::cout << "Time elapsed: " << elapsed.count() << " s\n";

        std::cout << "X size : " << stats.x_size << "\n";
        std::cout << "Y size : " << stats.y_size << "\n";

        std::cout << "PO benchmark : " << benchmark_network.num_pos() << "\n";
        std::cout << "PO planarized : " << planarized_b.num_pos() << "\n";
        std::cout << "PO layout : " << layout.num_pos() << "\n";

        fiction::gate_level_drv_params ps_d{};
        fiction::gate_level_drv_stats  st_d{};

        fiction::gate_level_drvs(layout, ps_d, &st_d);

        /*fiction::equivalence_checking_stats eq_s_ortho{};
        // check equivalence
        const auto eq_stats_ortho =
            fiction::equivalence_checking<decltype(planarized_b), gate_lyt>(planarized_b, layout, &eq_s_ortho);
        std::cout << "eq_checking gone\n";
        const std::string eq_result_ortho = eq_stats_ortho == fiction::eq_type::STRONG ? "STRONG" :
                                            eq_stats_ortho == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                         "NO";*/

        const auto miter = mockturtle::miter<mockturtle::klut_network>(planarized_b, layout);
        bool eq;
        if (miter)
        {
            mockturtle::equivalence_checking_stats st;

            const auto ce = mockturtle::equivalence_checking(*miter, {}, &st);
            eq = ce.value();
        }

        // fiction::debug::write_dot_layout(layout);
        /*fiction::debug::write_dot_network(_b, "ntk_b");
        fiction::debug::write_dot_network(planarized_b, "ntk_p");*/

        // log results
        wiring_reduction_exp(benchmark, benchmark_network.num_pis(), planarized_b.num_virtual_pis(), benchmark_network.num_pos(),
                             benchmark_network.num_gates(), _b.num_gates(), planarized_b.num_gates(), is_planar, cec_m.value(), eq);

        wiring_reduction_exp.save();
        wiring_reduction_exp.table();
    }

    return EXIT_SUCCESS;
}
