//
// Created by marcel on 04.03.20.
//

#ifndef FICTION_CMD_EQUIV_HPP
#define FICTION_CMD_EQUIV_HPP

#include <fiction/algorithms/design_rule_violations.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <sstream>
#include <variant>

namespace alice
{

/**
 * Performs equality checks on logic networks; either between specification and gate layout or between two gate
 * layouts.
 */
class equiv_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit equiv_command(const environment::ptr& e) :
            command(e, "Performs logical and delay equivalence checks of gate layouts against a specification "
                       "that can either be its own associated logic network or another gate layout.")
    {
        add_flag("--logic_network,-n", "Perform equivalence checking with a logic network");
        add_flag("--gate_layout,-g", "Perform equivalence checking with a gate-level layout");
    }

  protected:
    /**
     * Function to perform the equivalence check call. Fetches the layout(s) and calls the checker.
     */
    void execute() override
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

            if (const auto& lyt = gls.current(); has_no_drvs(lyt))
            {
                equivalence_checking(lns.current(), lyt);
            }
            else
            {
                env->out() << "[e] specified layout has DRVs and, thus, cannot be checked for equivalence" << std::endl;
            }
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

            if (const auto& lyt1 = gls[gls.size() - 1], lyt2 = gls[gls.size() - 2];
                has_no_drvs(lyt1) && has_no_drvs(lyt2))
            {
                equivalence_checking(lyt1, lyt2);
            }
            else
            {
                env->out() << "[e] specified layout has DRVs and, thus, cannot be checked for equivalence" << std::endl;
            }
        }
        else
        {
            env->out() << "[w] at least one store must be specified" << std::endl;
        }

        //
        //        env->out() << "[i] the layout is "
        //                   << (result.eq == equivalence_checker::equiv_result::eq_type::NONE ? "NOT" :
        //                       result.eq == equivalence_checker::equiv_result::eq_type::WEAK ? "WEAKLY" :
        //                                                                                       "STRONGLY")
        //                   << " equivalent to its specification";
        //
        //        if (result.eq == equivalence_checker::equiv_result::eq_type::NONE && !result.counter_example.empty())
        //        {
        //            env->out() << " with counter example ";
        //            for (const auto c : result.counter_example) env->out() << c;
        //        }
        //        else if (result.eq == equivalence_checker::equiv_result::eq_type::WEAK)
        //        {
        //            if (!is_set("gate_layout"))
        //                env->out() << " with a delay of " << result.delay << " cycles";
        //        }
        //        env->out() << std::endl;
    }

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the equivalence checking process.
     */
    nlohmann::json log() const override
    {
        return nlohmann::json{{"equivalence type", result.eq == equiv_result::eq_type::NO   ? "NOT EQ" :
                                                   result.eq == equiv_result::eq_type::WEAK ? "WEAK" :
                                                                                              "STRONG"},
                              {"counter example", result.counter_example},
                              {"delay", result.delay},
                              {"runtime (s)", mockturtle::to_seconds(result.runtime)}};
    }

  private:
    /**
     * Encapsulates the equivalence call information.
     */
    struct equiv_result
    {
        /**
         * The different equivalence types possible. See above for more information.
         */
        enum class eq_type
        {
            NO,
            WEAK,
            STRONG
        };
        /**
         * Stores the equivalence type.
         */
        eq_type eq;
        /**
         * Delay value at which weak equivalence manifests.
         */
        std::size_t delay = 0ul;
        /**
         * Stores a possible counter example.
         */
        std::vector<bool> counter_example{};
        /**
         * Stores the runtime.
         */
        mockturtle::stopwatch<>::duration runtime{0};
    };

    equiv_result result{};

    template <typename LytVariant>
    bool has_no_drvs(const LytVariant& lyt_variant) const noexcept
    {
        const auto num_drvs = [](auto&& lyt_ptr) -> uint64_t
        {
            fiction::gate_level_drv_stats  st{};
            fiction::gate_level_drv_params ps{};

            // suppress DRV output
            std::ostringstream null_stream{};
            ps.out = &null_stream;

            gate_level_drvs(*lyt_ptr, ps, &st);

            return st.drvs;
        };

        return std::visit(num_drvs, lyt_variant) == 0;
    }

    template <typename NtkOrLytVariant1, typename NtkOrLytVariant2>
    void equivalence_checking(const NtkOrLytVariant1& ntk_or_lyt_variant1, const NtkOrLytVariant2& ntk_or_lyt_variant_2)
    {
        const auto equiv_check = [this](auto&& ntk_or_lyt_ptr1, auto&& ntk_or_lyt_ptr2) -> bool
        {
            mockturtle::stopwatch stop{result.runtime};

            auto miter = mockturtle::miter<mockturtle::klut_network>(*ntk_or_lyt_ptr1, *ntk_or_lyt_ptr2);

            if (miter)
            {
                mockturtle::equivalence_checking_stats st;

                auto eq = mockturtle::equivalence_checking(*miter, {}, &st);

                if (eq.has_value())
                {
                    result.eq = *eq ? equiv_result::eq_type::STRONG : equiv_result::eq_type::NO;

                    if (!(*eq))
                        result.counter_example = st.counter_example;
                }
                else
                {
                    env->out() << "[e] resource limit exceeded" << std::endl;
                    return false;
                }
            }
            else
            {
                env->out() << "[w] both networks/layouts must have the same number of primary inputs and outputs"
                           << std::endl;
                return false;
            }

            return true;
        };

        const auto get_name = [](auto&& ntk_or_lyt_ptr) { return ntk_or_lyt_ptr->get_network_name(); };

        bool success = std::visit(equiv_check, ntk_or_lyt_variant1, ntk_or_lyt_variant_2);

        if (success)
        {
            env->out() << fmt::format("[i] {} and {} are{} equivalent", std::visit(get_name, ntk_or_lyt_variant1),
                                      std::visit(get_name, ntk_or_lyt_variant_2),
                                      result.eq == equiv_result::eq_type::NO ? " NOT" : "")
                       << std::endl;
        }
    }
};

ALICE_ADD_COMMAND(equiv, "Verification")

}  // namespace alice

#endif  // FICTION_CMD_EQUIV_HPP
