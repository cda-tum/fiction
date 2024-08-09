//
// Created by marcel on 06.01.20.
//

#if (FICTION_Z3_SOLVER)

#ifndef FICTION_CMD_EXACT_HPP
#define FICTION_CMD_EXACT_HPP

#include <fiction/algorithms/physical_design/exact.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <memory>
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
        add_option("--clk_scheme,-s", ps.scheme,
                   "Clocking scheme to use {OPEN[3|4], COLUMNAR[3|4], ROW[3|4] 2DDWAVE[3|4], 2DDWAVEHEX[3|4], USE, "
                   "RES, ESR, CFE, RIPPLE, BANCS}",
                   true);
        add_option("--upper_area", ps.upper_bound_area, "Upper bound for the total number of tiles");
        add_option("--upper_x", ps.upper_bound_x, "Upper bound for the number of tiles in x-direction");
        add_option("--upper_y", ps.upper_bound_y, "Upper bound for the number of tiles in y-direction");
        add_option("--fixed_size,-f", ps.fixed_size,
                   "Execute only one iteration with the given number of upper bound tiles");
        add_option("--timeout,-t", ps.timeout, "Timeout in seconds");
        add_option("--async,-a", ps.num_threads, "Number of layout dimensions to examine in parallel (beta feature)");

        add_flag("--async_max,",
                 "Examine as many layout dimensions in parallel as threads are available (beta feature)");
        add_option("--hex", hexagonal_tile_shift,
                   "Use hexagonal tiles and specify tile shift. Possible values are 'odd_row', 'even_row', "
                   "'odd_column', or 'even_column'");
        add_flag("--crossings,-x", ps.crossings, "Enable wire crossings");
        //        add_flag("--io_ports,-i", ps.io_ports, "Use I/O port elements instead of gate pins"); // TODO has to
        //        be true currently
        add_flag("--border_io,-b", ps.border_io, "Enforce primary I/O to be placed at the layout's borders");
        add_flag("--straight_inverters,-n", ps.straight_inverters, "Enforce NOT gates to be routed non-bending only");
        add_flag("--desynchronize,-d", ps.desynchronize,
                 "Do not enforce fan-in path balancing (reduces area and throughput, speeds up runtime)");
        add_flag("--minimize_wires,-w", ps.minimize_wires,
                 "Minimize the number of wire tiles to be used (slightly runtime expensive)");
        add_flag("--minimize_crossings,-c", ps.minimize_crossings,
                 "Minimize the number of crossing tiles to be used (slightly runtime expensive)");
        add_flag("--sync_elems,-e", ps.synchronization_elements,
                 "Allow synchronization elements to satisfy global synchronization (runtime expensive!)");

        add_flag("--topolinano", "Indicate the use of technology-specific constraints for iNML as used by ToPoliNano "
                                 "(to be used with COLUMNAR clocking)");
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

        if (is_set("timeout"))
        {
            // convert timeout entered in seconds to milliseconds
            ps.timeout *= 1000;
        }

        // target technology constraints
        if (this->is_set("topolinano"))
        {
            ps.technology_specifics = fiction::technology_constraints::TOPOLINANO;

            // shifted Cartesian layout
            exact_physical_design<fiction::cart_odd_col_gate_clk_lyt>();
        }
        else if (is_set("hex"))  // hexagonal layout
        {
            if (hexagonal_tile_shift == "odd_row")
            {
                exact_physical_design<fiction::hex_odd_row_gate_clk_lyt>();
            }
            else if (hexagonal_tile_shift == "even_row")
            {
                exact_physical_design<fiction::hex_even_row_gate_clk_lyt>();
            }
            else if (hexagonal_tile_shift == "odd_column")
            {
                exact_physical_design<fiction::hex_odd_col_gate_clk_lyt>();
            }
            else if (hexagonal_tile_shift == "even_column")
            {
                exact_physical_design<fiction::hex_even_col_gate_clk_lyt>();
            }
            else
            {
                env->out() << "[e] possible values for the hexagonal tile shift are 'odd_row', 'even_row', "
                              "'odd_column', and 'even_column'"
                           << std::endl;
            }
        }
        else  // Cartesian layout
        {
            exact_physical_design<fiction::cart_gate_clk_lyt>();
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
     * Parameters.
     */
    fiction::exact_physical_design_params ps{};
    /**
     * Statistics.
     */
    fiction::exact_physical_design_stats st{};
    /**
     * Tile shift for hexagonal layouts.
     */
    std::string hexagonal_tile_shift{};

    /**
     * Reset all flags. Necessary for some reason... alice bug?
     */
    void reset_flags()
    {
        ps                   = fiction::exact_physical_design_params{};
        hexagonal_tile_shift = {};
    }

    template <typename Lyt>
    std::shared_ptr<fiction::clocking_scheme<fiction::clock_zone<Lyt>>> fetch_clocking_scheme()
    {
        // fetch clocking scheme
        if (auto clk = fiction::get_clocking_scheme<Lyt>(ps.scheme); clk.has_value())
        {
            return fiction::ptr<Lyt>(std::move(*clk));
        }

        return nullptr;
    }

    template <typename Lyt>
    void exact_physical_design()
    {
        const auto get_name = [](auto&& ntk_ptr) -> std::string { return ntk_ptr->get_network_name(); };

        const auto perform_physical_design = [this](auto&& ntk_ptr) { return fiction::exact<Lyt>(*ntk_ptr, ps, &st); };

        const auto& ntk_ptr = store<fiction::logic_network_t>().current();

        // perform exact physical design
        try
        {
            const auto lyt = std::visit(perform_physical_design, ntk_ptr);

            if (lyt.has_value())
            {
                store<fiction::gate_layout_t>().extend() = std::make_shared<Lyt>(*lyt);
            }
            else
            {
                env->out() << fmt::format("[e] impossible to place and route '{}' within the given parameters",
                                          std::visit(get_name, ntk_ptr))
                           << std::endl;
            }
        }
        catch (const fiction::unsupported_clocking_scheme_exception&)
        {
            env->out() << fmt::format("\"{}\" does not refer to a supported clocking scheme", ps.scheme) << std::endl;
        }
        catch (const fiction::high_degree_fanin_exception& e)
        {
            env->out() << fmt::format("[e] {} of the given clocking scheme", e.what()) << std::endl;
        }
        catch (...)
        {
            env->out() << fmt::format("[e] an error occurred while placing and routing '{}' with the given parameters",
                                      std::visit(get_name, ntk_ptr))
                       << std::endl;
        }
    }
};

ALICE_ADD_COMMAND(exact, "Physical Design")

}  // namespace alice

#endif  // FICTION_CMD_EXACT_HPP

#endif  // FICTION_Z3_SOLVER
