//
// Created by marcel on 06.01.20.
//

#if (FICTION_Z3_SOLVER)

#ifndef FICTION_CMD_EXACT_HPP
#define FICTION_CMD_EXACT_HPP

#include <fiction/algorithms/exact.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <thread>
#include <variant>

namespace alice
{
/**
 * Executes an exact physical design approach utilizing the SMT solver Z3.
 * See algo/exact.h for more details.
 */
class exact_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit exact_command(const environment::ptr& e) :
            command(e, "Performs exact placement and routing of the current logic network in store. "
                       "A minimum FCN layout will be found that meets all given constraints.")
    {
        add_option("--clk_scheme,-s", clocking,
                   "Clocking scheme to use {OPEN[3|4], COLUMNAR[3|4]}, 2DDWAVE[3|4], USE, RES, BANCS", true);
        add_option("--upper_bound,-u", ps.upper_bound, "Number of FCN gate tiles to use at maximum");
        add_option("--fixed_size,-f", ps.fixed_size, "Execute only one iteration with the given number of tiles");
        add_option("--timeout,-t", ps.timeout, "Timeout in seconds");
        add_option("--async,-a", ps.num_threads, "Number of layout dimensions to examine in parallel");

        add_flag("--async_max,", "Examine as many layout dimensions in parallel as threads are available");
        add_flag("--crossings,-x", ps.crossings, "Enable wire crossings");
//        add_flag("--io_ports,-i", ps.io_ports, "Use I/O port elements instead of gate pins"); // TODO has to be true currently
        add_flag("--border_io,-b", ps.border_io, "Enforce primary I/O to be placed at the layout's borders");
        add_flag("--straight_inverters,-n", ps.straight_inverters, "Enforce NOT gates to be routed non-bending only");
        add_flag("--desynchronize,-d", ps.desynchronize,
                 "Do not enforce fan-in path balancing (reduces area and throughput, speeds up runtime)");
        add_flag("--minimize_wires,-w", ps.minimize_wires,
                 "Minimize the number of wire tiles to be used (slightly runtime expensive)");
        add_flag("--minimize_crossings,-c", ps.minimize_crossings,
                 "Minimize the number of crossing tiles to be used (slightly runtime expensive)");
        add_flag("--sync_elem,-e", ps.synchronization_elements,
                 "Allow synchronization elements to satisfy global synchronization (runtime expensive!)");
    }

  protected:
    /**
     * Function to perform the physical design call.
     * Given arguments are parsed and a placed and routed FCN gate layout is generated if possible.
     */
    void execute() override
    {
        auto& s = store<fiction::logic_network_t>();

        // error case: empty logic network store
        if (s.empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;
            reset_flags();
            return;
        }

        // error case: -f and -u are both set
        if (this->is_set("fixed_size") && this->is_set("upper_bound"))
        {
            env->out() << "[e] -u and -f cannot be set together" << std::endl;
            reset_flags();
            return;
        }
        // set the value of fixed_size as the upper bound if set
        else if (this->is_set("fixed_size"))
        {
            ps.upper_bound = ps.fixed_size;
        }

        // choose clocking
        if (auto clk = fiction::get_clocking_scheme<fiction::cart_gate_clk_lyt>(clocking); clk.has_value())
        {
            ps.scheme = std::make_shared<fiction::clocking_scheme<fiction::cart_gate_clk_lyt::tile>>(*clk);
        }
        else
        {
            env->out() << "[e] \"" << clocking << "\" does not refer to a supported clocking scheme" << std::endl;
            reset_flags();
            return;
        }

        // fetch number of threads available on the system
        if (this->is_set("async_max"))
        {
            if (auto threads_available = std::thread::hardware_concurrency(); threads_available == 0)
            {
                env->out() << "[w] could not detect the number of threads available to your system" << std::endl;
            }
            else
            {
                ps.num_threads = threads_available;
            }
        }

        // convert timeout entered in seconds to milliseconds
        ps.timeout *= 1000;

        const auto get_name = [](auto&& ntk_ptr) -> std::string { return ntk_ptr->get_network_name(); };

        const auto exact_physical_design = [this](auto&& ntk_ptr)
        { return fiction::exact<fiction::cart_gate_clk_lyt>(*ntk_ptr, ps, &st); };

        const auto ntk = s.current();

        // perform exact physical design
        try
        {
            const auto lyt = std::visit(exact_physical_design, ntk);

            if (lyt.has_value())
            {
                store<fiction::gate_layout_t>().extend() = std::make_shared<fiction::cart_gate_clk_lyt>(*lyt);
            }
            else
            {
                env->out() << fmt::format("[e] impossible to place and route {} within the given parameters",
                                          std::visit(get_name, ntk))
                           << std::endl;
            }
        }
        catch (...)
        {
            env->out() << fmt::format("[e] an error occurred while placing and routing {} with the given parameters",
                                      std::visit(get_name, ntk))
                       << std::endl;
        }

        reset_flags();
    }

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the solving process.
     */
    nlohmann::json log() const override
    {
        return nlohmann::json{
            {"runtime in seconds", mockturtle::to_seconds(st.time_total)},
            {"number of gates", st.num_gates},
            {"number of wires", st.num_wires},
            {"layout", {{"x-size", st.x_size}, {"y-size", st.y_size}, {"area", st.x_size * st.y_size}}}};
    }

  private:
    /**
     * Configuration object extracted from arguments and flags.
     */
    fiction::exact_physical_design_params<fiction::cart_gate_clk_lyt> ps{};

    fiction::exact_physical_design_stats st{};
    /**
     * Identifier of clocking scheme to use.
     */
    std::string clocking{"2DDWave"};

    /**
     * Reset all flags. Necessary for some reason... alice bug?
     */
    void reset_flags()
    {
        ps       = fiction::exact_physical_design_params<fiction::cart_gate_clk_lyt>{};
        clocking = "2DDWave";
    }
};

ALICE_ADD_COMMAND(exact, "Physical Design")

}  // namespace alice

#endif  // FICTION_CMD_EXACT_HPP

#endif  // FICTION_Z3_SOLVER