//
// Created by marcel on 16.07.18.
//

#ifndef FICTION_COMMANDS_H
#define FICTION_COMMANDS_H

#include "exact_pr.h"
#include "orthogonal_pr.h"
#include "fcn_gate_layout.h"
#include "fcn_cell_layout.h"
#include "qca_one_library.h"
#include <alice/alice.hpp>

namespace alice
{
    /**
     * Executes an exact placement and routing approach utilizing the SMT solver Z3.
     * See exact_pr.h for more details.
     */
    class exact_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit exact_command(const environment::ptr& env)
                :
                command(env, "Performs exact placement and routing of the current logic network in store. "
                             "A minimum FCN layout will be found that meets all given constraints.")
        {
            add_option("--upper_bound,-u", config.upper_bound,
                       "Number of FCN gate tiles to use at maximum")->required();
            add_option("--clocking_scheme,-s", clocking,
                       "Clocking scheme to be used {OPEN=0, USE=1, DIAGONAL=2}", true);
            add_option("--clock_numbers,-n", phases,
                       "Number of clock phases to be used {3 or 4}", true);
            add_option("--limit_crossings,-c", config.crossings_limit,
                       "Maximum number of tiles to use for crossings");
            add_option("--limit_wires,-w", config.wire_limit,
                       "Maximum number of wire elements per connection");
            add_option("--timeout,-t", config.timeout,
                       "Timeout in milliseconds");

            add_flag("--crossings,-x", config.crossings,
                     "Enable second layer for wire crossings");
            add_flag("--path_discrepancy,-p", config.path_discrepancy,
                     "Allows a discrepancy in fan-in paths (area vs. throughput)");
            add_flag("--artificial_latch,-a", config.artificial_latch,
                     "Allows clocked latch delays to balance fan-in paths");
            add_flag("--io_wires,-i", config.io_wires,
                     "Route extra wires to balance I/O port paths");
            add_flag("--border_io,-b", config.border_io,
                     "Enforce primary I/O to be placed at the layout's borders");
            add_flag("--fixed_size,-f", config.fixed_size,
                     "Execute only one run with the upper bound given as a fixed size");
        }

    protected:
        /**
         * Function to perform the P&R call. Given arguments are parsed and a FCN gate layout is generated if possible.
         */
        void execute() override
        {
            auto s = store<logic_network_ptr>();

            // error case: empty logic network store
            if (s.empty())
            {
                std::cout << "[e] no logic network in store" << std::endl;
                return;
            }

            if (phases == 3 && clocking == 1)
            {
                std::cout << "[e] 3-phase USE clocking is not supported yet" << std::endl;  // TODO support 3-phase USE
                return;
            }

            // choose clocking
            if (phases == 3)
            {
                config.scheme = std::make_shared<fcn_clocking_scheme>
                        (std::vector<fcn_clocking_scheme>{{open_3_clocking,
                                                           fcn_clocking_scheme{fcn_clock::cutout{}, 3u, false},  // TODO support 3-phase USE
                                                           diagonal_3_clocking
                                                         }}[clocking]);
            }
            else if (phases == 4)
            {
                config.scheme = std::make_shared<fcn_clocking_scheme>
                        (std::vector<fcn_clocking_scheme>{{open_4_clocking,
                                                           use_4_clocking,
                                                           diagonal_4_clocking
                                                          }}[clocking]);
            }
            else
            {
                std::cout << "[e] only 3- and 4-phase clocking schemes are supported yet" << std::endl;
                return;
            }

            // perform exact P&R
            exact_pr pr{std::move(s.current()), std::move(config)};
            auto result = pr.perform_place_and_route();

            if (result.success)
            {
                store<fcn_gate_layout_ptr>().extend() = std::move(pr.get_layout());
                pr_result = result.json;
            }
            else
                std::cout << "[e] not possible to place and route the specified network on "
                          << config.upper_bound << " layout tiles" << std::endl;

            // reset flags, necessary for some reason... alice bug?
            config = exact_pr_config{};
        }
        /**
         * Logs the resulting information in a log file.
         *
         * @return JSON object containing information about the solving process.
         */
        nlohmann::json log() const override
        {
            return pr_result;
        }

    private:
        /**
         * Configuration object extracted from arguments and flags.
         */
        exact_pr_config config{};
        /**
         * Identifier of clocking scheme to use (0=OPEN, 1=USE, 2=DIAGONAL).
         */
        unsigned clocking = 0u;
        /**
         * Number of clock phases to use. 3 and 4 are supported.
         */
        unsigned phases = 4u;
        /**
         * Resulting logging information.
         */
        nlohmann::json pr_result;
    };

    ALICE_ADD_COMMAND(exact, "Placement & Routing")


    /**
     * Executes a heuristic placement and routing approach based on an algorithm for orthogonal graph drawing by
     * Therese Biedl.
     * See orthogonal_pr.h for more details.
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
                command(env, "Performs heuristic placement and routing of the current logic network in store. "
                             "An FCN layout that is not minimal will be found in reasonable runtime.")
        {}

    protected:
        /**
         * Function to perform the P&R call. Generates an FCN gate layout.
         */
        void execute() override
        {
            auto s = store<logic_network_ptr>();

            // error case: empty logic network store
            if (s.empty())
            {
                std::cout << "[e] no logic network in store" << std::endl;
                return;
            }

            // perform heuristic P&R
            orthogonal_pr pr{std::move(s.current())};
            auto result = pr.perform_place_and_route();

            if (result.success)
            {
                store<fcn_gate_layout_ptr>().extend() = std::move(pr.get_layout());
                pr_result = result.json;
            }
            else
                std::cout << "[e] not possible to place and route the specified network" << std::endl;
        }
        /**
         * Logs the resulting information in a log file.
         *
         * @return JSON object containing information about the P&R process.
         */
        nlohmann::json log() const override
        {
            return pr_result;
        }

    private:
        /**
         * Resulting logging information.
         */
        nlohmann::json pr_result;
    };

    ALICE_ADD_COMMAND(ortho, "Placement & Routing")


    /**
     * Converts an fcn_gate_layout to an fcn_cell_layout. Conversion process allows to choose from supported FCN gate
     * libraries.
     */
    class gate_to_cell_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit gate_to_cell_command(const environment::ptr& env)
                :
                command(env, "Converts the current gate layout in store to a cell layout. A gate library must be "
                             "specified in order to instruct the algorithm how to map gate tiles to cell tiles.")
        {
            add_option("--library,-l", library,
                       "Gate library to use {QCA-ONE=0}", true);
        }

    protected:
        /**
         * Function to perform the P&R call. Generates an FCN gate layout.
         */
        void execute() override
        {
            auto s = store<fcn_gate_layout_ptr>();

            // error case: empty gate layout store
            if (s.empty())
            {
                std::cout << "[w] no gate layout in store" << std::endl;
                return;
            }

            fcn_gate_library_ptr lib = nullptr;
            try
            {
                if (library == 0u)
                    lib = std::make_shared<qca_one_library>(std::move(s.current()));
                // else if (library == 1u)
                    // more libraries go here
                else
                {
                    std::cout << "[e] identifier " << library << " does not refer to a supported gate library" << std::endl;
                    return;
                }
            }
            catch (...)
            {
                std::cout << "[e] could not assign directions in gate layout to cell ports" << std::endl;
                return;
            }

            fcn_cell_layout_ptr fcl = nullptr;
            try
            {
                fcl = std::make_shared<fcn_cell_layout>(std::move(lib));
            }
            catch (...)
            {
                std::cout << "[e] mapping to cell layout with the given library was not successful" << std::endl;
                return;
            }

            // store new layout
            store<fcn_cell_layout_ptr>().extend() = std::move(fcl);
        }

    private:
        /**
         * Identifier of gate library to use.
         */
        unsigned library = 0u;
    };

    ALICE_ADD_COMMAND(gate_to_cell, "Conversion")
}


#endif //FICTION_COMMANDS_H
