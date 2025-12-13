//
// Created by benjamin on 21.11.25.
//

#include "cmd/physical_design/include/plane.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>
#include <fiction/algorithms/physical_design/planar_layout_from_network_embedding.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/network_utils.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>
#include <nlohmann/json.hpp>

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>

namespace alice
{

plane_command::plane_command(const environment::ptr& e) :
        command(e, "First, a planar embedding is obtained by performing fanout substitution, path balancing, and "
                   "planarization (node duplication). Then, scalable placement and routing are carried out on the "
                   "planar layout of the current logic network in store. Compared to the gold algorithm, the "
                   "resulting layout may occupy more area, but it is generated in reasonable runtime.")
{
    // fanout substitution options
    fan_ps.degree = 2;
    // balancing option
    bal_ps.unify_outputs = true;
    // planarization option
    add_option("--po-order, -p", po_order, "PO order: keep=0, random=1")->set_type_name("{0,1}");
    // plane options
    add_option("--clock_numbers,-n", num_clock_phases, "Number of clock phases to be used {3 or 4}");
    add_flag("--verbose,-v", ps.verbose, "Be verbose");
}

void plane_command::execute()
{
    auto& ns = store<fiction::logic_network_t>();
    auto& ls = store<fiction::gate_layout_t>();

    if (ns.empty())
    {
        env->out() << "[w] no logic network in store\n";
        return;
    }
    // error case: phases out of range
    if (num_clock_phases != 3u && num_clock_phases != 4u)
    {
        env->out() << "[e] only 3- and 4-phase clocking schemes are supported\n";
        ps = {};
        return;
    }

    ps.number_of_clock_phases = num_clock_phases == 3 ? fiction::num_clks::THREE : fiction::num_clks::FOUR;

    using po_enum = decltype(dup_ps.po_order);
    switch (po_order)
    {
        case 0u:
        {
            dup_ps.po_order = po_enum::KEEP_PO_ORDER;
            break;
        }
        case 1u:
        {
            dup_ps.po_order = po_enum::RANDOM_PO_ORDER;
            break;
        }
        default:
        {
            env->out() << "[w] invalid --po-order, defaulting to keep\n";
            dup_ps.po_order = po_enum::KEEP_PO_ORDER;
            break;
        }
    }

    const auto perform_fanouts_and_balance = [this](auto&& ntk_ptr)
    {
        auto tec_f = fiction::fanout_substitution<fiction::tec_nt>(*ntk_ptr, fan_ps);
        return std::make_shared<fiction::tec_nt>(fiction::network_balancing<fiction::tec_nt>(tec_f, bal_ps));
    };

    auto tec_b = std::visit(perform_fanouts_and_balance, ns.current());

    const fiction::mutable_rank_view vpi_r(*tec_b);

    auto planarized_ntk = fiction::node_duplication_planarization(vpi_r, dup_ps);

    try
    {
        ls.extend() = std::make_shared<fiction::cart_gate_clk_lyt>(
            fiction::plane<fiction::cart_gate_clk_lyt>(planarized_ntk, ps, &st));
    }
    catch (const fiction::high_degree_fanin_exception& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (const std::invalid_argument& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (const std::exception& e)
    {
        env->out() << fmt::format("[e] unexpected error: {}\n", e.what());
    }

    fan_ps = {};
    bal_ps = {};
    dup_ps = {};
}

nlohmann::json plane_command::log() const
{
    return nlohmann::json{{"runtime in seconds", mockturtle::to_seconds(st.time_total)},
                          {"number of gates", st.num_gates},
                          {"number of wires", st.num_wires},
                          {"number of crossings", st.num_crossings},
                          {"layout", {{"x-size", st.x_size}, {"y-size", st.y_size}, {"area", st.x_size * st.y_size}}}};
}

}  // namespace alice
