//
// Created by marcel on 06.02.23.
//

#include "cmd/io/include/verilog.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <mockturtle/io/write_verilog.hpp>
#include <mockturtle/traits.hpp>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{

verilog_command::verilog_command(const environment::ptr& e) :
        command(e, "Generates a Verilog file for the current logic network in store.")
{
    add_option("filename", filename, "Verilog file name");
}

void verilog_command::execute()
{
    auto& s = store<fiction::logic_network_t>();

    // error case: empty logic network store
    if (s.empty())
    {
        env->out() << "[w] no logic network in store\n";
        filename.clear();
        return;
    }

    write_verilog_callback(s.current());
}

template <typename NtkOrLytVariant>
void verilog_command::write_verilog_callback(const NtkOrLytVariant& ntk_or_lyt_variant)
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
        try
        {
            mockturtle::write_verilog(*ntk_or_lyt_ptr, filename);
        }
        catch (...)
        {
            if constexpr (mockturtle::has_set_network_name_v<Ntk>)
            {
                if (!name.empty())
                {
                    ntk_or_lyt_ptr->set_network_name(name);
                }
            }
            throw;
        }
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
        env->out() << "[e] cannot override a directory\n";
        filename.clear();
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
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (...)
    {
        env->out() << "[e] an error occurred while the file was being written; it could be corrupted\n";
    }

    filename.clear();
}

}  // namespace alice
