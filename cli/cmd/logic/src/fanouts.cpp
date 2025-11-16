//
// Created by marcel on 24.10.19.
//

#include "cmd/logic/include/fanouts.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <variant>

namespace alice
{

fanouts_command::fanouts_command(const environment::ptr& e) :
        command(e, "Substitutes high-degree outputs of gates of the current logic network in store "
                   "and replaces them with fan-out nodes. Some algorithms require fan-out nodes explicitly and "
                   "will break down networks if needed. Others might be able to handle both.")
{
    add_option("--degree,-d", ps.degree, "Maximum number of outputs a fan-out node can have", true)
        ->set_type_name("{2, 3}");
    add_option("--strategy,-s", ps.strategy,
               "Chain fan-outs in a balanced tree (breadth), a DFS tree (depth), or a random fashion", true)
        ->set_type_name("{breadth=0, depth=1, random=2}");
    add_option("--threshold,-t", ps.threshold,
               "Maximum number of outputs any gate can have before substitution applies", true);
    add_option("--seed,-r", seed, "Random seed used for random fanout substitution.");
}

void fanouts_command::execute()
{
    auto& s = store<fiction::logic_network_t>();

    // error case: empty logic network store
    if (s.empty())
    {
        env->out() << "[w] no logic network in store\n";
        ps = {};
        return;
    }

    if (ps.degree < 2 || ps.degree > 3)
    {
        env->out() << "[w] " << ps.degree << " outputs per fan-out are not supported\n";
        ps = {};
        return;
    }

    if (ps.strategy > 2)
    {
        env->out() << "[w] " << ps.strategy << " does not refer to a valid strategy\n";
        ps = {};
        return;
    }

    if (is_set("seed"))
    {
        ps.seed = seed;
    }

    const auto perform_substitution = [this](auto&& ntk_ptr)
    { return std::make_shared<fiction::tec_nt>(fiction::fanout_substitution<fiction::tec_nt>(*ntk_ptr, ps)); };

    s.extend() = std::visit(perform_substitution, s.current());

    ps = {};
}

}  // namespace alice
