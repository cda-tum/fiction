//
// Created by benjamin on 20.04.23.
//

#ifndef FICTION_OPTIMIZE_HPP
#define FICTION_OPTIMIZE_HPP

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
class optimize_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit optimize_command(const environment::ptr& e) : command(e, "Optimizes logic_networks with selected method.")
    {
        add_flag("--inv,-i",
                 "Optimize Inverters at fan-out nodes. Note that a fan-out substitution is automatically performed");
    }

  protected:
    void execute() override
    {
        auto& s = store<fiction::logic_network_t>();
        if (s.empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;

            return;
        }
        if (!is_set("inv"))
        {
            env->out() << "[e] at least one optimization type must be specified" << std::endl;
        }
        else if (is_set("inv"))
        {
            const auto perform_optimization = [this](auto&& ntk_ptr)
            {
                auto src_ntk{fiction::fanout_substitution<fiction::tec_nt>(*ntk_ptr)};
                auto optimized_ntk{fiction::inverter_balancing(src_ntk)};
                auto gate_dif = src_ntk.num_gates() - optimized_ntk.num_gates();
                env->out() << "Number of balanced inverters: " << gate_dif << std::endl;

                auto result = std::make_shared<fiction::tec_nt>(optimized_ntk);

                return result;
            };

            s.extend() = std::visit(perform_optimization, s.current());
        }
    }
};

ALICE_ADD_COMMAND(optimize, "Logic")

}  // namespace alice

#endif  // FICTION_OPTIMIZE_HPP
