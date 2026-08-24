//
// Created by marcel on 12.12.23.
//

#include "cmd/simulation/include/opdom.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/io/write_operational_domain.hpp>
#include <fiction/networks/utils/name_utils.hpp>
#include <fiction/technology/sidb/simulation/engine.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace alice
{

opdom_command::opdom_command(const environment::ptr& e) :
        command(e,
                "Computes the operational domain for the current SiDB cell-level layout in store. An operational "
                "domain is a set of simulation parameter values for which a given SiDB layout is logically operational."
                "This means that a layout is deemed operational if the layout's ground state corresponds with a given "
                "Boolean function at the layout's outputs for all possible input combinations.")
{
    add_option("--random_sampling,-r", num_random_samples,
               "Use random sampling instead of grid search with this many random samples");
    add_option("--flood_fill,-f", num_random_samples,
               "Use flood fill instead of grid search with this many initial random samples (needs 2 or more sweep "
               "dimensions)");
    add_option("--contour_tracing,-c", num_random_samples,
               "Use contour tracing instead of grid search with this many random samples (needs 2 or more sweep "
               "dimensions; collects the boundary surface in 3 dimensions)");

    add_option("filename", filename, "CSV filename to write the operational domain to")->required();
    add_flag("--omit_non_op_samples,-o", omit_non_operational_samples,
             "Omit non-operational samples in the CSV file to reduce file size and increase visibility in 3D plots");
    add_flag("--sketch,-s", sketch,
             "Compute the operational domain sketch: determine the operational status by filtering alone instead of by "
             "physical simulation. Much faster, but reports some non-operational points as operational. Implies kink "
             "rejection and requires a layout with 'LOGIC' cells");

    add_option("--epsilon_r,-e", params.operational_params.sim_params.epsilon_r,
               "Electric permittivity of the substrate (unit-less)", true);
    add_option("--lambda_tf,-l", params.operational_params.sim_params.lambda_tf,
               "Thomas-Fermi screening distance (unit: nm)", true);
    add_option("--mu_minus,-m", params.operational_params.sim_params.mu_minus,
               "Energy transition level (0/-) (unit: eV)", true);

    add_option("--x_sweep,-x", x_sweep, "Sweep parameter of the x dimension [epsilon_r, lambda_tf, mu_minus]", true);
    add_option("--y_sweep,-y", y_sweep, "Sweep parameter of the y dimension [epsilon_r, lambda_tf, mu_minus]", true);
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

    add_option("--base", sim_params.base,
               "The simulation base, can be 2 or 3 (only ClusterComplete supports base-3 simulation)", true);
    add_option("--engine", sim_engine_str,
               "The simulation engine to use {QuickExact [default], ClusterComplete, QuickSim, ExGS}", true);
}

void opdom_command::execute()
{
    // reset operational domain and stats
    op_domain = {};
    stats     = {};

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

    if (params.operational_params.sim_params.epsilon_r <= 0)
    {
        env->out() << "[e] epsilon_r must be positive\n";
        reset_params();
        return;
    }
    if (params.operational_params.sim_params.lambda_tf <= 0)
    {
        env->out() << "[e] lambda_tf must be positive\n";
        reset_params();
        return;
    }

    // make sure that at most one algorithm is selected
    const std::array algorithm_selections = {is_set("random_sampling"), is_set("flood_fill"),
                                             is_set("contour_tracing")};
    if (std::ranges::count(algorithm_selections, true) > 1)
    {
        env->out() << "[e] only one algorithm can be selected at a time\n";
        reset_params();
        return;
    }

    // require positive number of samples for sampling-based algorithms
    if ((is_set("random_sampling") || is_set("flood_fill") || is_set("contour_tracing")) && num_random_samples == 0)
    {
        env->out() << "[e] number of samples must be > 0 for the selected algorithm\n";
        reset_params();
        return;
    }

    // make sure that z is not set if y is not, and that y is not set if x is not
    if (is_set("z_sweep") && !is_set("y_sweep"))
    {
        env->out() << "[e] z sweep parameter cannot be set if y sweep parameter is not set. Pass the first two "
                      "dimensions explicitly, e.g. '-x epsilon_r -y lambda_tf -z mu_minus'\n";
        reset_params();
        return;
    }
    if (is_set("y_sweep") && !is_set("x_sweep"))
    {
        env->out() << "[e] y sweep parameter cannot be set if x sweep parameter is not set\n";
        reset_params();
        return;
    }

    // overwrite the sweeps with their respective lower-case string representations. `std::tolower` is undefined for
    // negative values, which a plain `char` can hold, so each character is widened through `unsigned char` first
    const auto lowercase = [](std::string& str) noexcept
    {
        std::ranges::transform(str, str.begin(),
                               [](const unsigned char c) noexcept { return static_cast<char>(std::tolower(c)); });
    };

    lowercase(x_sweep);
    lowercase(y_sweep);
    lowercase(z_sweep);

    static constexpr const std::array valid_sweep_params = {"epsilon_r", "lambda_tf", "mu_minus"};

    // check if x sweep parameter is valid
    if (std::ranges::find(valid_sweep_params, x_sweep) == valid_sweep_params.cend())
    {
        env->out() << "[e] invalid x sweep parameter \"" << x_sweep
                   << "\". Has to be one of [epsilon_r, lambda_tf, "
                      "mu_minus]\n";
        reset_params();
        return;
    }

    // check if y sweep parameter is valid
    if (std::ranges::find(valid_sweep_params, y_sweep) == valid_sweep_params.cend())
    {
        env->out() << "[e] invalid y sweep parameter \"" << y_sweep
                   << "\". Has to be one of [epsilon_r, lambda_tf, "
                      "mu_minus]\n";
        reset_params();
        return;
    }

    // check if z sweep parameter is valid if set
    if (is_set("z_sweep"))
    {
        if (std::ranges::find(valid_sweep_params, z_sweep) == valid_sweep_params.cend())
        {
            env->out() << "[e] invalid z sweep parameter \"" << z_sweep
                       << "\". Has to be one of [epsilon_r, lambda_tf, "
                          "mu_minus]\n";
            reset_params();
            return;
        }
    }

    // assign x sweep parameters
    if (x_sweep == "epsilon_r")
    {
        sweep_dimensions[0].dimension = fiction::sidb::simulation::logic::sweep_parameter::EPSILON_R;
    }
    else if (x_sweep == "lambda_tf")
    {
        sweep_dimensions[0].dimension = fiction::sidb::simulation::logic::sweep_parameter::LAMBDA_TF;
    }
    else if (x_sweep == "mu_minus")
    {
        sweep_dimensions[0].dimension = fiction::sidb::simulation::logic::sweep_parameter::MU_MINUS;
    }

    // assign y sweep parameters
    if (y_sweep == "epsilon_r")
    {
        sweep_dimensions[1].dimension = fiction::sidb::simulation::logic::sweep_parameter::EPSILON_R;
    }
    else if (y_sweep == "lambda_tf")
    {
        sweep_dimensions[1].dimension = fiction::sidb::simulation::logic::sweep_parameter::LAMBDA_TF;
    }
    else if (y_sweep == "mu_minus")
    {
        sweep_dimensions[1].dimension = fiction::sidb::simulation::logic::sweep_parameter::MU_MINUS;
    }

    if (is_set("z_sweep"))
    {
        // assign z sweep parameters
        if (z_sweep == "epsilon_r")
        {
            sweep_dimensions[2].dimension = fiction::sidb::simulation::logic::sweep_parameter::EPSILON_R;
        }
        else if (z_sweep == "lambda_tf")
        {
            sweep_dimensions[2].dimension = fiction::sidb::simulation::logic::sweep_parameter::LAMBDA_TF;
        }
        else if (z_sweep == "mu_minus")
        {
            sweep_dimensions[2].dimension = fiction::sidb::simulation::logic::sweep_parameter::MU_MINUS;
        }
    }
    else
    {
        // remove z sweep parameter if not set
        sweep_dimensions.pop_back();
    }

    const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::networks::utils::get_name(*lyt_ptr); };

    const auto opdom = [this, &ts, &get_name](auto&& lyt_ptr)
    {
        const auto tt_ptr = ts.current();

        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (!fiction::has_sidb_technology_v<Lyt>)
        {
            env->out() << fmt::format("[e] '{}' is not an SiDB layout\n", get_name(lyt_ptr));
        }

        if (lyt_ptr->num_pis() == 0 || lyt_ptr->num_pos() == 0)
        {
            env->out() << fmt::format("[e] '{}' requires primary input and output cells to simulate its "
                                      "Boolean function\n",
                                      get_name(lyt_ptr));
            reset_params();
            return;
        }

        const auto engine = fiction::sidb::simulation::get_engine(sim_engine_str);

        if (!engine.has_value())
        {
            env->out() << fmt::format("[e] {} is not a supported SiDB simulation engine\n", sim_engine_str);
            return;
        }

        // set parameters
        params.operational_params.sim_params.base = sim_params.base;
        params.sweep_dimensions                   = sweep_dimensions;
        params.operational_params.sim_engine      = engine.value();

        if (sketch)
        {
            params.operational_params.strategy_to_analyze_operational_status =
                fiction::sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_ONLY;

            // the filtering steps are only defined when kinks are rejected, so the sketch implies the condition rather
            // than rejecting the request for not having set it by hand
            params.operational_params.op_condition =
                fiction::sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;
        }

        // Cache the engine name and the sketch setting for logging before any potential reset
        last_engine_name = fiction::sidb::simulation::engine_name(params.operational_params.sim_engine);
        last_sketch      = sketch;

        // To aid the compiler
        if constexpr (fiction::has_sidb_technology_v<Lyt>)
        {
            try
            {
                if (is_set("random_sampling"))
                {
                    op_domain = fiction::sidb::simulation::logic::operational_domain_random_sampling(
                        *lyt_ptr, std::vector{*tt_ptr}, num_random_samples, params, &stats);
                }
                else if (is_set("flood_fill"))
                {
                    op_domain = fiction::sidb::simulation::logic::operational_domain_flood_fill(
                        *lyt_ptr, std::vector{*tt_ptr}, num_random_samples, params, &stats);
                }
                else if (is_set("contour_tracing"))
                {
                    op_domain = fiction::sidb::simulation::logic::operational_domain_contour_tracing(
                        *lyt_ptr, std::vector{*tt_ptr}, num_random_samples, params, &stats);
                }
                else
                {
                    op_domain = fiction::sidb::simulation::logic::operational_domain_grid_search(
                        *lyt_ptr, std::vector{*tt_ptr}, params, &stats);
                }
            }
            catch (std::invalid_argument& e)
            {
                env->out() << fmt::format("[e] {}\n", e.what());
            }
            catch (...)
            {
                env->out() << "[e] an unknown error occurred during operational domain computation\n";
            }
        }
    };

    std::visit(opdom, cs.current());

    write_op_domain();

    reset_params();
}

void opdom_command::write_op_domain()
{
    // if the operational domain call was unsuccessful, do not attempt to write anything
    if (op_domain.empty())
    {
        reset_params();
        return;
    }

    // set up parameters
    fiction::write_operational_domain_params write_opdom_params{};
    write_opdom_params.non_operational_tag = "0";
    write_opdom_params.operational_tag     = "1";
    write_opdom_params.writing_mode =
        omit_non_operational_samples ? fiction::write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY :
                                       fiction::write_operational_domain_params::sample_writing_mode::ALL_SAMPLES;

    try
    {
        write_operational_domain(op_domain, filename, write_opdom_params);
    }
    catch (const std::exception& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
        reset_params();
    }
    catch (...)
    {
        env->out() << "[e] an unknown error occurred while writing the operational domain data\n";
        reset_params();
    }
}

nlohmann::json opdom_command::log() const
{
    return nlohmann::json{
        {"Algorithm name", last_engine_name},
        {"Runtime in seconds", mockturtle::to_seconds(stats.time_total)},
        {"Number of simulator invocations", stats.num_simulator_invocations},
        {"Number of evaluated parameter combinations", stats.num_evaluated_parameter_combinations},
        {"Number of operational parameter combinations", stats.num_operational_parameter_combinations},
        {"Number of non-operational parameter combinations", stats.num_non_operational_parameter_combinations},
        {"Operational domain sketch", last_sketch}};
}

void opdom_command::reset_params()
{
    sim_params       = fiction::sidb::model::simulation_parameters{2, -0.32, 5.6, 5.0};
    sweep_dimensions = std::vector<fiction::sidb::simulation::logic::operational_domain_value_range>{
        {fiction::sidb::simulation::logic::sweep_parameter::EPSILON_R, 1.0, 10.0, 0.1},
        {fiction::sidb::simulation::logic::sweep_parameter::LAMBDA_TF, 1.0, 10.0, 0.1},
        {fiction::sidb::simulation::logic::sweep_parameter::MU_MINUS, -0.50, -0.10, 0.025}};
    params = {};

    x_sweep  = "epsilon_r";
    y_sweep  = "lambda_tf";
    z_sweep  = "";
    filename = "";

    omit_non_operational_samples = false;
    sketch                       = false;
}

}  // namespace alice
