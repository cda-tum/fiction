//
// Created by Simon Hofmann on 30.01.24.
//
#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>  // graph-oriented layout design algorithm
#include <fiction/algorithms/properties/count_gate_types.hpp>
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/io/network_reader.hpp>                                          // read networks from files
#include <fiction/layouts/bounding_box.hpp>                                       // calculate area of generated layouts

#include <fmt/format.h>  // output formatting

#include <cstdlib>
#include <ostream>
#include <string>

template <typename Ntk>
Ntk read_ntk(const std::string& name)
{
    fmt::print("[i] processing {}\n", name);

    std::ostringstream                        os{};
    fiction::network_reader<fiction::tec_ptr> reader{fiction_experiments::benchmark_path(name), os};
    const auto                                nets    = reader.get_networks();
    const auto                                network = *nets.front();

    return network;
}

int main()  // NOLINT
{
    using gate_lyt =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;

    experiments::experiment<std::string, std::string, uint64_t, uint64_t, uint64_t, uint64_t, double, std::string>
        graph_oriented_layout_design_exp{"graph_oriented_layout_design_exp",
                                         "benchmark",
                                         "cost function",
                                         "Area",
                                         "|C|",
                                         "|W|",
                                         "ACP",
                                         "runtime (in sec)",
                                         "equivalent"};

    fiction::graph_oriented_layout_design_stats  graph_oriented_layout_design_stats{};
    fiction::graph_oriented_layout_design_params graph_oriented_layout_design_params{};
    graph_oriented_layout_design_params.mode =
        fiction::graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;
    graph_oriented_layout_design_params.verbose      = true;
    graph_oriented_layout_design_params.return_first = false;
    graph_oriented_layout_design_params.timeout      = 60000;

    static constexpr const uint64_t bench_select = fiction_experiments::trindade16 | fiction_experiments::fontes18;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        for (uint64_t cost = 0; cost < 4; cost++)
        {
            if (cost == 0)
            {
                graph_oriented_layout_design_params.cost =
                    fiction::graph_oriented_layout_design_params::cost_objective::AREA;
            }
            else if (cost == 1)
            {
                graph_oriented_layout_design_params.cost =
                    fiction::graph_oriented_layout_design_params::cost_objective::CROSSINGS;
            }
            else if (cost == 2)
            {
                graph_oriented_layout_design_params.cost =
                    fiction::graph_oriented_layout_design_params::cost_objective::WIRES;
            }
            else
            {
                graph_oriented_layout_design_params.cost =
                    fiction::graph_oriented_layout_design_params::cost_objective::ACP;
            }

            auto network = read_ntk<fiction::tec_nt>(benchmark);

            auto gate_level_layout = fiction::graph_oriented_layout_design<gate_lyt, fiction::tec_nt>(
                network, graph_oriented_layout_design_params, &graph_oriented_layout_design_stats);

            if (gate_level_layout.has_value())
            {
                // check equivalence
                const auto eq_stats =
                    fiction::equivalence_checking<fiction::technology_network, gate_lyt>(network, *gate_level_layout);

                const std::string eq_result = eq_stats == fiction::eq_type::STRONG ? "STRONG" :
                                              eq_stats == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                     "NO";

                // calculate bounding box
                const auto bounding_box = fiction::bounding_box_2d(*gate_level_layout);

                const auto width  = bounding_box.get_x_size() + 1;
                const auto height = bounding_box.get_y_size() + 1;
                const auto area   = width * height;

                // fiction::print_gate_level_layout(std::cout, *gate_level_layout);
                // fiction::count_gate_types_stats st{};
                // fiction::count_gate_types(*gate_level_layout, &st);
                // st.report();
                // fiction::gate_level_drv_params ps{};
                // fiction::gate_level_drv_stats  st{};

                // fiction::gate_level_drvs(*gate_level_layout, ps, &st);

                const auto num_wires =
                    gate_level_layout->num_wires() - gate_level_layout->num_pis() - gate_level_layout->num_pos();
                const auto  num_crossings = gate_level_layout->num_crossings();
                std::string cost_function = "A";
                if (cost == 1)
                {
                    cost_function = "|C|";
                }
                else if (cost == 2)
                {
                    cost_function = "|W|";
                }
                else if (cost == 3)
                {
                    cost_function = "ACP";
                }
                // log results
                graph_oriented_layout_design_exp(
                    benchmark, cost_function, area, num_crossings, num_wires, area * (num_crossings + 1),
                    mockturtle::to_seconds(graph_oriented_layout_design_stats.time_total), eq_result);
            }

            graph_oriented_layout_design_exp.save();
            graph_oriented_layout_design_exp.table();
        }
    }

    return EXIT_SUCCESS;
}
