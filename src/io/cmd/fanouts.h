//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_FANOUTS_H
#define FICTION_FANOUTS_H


#include "logic_network.h"
#include <alice/alice.hpp>


namespace alice
{
    /**
     * Substitutes multi-outputs of gate vertices and replaces them with fan-out ones.
     */
    class fanouts_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit fanouts_command(const environment::ptr& env)
                :
                command(env,
                        "Substitutes high-degree outputs of gate vertices of the current logic network in store "
                        "and replaces them with fan-out nodes. Some algorithms require fan-out vertices explicitly and "
                        "will break down networks if needed. Others might be able to handle both.")
        {
            add_option("--degree,-d", degree,
                       "Maximum number of outputs a fan-out "
                       "vertex can have", true)->set_type_name("{2, 3}");
            add_option("--strategy,-s", strategy,
                       "Chain fan-outs in a balanced tree (breadth) "
                       "or a DFS tree (depth) fashion", true)->set_type_name("{breadth=0, depth=1}");
            add_option("--threshold,-t", threshold,
                       "Maximum number of outputs an AND/OR/MAJ gate can have before "
                       "substitution applies", true);
        }

    protected:
        /**
         * Function to perform the fanouts call. Substitutes high-degree outputs and replaces them by fan-out vertices.
         */
        void execute() override
        {
            auto& s = store<logic_network_ptr>();

            // error case: empty logic network store
            if (s.empty())
            {
                env->out() << "[w] no logic network in store" << std::endl;
                reset_flags();
                return;
            }

            if (degree < 2 || degree > 3)
            {
                env->out() << "[w] " << degree << " outputs per fan-out are not supported" << std::endl;
                reset_flags();
                return;
            }

            if (strategy != 0 && strategy != 1)
            {
                env->out() << "[w] " << strategy << " does not refer to a valid strategy" << std::endl;
                reset_flags();
                return;
            }

            s.current()->substitute_fan_outs(degree, strategy, threshold);

            reset_flags();
        }

    private:
        /**
         * Maximum output degree of each fan-out gate.
         */
        std::size_t degree = 2u;
        /**
         * Decomposition strategy (DEPTH vs. BREADTH).
         */
        logic_network::substitution_strategy strategy = logic_network::substitution_strategy::BREADTH;
        /**
         * Maximum number of outputs an AND/OR/MAJ gate is allowed to have before substitution applies.
         */
        std::size_t threshold = 1u;

        /**
         * Reset all flags. Necessary for some reason... alice bug?
         */
        void reset_flags()
        {
            degree = 2u;
            strategy = logic_network::substitution_strategy::BREADTH;
            threshold = 1u;
        }
    };

    ALICE_ADD_COMMAND(fanouts, "Logic")
}


#endif //FICTION_FANOUTS_H
