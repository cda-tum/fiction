//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_AKERS_H
#define FICTION_AKERS_H

#include "types.hpp"

#include <alice/alice.hpp>
#include <mockturtle/algorithms/akers_synthesis.hpp>
#include <mockturtle/algorithms/cleanup.hpp>

#include <memory>

namespace alice
{
/**
 * Performs logic synthesis to generate a network from a truth table.
 */
class akers_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param env alice::environment that specifies stores etc.
     */
    explicit akers_command(const environment::ptr& env) :
            command(env, "Synthesizes a Majority logic network from the current truth table in store.")
    {
        add_flag("--aig,-a", "Generate network as AIG");
        //        add_flag("--mig,-m", "Generate network as MIG");
    }

  protected:
    /**
     * Function to perform the synth call. Generates a logic network from a truth table.
     */
    void execute() override
    {
        auto& s = store<fiction::truth_table_t>();

        // error case: empty truth table store
        if (s.empty())
        {
            env->out() << "[w] no truth table in store" << std::endl;
            return;
        }

        auto tt = s.current();

        if (is_set("aig"))
        {
            synthesize_and_store<fiction::aig_nt>(tt);
        }
        //            else if (is_set("mig"))
        //            {
        //                  synthesize_and_store<fiction::mig_nt>(tt);
        //            }
    }

  private:
    template <typename Ntk>
    void synthesize_and_store(const fiction::truth_table_t& tt)
    {
        auto ntk = mockturtle::cleanup_dangling(mockturtle::akers_synthesis<Ntk>(*tt, *tt));

        store<fiction::logic_network_t>().extend() = std::make_shared<Ntk>(std::move(ntk), kitty::to_hex(*tt));
    }
};

ALICE_ADD_COMMAND(akers, "Logic")

}  // namespace alice

#endif  // FICTION_AKERS_H
