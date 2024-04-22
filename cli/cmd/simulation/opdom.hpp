//
// Created by marcel on 12.12.23.
//

#ifndef FICTION_CMD_OPDOM_HPP
#define FICTION_CMD_OPDOM_HPP

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/io/write_operational_domain.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <exception>
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
class opdom_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit opdom_command(const environment::ptr& e) :
            command(
                e,
                "Computes the operational domain for the current SiDB cell-level layout in store. An operational "
                "domain is a set of simulation parameter values for which a given SiDB layout is logically operational."
                "This means that a layout is deemed operational if the layout's ground state corresponds with a given "
                "Boolean function at the layout's outputs for all possible input combinations.")
    {
        add_option("--random_sampling,-r", num_random_samples,
                   "Use random sampling instead of grid search with this many random samples");
        add_option("--flood_fill,-f", num_random_samples,
                   "Use flood fill instead of grid search with this many initial random samples");
        add_option("--contour_tracing,-c", num_random_samples,
                   "Use contour tracing instead of grid search with up to this many random "
                   "samples");

        add_option("filename", filename, "CSV filename to write the operational domain to")->required();

        add_option("--epsilon_r,-e", simulation_params.epsilon_r, "Electric permittivity of the substrate (unit-less)",
                   true);
        add_option("--lambda_tf,-l", simulation_params.lambda_tf, "Thomas-Fermi screening distance (unit: nm)", true);
        add_option("--mu_minus,-m", simulation_params.mu_minus, "Energy transition level (0/-) (unit: eV)", true);

        add_option("--x_sweep", x_sweep, "Sweep parameter of the x dimension [epsilon_r, lambda_tf, mu_minus]", true);
        add_option("--y_sweep", y_sweep, "Sweep parameter of the y dimension [epsilon_r, lambda_tf, mu_minus]", true);

        add_option("--x_min", params.x_min, "Minimum value of the x dimension sweep", true);
        add_option("--x_max", params.x_max, "Maximum value of the x dimension sweep", true);
        add_option("--x_step", params.x_step, "Step size of the x dimension sweep", true);
        add_option("--y_min", params.y_min, "Minimum value of the y dimension sweep", true);
        add_option("--y_max", params.y_max, "Maximum value of the y dimension sweep", true);
        add_option("--y_step", params.y_step, "Step size of the y dimension sweep", true);
        add_option("--base", simulation_params.base,
                   "The simulation base, can be 2 or 3 (only ClusterComplete supports base 3 simulation)", true);
        add_option("--engine", sim_engine_str,
                   "The simulation engine to use {QuickExact [default], ClusterComplete, QuickSim, ExGS}", true);
    }

  protected:
    /**
     * Function to perform the operational domain call.
     */
    void execute() override
    {
        // reset operational domain and stats
        op_domain = {};
        stats     = {};

        if (simulation_params.epsilon_r <= 0)
        {
            env->out() << "[e] epsilon_r must be positive\n";
            reset_params();
            return;
        }
        if (simulation_params.lambda_tf <= 0)
        {
            env->out() << "[e] lambda_tf must be positive\n";
            reset_params();
            return;
        }

        // check for valid x and y parameter bounds
        if (params.x_min >= params.x_max)
        {
            env->out() << "[e] x_min must be smaller than x_max\n";
            reset_params();
            return;
        }
        if (params.y_min >= params.y_max)
        {
            env->out() << "[e] y_min must be smaller than y_max\n";
            reset_params();
            return;
        }

        // make sure that at most one algorithm is selected
        const std::array algorithm_selections = {is_set("random_sampling"), is_set("flood_fill"),
                                                 is_set("contour_tracing")};
        if (std::count(algorithm_selections.cbegin(), algorithm_selections.cend(), true) > 1)
        {
            env->out() << "[e] only one algorithm can be selected at a time\n";
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
        if (ts.empty())
        {
            env->out() << "[w] no truth table in store\n";
            reset_params();
            return;
        }

        // default sweep parameters
        if (x_sweep.empty() && y_sweep.empty())
        {
            x_sweep = "epsilon_r";
            y_sweep = "lambda_tf";
        }
        else
        {
            // overwrite x and y sweep with their respective lower-case string representations
            std::transform(x_sweep.begin(), x_sweep.end(), x_sweep.begin(), ::tolower);
            std::transform(y_sweep.begin(), y_sweep.end(), y_sweep.begin(), ::tolower);

            static constexpr const std::array valid_sweep_params = {"epsilon_r", "lambda_tf", "mu_minus"};

            // check if x sweep parameter is valid
            if (std::find(valid_sweep_params.cbegin(), valid_sweep_params.cend(), x_sweep) == valid_sweep_params.cend())
            {
                env->out() << "[e] invalid x sweep parameter \"" << x_sweep
                           << "\". Has to be one of [epsilon_r, lambda_tf, "
                              "mu_minus]\n";
                reset_params();
                return;
            }

            // check if y sweep parameter is valid
            if (std::find(valid_sweep_params.cbegin(), valid_sweep_params.cend(), y_sweep) == valid_sweep_params.cend())
            {
                env->out() << "[e] invalid y sweep parameter \"" << y_sweep
                           << "\". Has to be one of [epsilon_r, lambda_tf, "
                              "mu_minus]\n";
                reset_params();
                return;
            }
        }

        // assign x sweep parameters
        if (x_sweep == "epsilon_r")
        {
            params.x_dimension = fiction::operational_domain::sweep_parameter::EPSILON_R;
        }
        else if (x_sweep == "lambda_tf")
        {
            params.x_dimension = fiction::operational_domain::sweep_parameter::LAMBDA_TF;
        }
        else if (x_sweep == "mu_minus")
        {
            params.x_dimension = fiction::operational_domain::sweep_parameter::MU_MINUS;
        }

        // assign y sweep parameters
        if (y_sweep == "epsilon_r")
        {
            params.y_dimension = fiction::operational_domain::sweep_parameter::EPSILON_R;
        }
        else if (y_sweep == "lambda_tf")
        {
            params.y_dimension = fiction::operational_domain::sweep_parameter::LAMBDA_TF;
        }
        else if (y_sweep == "mu_minus")
        {
            params.y_dimension = fiction::operational_domain::sweep_parameter::MU_MINUS;
        }

        const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

        const auto opdom = [this, &ts, &get_name](auto&& lyt_ptr)
        {
            const auto tt_ptr = ts.current();

            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::has_sidb_technology_v<Lyt>)
            {
                if (lyt_ptr->num_pis() == 0 || lyt_ptr->num_pos() == 0)
                {
                    env->out() << fmt::format("[e] {} requires primary input and output cells to simulate its "
                                              "Boolean function\n",
                                              get_name(lyt_ptr));
                    return;
                }

                params.sim_engine = get_sim_engine();

                params.sim_params = simulation_params;

                if (is_set("random_sampling"))
                {
                    op_domain = fiction::operational_domain_random_sampling(*lyt_ptr, std::vector<fiction::tt>{*tt_ptr},
                                                                            num_random_samples, params, &stats);
                }
                else if (is_set("flood_fill"))
                {
                    op_domain = fiction::operational_domain_flood_fill(*lyt_ptr, std::vector<fiction::tt>{*tt_ptr},
                                                                       num_random_samples, params, &stats);
                }
                else if (is_set("contour_tracing"))
                {
                    op_domain = fiction::operational_domain_contour_tracing(*lyt_ptr, std::vector<fiction::tt>{*tt_ptr},
                                                                            num_random_samples, params, &stats);
                }
                else
                {
                    op_domain = fiction::operational_domain_grid_search(*lyt_ptr, std::vector<fiction::tt>{*tt_ptr},
                                                                        params, &stats);
                }
            }
            else
            {
                env->out() << fmt::format("[e] {} is not an SiDB layout\n", get_name(lyt_ptr));
            }
        };

        std::visit(opdom, cs.current());

        write_op_domain();

        reset_params();
    }

  private:
    /**
     * Physical parameters for the simulation.
     */
    fiction::sidb_simulation_parameters simulation_params{2, -0.32, 5.6, 5.0};
    /**
     * Operational domain parameters.
     */
    fiction::operational_domain_params params{};
    /**
     * Operational domain stats.
     */
    fiction::operational_domain_stats stats{};
    /**
     * Number of random samples.
     */
    std::size_t num_random_samples{};
    /**
     * User input for the x dimension sweep parameter.
     */
    std::string x_sweep{};
    /**
     * User input for the y dimension sweep parameter.
     */
    std::string y_sweep{};
    /**
     * The simulation engine to use.
     */
    std::string sim_engine_str{"QuickExact"};
    /**
     * CSV filename to write the operational domain to.
     */
    std::string filename{};
    /**
     * The operational domain.
     */
    fiction::operational_domain op_domain{};
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

        if (sim_engine_str == "QuickSim")
        {
            return fiction::sidb_simulation_engine::QUICKSIM;
        }

        if (sim_engine_str == "ExGS")
        {
            return fiction::sidb_simulation_engine::EXGS;
        }

        return fiction::sidb_simulation_engine::QUICKEXACT;
    }
    /**
     * Writes the operational domain to the specified CSV file.
     */
    void write_op_domain() const
    {
        static const fiction::write_operational_domain_params write_opdom_params{"1", "0"};
        try
        {
            fiction::write_operational_domain(op_domain, filename, write_opdom_params);
        }
        catch (const std::exception& e)
        {
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
    }
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the operational domain.
     */
    [[nodiscard]] nlohmann::json log() const override
    {
        return nlohmann::json{
            {"Algorithm name", sidb_simulation_engine_name(params.sim_engine)},
            {"Runtime in seconds", mockturtle::to_seconds(stats.time_total)},
            {"Number of simulator invocations", stats.num_simulator_invocations},
            {"Number of evaluated parameter combinations", stats.num_evaluated_parameter_combinations},
            {"Number of operational parameter combinations", stats.num_operational_parameter_combinations},
            {"Number of non-operational parameter combinations", stats.num_non_operational_parameter_combinations}};
    }
    /**
     * Resets the parameters to their default values.
     */
    void reset_params()
    {
        simulation_params = fiction::sidb_simulation_parameters{2, -0.32, 5.6, 5.0};
        params            = {};
        x_sweep           = {};
        y_sweep           = {};
        filename          = {};
    }
};

ALICE_ADD_COMMAND(opdom, "Simulation")

}  // namespace alice

#endif  // FICTION_CMD_OPDOM_HPP
