//
// Created by marcel on 16.07.18.
//

#ifndef FICTION_STORES_H
#define FICTION_STORES_H

#include "logic_network.h"
#include "fcn_gate_layout.h"
#include "fcn_cell_layout.h"
#include "svg_writer.h"
#include "fmt/format.h"
#include "fmt/ostream.h"
#include <alice/alice.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/print.hpp>

namespace alice
{
    /**
     * Truth tables.
     *
     * Parts of this code are originally from: https://github.com/msoeken/cirkit/blob/cirkit3/cli/stores/tt.hpp.
     */
    ALICE_ADD_STORE(kitty::dynamic_truth_table, "truth_table", "t", "truth table", "truth tables")

    ALICE_DESCRIBE_STORE(kitty::dynamic_truth_table, tt)
    {
        if (tt.num_vars() <= 6)
            return fmt::format("{} vars, hex: {}, bin: {}", tt.num_vars(), kitty::to_hex(tt), kitty::to_binary(tt));
        else
            return fmt::format("{} vars, (description omitted due to truth table size)", tt.num_vars());
    }

    ALICE_PRINT_STORE_STATISTICS(kitty::dynamic_truth_table, os, tt)
    {
        if (tt.num_vars() <= 6)
            os << fmt::format(" {} vars, hex: {}, bin: {}", tt.num_vars(), kitty::to_hex(tt), kitty::to_binary(tt)) << std::endl;
        else
            os << fmt::format(" {} vars, (description omitted due to truth table size)", tt.num_vars()) << std::endl;
    }

    ALICE_LOG_STORE_STATISTICS(kitty::dynamic_truth_table, tt)
    {
        return
        {
            {"vars",   tt.num_vars()},
            {"hex",    kitty::to_hex(tt)},
            {"binary", kitty::to_binary(tt)}
        };
    }

    ALICE_PRINT_STORE(kitty::dynamic_truth_table, os, tt)
    {
        os << fmt::format(" {} vars, hex: {}, bin: {}", tt.num_vars(), kitty::to_hex(tt), kitty::to_binary(tt)) << std::endl;
    }


    /**
     * Logic networks.
     */
    ALICE_ADD_STORE(logic_network_ptr, "network", "n", "logic network", "logic networks")

    ALICE_DESCRIBE_STORE(logic_network_ptr, ln)
    {
        return fmt::format("{} - I/O: {}/{}, #V: {}", ln->get_name(), ln->num_pis(), ln->num_pos(), ln->vertex_count());
    }

    ALICE_PRINT_STORE_STATISTICS(logic_network_ptr, os, ln)
    {
        os << fmt::format(" {} - I/O: {}/{}, #V: {}", ln->get_name(), ln->num_pis(),
                          ln->num_pos(), ln->vertex_count()) << std::endl;
    }

    ALICE_LOG_STORE_STATISTICS(logic_network_ptr, ln)
    {
        return nlohmann::json
        {
            {"name", ln->get_name()},
            {"inputs", ln->num_pis()},
            {"outputs", ln->num_pos()},
            {"vertices", ln->vertex_count()},
            {"ANDs", ln->operation_count(operation::AND)},
            {"ORs", ln->operation_count(operation::OR)},
            {"INVs", ln->operation_count(operation::NOT)},
            {"fan-outs", ln->operation_count(operation::F1O2) + ln->operation_count(operation::F1O3)},
            {"MAJs", ln->operation_count(operation::MAJ)},
            {"wires", ln->operation_count(operation::W)}
        };
    }

    template<>
    bool can_show<logic_network_ptr>(std::string& extension, command& cmd)
    {
        extension = "dot";

        cmd.add_flag("--simple,-s", "Less detailed visualization "
                                    "(structural information only)")->group("logic_network (-n) / cell_layout (-c)");

        return true;
    }

    template<>
    void show<logic_network_ptr>(std::ostream& os, const logic_network_ptr& element, const command& cmd)  // const & for pointer because alice says so...
    {
        try
        {
            if (cmd.is_set("simple"))
            {
                element->write_simple_dot(os);
            }
            else
            {
                element->write_dot(os);
            }
        }
        catch (const std::invalid_argument& e)
        {
            cmd.env->out() << "[e] " << e.what() << std::endl;
        }
    }


    /**
     * FCN gate layouts.
     */
    ALICE_ADD_STORE(fcn_gate_layout_ptr, "gate_layout", "g", "gate layout", "gate layouts")

    ALICE_PRINT_STORE(fcn_gate_layout_ptr, os, layout)
    {
        layout->write_layout(os);
    }

    ALICE_DESCRIBE_STORE(fcn_gate_layout_ptr, layout)
    {
        return fmt::format("{} - {} × {}", layout->get_name(), layout->x(), layout->y());
    }

    ALICE_PRINT_STORE_STATISTICS(fcn_gate_layout_ptr, os, layout)
    {
        auto [cp, tp] = layout->critical_path_length_and_throughput();
        os << fmt::format(" {} - {} × {}, #G: {}, #W: {}, #C: {}, #L: {}, CP: {}, TP: 1/{}", layout->get_name(),
                          layout->x(), layout->y(), layout->gate_count(true), layout->wire_count(true),
                          layout->crossing_count(true), layout->latch_count(), cp, tp) << std::endl;
    }

    ALICE_LOG_STORE_STATISTICS(fcn_gate_layout_ptr, layout)
    {
        auto area = layout->x() * layout->y();
        auto bb = layout->determine_bounding_box();
        auto gate_tiles = layout->gate_count(true);
        auto wire_tiles = layout->wire_count(true);
        auto crossings  = layout->crossing_count(true);
        auto [cp, tp] = layout->critical_path_length_and_throughput();

        return nlohmann::json
        {
            {"name", layout->get_name()},
            {"layout",
             {
                {"x-size", layout->x()},
                {"y-size", layout->y()},
                {"area", area}
             }
            },
            {"bounding box",
             {
                {"x-size", bb.x_size},
                {"y-size", bb.y_size},
                {"area", bb.area()}
             }
            },
            {"gate tiles", gate_tiles},
            {"wire tiles", wire_tiles},
            {"free tiles", area - (gate_tiles + wire_tiles - crossings)},  // free tiles in ground layer
            {"crossings", crossings},
            {"latches", layout->latch_count()},
            {"critical path", cp},
            {"throughput", fmt::format("1/{}", tp)}
        };
    }


    /**
     * FCN cell layouts.
     */
    ALICE_ADD_STORE(fcn_cell_layout_ptr, "cell_layout", "c", "cell layout", "cell layouts")

    ALICE_PRINT_STORE(fcn_cell_layout_ptr, os, layout)
    {
        layout->write_layout(os);
    }

    ALICE_DESCRIBE_STORE(fcn_cell_layout_ptr, layout)
    {
        return fmt::format("{} ({}) - {} × {}", layout->get_name(), layout->get_technology(), layout->x(), layout->y());
    }

    ALICE_PRINT_STORE_STATISTICS(fcn_cell_layout_ptr, os, layout)
    {
        os << fmt::format(" {} ({}) - {} × {}, #Cells: {}", layout->get_name(), fcn::to_string(layout->get_technology()),
                          layout->x(), layout->y(), layout->cell_count()) << std::endl;
    }

    ALICE_LOG_STORE_STATISTICS(fcn_cell_layout_ptr, layout)
    {
        auto bb = layout->determine_bounding_box();

        return nlohmann::json
        {
            {"name", layout->get_name()},
            {"technology", fcn::to_string(layout->get_technology())},
            {"layout",
             {
                {"x-size", layout->x()},
                {"y-size", layout->y()},
                {"area", layout->x() * layout->y()}
             }
            },
            {"bounding box",
             {
               {"x-size", bb.x_size},
               {"y-size", bb.y_size},
               {"area", bb.area()}
             }
            },
            {"cells", layout->cell_count()}
        };
    }

    template<>
    bool can_show<fcn_cell_layout_ptr>(std::string& extension, [[maybe_unused]] command& cmd)
    {
        extension = "svg";

        return true;
    }

    template<>
    void show<fcn_cell_layout_ptr>(std::ostream& os, const fcn_cell_layout_ptr& element, const command& cmd)  // const & for pointer because alice says so...
    {
        if (auto tech = element->get_technology(); tech != fcn::technology::QCA)
        {
            cmd.env->out() << "[w] currently, only QCA layouts can be shown, but " << element->get_name() << " is an "
                           << tech << " layout" << std::endl;
            return;
        }

        try
        {
            os << svg::generate_svg_string(element, cmd.is_set("simple")) << std::endl;
        }
        catch (const std::invalid_argument& e)
        {
            cmd.env->out() << "[e] " << e.what() << std::endl;
        }
    }

}


#endif //FICTION_STORES_H
