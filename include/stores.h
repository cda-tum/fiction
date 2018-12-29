//
// Created by marcel on 16.07.18.
//

#ifndef FICTION_STORES_H
#define FICTION_STORES_H

#include "logic_network.h"
#include "fcn_gate_layout.h"
#include "fcn_cell_layout.h"
#include <boost/format.hpp>
#include <alice/alice.hpp>

namespace alice
{
    /**
     * Logic networks.
     */
    ALICE_ADD_STORE(logic_network_ptr, "network", "w", "logic network", "logic networks")

    ALICE_PRINT_STORE(logic_network_ptr, os, ln)
    {
        ln->write_network(os);
    }

    ALICE_DESCRIBE_STORE(logic_network_ptr, ln)
    {
        return boost::str(boost::format("%s - I/O: %d/%d, #V: %d")
                          % ln->get_name() % ln->pi_count() % ln->po_count() % ln->vertex_count());
    }

    ALICE_PRINT_STORE_STATISTICS(logic_network_ptr, os, ln)
    {
        os << boost::str(boost::format("%s - I/O: %d/%d, #V: %d")
                         % ln->get_name() % ln->pi_count()
                         % ln->po_count() % ln->vertex_count()) << std::endl;
    }

    ALICE_LOG_STORE_STATISTICS(logic_network_ptr, ln)
    {
        return nlohmann::json
        {
            {"name", ln->get_name()},
            {"inputs", ln->pi_count()},
            {"outputs", ln->po_count()},
            {"vertices", ln->vertex_count()}
        };
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
        return boost::str(boost::format("%s - %d × %d") % layout->get_name() % layout->x() % layout->y());
    }

    ALICE_PRINT_STORE_STATISTICS(fcn_gate_layout_ptr, os, layout)
    {
        os << boost::str(boost::format("%s - %d × %d, #G: %d, #W: %d, #C: %d, #L: %d")
                         % layout->get_name() % layout->x() % layout->y()
                         % layout->gate_count() % layout->wire_count()
                         % layout->crossing_count() % layout->latch_count()) << std::endl;
    }

    ALICE_LOG_STORE_STATISTICS(fcn_gate_layout_ptr, layout)
    {
        auto bb = layout->determine_bounding_box();
        auto energy = layout->calculate_energy();

        return nlohmann::json
        {
            {"name", layout->get_name()},
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
            {"gate tiles", layout->gate_count()},
            {"wire tiles", layout->wire_count()},
            {"crossings", layout->crossing_count()},
            {"latches", layout->latch_count()},
            {"energy (meV, QCA)",
             {
                {"slow (25 GHz)", energy.first},
                {"fast (100 GHz)", energy.second}
             }
            }
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
        return boost::str(boost::format("%s (%s) - %d × %d")
                          % layout->get_name() % layout->get_technology() % layout->x() % layout->y());
    }

    ALICE_PRINT_STORE_STATISTICS(fcn_cell_layout_ptr, os, layout)
    {
        os << boost::str(boost::format("%s (%s) - %d × %d, #Cells: %d")
                         % layout->get_name() % fcn::to_string(layout->get_technology()) % layout->x()
                         % layout->y() % layout->cell_count()) << std::endl;
    }

    ALICE_LOG_STORE_STATISTICS(fcn_cell_layout_ptr, layout)
    {
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
            {"cells", layout->cell_count()}
        };
    }
}


#endif //FICTION_STORES_H
