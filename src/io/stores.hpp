//
// Created by marcel on 16.07.18.
//

#ifndef FICTION_STORES_HPP
#define FICTION_STORES_HPP

#include "types.hpp"
//#include "svg_writer.h"
#include "dot_drawers.hpp"
#include "print_layout.hpp"

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <kitty/print.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <type_traits>
#include <variant>

namespace alice
{
/**
 * Truth tables.
 *
 * Parts of this code are originally from: https://github.com/msoeken/cirkit/blob/cirkit3/cli/stores/tt.hpp.
 */
ALICE_ADD_STORE(fiction::truth_table_t, "truth_table", "t", "truth table", "truth tables")

ALICE_DESCRIBE_STORE(fiction::truth_table_t, tt)
{
    if (tt->num_vars() <= 6)
        return fmt::format("{} vars, hex: {}, bin: {}", tt->num_vars(), kitty::to_hex(*tt), kitty::to_binary(*tt));
    else
        return fmt::format("{} vars, (description omitted due to truth table size)", tt->num_vars());
}

ALICE_PRINT_STORE_STATISTICS(fiction::truth_table_t, os, tt)
{
    if (tt->num_vars() <= 6)
        fmt::print(os, " {} vars, hex: {}, bin: {}", tt->num_vars(), kitty::to_hex(*tt), kitty::to_binary(*tt));
    else
        fmt::print(os, " {} vars, (description omitted due to truth table size)", tt->num_vars());
}

ALICE_LOG_STORE_STATISTICS(fiction::truth_table_t, tt)
{
    return {{"vars", tt->num_vars()}, {"hex", kitty::to_hex(*tt)}, {"binary", kitty::to_binary(*tt)}};
}

ALICE_PRINT_STORE(fiction::truth_table_t, os, tt)
{
    fmt::print(os, " {} vars, hex: {}, bin: {}", tt->num_vars(), kitty::to_hex(*tt), kitty::to_binary(*tt));
}

/**
 * Logic networks.
 */
ALICE_ADD_STORE(fiction::logic_network_t, "network", "n", "logic network", "logic networks")

ALICE_DESCRIBE_STORE(fiction::logic_network_t, ln)
{
    const auto describe = [](auto&& net)
    {
        using Ntk = typename std::decay_t<decltype(net)>::element_type;

        mockturtle::depth_view depth_net{*net};

        return fmt::format("{} ({}) - I/O: {}/{}, gates: {}, level: {}", net->get_network_name(),
                           fiction::ntk_type_name<Ntk>, net->num_pis(), net->num_pos(), net->num_gates(),
                           depth_net.depth());
    };

    return std::visit(describe, ln);
}

ALICE_PRINT_STORE_STATISTICS(fiction::logic_network_t, os, ln)
{
    const auto print_statistics = [&os](auto&& net)
    {
        using Ntk = typename std::decay_t<decltype(net)>::element_type;

        mockturtle::depth_view depth_net{*net};

        fmt::print(os, "{} ({}) - I/O: {}/{}, gates: {}, level: {}\n", net->get_network_name(),
                   fiction::ntk_type_name<Ntk>, net->num_pis(), net->num_pos(), net->num_gates(), depth_net.depth());
    };

    std::visit(print_statistics, ln);
}

ALICE_LOG_STORE_STATISTICS(fiction::logic_network_t, ln)
{
    const auto log_statistics = [](auto&& net)
    {
        using Ntk = typename std::decay_t<decltype(net)>::element_type;

        mockturtle::depth_view depth_net{*net};

        return nlohmann::json{{"name", net->get_network_name()}, {"type", fiction::ntk_type_name<Ntk>},
                              {"inputs", net->num_pis()},        {"outputs", net->num_pos()},
                              {"gates", net->num_gates()},       {"level", depth_net.depth()}};
    };

    return std::visit(log_statistics, ln);
}

template <>
bool can_show<fiction::logic_network_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    extension = "dot";

    cmd.add_flag("--indexes,-i", "Show node indexes")->group("logic_network (-n)");

    return true;
}

template <>
void show<fiction::logic_network_t>(std::ostream& os, const fiction::logic_network_t& element, const command& cmd)
{
    const auto show_net = [&os, &cmd](auto&& net)
    {
        try
        {
            mockturtle::depth_view depth_net{*net};

            using Ntk = typename std::decay_t<decltype(depth_net)>;

            if (cmd.is_set("indexes"))
            {
                mockturtle::write_dot(depth_net, os, fiction::topology_dot_drawer<Ntk, true>());
            }
            else
            {
                mockturtle::write_dot(depth_net, os, fiction::topology_dot_drawer<Ntk, false>());
            }
        }
        catch (const std::invalid_argument& e)
        {
            cmd.env->out() << "[e] " << e.what() << std::endl;
        }
    };

    std::visit(show_net, element);
}

/**
 * FCN gate-level layouts.
 */
ALICE_ADD_STORE(fiction::gate_layout_t, "gate_layout", "g", "gate layout", "gate layouts")

ALICE_PRINT_STORE(fiction::gate_layout_t, os, layout)
{
    const auto print = [&os](auto&& lyt) { fiction::print_gate_level_layout(os, *lyt); };

    std::visit(print, layout);
}

ALICE_DESCRIBE_STORE(fiction::gate_layout_t, layout)
{
    const auto describe = [](auto&& lyt)
    {
        return fmt::format("{} - {} × {}, gates: {}, wires: {}", lyt->get_network_name(), lyt->x() + 1, lyt->y() + 1,
                           lyt->num_gates(), lyt->num_wires());
    };

    return std::visit(describe, layout);
}

ALICE_PRINT_STORE_STATISTICS(fiction::gate_layout_t, os, layout)
{
    // TODO crossings, latches, critical path, throughput
    const auto print_statistics = [&os](auto&& lyt)
    {
        mockturtle::depth_view depth_lyt{*lyt};

        return fmt::print(os, "{} - {} × {}, gates: {}, wires: {}, CP: {}\n", lyt->get_network_name(), lyt->x() + 1,
                          lyt->y() + 1, lyt->num_gates(), lyt->num_wires(), depth_lyt.depth());
    };

    std::visit(print_statistics, layout);
}

ALICE_LOG_STORE_STATISTICS(fiction::gate_layout_t, layout)
{
    const auto log_statistics = [](auto&& lyt)
    {
        mockturtle::depth_view depth_lyt{*lyt};

        return nlohmann::json{
            {"name", lyt->get_network_name()},
            {"inputs", lyt->num_pis()},
            {"outputs", lyt->num_pos()},
            {"gates", lyt->num_gates()},
            // {"layout", {{"x-size", layout->x() + 1}, {"y-size", layout->y() + 1}, {"area", area}}},
            // {"bounding box", {{"x-size", bb.x_size}, {"y-size", bb.y_size}, {"area", bb.area()}}},
            // {"gate tiles", gate_tiles},
            // {"wire tiles", wire_tiles},
            // {"free tiles", area - (gate_tiles + wire_tiles - crossings)},  // free tiles in ground layer
            // {"crossings", crossings},
            // {"latches", layout->latch_count()},
            {"critical path", depth_lyt.depth()}
            // {"throughput", fmt::format("1/{}", tp)}};
        };
    };

    return std::visit(log_statistics, layout);
}

///**
// * FCN cell layouts.
// */
// ALICE_ADD_STORE(fcn_cell_layout_ptr, "cell_layout", "c", "cell layout", "cell layouts")
//
// ALICE_PRINT_STORE(fcn_cell_layout_ptr, os, layout)
//{
//    layout->write_layout(os);
//}
//
// ALICE_DESCRIBE_STORE(fcn_cell_layout_ptr, layout)
//{
//    return fmt::format("{} ({}) - {} × {}", layout->get_name(), layout->get_technology(), layout->x(),
//    layout->y());
//}
//
// ALICE_PRINT_STORE_STATISTICS(fcn_cell_layout_ptr, os, layout)
//{
//    os << fmt::format(" {} ({}) - {} × {}, #Cells: {}", layout->get_name(),
//    fcn::to_string(layout->get_technology()),
//                      layout->x(), layout->y(), layout->cell_count())
//       << std::endl;
//}
//
// ALICE_LOG_STORE_STATISTICS(fcn_cell_layout_ptr, layout)
//{
//    auto bb = layout->determine_bounding_box();
//
//    return nlohmann::json{
//        {"name", layout->get_name()},
//        {"technology", fcn::to_string(layout->get_technology())},
//        {"layout", {{"x-size", layout->x()}, {"y-size", layout->y()}, {"area", layout->x() * layout->y()}}},
//        {"bounding box", {{"x-size", bb.x_size}, {"y-size", bb.y_size}, {"area", bb.area()}}},
//        {"cells", layout->cell_count()}};
//}
//
// template <>
// bool can_show<fcn_cell_layout_ptr>(std::string& extension, [[maybe_unused]] command& cmd)
//{
//    extension = "svg";
//
//    return true;
//}
//
// template <>
// void show<fcn_cell_layout_ptr>(std::ostream& os, const fcn_cell_layout_ptr& element,
//                               const command& cmd)  // const & for pointer because alice says so...
//{
//    if (auto tech = element->get_technology(); tech != fcn::technology::QCA)
//    {
//        cmd.env->out() << "[w] currently, only QCA layouts can be shown, but " << element->get_name() << " is an "
//                       << tech << " layout" << std::endl;
//        return;
//    }
//
//    try
//    {
//        os << svg::generate_svg_string(element, cmd.is_set("simple")) << std::endl;
//    }
//    catch (const std::invalid_argument& e)
//    {
//        cmd.env->out() << "[e] " << e.what() << std::endl;
//    }
//}

}  // namespace alice

#endif  // FICTION_STORES_HPP
