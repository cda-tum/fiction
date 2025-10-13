//
// Created by marcel on 07.06.23.
//

#include "cmd/logic/include/miginvprop.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <mockturtle/algorithms/mig_inv_propagation.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <type_traits>
#include <variant>

namespace alice
{

miginvprop_command::miginvprop_command(const environment::ptr& e) :
        command(e,
                "Propagates inverting edges present in the current MIG in store to the primary inputs of the network, "
                "thus reducing inverter cost but potentially increasing node count due to logic duplication.")
{}

void miginvprop_command::execute()
{
    auto& s = store<fiction::logic_network_t>();

    // error case: empty logic network store
    if (s.empty())
    {
        env->out() << "[w] no logic network in store\n";

        return;
    }

    const auto perform_inverter_optimization = [&](auto&& ntk_ptr) -> void
    {
        using Ntk = typename std::decay_t<decltype(ntk_ptr)>::element_type;

        mockturtle::mig_inv_propagation_stats st{};  // NOLINT(misc-const-correctness)

        // if the current logic network is an MIG
        if constexpr (std::is_same_v<Ntk, fiction::mig_nt>)
        {
            // clone it because inverter optimization is an in-place algorithm
            auto ntk_clone = ntk_ptr->clone();

            mockturtle::fanout_view fo_ntk_clone{ntk_clone};
            mockturtle::mig_inv_propagation(fo_ntk_clone, &st);

            const auto mig_ptr = std::make_shared<fiction::mig_nt>(ntk_clone);

            env->out() << fmt::format("[i] inverter cost reduction: {}\n"
                                      "[i] node count increase: {}\n",
                                      st.total_gain, st.node_increase);

            fiction::restore_names(*ntk_ptr, *mig_ptr);

            s.extend() = mig_ptr;
        }
        else  // not an MIG
        {
            env->out() << fmt::format("[e] {} is not an MIG\n", fiction::get_name(*ntk_ptr));
        }
    };

    std::visit(perform_inverter_optimization, s.current());
}

}  // namespace alice
