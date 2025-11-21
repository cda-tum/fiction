//
// Created by marcel on 22.07.21.
//

#include "cmd/io/include/blif.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <mockturtle/io/write_blif.hpp>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>
#include <variant>

namespace alice
{

blif_command::blif_command(const environment::ptr& e) :
        command(e, "Generates a BLIF file for the current logic network in store.")
{
    add_option("filename", filename, "BLIF file name");
}

void blif_command::execute()
{
    auto& s = store<fiction::logic_network_t>();

    // error case: empty logic network store
    if (s.empty())
    {
        env->out() << "[w] no logic network in store\n";
        filename.clear();
        return;
    }

    write_blif_callback(s.current());
}

template <typename NtkOrLytVariant>
void blif_command::write_blif_callback(const NtkOrLytVariant& ntk_or_lyt_variant)
{
    const auto get_name = [](auto&& ntk_or_lyt_ptr) -> std::string { return fiction::get_name(*ntk_or_lyt_ptr); };

    const auto write_blif = [this](auto&& ntk_or_lyt_ptr) { mockturtle::write_blif(*ntk_or_lyt_ptr, filename); };

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
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (...)
    {
        env->out() << "[e] an error occurred while the file was being written; it could be corrupted\n";
    }

    filename.clear();
}

}  // namespace alice
