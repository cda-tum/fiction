//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_GATES_H
#define FICTION_GATES_H


#include "logic_network.h"
#include <alice/alice.hpp>


namespace alice
{
    /**
     * Prints numbers of gates for a current logic network.
     */
    class gates_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit gates_command(const environment::ptr& env)
                :
                command(env,
                        "Prints the gate count for each different vertex type "
                        "of the current logic network in store.")
        {}

    protected:
        /**
         * Function to perform the gates call. Outputs gates counts for each vertex type.
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

            env->out() << fmt::format("{}:\n"
                                      " [i] AND   = {}\n [i] OR    = {}\n [i] INV   = {}\n [i] FO    = {}\n"
                                      " [i] MAJ   = {}\n [i] wires = {}\n [i] total = {}",
                                      ln->get_name(),
                                      ln->operation_count(operation::AND), ln->operation_count(operation::OR),
                                      ln->operation_count(operation::NOT),
                                      ln->operation_count(operation::F1O2) + ln->operation_count(operation::F1O3),
                                      ln->operation_count(operation::MAJ), ln->operation_count(operation::W),
                                      ln->vertex_count())
                       << std::endl;
        }
    };

    ALICE_ADD_COMMAND(gates, "Logic")
}


#endif //FICTION_GATES_H
