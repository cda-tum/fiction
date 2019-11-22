//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CLEAR_H
#define FICTION_CLEAR_H


#include "fcn_cell_layout.h"
#include "fcn_gate_layout.h"
#include "logic_network.h"
#include <alice/alice.hpp>
#include <kitty/dynamic_truth_table.hpp>


namespace alice
{
    /**
    * Clears all stores by removing all their entries.
    */
    class clear_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit clear_command(const environment::ptr& env)
                :
                command(env,
                        "Removes all elements from the respectively flagged stores. If no store is specified, all "
                        "stores will be cleared.")
        {
            add_flag("--cell_layout,-c", cell,
                     "Clear cell layout store");
            add_flag("--gate_layout,-g", gate,
                     "Clear gate layout store");
            add_flag("--network,-n", network,
                     "Clear logic network store");
            add_flag("--truth_table,-t", table,
                     "Clear truth table store");
        }

    protected:
        /**
         * Function to perform the clear call. Calls alice's clear function for all stores.
         */
        void execute() override
        {
            // if no store was specified, clear all.
            if (!cell && !gate && !network && !table)
            {
                cell = true;
                gate = true;
                network = true;
                table = true;
            }

            if (cell)
                store<fcn_cell_layout_ptr>().clear();
            if (gate)
                store<fcn_gate_layout_ptr>().clear();
            if (network)
                store<logic_network_ptr>().clear();
            if (table)
                store<kitty::dynamic_truth_table>().clear();

            // reset flags, necessary for some reason... alice bug?
            cell = false;
            gate = false;
            network = false;
            table = false;
        }

    private:
        /**
         * Stores to clear.
         */
        bool cell = false, gate = false, network = false, table = false;
    };

    ALICE_ADD_COMMAND(clear, "General")
}


#endif //FICTION_CLEAR_H
