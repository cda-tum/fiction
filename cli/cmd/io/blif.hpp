//
// Created by marcel on 22.07.21.
//

#ifndef FICTION_CMD_BLIF_HPP
#define FICTION_CMD_BLIF_HPP

#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <mockturtle/io/write_blif.hpp>

#include <filesystem>
#include <ostream>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{
/**
 * Generates a BLIF file for the current logic network or gate-level layout in store and writes it to the given path.
 */
class blif_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit blif_command(const environment::ptr& e) :
            //            command(e, "Generates a BLIF file for the current logic network or gate-level layout in
            //            store.")
            command(e, "Generates a BLIF file for the current logic network in store.")
    {
        //        add_flag("--logic_network,-n", "Generate a BLIF file for a logic network");
        //        add_flag("--gate_layout,-g", "Generate a BLIF file for a gate-level layout");
        add_option("filename", filename, "BLIF file name");
    }

  protected:
    /**
     * Function to perform the output call. Generates a BLIF file.
     */
    void execute() override
    {
        //        if (!is_set("logic_network") && !is_set("gate_layout"))
        //        {
        //            env->out() << "[w] at least one store must be specified" << std::endl;
        //            return;
        //        }

        //        if (is_set("logic_network"))
        //        {
        auto& s = store<fiction::logic_network_t>();

        // error case: empty logic network store
        if (s.empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;
            return;
        }

        write_blif_callback(s.current());
        //        }

        //        if (is_set("gate_layout"))
        //        {
        //            auto& s = store<fiction::gate_layout_t>();
        //
        //            // error case: empty gate layout store
        //            if (s.empty())
        //            {
        //                env->out() << "[w] no gate layout in store" << std::endl;
        //                return;
        //            }
        //
        //            write_blif_callback(s.current());
        //        }
    }

  private:
    /**
     * File name to write the BLIF file into.
     */
    std::string filename;

    template <typename NtkOrLytVariant>
    void write_blif_callback(const NtkOrLytVariant& ntk_or_lyt_variant)
    {
        const auto get_name = [](auto&& ntk_or_lyt_ptr) -> std::string { return fiction::get_name(*ntk_or_lyt_ptr); };

        const auto write_blif = [this](auto&& ntk_or_lyt_ptr) { mockturtle::write_blif(*ntk_or_lyt_ptr, filename); };

        // error case: do not override directories
        if (std::filesystem::is_directory(filename))
        {
            env->out() << "[e] cannot override a directory" << std::endl;
            return;
        }
        // if filename was not given, use stored layout name
        if (filename.empty())
        {
            filename = std::visit(get_name, ntk_or_lyt_variant);
        }
        // add .blif file extension if necessary
        if (std::filesystem::path(filename).extension() != ".blif")
        {
            filename += ".blif";
        }

        try
        {
            std::visit(write_blif, ntk_or_lyt_variant);
        }
        catch (const std::ofstream::failure& e)
        {
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
        catch (...)
        {
            env->out() << "[e] an error occurred while the file was being written; it could be corrupted" << std::endl;
        }
    }
};

ALICE_ADD_COMMAND(blif, "I/O")

}  // namespace alice

#endif  // FICTION_CMD_BLIF_HPP
