//
// Created by marcel on 24.10.19.
//

#include "cmd/logic/include/simulate.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <memory>
#include <new>
#include <string>
#include <variant>

namespace alice
{
simulate_command::simulate_command(const environment::ptr& e) :
        command(e, "Computes truth tables for each primary output of the current logic network or "
                   "gate-level layout in store.")
{
    add_flag("--gate_layout,-g", "Simulate gate layout");
    add_flag("--network,-n", "Simulate logic network");
    add_flag("--store", "Store simulation results in truth table store");
    add_flag("--silent", "Do not print truth tables");
}

void simulate_command::execute()
{
    if (is_set("gate_layout") == is_set("network"))
    {
        env->out() << "[w] exactly one of --gate_layout or --network must be specified\n";
        return;
    }

    tables.clear();
    po_names.clear();

    if (is_set("gate_layout"))
    {
        auto& gls = store<fiction::gate_layout_t>();

        // error case: empty gate layout store
        if (gls.empty())
        {
            env->out() << "[w] no gate layout in store\n";
            return;
        }

        perform_simulation(gls.current());
    }
    else if (is_set("network"))
    {
        auto& lns = store<fiction::logic_network_t>();

        // error case: empty logic network store
        if (lns.empty())
        {
            env->out() << "[w] no logic network in store\n";
            return;
        }

        perform_simulation(lns.current());
    }

    auto& tts = store<fiction::truth_table_t>();
    for (std::size_t i = 0; i < tables.size(); ++i)
    {
        if (is_set("store"))
        {
            tts.extend() = std::make_shared<fiction::tt>(tables[i]);
        }
        if (!is_set("silent"))
        {
            env->out() << fmt::format("[i] {} - hex: {}, bin: {}\n", po_names[i], kitty::to_hex(tables[i]),
                                      kitty::to_binary(tables[i]));
        }
    }
}

nlohmann::json simulate_command::log() const
{
    nlohmann::json j;
    for (std::size_t i = 0; i < tables.size(); ++i)
    {
        j.push_back({{"po", po_names[i]},
                     {"vars", tables[i].num_vars()},
                     {"hex", kitty::to_hex(tables[i])},
                     {"binary", kitty::to_binary(tables[i])}});
    }

    return {{"tables", j}};
}

template <typename NtkOrLytVariant>
void simulate_command::perform_simulation(const NtkOrLytVariant& network_or_layout_variant)
{
    const auto get_name = [](auto&& ntk_or_lyt_ptr) -> std::string { return fiction::get_name(*ntk_or_lyt_ptr); };

    const auto store_po_names = [this](auto&& ntk_or_lyt_ptr)
    {
        ntk_or_lyt_ptr->foreach_po(
            [this, &ntk_or_lyt_ptr]([[maybe_unused]] const auto& po, auto i)
            {
                po_names.emplace_back(ntk_or_lyt_ptr->has_output_name(i) ? ntk_or_lyt_ptr->get_output_name(i) :
                                                                           fmt::format("po{}", i));
            });
    };

    const auto simulate = [this](auto&& ntk_or_lyt_ptr)
    {
        tables = mockturtle::simulate<fiction::tt>(
            *ntk_or_lyt_ptr,
            // NOLINTNEXTLINE
            mockturtle::default_simulator<fiction::tt>{static_cast<unsigned>(ntk_or_lyt_ptr->num_pis())});
    };

    std::visit(store_po_names, network_or_layout_variant);

    try
    {
        std::visit(simulate, network_or_layout_variant);
    }
    catch (const std::bad_alloc&)
    {
        env->out() << fmt::format("[e] {} has too many inputs to store its truth table\n",
                                  std::visit(get_name, network_or_layout_variant));
    }
}

}  // namespace alice
