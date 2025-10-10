//
// Created by marcel on 07.06.23.
//

#include "cmd/logic/include/miginvopt.hpp"

#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <mockturtle/algorithms/mig_inv_optimization.hpp>

#include <type_traits>
#include <variant>

namespace alice
{

miginvopt_command::miginvopt_command(const environment::ptr& e) :
        command(e, "Optimizes the inverter cost of the current MIG in store via node replacement.")
{}

void miginvopt_command::execute()
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

        mockturtle::mig_inv_optimization_stats st{};

        // if the current logic network is an MIG
        if constexpr (std::is_same_v<Ntk, fiction::mig_nt>)
        {
            // clone it because inverter optimization is an in-place algorithm
            auto ntk_clone = ntk_ptr->clone();

            //                mockturtle::fanout_view fo_ntk_clone{ntk_clone};
            mockturtle::mig_inv_optimization(ntk_clone, &st);

            const auto mig_ptr = std::make_shared<fiction::mig_nt>(ntk_clone);

            env->out() << fmt::format("[i] inverter cost reduction: {}\n", st.total_gain);

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
