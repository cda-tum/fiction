//
// Created by marcel on 24.10.19.
//

#include "cmd/logic/include/gates.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/properties/count_gate_types.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <variant>

namespace alice
{

gates_command::gates_command(const environment::ptr& e) :
        command(e, "Prints the gate count for each different node type "
                   "of the current logic network or gate-level layout in store.")
{
    add_flag("--gate_layout,-g", "Count gates in current gate-level layout");
    add_flag("--network,-n", "Count gates in current logic network");
    add_flag("--detailed", "Display a detailed listing of all supported gate types");
}

void gates_command::execute()
{
    if (!is_set("gate_layout") && !is_set("network"))
    {
        env->out() << "[w] at least one store must be specified\n";
        return;
    }

    const auto count = [this](auto&& ntk_or_lyt_ptr)
    {
        fiction::count_gate_types_stats st{};
        fiction::count_gate_types(*ntk_or_lyt_ptr, &st);

        st.report(env->out(), is_set("detailed"));
    };

    if (is_set("gate_layout"))
    {
        auto& s = store<fiction::gate_layout_t>();

        // error case: empty gate-level layout store
        if (s.empty())
        {
            env->out() << "[w] no gate layout in store\n";
            return;
        }

        std::visit(count, s.current());
    }

    if (is_set("network"))
    {
        auto& s = store<fiction::logic_network_t>();

        // error case: empty logic network store
        if (s.empty())
        {
            env->out() << "[w] no logic network in store\n";
            return;
        }

        std::visit(count, s.current());
    }
}

}  // namespace alice
