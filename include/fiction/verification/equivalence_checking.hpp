/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief SAT-based equivalence checking of a specification against an implementation.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/networks/extract_layout_network.hpp"
#include "fiction/networks/interface_matching.hpp"
#include "fiction/traits.hpp"
#include "fiction/verification/critical_path_length_and_throughput.hpp"
#include "fiction/verification/design_rule_violations.hpp"

#include <fmt/format.h>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace fiction::verification
{
/**
 * @brief Equivalence classification for logic and layout throughput.
 */
// NOLINTNEXTLINE(performance-enum-size): The public enum uses int in its C++ interface.
enum class eq_type
{
    /**
     * `Spec` and `Impl` differ logically, contain required topology defects, or either layout has DRVs.
     */
    NO,
    /**
     * `Spec` and `Impl` are logically equivalent and have different throughput denominators.
     */
    WEAK,
    /**
     * `Spec` and `Impl` are logically equivalent and have equal throughput denominators.
     */
    STRONG
};

/** @brief Physical equivalence result, throughput, and diagnostics. */
struct equivalence_checking_stats
{
    /**
     * Stores the equivalence type.
     */
    eq_type eq = eq_type::NO;
    /**
     * Throughput values at which weak equivalence manifests.
     */
    int64_t tp_spec = 1ll, tp_impl = 1ll, tp_diff = 0ll;
    /**
     * Stores a possible counter example.
     */
    std::vector<bool> counter_example{};
    /**
     * Stores the runtime.
     */
    mockturtle::stopwatch<>::duration runtime{0};
    /**
     * Stores DRVs.
     */
    fiction::verification::gate_level_drv_stats spec_drv_stats{}, impl_drv_stats{};
};

namespace detail
{

/** @brief Compares logical interfaces and physical layout timing. @tparam Spec Specification. @tparam Impl
 * Implementation. */
template <typename Spec, typename Impl>
class equivalence_checking_impl
{
  public:
    /**
     * Standard constructor.
     *
     * @param specification Logical specification of intended functionality.
     * @param implementation Implementation of specified functionality.
     * @param st Statistics.
     */
    explicit equivalence_checking_impl(const Spec& specification, const Impl& implementation,
                                       equivalence_checking_stats& st) :
            spec{specification},
            impl{implementation},
            pst{st}
    {}

    /** @brief Checks design rules, aligned logic, and throughput. @return Physical equivalence type. */
    eq_type run()
    {
        mockturtle::stopwatch stop{pst.runtime};

        if constexpr (is_gate_level_layout_v<Spec>)
        {
            if (has_drvs(spec, &pst.spec_drv_stats))
            {
                return eq_type::NO;
            }
        }
        if constexpr (is_gate_level_layout_v<Impl>)
        {
            if (has_drvs(impl, &pst.impl_drv_stats))
            {
                return eq_type::NO;
            }
        }

        if (spec.num_pis() != impl.num_pis() || spec.num_pos() != impl.num_pos())
        {
            return eq_type::NO;
        }
        try
        {
            return compare_logic();
        }
        catch (const std::invalid_argument&)
        {
            pst.eq = eq_type::NO;
            return pst.eq;
        }
    }

  private:
    /**
     * @brief Provides a logical network for a layout or an existing network.
     * @tparam NtkOrLyt Network or layout type.
     * @param source Comparison operand.
     * @return Extracted network value or const reference to the existing network.
     */
    template <typename NtkOrLyt>
    static decltype(auto) logical_network(const NtkOrLyt& source)
    {
        if constexpr (is_gate_level_layout_v<NtkOrLyt>)
        {
            return networks::extract_layout_network(source);
        }
        else
        {
            return (source);
        }
    }

    /** @brief Compares aligned logical interfaces and retains physical throughput. @return Equivalence type. */
    eq_type compare_logic()
    {
        const auto                                    matching = networks::match_interfaces(spec, impl);
        const auto&                                   spec_ntk = logical_network(spec);
        const auto&                                   impl_ntk = logical_network(impl);
        mockturtle::klut_network                      aligned{};
        std::vector<mockturtle::klut_network::signal> inputs(impl_ntk.num_pis());
        for (uint32_t input{}; input < spec_ntk.num_pis(); ++input)
        {
            inputs[matching.inputs[input]] = aligned.create_pi();
        }
        const auto outputs = mockturtle::cleanup_dangling(impl_ntk, aligned, inputs.begin(), inputs.end());
        for (const auto output : matching.outputs)
        {
            aligned.create_po(outputs[output]);
        }
        const auto miter = mockturtle::miter<mockturtle::klut_network>(spec_ntk, aligned);

        if (miter)
        {
            mockturtle::equivalence_checking_stats st;

            const auto eq = mockturtle::equivalence_checking(*miter, {}, &st);

            if (eq.has_value())
            {
                pst.eq = *eq ? eq_type::STRONG : eq_type::NO;

                if (pst.eq == eq_type::STRONG)
                {
                    // compute TP of specification
                    if constexpr (fiction::is_gate_level_layout_v<Spec>)
                    {
                        const auto cp_tp = fiction::verification::critical_path_length_and_throughput(spec);

                        pst.tp_spec = static_cast<int64_t>(cp_tp.throughput);
                    }
                    // compute TP of implementation
                    if constexpr (fiction::is_gate_level_layout_v<Impl>)
                    {
                        const auto cp_tp = fiction::verification::critical_path_length_and_throughput(impl);

                        pst.tp_impl = static_cast<int64_t>(cp_tp.throughput);
                    }

                    pst.tp_diff = std::abs(pst.tp_spec - pst.tp_impl);

                    if (pst.tp_diff != 0)
                    {
                        pst.eq = eq_type::WEAK;
                    }
                }

                if (!(*eq))
                {
                    pst.counter_example = st.counter_example;
                }
            }
            else
            {
                std::cout << "[e] resource limit exceeded" << '\n';

                return eq_type::NO;
            }
        }
        else
        {
            std::cout << "[w] both networks/layouts must have the same number of primary inputs and outputs" << '\n';

            return eq_type::NO;
        }

        return pst.eq;
    }

    /**
     * Specification.
     */
    const Spec& spec;
    /**
     * Implementation.
     */
    const Impl& impl;

    /** @brief Result statistics. */
    equivalence_checking_stats& pst;

    /** @brief Checks physical legality without printing a report. @tparam NtkOrLyt Layout type. @param ntk_or_lyt
     * Layout. @param stats DRV statistics. @return Whether a DRV exists. */
    template <typename NtkOrLyt>
    bool has_drvs(const NtkOrLyt& ntk_or_lyt, gate_level_drv_stats* stats) const
    {
        fiction::verification::gate_level_drv_params drv_ps{};

        // suppress DRV output
        std::ostringstream null_stream{};
        drv_ps.out = &null_stream;

        gate_level_drvs(ntk_or_lyt, drv_ps, stats);

        return stats->drvs != 0;
    }
};

}  // namespace detail

/**
 * Performs SAT-based equivalence checking between a specification of type `Spec` and an implementation of type `Impl`.
 * Each operand is a logic network or a placed gate-level layout. Layout logic is extracted before SAT checking.
 * Interfaces match by names unique on both sides, then by remaining declared positions. Unequal interface sizes,
 * missing required inputs, and required dependency cycles return `NO`.
 *
 * This implementation enables the comparison of two logic networks, a logic network and a gate-level layout or two
 * gate-level layouts. Since gate-level layouts have a notion of timing that logic networks do not, this function does
 * not simply prove logical equivalence but, additionally, takes timing aspects into account as well.
 *
 * Thereby, three different types of equivalences arise:
 *
 * - `NO` equivalence: Spec and Impl are not logically equivalent or one of them is a gate-level layout that contains
 * DRVs and, thus, cannot be checked for equivalence.
 * - `WEAK` equivalence: Spec and Impl are logically equivalent and have different throughput denominators.
 * - `STRONG` equivalence: Spec and Impl are logically equivalent and have equal throughput denominators.
 * Logic networks have throughput denominator one.
 *
 * This approach was first proposed in \"Verification for Field-coupled Nanocomputing Circuits\" by M. Walter, R. Wille,
 * F. Sill Torres, D. Große, and R. Drechsler in DAC 2020.
 *
 * @tparam Spec Specification type.
 * @tparam Impl Implementation type.
 * @param spec The specification.
 * @param impl The implementation.
 * @param pst Statistics.
 * @return The equivalence type of `spec` and `impl`.
 */
template <typename Spec, typename Impl>
eq_type equivalence_checking(const Spec& spec, const Impl& impl, equivalence_checking_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Spec> || is_gate_level_layout_v<Spec>,
                  "Spec is not a network or gate layout");
    static_assert(mockturtle::is_network_type_v<Impl> || is_gate_level_layout_v<Impl>,
                  "Impl is not a network or gate layout");

    equivalence_checking_stats        st{};
    detail::equivalence_checking_impl p{spec, impl, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::verification
