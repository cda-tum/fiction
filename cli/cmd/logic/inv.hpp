//
// Created by benjamin on 20.04.23.
//

#ifndef FICTION_INV_HPP
#define FICTION_INV_HPP

#include <fiction/algorithms/network_transformation/inverter_substitution.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>

#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>

namespace alice
{
/**
 * Creates a new logic network of the current logic network type in store and performs a inverter substitution
 */
class inv_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit inv_command(const environment::ptr& e) :
            command(e, "Substitutes two inverters at the fan-outs of fan-out nodes with one inverter at their fan-in.")
    {}

  protected:
    void execute() override
    {
        auto& s = store<fiction::logic_network_t>();
        if (s.empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;

            return;
        }

        const auto substitute_inverters = [this, &s](auto&& ntk_ptr)
        {
            using Ntk = typename std::decay_t<decltype(ntk_ptr)>::element_type;
            if constexpr (fiction::has_is_fanout_v<Ntk>)
            {
                const auto optimized_ntk{fiction::inverter_substitution(*ntk_ptr)};
                const auto gate_dif = (*ntk_ptr).num_gates() - optimized_ntk.num_gates();
                env->out() << "[i] the number of inverters has been reduced by: " << gate_dif << std::endl;

                s.extend() = std::make_shared<fiction::tec_nt>(optimized_ntk);
            }
            else
            {
                env->out() << "[e] network is not fan-out substituted. You may want to try command 'fanouts' before "
                              "running this command."
                           << std::endl;
            }
        };

        std::visit(substitute_inverters, s.current());
    }
};

ALICE_ADD_COMMAND(inv, "Logic")

}  // namespace alice

#endif  // FICTION_INV_HPP
