//
// Created by marcel on 09.04.20.
//

#if (MUGEN)

#include "cmd/physical_design/include/onepass.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/physical_design/one_pass_synthesis.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <kitty/print.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <nlohmann/json.hpp>

#include <memory>
#include <new>
#include <string>
#include <variant>
#include <vector>

#ifndef __APPLE__
#include <thread>
#endif

namespace alice
{

onepass_command::onepass_command(const environment::ptr& e) :
        command(e, "SAT-driven topology-based logic re-synthesis, i.e., one-pass synthesis. Uses "
                   "Mugen by Winston Haaswijk to synthesize a specification in terms of a truth table or a "
                   "logic network onto a given clocking scheme. Gate types to be used can be specified. If "
                   "none are given, all are enabled, because synthesis without gates cannot work. Layouts "
                   "resulting from this approach might be desynchronized. I/Os are always located at the "
                   "layout's borders.")
{
    add_option("--clk_scheme,-s", ps.scheme, "Clocking scheme to use {2DDWAVE[3|4], USE, RES, ESR, CFE, SRS, BANCS}",
               true);
    add_option("--upper_x", ps.upper_bound_x, "Number of FCN gate tiles to use at maximum in x-direction");
    add_option("--upper_y", ps.upper_bound_y, "Number of FCN gate tiles to use at maximum in y-direction");
    add_option("--fixed_size,-f", ps.fixed_size,
               "Execute only one iteration with the given number of upper bound tiles");
    add_option("--timeout,-t", ps.timeout, "Timeout in seconds");
#ifndef __APPLE__
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

void onepass_command::execute()
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

#ifndef __APPLE__
    // fetch number of threads available on the system
    if (this->is_set("async_max"))
    {
        if (const auto threads_available = std::thread::hardware_concurrency(); threads_available == 0)
        {
            env->out() << "[w] could not detect the number of threads available to the system\n";
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
            env->out() << "[w] no logic network in store\n";

            reset_flags();
            return;
        }

        const auto get_name = [](auto&& ntk_ptr) -> std::string { return fiction::get_name(*ntk_ptr); };

        const auto one_pass_with_ntk = [this](auto&& ntk_ptr)
        { return fiction::one_pass_synthesis<fiction::cart_gate_clk_lyt>(*ntk_ptr, ps, &st); };

        const auto ntk = s.current();
        ps.name        = std::visit(get_name, ntk);

        try
        {
            const auto lyt = std::visit(one_pass_with_ntk, ntk);

            if (lyt.has_value())
            {
                store<fiction::gate_layout_t>().extend() = std::make_shared<fiction::cart_gate_clk_lyt>(*lyt);
            }
            else
            {
                env->out() << fmt::format("[e] impossible to synthesize {} within the given parameters\n", ps.name);
            }
        }
        catch (const fiction::unsupported_clocking_scheme_exception&)
        {
            env->out() << fmt::format("[e] \"{}\" does not refer to a supported clocking scheme or the selected "
                                      "clocking scheme is not supported by this approach\n",
                                      ps.scheme);

            reset_flags();
            return;
        }
        catch (const std::bad_alloc&)
        {
            env->out() << fmt::format("[e] {} has too many inputs to store its truth table\n",
                                      std::visit(get_name, ntk));
            reset_flags();
            return;
        }
    }
    else
    {
        auto& s = store<fiction::truth_table_t>();

        // error case: empty truth table store
        if (s.empty())
        {
            env->out() << "[w] no truth table in store\n";

            reset_flags();
            return;
        }

        try
        {
            const auto tt = s.current();
            ps.name       = kitty::to_hex(*tt);

            const auto lyt =
                fiction::one_pass_synthesis<fiction::cart_gate_clk_lyt>(std::vector<fiction::tt>{*tt}, ps, &st);

            if (lyt.has_value())
            {
                store<fiction::gate_layout_t>().extend() = std::make_shared<fiction::cart_gate_clk_lyt>(*lyt);
            }
            else
            {
                env->out() << fmt::format("[e] impossible to synthesize {} within the given parameters\n", ps.name);
            }
        }
        catch (const fiction::unsupported_clocking_scheme_exception&)
        {
            env->out() << fmt::format("[e] \"{}\" does not refer to a supported clocking scheme or the selected "
                                      "clocking scheme is not supported by this approach\n",
                                      ps.scheme);
            reset_flags();
            return;
        }
    }

    reset_flags();
}

nlohmann::json onepass_command::log() const
{
    return nlohmann::json{{"runtime in seconds", mockturtle::to_seconds(st.time_total)},
                          {"number of gates", st.num_gates},
                          {"number of wires", st.num_wires},
                          {"number of crossings", st.num_crossings},
                          {"layout", {{"x-size", st.x_size}, {"y-size", st.y_size}, {"area", st.x_size * st.y_size}}}};
}

void onepass_command::reset_flags() noexcept
{
    ps = {};
}

}  // namespace alice

#endif  // MUGEN
