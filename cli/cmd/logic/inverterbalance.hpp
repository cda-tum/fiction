//
// Created by benjamin on 20.04.23.
//

#ifndef FICTION_INVERTERBALANCE_HPP
#define FICTION_INVERTERBALANCE_HPP

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/network_transformation/inverter_balancing.hpp"

#include <fiction/io/network_reader.hpp>
#include <fiction/io/read_fqca_layout.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>

#include <memory>
#include <string>

namespace alice
{
/**
 * Shorthand for all read commands. Chooses the proper function by the file ending.
 *
 * Currently parses Verilog, AIGER, and BLIF using the lorina parsers.
 *
 * Parses FQCA via a custom reader function.
 *
 * For more information see: https://github.com/hriener/lorina
 */
class inverterbalance_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit inverterbalance_command(const environment::ptr& e) : command(e, "Balances inverters at fan-out nodes.") {}

  protected:
    void execute() override
    {
        auto& s = store<fiction::logic_network_t>();
        if (s.empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;

            return;
        }
        const auto perform_optimization = [this](auto&& ntk_ptr)
        {
            auto src_ntk{*ntk_ptr};
            if (!is_fanout_substituted(src_ntk, ps))
            {
                env->out() << "[e] network is not fan-out substituted. You may want to try command 'fanouts' before "
                              "running this command."
                           << std::endl;
            }
            else
            {
                auto src_ntk_f{fiction::fanout_substitution<fiction::tec_nt>(*ntk_ptr)};
                auto optimized_ntk{fiction::inverter_balancing(src_ntk_f)};
                auto gate_dif = src_ntk.num_gates() - optimized_ntk.num_gates();
                env->out() << "Number of balanced inverters: " << gate_dif << std::endl;

                auto result = std::make_shared<fiction::tec_nt>(optimized_ntk);

                return result;
            }
        };

        s.extend() = std::visit(perform_optimization, s.current());
    }

  private:
    fiction::fanout_substitution_params ps{};
};

ALICE_ADD_COMMAND(inverterbalance, "Logic")

}  // namespace alice

#endif  // FICTION_INVERTERBALANCE_HPP
