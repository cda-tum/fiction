//
// Created by marcel on 24.10.19.
//

#include "cmd/io/include/read.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/io/network_reader.hpp>
#include <fiction/io/read_fgl_layout.hpp>
#include <fiction/io/read_fqca_layout.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>

#include <filesystem>
#include <memory>
#include <string>

namespace alice
{

read_command::read_command(const environment::ptr& e) :
        command(e, "Reads a file or a directory of files and creates logic network or FCN layout objects "
                   "which will be put into the respective store. Current supported file types are:\n"
                   "Logic networks: Verilog, AIGER, BLIF.\n"
                   "Gate-level layouts: FGL.\n"
                   "Cell-level layouts: SQD, FQCA.\n"
                   "In a directory, only files with extension '.v', '.aig', '.blif' are considered.")
{
    add_option("filename", filename, "Filename or directory")->required();
    add_option("topology", topology,
               "Topology for gate-level layouts. Can be 'cartesian' or of the form "
               "'<odd|even>_<row|column>_<cartesian|hex>'");
    add_option("--lattice_orientation,-o", orientation, "Lattice orientation for SQD files to use {100, 111}");
    add_flag("--aig,-a", "Parse Verilog file as AIG");
    add_flag("--xag,-x", "Parse Verilog file as XAG");
    add_flag("--mig,-m", "Parse Verilog file as MIG");
    add_flag("--tec,-t", "Parse Verilog file as technology network");
    add_flag("--fgl,-f", "Parse FGL file as fiction gate-level layout");
    add_flag("--sqd,-s", "Parse SQD file as SiDB cell-level layout");
    add_flag("--fqca,-q", "Parse FQCA file as QCA cell-level layout");
    add_flag("--sort", sort, "Sort networks in given directory by node count prior to storing them");
}

void read_command::execute()
{
    if (!is_set("aig") && !is_set("xag") && !is_set("mig") && !is_set("tec") && !is_set("fgl") && !is_set("sqd") &&
        !is_set("fqca"))
    {
        env->out() << "[e] at least one network or layout type must be specified\n";
    }
    else if ((is_set("aig") || is_set("xag") || is_set("mig") || is_set("tec")) && is_set("fgl"))
    {
        env->out() << "[e] cannot parse files as both logic networks and gate-level layouts\n";
    }
    else if ((is_set("aig") || is_set("xag") || is_set("mig") || is_set("tec")) && (is_set("sqd") || is_set("fqca")))
    {
        env->out() << "[e] cannot parse files as both logic networks and cell-level layouts\n";
    }
    else if (is_set("fgl") && (is_set("sqd") || is_set("fqca")))
    {
        env->out() << "[e] cannot parse files as both gate-level and cell-level layouts\n";
    }
    else if (static_cast<int>(is_set("aig")) + static_cast<int>(is_set("xag")) + static_cast<int>(is_set("mig")) +
                 static_cast<int>(is_set("tec")) >
             1)
    {
        env->out() << "[e] select exactly one logic network type (-a/-x/-m/-t)\n";
    }
    else if (is_set("sqd") && is_set("fqca"))
    {
        env->out() << "[e] select exactly one cell-level layout type (--sqd or --fqca)\n";
    }
    else
    {
        const auto store_ntks = [&](auto&& reader)
        {
            for (const auto& ln : reader.get_networks(sort))
            {
                store<fiction::logic_network_t>().extend() = ln;
            }
        };

        try
        {
            if (is_set("aig"))
            {
                fiction::network_reader<fiction::aig_ptr> reader{filename, env->out()};

                store_ntks(reader);
            }
            else if (is_set("xag"))
            {
                fiction::network_reader<fiction::xag_ptr> reader{filename, env->out()};

                store_ntks(reader);
            }
            else if (is_set("mig"))
            {
                fiction::network_reader<fiction::mig_ptr> reader{filename, env->out()};

                store_ntks(reader);
            }
            else if (is_set("tec"))
            {
                fiction::network_reader<fiction::tec_ptr> reader{filename, env->out()};

                store_ntks(reader);
            }
            if (is_set("fgl") || is_set("sqd") || is_set("fqca"))
            {
                if (std::filesystem::exists(filename))
                {
                    if (std::filesystem::is_regular_file(filename))
                    {
                        if (is_set("fgl"))
                        {
                            if (!topology.empty())
                            {
                                try
                                {
                                    if (topology == "cartesian")
                                    {
                                        store<fiction::gate_layout_t>().extend() =
                                            std::make_shared<fiction::cart_gate_clk_lyt>(
                                                fiction::read_fgl_layout<fiction::cart_gate_clk_lyt>(filename));
                                    }
                                    else if (topology == "odd_row_cartesian")
                                    {
                                        store<fiction::gate_layout_t>().extend() =
                                            std::make_shared<fiction::cart_odd_row_gate_clk_lyt>(
                                                fiction::read_fgl_layout<fiction::cart_odd_row_gate_clk_lyt>(filename));
                                    }
                                    else if (topology == "even_row_cartesian")
                                    {
                                        store<fiction::gate_layout_t>().extend() =
                                            std::make_shared<fiction::cart_even_row_gate_clk_lyt>(
                                                fiction::read_fgl_layout<fiction::cart_even_row_gate_clk_lyt>(
                                                    filename));
                                    }
                                    else if (topology == "odd_column_cartesian")
                                    {
                                        store<fiction::gate_layout_t>().extend() =
                                            std::make_shared<fiction::cart_odd_col_gate_clk_lyt>(
                                                fiction::read_fgl_layout<fiction::cart_odd_col_gate_clk_lyt>(filename));
                                    }
                                    else if (topology == "even_column_cartesian")
                                    {
                                        store<fiction::gate_layout_t>().extend() =
                                            std::make_shared<fiction::cart_even_col_gate_clk_lyt>(
                                                fiction::read_fgl_layout<fiction::cart_even_col_gate_clk_lyt>(
                                                    filename));
                                    }
                                    else if (topology == "odd_row_hex")
                                    {
                                        store<fiction::gate_layout_t>().extend() =
                                            std::make_shared<fiction::hex_odd_row_gate_clk_lyt>(
                                                fiction::read_fgl_layout<fiction::hex_odd_row_gate_clk_lyt>(filename));
                                    }
                                    else if (topology == "even_row_hex")
                                    {
                                        store<fiction::gate_layout_t>().extend() =
                                            std::make_shared<fiction::hex_even_row_gate_clk_lyt>(
                                                fiction::read_fgl_layout<fiction::hex_even_row_gate_clk_lyt>(filename));
                                    }
                                    else if (topology == "odd_column_hex")
                                    {
                                        store<fiction::gate_layout_t>().extend() =
                                            std::make_shared<fiction::hex_odd_col_gate_clk_lyt>(
                                                fiction::read_fgl_layout<fiction::hex_odd_col_gate_clk_lyt>(filename));
                                    }
                                    else if (topology == "even_column_hex")
                                    {
                                        store<fiction::gate_layout_t>().extend() =
                                            std::make_shared<fiction::hex_even_col_gate_clk_lyt>(
                                                fiction::read_fgl_layout<fiction::hex_even_col_gate_clk_lyt>(filename));
                                    }
                                    else
                                    {
                                        env->out() << fmt::format("[e] given topology does not exist: {}\n", topology);
                                    }
                                }
                                catch (const fiction::fgl_parsing_error& e)
                                {
                                    env->out() << fmt::format("[e] {}\n", e.what());
                                }
                            }
                            else
                            {
                                env->out() << "[e] for reading gate-level layouts, the topology has to be set. "
                                           << "Valid values: 'cartesian', 'odd_row_cartesian', 'even_row_cartesian', "
                                           << "'odd_column_cartesian', 'even_column_cartesian', 'odd_row_hex', "
                                           << "'even_row_hex', 'odd_column_hex', 'even_column_hex'\n";
                            }
                        }
                        else if (is_set("sqd"))
                        {
                            try
                            {
                                const auto layout_name = std::filesystem::path{filename}.stem().string();

                                if (orientation == "100")
                                {
                                    const auto layout =
                                        fiction::read_sqd_layout<fiction::sidb_100_cell_clk_lyt>(filename, layout_name);
                                    store<fiction::cell_layout_t>().extend() =
                                        std::make_shared<fiction::sidb_100_cell_clk_lyt>(layout);
                                }
                                else if (orientation == "111")
                                {
                                    const auto layout =
                                        fiction::read_sqd_layout<fiction::sidb_111_cell_clk_lyt>(filename, layout_name);
                                    store<fiction::cell_layout_t>().extend() =
                                        std::make_shared<fiction::sidb_111_cell_clk_lyt>(layout);
                                }

                                else
                                {
                                    env->out() << "[e] 'lattice_orientation' must be either '100' or '111'\n";
                                }
                            }
                            catch (const fiction::sqd_parsing_error& e)
                            {
                                env->out() << fmt::format("[e] {}\n", e.what());
                            }
                        }
                        else if (is_set("fqca"))
                        {
                            try
                            {
                                const auto layout_name = std::filesystem::path{filename}.stem().string();

                                store<fiction::cell_layout_t>().extend() =
                                    std::make_shared<fiction::stacked_qca_cell_clk_lyt>(
                                        fiction::read_fqca_layout<fiction::stacked_qca_cell_clk_lyt>(filename,
                                                                                                     layout_name));
                            }
                            catch (const fiction::unsupported_character_exception& e)
                            {
                                env->out() << fmt::format("[e] character '{}' is not supported as a cell designator\n",
                                                          e.which());
                            }
                            catch (const fiction::undefined_cell_label_exception& e)
                            {
                                env->out()
                                    << fmt::format("[e] cell label '{}' was used in the cell definition section but "
                                                   "never defined above\n",
                                                   e.which());
                            }
                            catch (const fiction::unrecognized_cell_definition_exception& e)
                            {
                                env->out() << fmt::format("[e] unsupported cell definition in line {}\n", e.where());
                            }
                        }
                    }
                    else
                    {
                        env->out() << "[e] given file name does not point to a regular file\n";
                    }
                }
                else
                {
                    env->out() << "[e] given file name does not exist\n";
                }
            }
        }
        catch (...)
        {
            env->out() << "[e] I/O error: no file could be read\n";
        }
    }

    // reset flags, necessary for some reason... alice bug?
    sort = false;
}

}  // namespace alice
