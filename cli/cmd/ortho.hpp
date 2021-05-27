//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_ORTHO_HPP
#define FICTION_ORTHO_HPP


#include "../../algo/orthogonal.h"
#include "fcn_gate_layout.h"
#include "logic_network.h"
#include <alice/alice.hpp>
#include <nlohmann/json.hpp>


namespace alice
{
    /**
     * Executes a heuristic physical approach based on "Improved Orthogonal Drawing of 3-graphs" by Therese Biedl, 1996.
     * See algo/orthogonal.h for more details.
     */
    class ortho_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit ortho_command(const environment::ptr& env)
                :
                command(env, "Performs scalable placement and routing of the current logic network in store. "
                             "An FCN layout that is not minimal will be found in reasonable runtime.")
        {
            add_option("--clock_numbers,-n", phases,
                       "Number of clock phases to be used {3 or 4}", true);
            add_flag("--io_ports,-i", io_ports,
                     "Use I/O port elements instead of gate pins");
            add_flag("--border_io,-b", border_ios,
                     "Enforce primary I/O to be placed at the layout's borders");
        }

    protected:
        /**
         * Function to perform the physical design call. Generates a placed and routed FCN gate layout.
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
            // error case: phases out of range
            if (phases != 3u && phases != 4u)
            {
                env->out() << "[e] only 3- and 4-phase clocking schemes are supported" << std::endl;
                reset_flags();
                return;
            }
            // if border_ios is set, set io_ports too
            if (border_ios)
            {
                io_ports = true;
            }

            // perform scalable orthogonal physical design
            orthogonal physical_design{s.current(), phases, io_ports, border_ios};

            if (auto result = physical_design(); result.success)
            {
                store<fcn_gate_layout_ptr>().extend() = physical_design.get_layout();
                pd_result = result.json;
            }
            else
                env->out() << "[e] impossible to place and route " << s.current()->get_name() << std::endl;

            reset_flags();
        }

        /**
         * Logs the resulting information in a log file.
         *
         * @return JSON object containing information about the physical design process.
         */
        nlohmann::json log() const override
        {
            return pd_result;
        }

    private:
        /**
         * Number of clock phases to use. 3 and 4 are supported.
         */
        unsigned phases = 4u;
        /**
         * Flag to indicate that designated I/O ports should be placed.
         */
        bool io_ports = false;
        /**
         * Flag to indicate that designated I/O ports should be routed to the layout's borders.
         */
        bool border_ios = false;
        /**
         * Resulting logging information.
         */
        nlohmann::json pd_result{};

        /**
         * Reset all flags. Necessary for some reason... alice bug?
         */
        void reset_flags()
        {
            phases = 4u;
            io_ports = false;
            border_ios = false;
        }
    };

    ALICE_ADD_COMMAND(ortho, "Physical Design")
}


#endif //FICTION_ORTHO_HPP
