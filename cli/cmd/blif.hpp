//
// Created by marcel on 22.07.21.
//

#ifndef FICTION_BLIF_HPP
#define FICTION_BLIF_HPP

#include <fiction/types.hpp>

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

    template <typename LytOrNtk>
    void write_blif_callback(const LytOrNtk& lyt_or_ntk)
    {
        const auto get_name = [](auto&& net) -> std::string { return net->get_network_name(); };

        const auto write_blif = [this](auto&& net) { mockturtle::write_blif(*net, filename); };

        // error case: do not override directories
        if (std::filesystem::is_directory(filename))
        {
            env->out() << "[e] cannot override a directory" << std::endl;
            return;
        }
        // if filename was not given, use stored layout name
        if (!is_set("filename"))
        {
            filename = std::visit(get_name, lyt_or_ntk);
        }
        // add .v file extension if necessary
        if (std::filesystem::path(filename).extension() != ".blif")
        {
            filename += ".blif";
        }

        try
        {
            std::visit(write_blif, lyt_or_ntk);
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

#endif  // FICTION_BLIF_HPP
