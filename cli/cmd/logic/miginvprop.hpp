//
// Created by marcel on 07.06.23.
//

#ifndef FICTION_CMD_MIGINVPROP_HPP
#define FICTION_CMD_MIGINVPROP_HPP

#include <fiction/algorithms/network_transformation/network_conversion.hpp>
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
/**
 * Optimizes inverter costs of the current MIG in store.
 */
class miginvprop_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit miginvprop_command(const environment::ptr& e) :
            command(
                e,
                "Propagates inverting edges present in the current MIG in store to the primary inputs of the network, "
                "thus reducing inverter cost but potentially increasing node count due to logic duplication.")
    {}

  protected:
    /**
     * Function to perform the balancing call. Inserts buffer nodes to equalize path lengths.
     */
    void execute() override
    {
        auto& s = store<fiction::logic_network_t>();

        // error case: empty logic network store
        if (s.empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;

            return;
        }

        const auto perform_inverter_optimization = [&](auto&& ntk_ptr) -> void
        {
            using Ntk = typename std::decay_t<decltype(ntk_ptr)>::element_type;

            mockturtle::mig_inv_propagation_stats st{};

            // if the current logic network is an MIG
            if constexpr (std::is_same_v<Ntk, fiction::mig_nt>)
            {
                // clone it because inverter optimization is an in-place algorithm
                auto ntk_clone = ntk_ptr->clone();

                mockturtle::fanout_view fo_ntk_clone{ntk_clone};
                mockturtle::mig_inv_propagation(fo_ntk_clone, &st);

                const auto mig_ptr = std::make_shared<fiction::mig_nt>(ntk_clone);

                env->out() << fmt::format("[i] inverter cost reduction: {}\n"
                                          "[i] node count increase: {}",
                                          st.total_gain, st.node_increase)
                           << std::endl;

                fiction::restore_names(*ntk_ptr, *mig_ptr);

                s.extend() = mig_ptr;
            }
            else  // not an MIG
            {
                env->out() << fmt::format("[e] {} is not an MIG", fiction::get_name(ntk_ptr)) << std::endl;
            }
        };

        std::visit(perform_inverter_optimization, s.current());
    }
};

ALICE_ADD_COMMAND(miginvprop, "Logic")

}  // namespace alice

#endif  // FICTION_CMD_MIGINVPROP_HPP
