//
// Created by marcel on 04.03.20.
//

#ifndef FICTION_EQUIV_H
#define FICTION_EQUIV_H

#include "fcn_gate_layout.h"
#include "equivalence_checker.h"
#include <alice/alice.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <nlohmann/json.hpp>


namespace alice
{

/**
  * Performs equality checks on logic networks; either between specification and gate layout or between two gate
  * layouts.
  */
class equiv_command : public command
{
public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param env alice::environment that specifies stores etc.
     */
    explicit equiv_command(const environment::ptr& env)
            :
            command(env, "Performs logical and delay equivalence checks of gate layouts against a specification "
                         "that can either be its own associated logic network or another gate layout.")
    {
        add_option("--gate_layout,-g", gli,
                   "Store index of gate layout to compare the current one against", false);
    }

protected:
    /**
     * Function to perform the equivalence check call. Fetches the layout(s) and calls the checker.
     */
    void execute() override
    {
        // reset result container
        result = {};

        auto& s = store<fcn_gate_layout_ptr>();

        if (s.empty())
        {
            std::cout << "[w] no gate layout in store" << std::endl;
            reset_flags();
            return;
        }

        auto fgl1 = s.current();

        if (!fgl1->has_io_pins())
        {
            env->out() << "[w] " << fgl1->get_name() << " must have designated I/O pins for equivalence checking" << std::endl;
            reset_flags();
            return;
        }

        // check whether an index was given
        if (this->is_set("gate_layout"))
        {
            if (gli == s.current_index())
            {
                std::cout << "[w] comparison against self" << std::endl;
                reset_flags();
                return;
            }

            fcn_gate_layout_ptr fgl2;

            // try accessing store at given index
            try
            {
                fgl2 = s[gli];
            }
            catch (...)
            {
                std::cout << "[w] no gate layout in store at index " << gli << std::endl;
                reset_flags();
                return;
            }

            if (!fgl2->has_io_pins())
            {
                env->out() << "[w] " << fgl2->get_name() << " must have designated I/O pins for equivalence checking" << std::endl;
                reset_flags();
                return;
            }

            result = equivalence_checker{fgl1, fgl2}();
        }
        else
        {
            result = equivalence_checker{fgl1}();
        }

        env->out() << "[i] the layout is " <<
                      (result.eq == equivalence_checker::equiv_result::eq_type::NONE ? "NOT" :
                       result.eq == equivalence_checker::equiv_result::eq_type::WEAK ? "WEAKLY" : "STRONGLY") <<
                      " equivalent to its specification";

        if (result.eq == equivalence_checker::equiv_result::eq_type::NONE && !result.counter_example.empty())
        {
            env->out() << " with counter example ";
            for (const auto c : result.counter_example)
                env->out() << c;
        }
        else if (result.eq == equivalence_checker::equiv_result::eq_type::WEAK)
        {
            if (!is_set("gate_layout"))
                env->out() << " with a delay of " << result.delay << " cycles";
        }
        env->out() << std::endl;

        reset_flags();
    }

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the equivalence checking process.
     */
    nlohmann::json log() const override
    {
        return nlohmann::json
        {
            {"equivalence type", result.eq == equivalence_checker::equiv_result::eq_type::NONE ? "NOT EQ" :
                                 result.eq == equivalence_checker::equiv_result::eq_type::WEAK ? "WEAK" : "STRONG"},
            {"counter example", result.counter_example},
            {"delay", result.delay},
            {"runtime (s)", mockturtle::to_seconds(result.runtime)}
        };
    }

private:
    /**
     * Index of gate layout that current gate layout is to be compared against.
     */
    int gli = -1;
    /**
     * Stores the result of the last equivalence check for easier access to result and logging data.
     */
    equivalence_checker::equiv_result result;

    /**
     * Reset all flags. Necessary for some reason... alice bug?
     */
    void reset_flags()
    {
        gli = -1;
    }
};

ALICE_ADD_COMMAND(equiv, "Verification")

}

#endif //FICTION_EQUIV_H
