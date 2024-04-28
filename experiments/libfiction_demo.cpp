//
// Created by marcel on 27.10.21.
//

#if (FICTION_Z3_SOLVER)

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>  // substitute multi-output gates with fan-out cascades
#include <fiction/algorithms/physical_design/apply_gate_library.hpp>  // layout conversion to cell-level
#include <fiction/algorithms/physical_design/exact.hpp>               // SMT-based physical design of FCN layouts
#include <fiction/algorithms/physical_design/orthogonal.hpp>          // scalable physical design of FCN layouts
#include <fiction/io/dot_drawers.hpp>                                 // DOT drawers for logic networks and layouts
#include <fiction/io/write_qca_layout.hpp>           // writer for QCADesigner files (physical simulation)
#include <fiction/io/write_sqd_layout.hpp>           // writer for SiQAD files (physical simulation)
#include <fiction/io/write_svg_layout.hpp>           // SVG writer for cell-level layout representation
#include <fiction/layouts/cartesian_layout.hpp>      // Cartesian grid layouts
#include <fiction/layouts/cell_level_layout.hpp>     // cell-level abstraction of layouts
#include <fiction/layouts/coordinates.hpp>           // coordinate systems
#include <fiction/layouts/gate_level_layout.hpp>     // gate-level abstraction of layouts
#include <fiction/layouts/tile_based_layout.hpp>     // tile-based abstraction of layouts
#include <fiction/technology/area.hpp>               // area requirement calculations
#include <fiction/technology/cell_technologies.hpp>  // pre-defined cell implementations
#include <fiction/technology/qca_one_library.hpp>    // a pre-defined QCA gate library
#include <fiction/types.hpp>                         // pre-defined types suitable for the FCN domain
#include <fiction/utils/debug/network_writer.hpp>    // DOT writer for logic networks and layouts

#include <fmt/format.h>                                        // output formatting
#include <lorina/lorina.hpp>                                   // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/algorithms/cut_rewriting.hpp>             // logic optimization with cut rewriting
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>  // NPN databases for cut rewriting of XAGs and AIGs
#include <mockturtle/io/verilog_reader.hpp>                    // call-backs to read Verilog files into networks
#include <mockturtle/networks/aig.hpp>                         // AND-inverter graphs
#include <mockturtle/networks/mig.hpp>                         // MAJ-inverter graphs
#include <mockturtle/views/depth_view.hpp>                     // to determine network levels
#include <mockturtle/views/names_view.hpp>                     // to assign names to network signals

#include <cstdlib>     // exit codes
#include <filesystem>  // filesystem access
#include <iostream>    // output
#include <string>      // strings

template <typename Ntk>
void print_network_properties(const Ntk& ntk)
{
    // determine depth
    mockturtle::depth_view<Ntk> depth_ntk{ntk};

    // print statistics
    std::cout << fmt::format("[i] Network: inputs = {}, outputs = {}, gates = {}, levels = {}", ntk.num_pis(),
                             ntk.num_pos(), ntk.num_gates(), depth_ntk.depth())
              << std::endl;
}

template <typename Lyt>
void print_gate_layout_properties(const Lyt& lyt)
{
    // determine depth
    mockturtle::depth_view<Lyt> depth_lyt{lyt};

    // print statistics
    std::cout << fmt::format(
                     "[i] Gate-level layout:     aspect ratio = {} × {}, inputs = {}, outputs = {}, gates = {}, "
                     "wires = {}, critical path = {}",
                     lyt.x() + 1, lyt.y() + 1, lyt.num_pis(), lyt.num_pos(), lyt.num_gates(), lyt.num_wires(),
                     depth_lyt.depth())
              << std::endl;
}

template <typename CellLyt>
void print_cell_layout_properties(const CellLyt& cell_lyt)
{
    fiction::area_params<fiction::technology<CellLyt>> ps{};
    fiction::area_stats                                st{};
    // determine area
    fiction::area(cell_lyt, ps, &st);

    // print statistics
    std::cout
        << fmt::format(
               "[i] Cell-level {} layout: aspect ratio = {} × {}, inputs = {}, outputs = {}, cells = {}, area = {}nm²",
               fiction::tech_impl_name<fiction::technology<CellLyt>>, cell_lyt.x() + 1, cell_lyt.y() + 1,
               cell_lyt.num_pis(), cell_lyt.num_pos(), cell_lyt.num_cells(), st.area)
        << std::endl;
}

int main(int argc, char* argv[])  // NOLINT
{
    // check arguments
    if (argc == 1)
    {
        std::cout << "[e] file path to a Verilog network must be given" << std::endl;
        return EXIT_FAILURE;
    }

    /**************************************************************/
    /************************ Input parsing ***********************/
    /**************************************************************/

    // convert input to a file path
    std::filesystem::path file_path{argv[1]};  // NOLINT: pointer arithmetic is okay here

    // check if file path exists
    if (!std::filesystem::exists(file_path))
    {
        std::cout << fmt::format("[e] given file path '{}' does not exist", file_path.string()) << std::endl;
        return EXIT_FAILURE;
    }
    // check if file path points to a regular file
    if (!std::filesystem::is_regular_file(file_path))
    {
        std::cout << fmt::format("[e] given file path '{}' does not point to a regular file", file_path.string())
                  << std::endl;
        return EXIT_FAILURE;
    }

    // defining the type of logic network to use (also already pre-defined in fiction/types.hpp as aig_nt
    using logic_network = mockturtle::names_view<mockturtle::aig_network>;

    // instantiate an empty network
    logic_network ntk{};

    // parse input file into ntk
    if (lorina::read_verilog(file_path.string(), mockturtle::verilog_reader(ntk)) != lorina::return_code::success)
    {
        std::cout << fmt::format("[e] given file '{}' could not be parsed as a valid Verilog network",
                                 file_path.string())
                  << std::endl;
        return EXIT_FAILURE;
    }

    // create a folder for the design files
    std::string designs{"./designs/" + file_path.stem().string() + "/"};
    if (!std::filesystem::exists(designs))
    {
        std::filesystem::create_directory(designs);
    }

    // ntk now contains a representation of the given file; let's have some fun with it

    // first, print some properties
    print_network_properties(ntk);
    // and draw the network
    fiction::debug::write_dot_network(ntk, "ntk", designs);

    std::cout << std::endl;

    /**************************************************************/
    /********************* Logic Optimization *********************/
    /**************************************************************/

    std::cout << "[i] cut rewriting" << std::endl;

    // instantiate a complete AIG NPN database for node re-synthesis
    mockturtle::xag_npn_resynthesis<logic_network,                              // the input network type
                                    mockturtle::aig_network,                    // the database network type
                                    mockturtle::xag_npn_db_kind::aig_complete>  // the kind of database to use
        resynthesis_function{};

    mockturtle::cut_rewriting_params cut_params{};
    cut_params.cut_enumeration_ps.cut_size = 4;

    // rewrite network cuts using the given re-synthesis function
    ntk = mockturtle::cut_rewriting(ntk, resynthesis_function, cut_params);

    // print network properties again
    print_network_properties(ntk);
    // draw the network again
    fiction::debug::write_dot_network(ntk, "cut_ntk", designs);

    std::cout << std::endl;

    /**************************************************************/
    /******************* Network pre-processing *******************/
    /**************************************************************/

    std::cout << "[i] fanout substitution" << std::endl;

    // set up parameters for fanout substitution
    fiction::fanout_substitution_params fanout_params{};
    fanout_params.strategy  = fiction::fanout_substitution_params::substitution_strategy::BREADTH;
    fanout_params.degree    = 2;
    fanout_params.threshold = 1;

    // substitute high-degree output nodes by fanout nodes (converts network into a topology_network)
    auto top_ntk = fiction::fanout_substitution<fiction::tec_nt>(ntk, fanout_params);

    // print network properties again
    print_network_properties(top_ntk);
    // draw network again
    fiction::debug::write_dot_network(top_ntk, "top_ntk", designs);

    std::cout << std::endl;

    // how about some layout generation next?

    /**************************************************************/
    /***************** Scalable layout generation *****************/
    /**************************************************************/

    // defining the type of gate-level layout to use (also already pre-defined in fiction/types.hpp as cart_gate_clk_lyt
    using fcn_gate_level_layout = fiction::gate_level_layout<
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

    // defining the type of cell-level layout to use (also already pre-defined in fiction/types.hpp as qca_cell_clk_lyt
    using qca_cell_level_layout =
        fiction::cell_level_layout<fiction::qca_technology,
                                   fiction::clocked_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>;

    std::cout << "[i] orthogonal physical design" << std::endl;

    // set up parameters for orthogonal physical design
    fiction::orthogonal_physical_design_params ortho_params{};
    ortho_params.number_of_clock_phases = fiction::num_clks::FOUR;
    fiction::orthogonal_physical_design_stats ortho_stats{};

    // perform layout generation with a scalable algorithm
    auto ortho_gate_lyt = fiction::orthogonal<fcn_gate_level_layout>(top_ntk, ortho_params, &ortho_stats);

    // print layout properties
    print_gate_layout_properties(ortho_gate_lyt);
    // draw the layout
    fiction::debug::write_dot_layout<fcn_gate_level_layout,
                                     fiction::gate_layout_cartesian_drawer<fcn_gate_level_layout>>(
        ortho_gate_lyt, designs + "ortho_lyt");

    // apply the QCA ONE gate library to retrieve a cell-level layout
    auto ortho_cell_layout =
        fiction::apply_gate_library<qca_cell_level_layout, fiction::qca_one_library>(ortho_gate_lyt);

    // print cell properties
    print_cell_layout_properties(ortho_cell_layout);

    // write an SVG image of the layout
    fiction::write_qca_layout_svg(ortho_cell_layout, designs + "ortho_qca.svg");
    // write a QCADesigner simulation file
    fiction::write_qca_layout(ortho_cell_layout, designs + "ortho_qca.qca");
    // write a SiQAD simulation file
    fiction::write_sqd_layout(ortho_cell_layout, designs + "ortho_qca.sqd");

    std::cout << std::endl;

    /**************************************************************/
    /******************* Exact layout generation ******************/
    /**************************************************************/

    // attempt exact physical design only if the number of gates is manageable
    if (top_ntk.num_gates() < 28)
    {
        std::cout << "[i] SMT-based physical design" << std::endl;

        // set up parameters for SMT-based physical design
        fiction::exact_physical_design_params exact_params{};
        exact_params.scheme    = "2DDWave";
        exact_params.crossings = true;
        exact_params.border_io = true;
        exact_params.timeout   = 180000;  // 3min in ms
        fiction::exact_physical_design_stats exact_stats{};

        // perform layout generation with an SMT-based exact algorithm
        auto exact_gate_lyt = fiction::exact<fcn_gate_level_layout>(top_ntk, exact_params, &exact_stats);

        // if attempt was successful
        if (exact_gate_lyt.has_value())
        {
            // print layout properties
            print_gate_layout_properties(*exact_gate_lyt);
            // draw the layout
            fiction::debug::write_dot_layout<fcn_gate_level_layout,
                                             fiction::gate_layout_cartesian_drawer<fcn_gate_level_layout>>(
                *exact_gate_lyt, designs + "exact_lyt");

            // apply the QCA ONE gate library to retrieve a cell-level layout
            auto exact_cell_layout =
                fiction::apply_gate_library<qca_cell_level_layout, fiction::qca_one_library>(*exact_gate_lyt);

            // print cell properties
            print_cell_layout_properties(exact_cell_layout);

            // write an SVG image of the layout
            fiction::write_qca_layout_svg(exact_cell_layout, designs + "exact_qca.svg");
            // write a QCADesigner simulation file
            fiction::write_qca_layout(exact_cell_layout, designs + "exact_qca.qca");
            // write a SiQAD simulation file
            fiction::write_sqd_layout(exact_cell_layout, designs + "exact_qca.sqd");
        }
        // not successful
        else
        {
            std::cout << "[e] could not generate an exact layout within the given parameters" << std::endl;
        }
    }
    else
    {
        std::cout << "[w] network is too large to attempt exact physical design" << std::endl;
    }

    return EXIT_SUCCESS;
}

#else  // FICTION_Z3_SOLVER

#include <cstdlib>
#include <iostream>

int main()  // NOLINT
{
    std::cerr << "[e] Z3 solver is not available, please install Z3 and recompile the code" << std::endl;

    return EXIT_FAILURE;
}

#endif  // FICTION_Z3_SOLVER
