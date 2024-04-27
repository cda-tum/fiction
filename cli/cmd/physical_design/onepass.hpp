//
// Created by marcel on 09.04.20.
//

#if (MUGEN)

#ifndef FICTION_CMD_ONEPASS_HPP
#define FICTION_CMD_ONEPASS_HPP

#include <fiction/algorithms/physical_design/one_pass_synthesis.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#if !defined(__APPLE__)
#include <thread>
#endif

namespace alice
{
/**
 * Executes a SAT-driven topology-based logic re-synthesis, i.e., a one-pass synthesis. Utilizes the synthesis tool
 * Mugen by Winston Haaswijk.
 *
 * See fiction/algorithms/one_pass_synthesis.hpp for more details.
 */
class onepass_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit onepass_command(const environment::ptr& e) :
            command(e, "SAT-driven topology-based logic re-synthesis, i.e., one-pass synthesis. Uses "
                       "Mugen by Winston Haaswijk to synthesize a specification in terms of a truth table or a "
                       "logic network onto a given clocking scheme. Gate types to be used can be specified. If "
                       "none are given, all are enabled, because synthesis without gates cannot work. Layouts "
                       "resulting from this approach might be desynchronized. I/Os are always located at the "
                       "layout's borders.")
    {
        add_option("--clk_scheme,-s", ps.scheme, "Clocking scheme to use {2DDWAVE[3|4], USE, RES, ESR, CFE, BANCS}",
                   true);
        add_option("--upper_x", ps.upper_bound_x, "Number of FCN gate tiles to use at maximum in x-direction");
        add_option("--upper_y", ps.upper_bound_y, "Number of FCN gate tiles to use at maximum in y-direction");
        add_option("--fixed_size,-f", ps.fixed_size,
                   "Execute only one iteration with the given number of upper bound tiles");
        add_option("--timeout,-t", ps.timeout, "Timeout in seconds");
#if !defined(__APPLE__)
        add_option("--async,-a", ps.num_threads, "Number of threads to use for parallel solving (beta feature)");
        add_flag("--async_max", "Use the maximum number of threads available to the system (beta feature)");
#endif
        add_flag("--network,-n", "Re-synthesize the current logic network in store instead of the current truth table");
        add_flag("--and,-A", ps.enable_and, "Enable the use of AND gates");
        add_flag("--or,-O", ps.enable_or, "Enable the use of OR gates");
        add_flag("--not,-N", ps.enable_not, "Enable the use of NOT gates");
        add_flag("--maj,-M", ps.enable_maj, "Enable the use of MAJ gates");
        add_flag("--wires,-W", ps.enable_wires, "Enable the use of wire segments and fan-outs");
        add_flag("--crossings,-x", ps.crossings, "Enable wire crossings");
        //        add_flag("--io_pins,-i", ps.io_pins, "Use I/O port elements instead of gate pins");  // TODO this
        //        toggle does not work yet
    }

  protected:
    /**
     * Function to perform the physical design call.
     * Given arguments are parsed and a placed and routed FCN gate layout is generated if possible.
     */
    void execute() override
    {
        // if no gate types are specified, enable them all
        if (!ps.enable_and && !ps.enable_or && !ps.enable_not && !ps.enable_maj && !ps.enable_wires)
        {
            ps.enable_and   = true;
            ps.enable_or    = true;
            ps.enable_not   = true;
            ps.enable_maj   = true;
            ps.enable_wires = true;
        }

#if !defined(__APPLE__)
        // fetch number of threads available on the system
        if (this->is_set("async_max"))
        {
            if (auto threads_available = std::thread::hardware_concurrency(); threads_available == 0)
            {
                env->out() << "[w] could not detect the number of threads available to the system" << std::endl;
            }
            else
            {
                ps.num_threads = threads_available;
            }
        }
#endif

        if (this->is_set("network"))
        {
            auto& s = store<fiction::logic_network_t>();

            // error case: empty logic network store
            if (s.empty())
            {
                env->out() << "[w] no logic network in store" << std::endl;

                reset_flags();
                return;
            }

            const auto get_name = [](auto&& ntk_ptr) -> std::string { return ntk_ptr->get_network_name(); };

            const auto one_pass_with_ntk = [this](auto&& ntk_ptr)
            { return fiction::one_pass_synthesis<fiction::cart_gate_clk_lyt>(*ntk_ptr, ps, &st); };

            auto ntk = s.current();
            ps.name  = std::visit(get_name, ntk);

            try
            {
                auto lyt = std::visit(one_pass_with_ntk, ntk);

                if (lyt.has_value())
                {
                    store<fiction::gate_layout_t>().extend() = std::make_shared<fiction::cart_gate_clk_lyt>(*lyt);
                }
                else
                {
                    env->out() << fmt::format("[e] impossible to synthesize {} within the given parameters", ps.name)
                               << std::endl;
                }
            }
            catch (const fiction::unsupported_clocking_scheme_exception&)
            {
                env->out() << fmt::format("[e] \"{}\" does not refer to a supported clocking scheme or the selected "
                                          "clocking scheme is not supported by this approach",
                                          ps.scheme)
                           << std::endl;

                reset_flags();
                return;
            }
            catch (const std::bad_alloc&)
            {
                env->out() << fmt::format("[e] {} has too many inputs to store its truth table",
                                          std::visit(get_name, ntk))
                           << std::endl;
                return;
            }
        }
        else
        {
            auto& s = store<fiction::truth_table_t>();

            // error case: empty truth table store
            if (s.empty())
            {
                env->out() << "[w] no truth table in store" << std::endl;

                reset_flags();
                return;
            }

            auto tt = s.current();
            ps.name = kitty::to_hex(*tt);

            auto lyt = fiction::one_pass_synthesis<fiction::cart_gate_clk_lyt>(std::vector<fiction::tt>{*tt}, ps, &st);

            if (lyt.has_value())
            {
                store<fiction::gate_layout_t>().extend() = std::make_shared<fiction::cart_gate_clk_lyt>(*lyt);
            }
            else
            {
                env->out() << fmt::format("[e] impossible to synthesize {} within the given parameters", ps.name)
                           << std::endl;
            }
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
    fiction::one_pass_synthesis_params ps{};
    /**
     * Statistics.
     */
    fiction::one_pass_synthesis_stats st{};
    /**
     * Reset flags. Necessary due to an alice bug.
     */
    void reset_flags() noexcept
    {
        ps = {};
    }
};

ALICE_ADD_COMMAND(onepass, "Physical Design")

}  // namespace alice

#endif  // FICTION_CMD_ONEPASS_HPP

#endif  // MUGEN
