//
// Created by marcel on 24.10.19.
//

#include "cmd/technology/include/cell.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_ports.hpp>
#include <fiction/technology/fcn_gate_library.hpp>
#include <fiction/technology/inml_topolinano_library.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{

cell_command::cell_command(const environment::ptr& e) :
        command(e, "Compiles the current gate layout in store down to a cell-level layout. A gate library must be "
                   "specified in order to instruct the algorithm how to map gate tiles to cell blocks.")
{
    add_option("--library,-l", library, "Gate library to use for mapping {QCA-ONE, ToPoliNano, Bestagon}", true);
}

void cell_command::execute()
{
    auto& s = store<fiction::gate_layout_t>();

    // error case: empty gate layout store
    if (s.empty())
    {
        env->out() << "[w] no gate layout in store\n";
        reset_flags();
        return;
    }

    const auto lib_orig = library;
    std::transform(library.begin(), library.end(), library.begin(), ::toupper);

    if (library == "QCA-ONE" || library == "QCAONE" || library == "QCA ONE")
    {
        const auto apply_qca_one = [](auto&& lyt_ptr)
        {
            return std::make_shared<fiction::qca_cell_clk_lyt>(
                fiction::apply_gate_library<fiction::qca_cell_clk_lyt, fiction::qca_one_library>(*lyt_ptr));
        };

        try
        {
            store<fiction::cell_layout_t>().extend() = std::visit(apply_qca_one, s.current());
        }
        catch (const fiction::unsupported_gate_type_exception<fiction::offset::ucoord_t>& e)
        {
            std::cout << fmt::format("[e] unsupported gate type at tile position {}\n", e.where());
        }
        catch (
            const fiction::unsupported_gate_orientation_exception<fiction::offset::ucoord_t, fiction::port_position>& e)
        {
            std::cout << fmt::format("[e] unsupported gate orientation at tile position {} with ports {}\n", e.where(),
                                     e.which_ports());
        }
    }
    else if (library == "TOPOLINANO")
    {
        const auto apply_inml_topolinano = [](auto&& lyt_ptr) -> std::optional<fiction::inml_cell_clk_lyt_ptr>
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::is_shifted_cartesian_layout_v<Lyt>)
            {
                if constexpr (fiction::has_vertically_shifted_cartesian_orientation_v<Lyt>)
                {
                    return std::make_shared<fiction::inml_cell_clk_lyt>(
                        fiction::apply_gate_library<fiction::inml_cell_clk_lyt, fiction::inml_topolinano_library>(
                            *lyt_ptr));
                }
                else
                {
                    std::cout << "[e] Cartesian shift must be vertical\n";
                }
            }
            else
            {
                std::cout << "[e] layout topology must be shifted Cartesian\n";
            }

            return std::nullopt;
        };

        try
        {
            if (const auto cell_lyt = std::visit(apply_inml_topolinano, s.current()); cell_lyt.has_value())
            {
                store<fiction::cell_layout_t>().extend() = *cell_lyt;
            }
        }
        catch (const fiction::unsupported_gate_type_exception<fiction::offset::ucoord_t>& e)
        {
            std::cout << fmt::format("[e] unsupported gate type at tile position {}\n", e.where());
        }
        catch (
            const fiction::unsupported_gate_orientation_exception<fiction::offset::ucoord_t, fiction::port_position>& e)
        {
            std::cout << fmt::format("[e] unsupported gate orientation at tile position {} with ports {}\n", e.where(),
                                     e.which_ports());
        }
    }
    else if (library == "BESTAGON")
    {
        const auto apply_sidb_bestagon = [](auto&& lyt_ptr) -> std::optional<fiction::sidb_100_cell_clk_lyt_ptr>
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::is_hexagonal_layout_v<Lyt>)
            {
                if constexpr (fiction::has_pointy_top_hex_orientation_v<Lyt>)
                {
                    return std::make_shared<fiction::sidb_100_cell_clk_lyt>(
                        fiction::apply_gate_library<fiction::sidb_100_cell_clk_lyt, fiction::sidb_bestagon_library>(
                            *lyt_ptr));
                }
                else
                {
                    std::cout << "[e] hexagonal orientation must be pointy-top\n";
                }
            }
            else
            {
                std::cout << "[e] layout topology must be hexagonal\n";
            }

            return std::nullopt;
        };

        try
        {
            if (const auto cell_lyt = std::visit(apply_sidb_bestagon, s.current()); cell_lyt.has_value())
            {
                store<fiction::cell_layout_t>().extend() = *cell_lyt;
            }
        }
        catch (const fiction::unsupported_gate_type_exception<fiction::offset::ucoord_t>& e)
        {
            std::cout << fmt::format("[e] unsupported gate type at tile position {}\n", e.where());
        }
        catch (
            const fiction::unsupported_gate_orientation_exception<fiction::offset::ucoord_t, fiction::port_position>& e)
        {
            std::cout << fmt::format("[e] unsupported gate orientation at tile position {} with ports {}\n", e.where(),
                                     e.which_ports());
        }
        catch (const fiction::unsupported_gate_orientation_exception<fiction::offset::ucoord_t,
                                                                     fiction::port_direction>& e)
        {
            std::cout << fmt::format("[e] unsupported gate orientation at tile position {} with port directions {}\n",
                                     e.where(), e.which_ports());
        }
    }
    // more libraries go here
    else
    {
        env->out() << fmt::format("[e] identifier '{}' does not refer to a supported gate library\n", lib_orig);
        reset_flags();
        return;
    }

    reset_flags();
}

void cell_command::reset_flags()
{
    library = "QCA-ONE";
}

}  // namespace alice
