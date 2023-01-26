//
// Created by marcel on 26.01.23.
//

#define EXPERIMENTS_PATH "/home/marcel/git/fiction/libs/mockturtle/experiments/"

#include "fiction_experiments.hpp"

#include <fiction/algorithms/network_transformation/crossing_reduction.hpp>   // crossing reduction
#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>  // fanout substitution
// #include <fiction/algorithms/network_transformation/network_balancing.hpp>    // network balancing
// #include <fiction/networks/technology_network.hpp>                            // technology networks

#include <fmt/format.h>                                        // output formatting
#include <lorina/genlib.hpp>                                   // Genlib file parsing
#include <lorina/lorina.hpp>                                   // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/algorithms/collapse_mapped.hpp>           // collapse mapped networks into k-LUTs
#include <mockturtle/algorithms/cut_rewriting.hpp>             // logic optimization with cut rewriting
#include <mockturtle/algorithms/equivalence_checking.hpp>      // equivalence checking
#include <mockturtle/algorithms/lut_mapper.hpp>                // new LUT mapper
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>  // NPN databases for cut rewriting of XAGs and AIGs
#include <mockturtle/io/aiger_reader.hpp>                      // call-backs to read AIGER files into networks
#include <mockturtle/io/verilog_reader.hpp>                    // call-backs to read Verilog files into networks
#include <mockturtle/networks/klut.hpp>                        // k-LUT networks
#include <mockturtle/networks/xag.hpp>                         // XOR-AND-inverter graphs
#include <mockturtle/views/mapping_view.hpp>                   // to map networks to technology libraries

#include <cassert>
#include <cstdint>
#include <string>

int main()  // NOLINT
{
    //    const std::string network_folder = fmt::format("{}/bestagon/layouts", EXPERIMENTS_PATH);

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, double>
        crossing_exp{"crossings", "benchmark", "inputs", "outputs", "initial nodes", "nodes after opt.",
                     "LUTs after mapping",
                     //                     "nodes after balancing",
                     "initial crossings", "crossings after opt.", "runtime (in sec)"};


    // instantiate a complete XAG NPN database for node re-synthesis
    const mockturtle::xag_npn_resynthesis<mockturtle::xag_network,                    // the input network type
                                          mockturtle::xag_network,                    // the database network type
                                          mockturtle::xag_npn_db_kind::xag_complete>  // the kind of database to use
        resynthesis_function{};

    // parameters for cut rewriting
    mockturtle::cut_rewriting_params cut_params{};
    cut_params.cut_enumeration_ps.cut_size = 4;

    // parameters for crossing reduction
    fiction::crossing_reduction_params crossing_params{};
    crossing_params.initial_temperature = 100;
    crossing_params.number_of_cycles    = 10;

    // statistics for crossing reduction
    fiction::crossing_reduction_stats crossing_stats{};

    static constexpr const uint64_t bench_select = experiments::iscas;

    for (const auto& benchmark : experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        mockturtle::xag_network xag{};

        [[maybe_unused]] const auto read_verilog_result =
            lorina::read_aiger(experiments::benchmark_path(benchmark), mockturtle::aiger_reader(xag));
        assert(read_verilog_result == lorina::return_code::success);

        // rewrite network cuts using the given re-synthesis function
        const auto cut_xag = mockturtle::cut_rewriting(xag, resynthesis_function, cut_params);

        // k-LUT mapping
        mockturtle::mapping_view mapped_xag{cut_xag};
        lut_map(mapped_xag);

        const auto collapsed_klut = mockturtle::collapse_mapped_network<mockturtle::klut_network>(mapped_xag);
        assert(collapsed_klut.has_value() && "mapping failed");

        //        const auto fanout_substituted_klut =
        //        fiction::fanout_substitution<fiction::technology_network>(*collapsed_klut); const auto balanced_klut =
        //        fiction::network_balancing<fiction::technology_network>(fanout_substituted_klut);

        // perform crossing reduction
        [[maybe_unused]] const auto rank_ntk =
            fiction::crossing_reduction(*collapsed_klut, crossing_params, &crossing_stats);

        //            // check equivalence
        //            const auto miter = mockturtle::miter<fiction::technology_network>(mapped_network,
        //            *gate_level_layout); const auto eq    = mockturtle::equivalence_checking(*miter);
        //            assert(eq.has_value());

        // log results
        crossing_exp(benchmark, xag.num_pis(), xag.num_pos(), xag.num_gates(), cut_xag.num_gates(),
                     collapsed_klut->num_gates(), crossing_stats.crossings_before, crossing_stats.crossings_after,
                     mockturtle::to_seconds(crossing_stats.time_total));

        crossing_exp.save();
        crossing_exp.table();
    }

    return EXIT_SUCCESS;
}
