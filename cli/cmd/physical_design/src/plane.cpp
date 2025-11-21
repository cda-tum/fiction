//
// Created by benjamin on 21.11.25.
//

#include "cmd/physical_design/include/plane.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/algorithms/physical_design/orthogonal_planar.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/network_utils.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>

namespace alice
{

plane_command::plane_command(const environment::ptr& e) :
        command(e, "Performs fanout substitution, path balancing, planarization (node duplication), "
                   "and produces an orthogonal planar layout.")
{
    // fanout substitution options
    add_option("--degree,-d", fan_ps.degree, "Maximum number of outputs a fan-out node can have", true)
        ->set_type_name("{2, 3}");

    add_option("--strategy,-s", fan_ps.strategy,
               "Chain fan-outs in balanced tree (breadth=0), DFS tree (depth=1), or random (2)", true)
        ->set_type_name("{0,1,2}");

    add_option("--threshold,-t", fan_ps.threshold,
               "Maximum number of outputs any gate can have before substitution applies", true);

    add_option("--seed,-r", seed, "Random seed used for random fanout substitution.");

    // balancing option
    bal_ps.unify_outputs = true;

    // planarization option
    add_option("--po-order, -p", po_order, "PO order: keep=0, random=1")->set_type_name("{0,1}");
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

    if (is_set("seed"))
    {
        fan_ps.seed = seed;
    }

    using po_enum = decltype(dup_ps.po_order);
    switch (po_order)
    {
        case 0u: dup_ps.po_order = po_enum::KEEP_PO_ORDER; break;
        case 1u: dup_ps.po_order = po_enum::RANDOM_PO_ORDER; break;
        default:
            env->out() << "[w] invalid --po-order, defaulting to keep\n";
            dup_ps.po_order = po_enum::KEEP_PO_ORDER;
            break;
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
            fiction::orthogonal_planar<fiction::cart_gate_clk_lyt>(planarized_ntk, {}, &orth_stats));

        env->out() << fmt::format("[i] Layout generated: {} × {} | gates: {} | wires: {} | crossings: {}\n",
                                  orth_stats.x_size, orth_stats.y_size, orth_stats.num_gates, orth_stats.num_wires,
                                  orth_stats.num_crossings);
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

}  // namespace alice
