//
// Created by marcel on 06.12.23.
//

#ifndef FICTION_CMD_TEMP_HPP
#define FICTION_CMD_TEMP_HPP

#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math_utils.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace alice
{
/**
 *
 */
class temp_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit temp_command(const environment::ptr& e) :
            command(e, "Temperature-aware simulation of SiDB layouts. Uses QuickExact to determine the critical "
                       "temperature at which the ground state of the current SiDB layout is populated with a "
                       "probability below a given confidence level. For gate-based simulation, the probability of "
                       "erroneous calculations of the gate are applied.")
    {
        add_option(
            "--confidence_level,-c", params.confidence_level,
            "Probability threshold for ground state population. The temperature at which the simulation finds the "
            "ground state to be populated with a probability of less than the given percentage, is determined to be "
            "the critical temperature. For gate-based simulation, this is the probability of erroneous calculations of "
            "the gate.",
            true);
        add_option("--max_temperature,-t", params.max_temperature,
                   "Maximum simulation temperature beyond which no simulation will be conducted (~ 126 °C by default) "
                   "(unit: K)",
                   true);
        add_flag("--gate_based,-g",
                 "Gate-based simulation that matches the current SiDB layout in store against the current truth table "
                 "in store and considers the probability of erroneous calculations of the gate.");

        add_option("--epsilon_r,-e", physical_params.epsilon_r, "Electric permittivity of the substrate (unit-less)",
                   true);
        add_option("--lambda_tf,-l", physical_params.lambda_tf, "Thomas-Fermi screening distance (unit: nm)", true);
        add_option("--mu_minus,-m", physical_params.mu_minus, "Energy transition level (0/-) (unit: eV)", true);
        add_option("--base", physical_params.base,
                   "The simulation base, can be 2 or 3, though base 3 critical temperature simulation is experimental "
                   "(only ClusterComplete supports base 3 simulation)",
                   true);
        add_option("--engine", sim_engine_str,
                   "The simulation engine to use {QuickExact [default], ClusterComplete, QuickSim}", true);
    }

  protected:
    /**
     * Function to perform the simulation call.
     */
    void execute() override
    {
        // reset statistics
        stats = {};

        if (params.confidence_level <= 0 || params.confidence_level > 1)
        {
            env->out() << "[e] confidence_level must be in (0, 1]\n";
            reset_params();
            return;
        }
        if (physical_params.epsilon_r <= 0)
        {
            env->out() << "[e] epsilon_r must be positive\n";
            reset_params();
            return;
        }
        if (physical_params.lambda_tf <= 0)
        {
            env->out() << "[e] lambda_tf must be positive\n";
            reset_params();
            return;
        }

        auto& cs = store<fiction::cell_layout_t>();

        // error case: empty cell layout store
        if (cs.empty())
        {
            env->out() << "[w] no cell layout in store\n";
            reset_params();
            return;
        }

        auto& ts = store<fiction::truth_table_t>();

        // error case: empty truth table store
        if (is_set("gate_based"))
        {
            if (ts.empty())
            {
                env->out() << "[w] no truth table in store\n";
                reset_params();
                return;
            }
        }

        const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

        const auto temp = [this, &ts, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::has_sidb_technology_v<Lyt>)
            {
                if constexpr (fiction::is_charge_distribution_surface_v<Lyt>)
                {
                    env->out() << fmt::format(
                        "[w] {} already possesses a charge distribution; no simulation is conducted\n",
                        get_name(lyt_ptr));
                }
                else
                {
                    params.engine = get_sim_engine();

                    params.physical_parameters = physical_params;

                    if (is_set("gate_based"))
                    {
                        if (lyt_ptr->num_pis() == 0 || lyt_ptr->num_pos() == 0)
                        {
                            env->out() << fmt::format("[e] {} requires primary input and output cells to simulate its "
                                                      "Boolean function\n",
                                                      get_name(lyt_ptr));
                            return;
                        }

                        const auto tt_ptr = ts.current();

                        fiction::critical_temperature_gate_based(*lyt_ptr, std::vector<fiction::tt>{*tt_ptr}, params,
                                                                 &stats);
                    }
                    else
                    {
                        fiction::critical_temperature_non_gate_based(*lyt_ptr, params, &stats);
                    }

                    if (stats.num_valid_lyt == 0)
                    {
                        env->out() << fmt::format("[e] ground state of {} could not be determined\n",
                                                  get_name(lyt_ptr));
                    }
                    else
                    {
                        env->out() << fmt::format("[i] critical temperature of {} is {}{} K\n", get_name(lyt_ptr),
                                                  (stats.critical_temperature == params.max_temperature ? "> " : ""),
                                                  stats.critical_temperature);

                        if (stats.num_valid_lyt > 1)
                        {
                            env->out() << fmt::format(
                                "[i] energy between the ground state and the first erroneous is {} meV\n",
                                fiction::round_to_n_decimal_places(
                                    stats.energy_between_ground_state_and_first_erroneous, 2));
                        }
                    }
                }
            }
            else
            {
                env->out() << fmt::format("[e] {} is not an SiDB layout\n", get_name(lyt_ptr));
            }
        };

        std::visit(temp, cs.current());

        reset_params();
    }

  private:
    /**
     * Physical parameters for the simulation.
     */
    fiction::sidb_simulation_parameters physical_params{2, -0.32, 5.6, 5.0};
    /**
     * Critical temperature parameters.
     */
    fiction::critical_temperature_params params{};
    /**
     * Critical temperature statistics.
     */
    fiction::critical_temperature_stats stats{};
    /**
     * The simulation engine to use.
     */
    std::string sim_engine_str{"QuickExact"};
    /**
     * Convert the simulation engine string to the appropriate engine enum member. QuickExact is set as default.
     *
     * @return The `sidb_simulation_engine` member associated with the identifier.
     */
    [[nodiscard]] inline fiction::sidb_simulation_engine get_sim_engine() const noexcept
    {
        if (sim_engine_str == "ClusterComplete")
        {
            return fiction::sidb_simulation_engine::CLUSTERCOMPLETE;
        }
        else if (sim_engine_str == "QuickSim")
        {
            return fiction::sidb_simulation_engine::QUICKSIM;
        }
        else
        {
            return fiction::sidb_simulation_engine::QUICKEXACT;
        }
    }
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the simulation.
     */
    [[nodiscard]] nlohmann::json log() const override
    {
        return nlohmann::json{{"Algorithm name", stats.algorithm_name},
                              {"Physical parameters",
                               {{"base", stats.physical_parameters.base},
                                {"epsilon_r", stats.physical_parameters.epsilon_r},
                                {"lambda_tf", stats.physical_parameters.lambda_tf},
                                {"mu_minus", stats.physical_parameters.mu_minus}}},
                              {"Critical temperature", stats.critical_temperature},
                              {"Number of stable states", stats.num_valid_lyt},
                              {"Energy difference between ground state and first erroneous state",
                               stats.energy_between_ground_state_and_first_erroneous}};
    }
    /**
     * Resets the parameters to their default values.
     */
    void reset_params()
    {
        physical_params = fiction::sidb_simulation_parameters{2, -0.32, 5.6, 5.0};
        params          = {};
    }
};

ALICE_ADD_COMMAND(temp, "Simulation")

}  // namespace alice

#endif  // FICTION_CMD_TEMP_HPP
