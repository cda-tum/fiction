//
// Created by marcel on 24.10.19.
//

#include "cmd/general/include/clear.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/types.hpp>

namespace alice
{

clear_command::clear_command(const environment::ptr& e) :
        command(e, "Removes all elements from the respectively flagged stores. If no store is specified, all "
                   "stores will be cleared.")
{
    add_flag("--cell_layout,-c", cell, "Clear cell layout store");
    add_flag("--gate_layout,-g", gate, "Clear gate layout store");
    add_flag("--network,-n", network, "Clear logic network store");
    add_flag("--truth_table,-t", table, "Clear truth table store");
}

void clear_command::execute()
{
    // if no store was specified, clear all.
    if (!cell && !gate && !network && !table)
    {
        cell    = true;
        gate    = true;
        network = true;
        table   = true;
    }

    if (cell)
    {
        store<fiction::cell_layout_t>().clear();
    }
    if (gate)
    {
        store<fiction::gate_layout_t>().clear();
    }
    if (network)
    {
        store<fiction::logic_network_t>().clear();
    }
    if (table)
    {
        store<fiction::truth_table_t>().clear();
    }

    // reset flags, necessary for some reason... alice bug?
    cell    = false;
    gate    = false;
    network = false;
    table   = false;
}

}  // namespace alice
