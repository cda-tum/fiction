//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_BALANCE_H
#define FICTION_BALANCE_H


#include "network_hierarchy.h"
#include "logic_network.h"
#include <alice/alice.hpp>


namespace alice
{
    /**
     * Creates a hierarchy of the current logic network in store and performs a path balancing.
     */
    class balance_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit balance_command(const environment::ptr& env)
                :
                command(env,
                        "Subdivides edges of the current logic network in store so that all paths leading to "
                        "any vertex have the same length.\nTherefore, balance wire vertices are inserted. This does not "
                        "respect possible crossings in the network.\nAlso, if a mapping from balance vertices to the "
                        "original subdivided edges is needed, one should call the hierarchy\nwithin an algorithm instead "
                        "of using this command because that information is lost after destruction of the hierarchy "
                        "object.")
        {
            add_flag("--unify_outputs,-u",
                     "Additionally, push all primary outputs down to the same level");
        }

    protected:
        /**
         * Function to perform the balance call. Inserts wire vertices to equalize paths.
         */
        void execute() override
        {
            auto& s = store<logic_network_ptr>();

            // error case: empty logic network store
            if (s.empty())
            {
                env->out() << "[w] no logic network in store" << std::endl;
                return;
            }

            auto ln = s.current();
            network_hierarchy hier(ln, false);

            if (is_set("unify_outputs"))
                hier.unify_output_ranks();

            hier.balance_paths();
        }
    };

    ALICE_ADD_COMMAND(balance, "Logic")
}


#endif //FICTION_BALANCE_H
