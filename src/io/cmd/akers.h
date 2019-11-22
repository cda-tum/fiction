//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_AKERS_H
#define FICTION_AKERS_H


#include "logic_network.h"
#include <alice/alice.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/algorithms/akers_synthesis.hpp>
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
        explicit akers_command(const environment::ptr& env)
                :
                command(env,
                        "Synthesizes a Majority logic network from the current truth table in store.")
        {}

    protected:
        /**
         * Function to perform the synth call. Generates a logic network from a truth table.
         */
        void execute() override
        {
            auto& s = store<kitty::dynamic_truth_table>();

            // error case: empty truth table store
            if (s.empty())
            {
                env->out() << "[w] no truth table in store" << std::endl;
                return;
            }

            auto tt = s.current();
            auto ntk = mockturtle::akers_synthesis<logic_network::mig_nt>(tt, tt);

            store<logic_network_ptr>().extend() = std::make_shared<logic_network>(std::move(ntk), kitty::to_hex(tt));
        }
    };

    ALICE_ADD_COMMAND(akers, "Logic")
}


#endif //FICTION_AKERS_H
