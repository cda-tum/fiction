#include "fiction_experiments.hpp"

#include <fiction/algorithms/network_transformation/network_conversion.hpp>  // conversion of networks
#include <fiction/algorithms/physical_design/determine_clocking.hpp>         // SAT-based clock number assignment
#include <fiction/algorithms/verification/equivalence_checking.hpp>          // SAT-based equivalence checking
#include <fiction/io/read_fgl_layout.hpp>                                    // custom reader for layouts
#include <fiction/types.hpp>                                                 // pre-defined types
#include <fiction/utils/name_utils.hpp>                                      // name utilities

#include <fmt/format.h>                    // output formatting
#include <mockturtle/utils/stopwatch.hpp>  // time measurements

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <string>

template <typename Lyt>
void remove_clocking(Lyt& lyt) noexcept
{
    static_assert(fiction::is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    lyt.foreach_tile([&lyt](const auto& t) { lyt.assign_clock_number(t, 0); });
}

int main()  // NOLINT
{
    // NOTE: you have to download the respective files from MNTBench. Select Fontes18 and Trindade16, all clocking
    // schemes, obtained with NanoPlaceR
    const std::string layout_folder =
        fmt::format("{}/clock_number_assignment/versatility_benchmarks/", EXPERIMENTS_PATH);

    using gate_lyt =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;

    experiments::experiment<std::string, std::string, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, double, bool>
        clock_number_assignment_exp{"clock number assignment",
                                    "clocking scheme",
                                    "benchmark",
                                    "inputs",
                                    "outputs",
                                    "width [tiles]",
                                    "height [tiles]",
                                    "area [tiles]",
                                    "runtime clocking [s]",
                                    "equivalent"};

    // iterate over each file in the layout folder
    for (const auto& file : std::filesystem::directory_iterator(layout_folder))
    {
        if (std::filesystem::is_regular_file(file))
        {
            const auto& path = file.path();

            // only consider *.fgl files
            if (path.extension() == ".fgl")
            {
                const auto benchmark = path.stem().string();

                // read layout from file
                auto original_layout = fiction::read_fgl_layout<gate_lyt>(path.string(), benchmark);

                fmt::print("[i] processing {}\n", benchmark);

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
                clock_number_assignment_exp(original_layout.get_clocking_scheme().name.data(), benchmark,
                                            original_layout.num_pis(), original_layout.num_pos(), width, height, area,
                                            mockturtle::to_seconds(stats.time_total), eq_result);

                clock_number_assignment_exp.save();
                clock_number_assignment_exp.table();
            }
        }
    }

    return EXIT_SUCCESS;
}
