//
// Created by marcel on 07.12.23.
//

#ifndef FICTION_CMD_QUICKSIM_HPP
#define FICTION_CMD_QUICKSIM_HPP

#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>

#include <variant>

namespace alice
{
/**
 *
 */
class quicksim_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit quicksim_command(const environment::ptr& e) :
            command(e,
                    "The QuickSim algorithm is a heuristic electrostatic ground state simulation algorithm for SiDB "
                    "layouts. It determines physically valid charge configurations (with minimal energy). Depending on "
                    "the simulation parameters, the ground state is found with a certain probability after one run.")
    {
        add_option("--epsilon_r,-e", physical_params.epsilon_r, "Electric permittivity of the substrate (unit-less)",
                   true);
        add_option("--lambda_tf,-l", physical_params.lambda_tf, "Thomas-Fermi screening distance (unit: nm)", true);
        add_option("--mu_minus,-m", physical_params.mu_minus, "Energy transition level (0/-) (unit: eV)", true);
        add_option("--base,-b", physical_params.base,
                   "2-state (neutral/negative) vs. 3-state (positive/neutral/negative) simulation", true);
        add_option("--iterations,-i", params.interation_steps, "Number of iterations to run the simulation for", true);
        add_option("--alpha,-a", params.alpha,
                   "alpha parameter (should be reduced if not charge distribution can be determined)", true);
    }

  protected:
    /**
     * Function to perform the simulation call.
     */
    void execute() override
    {
        // reset sim result
        sim_result = {};

        if (physical_params.epsilon_r <= 0)
        {
            env->out() << "[e] epsilon_r must be positive" << std::endl;
            reset_params();
            return;
        }

        if (physical_params.lambda_tf <= 0)
        {
            env->out() << "[e] lambda_tf must be positive" << std::endl;
            reset_params();
            return;
        }

        if (physical_params.base != 2 && physical_params.base != 3)
        {
            env->out() << "[e] base must be 2 or 3" << std::endl;
            reset_params();
            return;
        }

        if (params.alpha <= 0)
        {
            env->out() << "[e] alpha must be positive" << std::endl;
            reset_params();
            return;
        }

        auto& s = store<fiction::cell_layout_t>();

        // error case: empty cell layout store
        if (s.empty())
        {
            env->out() << "[w] no cell layout in store" << std::endl;
            reset_params();
            return;
        }

        const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

        const auto quicksim = [this, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::has_sidb_technology_v<Lyt>)
            {
                if constexpr (fiction::is_charge_distribution_surface_v<Lyt>)
                {
                    env->out() << fmt::format(
                                      "[w] {} already possesses a charge distribution; no simulation is conducted",
                                      get_name(lyt_ptr))
                               << std::endl;
                }
                else
                {
                    params.phys_params = physical_params;

                    sim_result = fiction::quicksim(*lyt_ptr, params);

                    if (sim_result.charge_distributions.empty())
                    {
                        env->out() << fmt::format("[e] no stable charge distribution could be determined for {}",
                                                  get_name(lyt_ptr))
                                   << std::endl;
                    }
                    else
                    {
                        store<fiction::cell_layout_t>().extend() =
                            std::make_shared<fiction::cds_sidb_cell_clk_lyt>(sim_result.charge_distributions.front());
                    }
                }
            }
            else
            {
                env->out() << fmt::format("[e] {} is not an SiDB layout", get_name(lyt_ptr)) << std::endl;
            }
        };

        std::visit(quicksim, s.current());

        reset_params();
    }

  private:
    /**
     * Physical parameters for the simulation.
     */
    fiction::sidb_simulation_parameters physical_params{2, -0.32, 5.6, 5.0};
    /**
     * QuickSim parameters.
     */
    fiction::quicksim_params params{};
    /**
     * Simulation result.
     */
    fiction::sidb_simulation_result<fiction::sidb_cell_clk_lyt> sim_result{};

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the simulation.
     */
    nlohmann::json log() const override
    {
        return nlohmann::json{
            {"Algorithm name", sim_result.algorithm_name},
            {"Simulation runtime", sim_result.simulation_runtime.count()},
            {"Physical parameters",
             {"base", sim_result.physical_parameters.base},
             {"epsilon_r", sim_result.physical_parameters.epsilon_r},
             {"lambda_tf", sim_result.physical_parameters.lambda_tf},
             {"mu_minus", sim_result.physical_parameters.mu_minus}},
            {"Lowest state energy (meV)", sim_result.charge_distributions.front().get_system_energy()},
            {"Number of stable states", sim_result.charge_distributions.size()}};
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

ALICE_ADD_COMMAND(quicksim, "Simulation")

}  // namespace alice

#endif  // FICTION_CMD_QUICKSIM_HPP
