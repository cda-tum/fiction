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
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace alice
{
/**
 * Executes operational domain computation for the current SiDB cell-level layout in store. The operational domain is a
 * set of simulation parameter values for which a given SiDB layout is logically operational. This means that a layout
 * is deemed operational if the layout's ground state corresponds with a given Boolean function at the layout's outputs
 * for all possible input combinations.
 *
 * The computation comes in different flavors:
 * - Grid search: Evaluates all possible parameter combinations within the specified ranges.
 * - Random sampling: Evaluates a specified number of random parameter combinations.
 * - Flood fill: Evaluates a specified number of random parameter combinations and then performs a flood fill to find
 *  the operational domain.
 *  - Contour tracing: Evaluates a specified number of random parameter combinations and then performs contour tracing
 *  to find the edges of the operational domain.
 *
 * The operational domain is written to a CSV file, which can be used for further analysis or visualization.
 *
 * For more information, see algorithms/simulation/sidb/operational_domain.hpp.
 *
 * BUG: Currently, the manual parameter reset (which is necessary due to an alice issue) triggers a different alice
 * problem: data is not read from the CLI properly after the manual reset. This causes error messages to be displayed
 * only once even if the same misconfigured command is executed twice in a row.
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
        add_option("--quicktrace,-c", num_random_samples,
                   "Use contour tracing instead of grid search with this many random samples");

        add_option("filename", filename, "CSV filename to write the operational domain to")->required();
        add_flag(
            "--omit_non_op_samples,-o", omit_non_operational_samples,
            "Omit non-operational samples in the CSV file to reduce file size and increase visibility in 3D plots");

        add_option("--epsilon_r,-e", params.operational_params.simulation_parameters.epsilon_r,
                   "Electric permittivity of the substrate (unit-less)", true);
        add_option("--lambda_tf,-l", params.operational_params.simulation_parameters.lambda_tf,
                   "Thomas-Fermi screening distance (unit: nm)", true);
        add_option("--mu_minus,-m", params.operational_params.simulation_parameters.mu_minus,
                   "Energy transition level (0/-) (unit: eV)", true);

        add_option("--x_sweep,-x", x_sweep, "Sweep parameter of the x dimension [epsilon_r, lambda_tf, mu_minus]",
                   true);
        add_option("--y_sweep,-y", y_sweep, "Sweep parameter of the y dimension [epsilon_r, lambda_tf, mu_minus]",
                   true);
        add_option("--z_sweep,-z", z_sweep,
                   "Sweep parameter of the z dimension (optional) [epsilon_r, lambda_tf, mu_minus]");

        add_option("--x_min", sweep_dimensions[0].min, "Minimum value of the x dimension sweep", true);
        add_option("--x_max", sweep_dimensions[0].max, "Maximum value of the x dimension sweep", true);
        add_option("--x_step", sweep_dimensions[0].step, "Step size of the x dimension sweep", true);
        add_option("--y_min", sweep_dimensions[1].min, "Minimum value of the y dimension sweep", true);
        add_option("--y_max", sweep_dimensions[1].max, "Maximum value of the y dimension sweep", true);
        add_option("--y_step", sweep_dimensions[1].step, "Step size of the y dimension sweep", true);
        add_option("--z_min", sweep_dimensions[2].min, "Minimum value of the z dimension sweep");
        add_option("--z_max", sweep_dimensions[2].max, "Maximum value of the z dimension sweep");
        add_option("--z_step", sweep_dimensions[2].step, "Step size of the z dimension sweep");
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

        auto& cs = store<fiction::cell_layout_t>();

        // error case: empty cell layout store
        if (cs.empty())
        {
            env->out() << "[w] no cell layout in store" << std::endl;
            reset_params();
            return;
        }

        auto& ts = store<fiction::truth_table_t>();

        // error case: empty truth table store
        if (ts.empty())
        {
            env->out() << "[w] no truth table in store" << std::endl;
            reset_params();
            return;
        }

        if (params.operational_params.simulation_parameters.epsilon_r <= 0)
        {
            env->out() << "[e] epsilon_r must be positive" << std::endl;
            reset_params();
            return;
        }
        if (params.operational_params.simulation_parameters.lambda_tf <= 0)
        {
            env->out() << "[e] lambda_tf must be positive" << std::endl;
            reset_params();
            return;
        }

        // make sure that at most one algorithm is selected
        const std::array algorithm_selections = {is_set("random_sampling"), is_set("flood_fill"),
                                                 is_set("quicktrace")};
        if (std::count(algorithm_selections.cbegin(), algorithm_selections.cend(), true) > 1)
        {
            env->out() << "[e] only one algorithm can be selected at a time" << std::endl;
            reset_params();
            return;
        }

        // make sure that z is not set if y is not, and that y is not set if x is not
        if (is_set("z_sweep") && !is_set("y_sweep"))
        {
            env->out() << "[e] z sweep parameter cannot be set if y sweep parameter is not set" << std::endl;
            reset_params();
            return;
        }
        if (is_set("y_sweep") && !is_set("x_sweep"))
        {
            env->out() << "[e] y sweep parameter cannot be set if x sweep parameter is not set" << std::endl;
            reset_params();
            return;
        }

        // overwrite the sweeps with their respective lower-case string representations
        std::transform(x_sweep.begin(), x_sweep.end(), x_sweep.begin(), ::tolower);
        std::transform(y_sweep.begin(), y_sweep.end(), y_sweep.begin(), ::tolower);
        std::transform(z_sweep.begin(), z_sweep.end(), z_sweep.begin(), ::tolower);

        static constexpr const std::array valid_sweep_params = {"epsilon_r", "lambda_tf", "mu_minus"};

        // check if x sweep parameter is valid
        if (std::find(valid_sweep_params.cbegin(), valid_sweep_params.cend(), x_sweep) == valid_sweep_params.cend())
        {
            env->out() << "[e] invalid x sweep parameter \"" << x_sweep
                       << "\". Has to be one of [epsilon_r, lambda_tf, "
                          "mu_minus]"
                       << std::endl;
            reset_params();
            return;
        }

        // check if y sweep parameter is valid
        if (std::find(valid_sweep_params.cbegin(), valid_sweep_params.cend(), y_sweep) == valid_sweep_params.cend())
        {
            env->out() << "[e] invalid y sweep parameter \"" << y_sweep
                       << "\". Has to be one of [epsilon_r, lambda_tf, "
                          "mu_minus]"
                       << std::endl;
            reset_params();
            return;
        }

        // check if z sweep parameter is valid if set
        if (is_set("z_sweep"))
        {
            if (std::find(valid_sweep_params.cbegin(), valid_sweep_params.cend(), z_sweep) == valid_sweep_params.cend())
            {
                env->out() << "[e] invalid z sweep parameter \"" << z_sweep
                           << "\". Has to be one of [epsilon_r, lambda_tf, "
                              "mu_minus]"
                           << std::endl;
                reset_params();
                return;
            }
        }

        // assign x sweep parameters
        if (x_sweep == "epsilon_r")
        {
            sweep_dimensions[0].dimension = fiction::sweep_parameter::EPSILON_R;
        }
        else if (x_sweep == "lambda_tf")
        {
            sweep_dimensions[0].dimension = fiction::sweep_parameter::LAMBDA_TF;
        }
        else if (x_sweep == "mu_minus")
        {
            sweep_dimensions[0].dimension = fiction::sweep_parameter::MU_MINUS;
        }

        // assign y sweep parameters
        if (y_sweep == "epsilon_r")
        {
            sweep_dimensions[1].dimension = fiction::sweep_parameter::EPSILON_R;
        }
        else if (y_sweep == "lambda_tf")
        {
            sweep_dimensions[1].dimension = fiction::sweep_parameter::LAMBDA_TF;
        }
        else if (y_sweep == "mu_minus")
        {
            sweep_dimensions[1].dimension = fiction::sweep_parameter::MU_MINUS;
        }

        if (is_set("z_sweep"))
        {
            // assign z sweep parameters
            if (z_sweep == "epsilon_r")
            {
                sweep_dimensions[2].dimension = fiction::sweep_parameter::EPSILON_R;
            }
            else if (z_sweep == "lambda_tf")
            {
                sweep_dimensions[2].dimension = fiction::sweep_parameter::LAMBDA_TF;
            }
            else if (z_sweep == "mu_minus")
            {
                sweep_dimensions[2].dimension = fiction::sweep_parameter::MU_MINUS;
            }
        }
        else
        {
            // remove z sweep parameter if not set
            sweep_dimensions.pop_back();
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
                    env->out() << fmt::format("[e] '{}' requires primary input and output cells to simulate its "
                                              "Boolean function",
                                              get_name(lyt_ptr))
                               << std::endl;
                    reset_params();
                    return;
                }

                // set parameters
                params.operational_params.simulation_parameters = simulation_params;
                params.operational_params.sim_engine            = fiction::sidb_simulation_engine::QUICKEXACT;
                params.sweep_dimensions                         = sweep_dimensions;

                try
                {
                    if (is_set("random_sampling"))
                    {
                        op_domain = fiction::operational_domain_random_sampling(
                            *lyt_ptr, std::vector<fiction::tt>{*tt_ptr}, num_random_samples, params, &stats);
                    }
                    else if (is_set("flood_fill"))
                    {
                        op_domain = fiction::operational_domain_flood_fill(*lyt_ptr, std::vector<fiction::tt>{*tt_ptr},
                                                                           num_random_samples, params, &stats);
                    }
                    else if (is_set("quicktrace"))
                    {
                        op_domain = fiction::operational_domain_contour_tracing(
                            *lyt_ptr, std::vector<fiction::tt>{*tt_ptr}, num_random_samples, params, &stats);
                    }
                    else
                    {
                        op_domain = fiction::operational_domain_grid_search(*lyt_ptr, std::vector<fiction::tt>{*tt_ptr},
                                                                            params, &stats);
                    }
                }
                catch (std::invalid_argument& e)
                {
                    env->out() << fmt::format("[e] {}", e.what()) << std::endl;
                    reset_params();
                    return;
                }
                catch (...)
                {
                    env->out() << "[e] an unknown error occurred during operational domain computation" << std::endl;
                    reset_params();
                    return;
                }
            }
            else
            {
                env->out() << fmt::format("[e] {} is not an SiDB layout", get_name(lyt_ptr)) << std::endl;
                reset_params();
                return;
            }
        };

        std::visit(opdom, cs.current());

        write_op_domain();

        reset_params();
    }

  private:
    /**
     * Default physical parameters for the simulation.
     */
    fiction::sidb_simulation_parameters simulation_params{2, -0.32, 5.6, 5.0};
    /**
     * Default value ranges for sweeping.
     */
    std::vector<fiction::operational_domain_value_range> sweep_dimensions{
        {fiction::sweep_parameter::EPSILON_R, 1.0, 10.0, 0.1},
        {fiction::sweep_parameter::LAMBDA_TF, 1.0, 10.0, 0.1},
        {fiction::sweep_parameter::MU_MINUS, -0.50, -0.10, 0.025}};
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
    std::string x_sweep{"epsilon_r"};
    /**
     * User input for the y dimension sweep parameter.
     */
    std::string y_sweep{"lambda_tf"};
    /**
     * User input for the z dimension sweep parameter.
     */
    std::string z_sweep{};
    /**
     * CSV filename to write the operational domain to.
     */
    std::string filename{};
    /**
     * Flag to omit non-operational samples.
     */
    bool omit_non_operational_samples = false;
    /**
     * The operational domain.
     */
    fiction::operational_domain<> op_domain{};

    /**
     * Writes the operational domain to the specified CSV file.
     */
    void write_op_domain()
    {
        // if the operational domain call was unsuccessful, do not attempt writing anything
        if (op_domain.operational_values.empty())
        {
            reset_params();
            return;
        }

        // set up parameters
        fiction::write_operational_domain_params write_opdom_params{};
        write_opdom_params.non_operational_tag = "0";
        write_opdom_params.operational_tag     = "1";
        write_opdom_params.writing_mode =
            omit_non_operational_samples ?
                fiction::write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY :
                fiction::write_operational_domain_params::sample_writing_mode::ALL_SAMPLES;

        try
        {
            fiction::write_operational_domain(op_domain, filename, write_opdom_params);
        }
        catch (const std::exception& e)
        {
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
            reset_params();
            return;
        }
        catch (...)
        {
            env->out() << "[e] an unknown error occurred while writing the operational domain data" << std::endl;
            reset_params();
            return;
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
            {"Algorithm name", "QuickExact"},
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
        sweep_dimensions  = std::vector<fiction::operational_domain_value_range>{
            {fiction::sweep_parameter::EPSILON_R, 1.0, 10.0, 0.1},
            {fiction::sweep_parameter::LAMBDA_TF, 1.0, 10.0, 0.1},
            {fiction::sweep_parameter::MU_MINUS, -0.50, -0.10, 0.025}};
        params = {};

        x_sweep  = "epsilon_r";
        y_sweep  = "lambda_tf";
        z_sweep  = "";
        filename = "";

        omit_non_operational_samples = false;
    }
};

ALICE_ADD_COMMAND(opdom, "Simulation")

}  // namespace alice

#endif  // FICTION_CMD_OPDOM_HPP
