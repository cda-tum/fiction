//
// Created by marcel on 24.10.19.
//

#include "cmd/logic/include/balance.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <variant>

namespace alice
{

balance_command::balance_command(const environment::ptr& e) :
        command(e, "Creates a new technology network from the current logic network in store. All paths in the new "
                   "network that are leading to any common node will have the same length. This is realized by "
                   "inserting buffer nodes. This does not respect possible crossings in the network.")
{
    add_flag("--unify_outputs,-u", ps.unify_outputs, "Additionally, push all primary outputs down to the same level");
}

void balance_command::execute()
{
    auto& s = store<fiction::logic_network_t>();

    // error case: empty logic network store
    if (s.empty())
    {
        env->out() << "[w] no logic network in store\n";

        ps = {};
        return;
    }

    const auto perform_balancing = [this](auto&& ntk_ptr)
    { return std::make_shared<fiction::tec_nt>(fiction::network_balancing<fiction::tec_nt>(*ntk_ptr, ps)); };

    s.extend() = std::visit(perform_balancing, s.current());

    ps = {};
}

}  // namespace alice
