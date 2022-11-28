//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_READ_HPP
#define FICTION_CMD_READ_HPP

#include <fiction/io/network_reader.hpp>
#include <fiction/io/read_fqca_layout.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>

#include <memory>
#include <string>

namespace alice
{
/**
 * Shorthand for all read commands. Chooses the proper function by the file ending.
 *
 * Currently parses Verilog, AIGER, and BLIF using the lorina parsers.
 *
 * Parses FQCA via a custom reader function.
 *
 * For more information see: https://github.com/hriener/lorina
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
                       "Cell-level layouts: FQCA.\n"
                       "In a directory, only files with extension '.v', '.aig', '.blif' are considered.")
    {
        add_option("filename", filename, "Filename or directory")->required();
        add_flag("--aig,-a", "Parse file as AIG");
        add_flag("--xag,-x", "Parse file as XAG");
        add_flag("--mig,-m", "Parse file as MIG");
        add_flag("--tec,-t", "Parse file as technology network");
        add_flag("--qca,-q", "Parse file as QCA cell-level layout");
        add_flag("--sort,-s", sort, "Sort networks in given directory by vertex count prior to storing them");
    }

  protected:
    /**
     * Function to perform the read call. Reads Verilog and creates a logic_network.
     */
    void execute() override
    {
        const auto store_ntks = [&](auto&& reader)
        {
            for (const auto& ln : reader.get_networks(sort))
            {
                store<fiction::logic_network_t>().extend() = ln;
            }
        };

        if (!is_set("aig") && !is_set("xag") && !is_set("mig") && !is_set("tec") && !is_set("qca"))
        {
            env->out() << "[e] at least one network or layout type must be specified" << std::endl;
        }
        else if ((is_set("aig") || is_set("xag") || is_set("mig") || is_set("tec")) && is_set("qca"))
        {
            env->out() << "[e] cannot parse files as both logic networks and cell-level layouts" << std::endl;
        }
        else
        {
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
                if (is_set("qca"))
                {
                    if (std::filesystem::exists(filename))
                    {
                        if (std::filesystem::is_regular_file(filename))
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
                                env->out()
                                    << fmt::format("character '{}' is not supported as a cell designator", e.which())
                                    << std::endl;
                            }
                            catch (const fiction::undefined_cell_label_exception& e)
                            {
                                env->out() << fmt::format("cell label '{}' was used in the cell definition section but "
                                                          "never defined above",
                                                          e.which())
                                           << std::endl;
                            }
                            catch (const fiction::unrecognized_cell_definition_exception& e)
                            {
                                env->out()
                                    << fmt::format("unsupported cell definition in line {}", e.where()) << std::endl;
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
                env->out() << "[e] no networks or layouts were read" << std::endl;
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
     * Flag to indicate that files should be sorted by file size.
     */
    bool sort = false;
};

ALICE_ADD_COMMAND(read, "I/O")

}  // namespace alice

#endif  // FICTION_CMD_READ_HPP
