//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_SIMULATE_H
#define FICTION_SIMULATE_H


#include "fcn_gate_layout.h"
#include "logic_network.h"
#include <alice/alice.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <nlohmann/json.hpp>
#include <itertools.hpp>


namespace alice
{
    /**
     * Simulates network or gate-level layout into truth tables.
     *
     * Parts of this code are originally from: https://github.com/msoeken/cirkit/blob/cirkit3/cli/algorithms/simulate.hpp.
     */
    class simulate_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit simulate_command(const environment::ptr& env)
                :
                command(env,
                        "Computes truth tables for each primary output of the current logic network or "
                        "gate layout in store.")
        {
            add_flag("--gate_layout,-g",
                     "Simulate gate layout");
            add_flag("--network,-n",
                     "Simulate logic network");
            add_flag("--store",
                     "Store simulation results in truth table store");
            add_flag("--silent", "Do not print truth tables");
        }

    protected:
        /**
         * Function to perform the simulate call. Computes truth tables.
         */
        void execute() override
        {
            if (is_set("gate_layout") == is_set("network"))
            {
                env->out() << "[w] exactly one store needs to be specified" << std::endl;
                return;
            }

            tables.clear();
            po_names.clear();

            if (is_set("gate_layout"))
            {
                auto& gls = store<fcn_gate_layout_ptr>();

                // error case: empty gate layout store
                if (gls.empty())
                {
                    env->out() << "[w] no gate layout in store" << std::endl;
                    return;
                }

                const auto fgl = gls.current();

                if (!fgl->has_io_pins())
                {
                    env->out() << "[w] " << fgl->get_name() << " must have designated I/O pins for simulation" << std::endl;
                    return;
                }

                // store PO names
                for (auto&& po : fgl->get_pos() | iter::sorted([&fgl](const auto& _t1, const auto& _t2)
                                                               {return *fgl->get_logic_vertex(_t1) <
                                                                       *fgl->get_logic_vertex(_t2); }))
                {
                    po_names.push_back(fgl->get_out_names(po).front());
                }

                try
                {
                    tables = fgl->simulate();
                }
                catch (const std::bad_alloc&)
                {
                    env->out() << "[e] " << fgl->get_name() << " has too many inputs to store its truth table" << std::endl;
                    return;
                }
            }
            else if (is_set("network"))
            {
                auto& lns = store<logic_network_ptr>();

                // error case: empty logic network store
                if (lns.empty())
                {
                    env->out() << "[w] no logic network in store" << std::endl;
                    return;
                }

                const auto ln = lns.current();

                // store PO names
                for (auto&& po : ln->get_pos())
                    po_names.push_back(ln->get_port_name(po));

                try
                {
                    tables = ln->simulate();
                }
                catch (const std::bad_alloc&)
                {
                    env->out() << "[e] " << ln->get_name() << " has too many inputs to store its truth table" << std::endl;
                    return;
                }
            }

            auto& tts = store<kitty::dynamic_truth_table>();
            for (auto i = 0ul; i < tables.size(); ++i)
            {
                if (is_set("store"))
                {
                    tts.extend() = tables[i];
                }
                if (!is_set("silent"))
                {
                    env->out() << fmt::format(" {} - hex: {}, bin: {}", po_names[i],
                                              kitty::to_hex(tables[i]), kitty::to_binary(tables[i])) << std::endl;
                }
            }
        }

        /**
         * Logs the resulting information in a log file.
         *
         * @return JSON object containing details about the truth tables.
         */
        nlohmann::json log() const override
        {
            nlohmann::json j;
            for (auto i = 0ul; i < tables.size(); ++i)
            {
                j.push_back({
                                {"po", po_names[i]},
                                {"vars",   tables[i].num_vars()},
                                {"hex",    kitty::to_hex(tables[i])},
                                {"binary", kitty::to_binary(tables[i])}
                            });
            }
            return {{"tables", j}};
        }

    private:
        /**
         * Stores truth tables.
         */
        std::vector<kitty::dynamic_truth_table> tables;
        /**
         * Stores PO names in order.
         */
        std::vector<std::string> po_names;
    };

    ALICE_ADD_COMMAND(simulate, "Logic")
}


#endif //FICTION_SIMULATE_H
