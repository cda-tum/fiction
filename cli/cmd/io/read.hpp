//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_READ_HPP
#define FICTION_CMD_READ_HPP

#include <fiction/io/network_reader.hpp>
#include <fiction/io/read_fgl_layout.hpp>
#include <fiction/io/read_fqca_layout.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>

#include <filesystem>
#include <memory>
#include <string>

namespace alice
{
/**
 * Shorthand for all read commands. Chooses the proper function by the file ending.
 *
 * Currently parses Verilog, AIGER, and BLIF using the lorina parsers.
 *
 * For more information see: https://github.com/hriener/lorina
 *
 * Parses FGL, SQD, and FQCA via custom reader functions.
 */
class read_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit read_command(const environment::ptr& e) :
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

  protected:
    /**
     * Function to perform the read call. Reads a network or layout from a file.
     */
    void execute() override
    {
        if (!is_set("aig") && !is_set("xag") && !is_set("mig") && !is_set("tec") && !is_set("fgl") && !is_set("sqd") &&
            !is_set("fqca"))
        {
            env->out() << "[e] at least one network or layout type must be specified" << std::endl;
        }
        else if ((is_set("aig") || is_set("xag") || is_set("mig") || is_set("tec")) && is_set("fgl"))
        {
            env->out() << "[e] cannot parse files as both logic networks and gate-level layouts" << std::endl;
        }
        else if ((is_set("aig") || is_set("xag") || is_set("mig") || is_set("tec")) &&
                 (is_set("sqd") || is_set("fqca")))
        {
            env->out() << "[e] cannot parse files as both logic networks and cell-level layouts" << std::endl;
        }
        else if (is_set("fgl") && (is_set("sqd") || is_set("fqca")))
        {
            env->out() << "[e] cannot parse files as both gate-level and cell-level layouts" << std::endl;
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
                if (is_set("xag"))
                {
                    fiction::network_reader<fiction::xag_ptr> reader{filename, env->out()};

                    store_ntks(reader);
                }
                if (is_set("mig"))
                {
                    fiction::network_reader<fiction::mig_ptr> reader{filename, env->out()};

                    store_ntks(reader);
                }
                if (is_set("tec"))
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
                                                    fiction::read_fgl_layout<fiction::cart_odd_row_gate_clk_lyt>(
                                                        filename));
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
                                                    fiction::read_fgl_layout<fiction::cart_odd_col_gate_clk_lyt>(
                                                        filename));
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
                                                    fiction::read_fgl_layout<fiction::hex_odd_row_gate_clk_lyt>(
                                                        filename));
                                        }
                                        else if (topology == "even_row_hex")
                                        {
                                            store<fiction::gate_layout_t>().extend() =
                                                std::make_shared<fiction::hex_even_row_gate_clk_lyt>(
                                                    fiction::read_fgl_layout<fiction::hex_even_row_gate_clk_lyt>(
                                                        filename));
                                        }
                                        else if (topology == "odd_column_hex")
                                        {
                                            store<fiction::gate_layout_t>().extend() =
                                                std::make_shared<fiction::hex_odd_col_gate_clk_lyt>(
                                                    fiction::read_fgl_layout<fiction::hex_odd_col_gate_clk_lyt>(
                                                        filename));
                                        }
                                        else if (topology == "even_column_hex")
                                        {
                                            store<fiction::gate_layout_t>().extend() =
                                                std::make_shared<fiction::hex_even_col_gate_clk_lyt>(
                                                    fiction::read_fgl_layout<fiction::hex_even_col_gate_clk_lyt>(
                                                        filename));
                                        }
                                        else
                                        {
                                            env->out() << fmt::format("[e] given topology does not exist: {}", topology)
                                                       << std::endl;
                                        }
                                    }
                                    catch (const fiction::fgl_parsing_error& e)
                                    {
                                        env->out() << e.what() << std::endl;
                                    }
                                }
                                else
                                {
                                    env->out() << "[e] for reading gate-level layouts, the topology has to be set"
                                               << std::endl;
                                }
                            }
                            else if (is_set("sqd"))
                            {
                                try
                                {
                                    const auto layout_name = std::filesystem::path{filename}.stem().string();

                                    if (orientation == "100")
                                    {
                                        const auto layout = fiction::read_sqd_layout<fiction::sidb_100_cell_clk_lyt>(
                                            filename, layout_name);
                                        store<fiction::cell_layout_t>().extend() =
                                            std::make_shared<fiction::sidb_100_cell_clk_lyt>(layout);
                                    }
                                    else if (orientation == "111")
                                    {
                                        const auto layout = fiction::read_sqd_layout<fiction::sidb_111_cell_clk_lyt>(
                                            filename, layout_name);
                                        store<fiction::cell_layout_t>().extend() =
                                            std::make_shared<fiction::sidb_111_cell_clk_lyt>(layout);
                                    }
                                }
                                catch (const fiction::sqd_parsing_error& e)
                                {
                                    env->out() << e.what() << std::endl;
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
                                    env->out() << fmt::format("character '{}' is not supported as a cell designator",
                                                              e.which())
                                               << std::endl;
                                }
                                catch (const fiction::undefined_cell_label_exception& e)
                                {
                                    env->out()
                                        << fmt::format("cell label '{}' was used in the cell definition section but "
                                                       "never defined above",
                                                       e.which())
                                        << std::endl;
                                }
                                catch (const fiction::unrecognized_cell_definition_exception& e)
                                {
                                    env->out() << fmt::format("unsupported cell definition in line {}", e.where())
                                               << std::endl;
                                }
                            }
                        }
                        else
                        {
                            env->out() << "[e] given file name does not point to a regular file" << std::endl;
                        }
                    }
                    else
                    {
                        env->out() << "[e] given file name does not exist" << std::endl;
                    }
                }
            }
            catch (...)
            {
                env->out() << "[e] I/O error: no file could be read" << std::endl;
            }
        }

        // reset flags, necessary for some reason... alice bug?
        sort = false;
    }

  private:
    /**
     * Verilog filename.
     */
    std::string filename;
    /**
     * Gate-level layout topology.
     */
    std::string topology;
    /**
     * Flag to indicate that files should be sorted by file size.
     */
    bool sort = false;
    /**
     * Identifier of H-Si lattice orientation.
     */
    std::string orientation{"100"};
};

ALICE_ADD_COMMAND(read, "I/O")

}  // namespace alice

#endif  // FICTION_CMD_READ_HPP
