//
// Created by marcel on 06.02.23.
//

#ifndef FICTION_CMD_VERILOG_HPP
#define FICTION_CMD_VERILOG_HPP

#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <mockturtle/io/write_verilog.hpp>
#include <mockturtle/traits.hpp>

#include <filesystem>
#include <ostream>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{
/**
 * Generates a Verilog file for the current logic network or gate-level layout in store and writes it to the given path.
 */
class verilog_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit verilog_command(const environment::ptr& e) :
            //            command(e, "Generates a verilog file for the current logic network or gate-level layout in
            //            store.")
            command(e, "Generates a Verilog file for the current logic network in store.")
    {
        //        add_flag("--logic_network,-n", "Generate a Verilog file for a logic network");
        //        add_flag("--gate_layout,-g", "Generate a Verilog file for a gate-level layout");
        add_option("filename", filename, "Verilog file name");
    }

  protected:
    /**
     * Function to perform the output call. Generates a Verilog file.
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

        write_verilog_callback(s.current());
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
        //            write_verilog_callback(s.current());
        //        }
    }

  private:
    /**
     * File name to write the Verilog file into.
     */
    std::string filename;

    template <typename NtkOrLytVariant>
    void write_verilog_callback(const NtkOrLytVariant& ntk_or_lyt_variant)
    {
        const auto get_name = [](auto&& ntk_or_lyt_ptr) -> std::string { return fiction::get_name(*ntk_or_lyt_ptr); };

        const auto write_verilog = [this](auto&& ntk_or_lyt_ptr)
        {
            using Ntk = std::decay_t<decltype(*ntk_or_lyt_ptr)>;

            std::string name{};

            // workaround for mockturtle's write_verilog function to not write the name of the network as module name
            if constexpr (mockturtle::has_get_network_name_v<Ntk>)
            {
                // save network name
                name = ntk_or_lyt_ptr->get_network_name();

                // set network name to empty string
                if constexpr (mockturtle::has_set_network_name_v<Ntk>)
                {
                    ntk_or_lyt_ptr->set_network_name("");
                }
            }

            // write verilog file
            mockturtle::write_verilog(*ntk_or_lyt_ptr, filename);

            // restore network name
            if constexpr (mockturtle::has_set_network_name_v<Ntk>)
            {
                if (!name.empty())
                {
                    ntk_or_lyt_ptr->set_network_name(name);
                }
            }
        };

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
        // add .v file extension if necessary
        if (std::filesystem::path(filename).extension() != ".v")
        {
            filename += ".v";
        }

        try
        {
            std::visit(write_verilog, ntk_or_lyt_variant);
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

ALICE_ADD_COMMAND(verilog, "I/O")

}  // namespace alice

#endif  // FICTION_CMD_VERILOG_HPP
