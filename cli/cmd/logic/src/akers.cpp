//
// Created by marcel on 24.10.19.
//

#include "cmd/logic/include/akers.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <kitty/print.hpp>
#include <mockturtle/algorithms/akers_synthesis.hpp>
#include <mockturtle/algorithms/cleanup.hpp>

#include <memory>

namespace alice
{

akers_command::akers_command(const environment::ptr& e) :
        command(e, "Synthesizes a Majority logic network from the current truth table in store.")
{
    add_flag("--aig,-a", "Generate as AIG");
    add_flag("--xag,-x", "Generate as XAG");
    add_flag("--mig,-m", "Generate as MIG");
    add_flag("--tec,-t", "Generate as technology network");
}

void akers_command::execute()
{
    if (!is_set("aig") && !is_set("xag") && !is_set("mig") && !is_set("tec"))
    {
        env->out() << "[e] at least one network type must be specified\n";
        return;
    }

    auto& s = store<fiction::truth_table_t>();

    // error case: empty truth table store
    if (s.empty())
    {
        env->out() << "[w] no truth table in store\n";
        return;
    }

    auto tt = s.current();

    if (is_set("aig"))
    {
        synthesize_and_store<fiction::aig_nt>(tt);
    }
    if (is_set("xag"))
    {
        synthesize_and_store<fiction::xag_nt>(tt);
    }
    if (is_set("mig"))
    {
        synthesize_and_store<fiction::mig_nt>(tt);
    }
    if (is_set("tec"))
    {
        synthesize_and_store<fiction::tec_nt>(tt);
    }
}

template <typename Ntk>
void akers_command::synthesize_and_store(const fiction::truth_table_t& tt)
{
    auto ntk = mockturtle::cleanup_dangling(mockturtle::akers_synthesis<Ntk>(*tt, *tt));

    store<fiction::logic_network_t>().extend() = std::make_shared<Ntk>(std::move(ntk), kitty::to_hex(*tt));
}

}  // namespace alice
