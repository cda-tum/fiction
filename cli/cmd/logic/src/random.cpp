//
// Created by marcel on 08.11.19.
//

#include "cmd/logic/include/random.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <mockturtle/generators/random_network.hpp>

#include <chrono>
#include <cstdint>
#include <string>

namespace alice
{

random_command::random_command(const environment::ptr& e) :
        command(e, "Generates a random logic network. The random seed will be used as its name for reproducibility.")
{
    add_flag("--aig,-a", "Create random AIG network");
    add_flag("--xag,-x", "Create random XAG network");
    add_flag("--mig,-m", "Create random MIG network");
    add_flag("--tec,-t", "Create random technology network");
    add_option("--rnd_num_inp,-n", ps.num_pis, "Number of primary inputs", true);
    add_option("--rnd_num_gates,-g", ps.num_gates, "Number of gates (excluding inverters and fan-outs)", true);
    add_option("--rnd_seed,-s", ps.seed, "Random seed");
}

void random_command::execute()
{
    if (!is_set("aig") && !is_set("xag") && !is_set("mig") && !is_set("tec"))
    {
        env->out() << "[e] at least one network type must be specified\n";
        return;
    }

    if (is_set("aig"))
    {
        generate<fiction::aig_nt>(mockturtle::random_aig_generator(ps));
    }
    if (is_set("xag"))
    {
        generate<fiction::xag_nt>(mockturtle::random_xag_generator(ps));
    }
    if (is_set("mig"))
    {
        generate<fiction::mig_nt>(mockturtle::random_mig_generator(ps));
    }
    if (is_set("tec"))
    {
        generate<fiction::tec_nt>(mockturtle::mixed_random_mig_generator(ps));
    }

    reset_flags();
}

void random_command::reset_flags()
{
    ps      = {};
    ps.seed = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
}

}  // namespace alice
