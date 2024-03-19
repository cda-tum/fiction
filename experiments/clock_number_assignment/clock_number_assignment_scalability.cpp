#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/determine_clocking.hpp>  // SAT-based clock number assignment
#include <fiction/algorithms/physical_design/orthogonal.hpp>  // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/verification/equivalence_checking.hpp>  // SAT-based equivalence checking
#include <fiction/io/network_reader.hpp>                             // custom reader for folders of networks
#include <fiction/technology/technology_mapping_library.hpp>         // library for technology mapping
#include <fiction/types.hpp>                                         // pre-defined types
#include <fiction/utils/name_utils.hpp>                              // name utilities

#include <fmt/format.h>                       // output formatting
#include <lorina/lorina.hpp>                  // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/algorithms/mapper.hpp>   // technology mapping
#include <mockturtle/io/genlib_reader.hpp>    // call-backs for the GENLIB format
#include <mockturtle/utils/stopwatch.hpp>     // time measurements
#include <mockturtle/utils/tech_library.hpp>  // technology library utils

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

template <typename Lyt>
void remove_clocking(Lyt& lyt) noexcept
{
    static_assert(fiction::is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    lyt.foreach_tile([&lyt](const auto& t) { lyt.assign_clock_number(t, 0); });
}

int main()  // NOLINT
{
    const std::string network_folder = fmt::format("{}/../benchmarks/IWLS93/", EXPERIMENTS_PATH);

    using gate_lyt =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;

    experiments::experiment<std::string, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, double, bool>
        clock_number_assignment_exp{"clock number assignment",
                                    "benchmark",
                                    "inputs",
                                    "outputs",
                                    "width [tiles]",
                                    "height [tiles]",
                                    "area [tiles]",
                                    "runtime clocking [s]",
                                    "equivalent"};

    // instantiate a technology mapping library (AND, OR, NOT)
    std::stringstream library_stream{};
    library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV
                   << fiction::GATE_AND2 << fiction::GATE_OR2;

    std::vector<mockturtle::gate> gates{};

    const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
    assert(read_genlib_result == lorina::return_code::success);
    const mockturtle::tech_library<2> gate_lib{gates};

    // parameters for technology mapping
    const mockturtle::map_params map_params{};

    // instantiate a network reader
    fiction::network_reader<fiction::aig_ptr> reader{network_folder, std::cout};

    for (const auto& benchmark : reader.get_networks(true))  // for each benchmark sorted by size (ascending)
    {
        const auto& network = *benchmark;

        fmt::print("[i] processing {}\n", network.get_network_name());

        // perform technology mapping with the given library (AND, OR, NOT)
        const auto mapped_network = mockturtle::map(network, gate_lib, map_params);

        // perform layout generation with an exact SMT-based algorithm
        const auto original_layout = fiction::orthogonal<gate_lyt>(mapped_network);

        // obtain layout characteristics
        const auto width  = original_layout.x() + 1;
        const auto height = original_layout.y() + 1;
        const auto area   = width * height;

        // deep-copy the original layout
        auto newly_clocked_layout = original_layout.clone();

        // remove clocking from copied layout
        remove_clocking(newly_clocked_layout);

        // parameters and statistics of the clock number assignment
        const fiction::determine_clocking_params params{};
        fiction::determine_clocking_stats        stats{};

        // perform clock number assignment
        fiction::determine_clocking(newly_clocked_layout, params, &stats);

        // check equivalence of the original and the newly clocked layout
        const auto eq_result =
            fiction::equivalence_checking(original_layout, newly_clocked_layout) != fiction::eq_type::NO;

        // log results
        clock_number_assignment_exp(network.get_network_name(), original_layout.num_pis(), original_layout.num_pos(),
                                    width, height, area, mockturtle::to_seconds(stats.time_total), eq_result);

        clock_number_assignment_exp.save();
        clock_number_assignment_exp.table();
    }

    return EXIT_SUCCESS;
}
