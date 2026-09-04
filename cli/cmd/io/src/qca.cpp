/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Implements the `qca` command.
 * @author Marcel Walter (marcelwa)
 */

#include "cmd/io/include/qca.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/networks/name_utils.hpp>
#include <fiction/technology/qca/io/write_qca_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{

qca_command::qca_command(const environment::ptr& e) :
        command(e, "Generates a QCADesigner file for the current QCA cell layout in store. "
                   "QCADesigner can be used to perform physical simulations.")
{
    add_option("filename", filename, "QCA file name");
    add_flag("--no_via_layers,-v", "Do not insert additional inter-layer via cells");
}

void qca_command::execute()
{
    auto& s = store<fiction::cell_layout_t>();

    // error case: empty cell layout store
    if (s.empty())
    {
        env->out() << "[w] no cell layout in store\n";

        ps = {};
        filename.clear();
        return;
    }

    if (is_set("no_via_layers"))
    {
        ps.create_inter_layer_via_cells = false;
    }

    const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::cli::name_of(*lyt_ptr); };

    const auto write_qca = [this, &get_name](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (fiction::cli::is_sidb_store_v<Lyt>)
        {
            env->out() << fmt::format("[e] {} is an SiDB layout\n", get_name(lyt_ptr));
        }
        else if constexpr (fiction::has_qca_technology_v<Lyt>)
        {
            fiction::qca::io::write_qca_layout(*lyt_ptr, filename, ps);
        }
        else
        {
            env->out() << fmt::format("[e] {}'s cell technology is not QCA but {}\n", get_name(lyt_ptr),
                                      fiction::tech_impl_name<fiction::technology<Lyt>>);
        }
    };

    const auto& lyt = s.current();

    // error case: do not override directories
    if (std::filesystem::is_directory(filename))
    {
        env->out() << "[e] cannot override a directory\n";

        ps = {};
        filename.clear();
        return;
    }
    // if filename was not given, use stored layout name
    if (filename.empty())
    {
        filename = std::visit(get_name, lyt);
    }
    // add .qca file extension if necessary
    if (std::filesystem::path(filename).extension() != ".qca")
    {
        filename += ".qca";
    }

    try
    {
        std::visit(write_qca, lyt);
    }
    catch (const std::ofstream::failure& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (...)
    {
        env->out() << "[e] an error occurred while the file was being written; it could be corrupted\n";
    }

    ps = {};
    filename.clear();
}

}  // namespace alice
