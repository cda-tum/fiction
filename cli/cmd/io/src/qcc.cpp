//
// Created by marcel on 24.10.19.
//

#include "cmd/io/include/qcc.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/io/write_qcc_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{

qcc_command::qcc_command(const environment::ptr& e) :
        command(e, "Generates a ToPoliNano/MagCAD component file for the current iNML cell layout in store. "
                   "ToPoliNano can reuse the component in hierarchical layouts while MagCAD is able to "
                   "perform physical simulations.")
{
    add_option("filename", filename, "QCC file name");
    add_flag("-c,--component_name", ps.use_filename_as_component_name,
             "Use given file name as the component's identifier");
}

void qcc_command::execute()
{
    auto& s = store<fiction::cell_layout_t>();

    // error case: empty cell layout store
    if (s.empty())
    {
        env->out() << "[w] no cell layout in store\n";

        ps = {};
        return;
    }

    const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

    const auto write_qcc = [this, &get_name](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (fiction::has_inml_technology_v<Lyt>)
        {
            fiction::write_qcc_layout(*lyt_ptr, filename, ps);
        }
        else
        {
            env->out() << fmt::format("[e] {}'s cell technology is not iNML but {}\n", get_name(lyt_ptr),
                                      fiction::tech_impl_name<fiction::technology<Lyt>>);
        }
    };

    const auto& lyt = s.current();

    // error case: do not override directories
    if (std::filesystem::is_directory(filename))
    {
        env->out() << "[e] cannot override a directory\n";

        ps = {};
        return;
    }
    // if filename was not given, use stored layout name
    if (filename.empty())
    {
        filename = std::visit(get_name, lyt);
    }
    // add .qca file extension if necessary
    if (std::filesystem::path(filename).extension() != ".qcc")
    {
        filename += ".qcc";
    }

    try
    {
        std::visit(write_qcc, lyt);
    }
    catch (const std::ofstream::failure& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (const std::invalid_argument& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (...)
    {
        env->out() << "[e] an error occurred while the file was being written; it could be corrupted\n";
    }

    ps = {};
}

}  // namespace alice
