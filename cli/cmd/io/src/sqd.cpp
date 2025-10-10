//
// Created by marcel on 14.07.21.
//

#include "cmd/io/include/sqd.hpp"

#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{

sqd_command::sqd_command(const environment::ptr& e) :
        command(e, "Generates a SiQAD file for the current QCA or SiDB cell layout in store. "
                   "SiQAD can be used to perform physical simulations.")
{
    add_option("filename", filename, "SQD file name");
}

void sqd_command::execute()
{
    auto& s = store<fiction::cell_layout_t>();

    // error case: empty cell layout store
    if (s.empty())
    {
        env->out() << "[w] no cell layout in store\n";
        return;
    }

    const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

    const auto write_sqd = [this, &get_name](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (fiction::has_qca_technology_v<Lyt> || fiction::has_sidb_technology_v<Lyt>)
        {
            fiction::write_sqd_layout(*lyt_ptr, filename);
        }
        else
        {
            env->out() << fmt::format("[e] {}'s cell technology is not QCA or SiDB but {}\n", get_name(lyt_ptr),
                                      fiction::tech_impl_name<fiction::technology<Lyt>>);
        }
    };

    const auto& lyt = s.current();

    // error case: do not override directories
    if (std::filesystem::is_directory(filename))
    {
        env->out() << "[e] cannot override a directory\n";
        return;
    }
    // if filename was not given, use stored layout name
    if (filename.empty())
    {
        filename = std::visit(get_name, lyt);
    }
    // add .sqd file extension if necessary
    if (std::filesystem::path(filename).extension() != ".sqd")
    {
        filename += ".sqd";
    }

    try
    {
        std::visit(write_sqd, lyt);
    }
    catch (const std::ofstream::failure& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (...)
    {
        env->out() << "[e] an error occurred while the file was being written; it could be corrupted\n";
    }
}

}  // namespace alice
