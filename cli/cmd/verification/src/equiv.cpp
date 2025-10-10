//
// Created by marcel on 04.03.20.
//

#include "cmd/verification/include/equiv.hpp"

#include <fiction/algorithms/verification/equivalence_checking.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <string>
#include <variant>

namespace alice
{

equiv_command::equiv_command(const environment::ptr& e) :
        command(e, "Performs logical and delay equivalence checks of gate layouts against a specification "
                   "that can either be its own associated logic network or another gate layout.")
{
    add_flag("--logic_network,-n", "Perform equivalence checking with a logic network");
    add_flag("--gate_layout,-g", "Perform equivalence checking with a gate-level layout");
}

void equiv_command::execute()
{
    result = {};

    if (is_set("logic_network") && is_set("gate_layout"))
    {
        auto& lns = store<fiction::logic_network_t>();

        if (lns.empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;
            return;
        }

        auto& gls = store<fiction::gate_layout_t>();

        if (gls.empty())
        {
            env->out() << "[w] no gate layout in store" << std::endl;
            return;
        }

        equivalence_checking(lns.current(), gls.current());
    }
    else if (is_set("logic_network"))
    {
        auto& lns = store<fiction::logic_network_t>();

        if (lns.size() < 2)
        {
            env->out() << "[w] need at least two logic networks in store" << std::endl;
            return;
        }

        equivalence_checking(lns[lns.size() - 1], lns[lns.size() - 2]);
    }
    else if (is_set("gate_layout"))
    {
        auto& gls = store<fiction::gate_layout_t>();

        if (gls.size() < 2)
        {
            env->out() << "[w] need at least two gate layouts in store" << std::endl;
            return;
        }

        equivalence_checking(gls[gls.size() - 1], gls[gls.size() - 2]);
    }
    else
    {
        env->out() << "[w] at least one store must be specified" << std::endl;
    }
}

nlohmann::json equiv_command::log() const
{
    return nlohmann::json{{"equivalence type", result.eq == fiction::eq_type::NO   ? "NOT EQ" :
                                               result.eq == fiction::eq_type::WEAK ? "WEAK" :
                                                                                     "STRONG"},
                          {"counter example", result.counter_example},
                          {"specification's throughput", result.tp_spec},
                          {"implementation's throughput", result.tp_impl},
                          {"throughput difference", result.tp_diff},
                          {"runtime (s)", mockturtle::to_seconds(result.runtime)}};
}

template <typename NtkOrLytVariant1, typename NtkOrLytVariant2>
void equiv_command::equivalence_checking(const NtkOrLytVariant1& ntk_or_lyt_variant1,
                                         const NtkOrLytVariant2& ntk_or_lyt_variant_2)
{
    const auto equiv_check = [this](auto&& ntk_or_lyt_ptr1, auto&& ntk_or_lyt_ptr2)
    { fiction::equivalence_checking(*ntk_or_lyt_ptr1, *ntk_or_lyt_ptr2, &result); };

    const auto get_name = [](auto&& ntk_or_lyt_ptr) -> std::string { return fiction::get_name(*ntk_or_lyt_ptr); };

    std::visit(equiv_check, ntk_or_lyt_variant1, ntk_or_lyt_variant_2);

    if (result.spec_drv_stats.drvs > 0)
    {
        env->out() << "[e] the layout given as specification has DRVs and, thus, cannot be checked for equivalence"
                   << std::endl;
    }
    else if (result.impl_drv_stats.drvs > 0)
    {
        env->out() << "[e] the layout given as implementation has DRVs and, thus, cannot be checked for equivalence"
                   << std::endl;
    }
    else
    {
        env->out() << fmt::format("[i] {} and {} are {} equivalent{}", std::visit(get_name, ntk_or_lyt_variant1),
                                  std::visit(get_name, ntk_or_lyt_variant_2),
                                  result.eq == fiction::eq_type::NO   ? "NOT" :
                                  result.eq == fiction::eq_type::WEAK ? "WEAKLY" :
                                                                        "STRONGLY",
                                  result.eq == fiction::eq_type::WEAK ?
                                      fmt::format(" with a delay difference of {} clock cycles", result.tp_diff) :
                                      "")
                   << std::endl;
    }
}

}  // namespace alice
