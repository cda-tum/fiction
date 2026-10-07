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
 * @brief Checks SAT equivalence through an installed fiction package.
 * @author Marcel Walter (marcelwa)
 */

#include <fiction/verification/equivalence_checking.hpp>

#include <mockturtle/networks/aig.hpp>

using namespace fiction;
using namespace fiction::verification;

/**
 * @brief Returns success when equivalent and distinct networks produce the expected results.
 * @return Zero on success.
 */
int main()
{
    mockturtle::aig_network spec{};
    const auto              a = spec.create_pi();
    const auto              b = spec.create_pi();
    spec.create_po(spec.create_and(a, b));

    mockturtle::aig_network impl{};
    const auto              c = impl.create_pi();
    const auto              d = impl.create_pi();
    impl.create_po(impl.create_and(d, c));

    if (equivalence_checking(spec, impl) != eq_type::STRONG)
    {
        return 1;
    }

    mockturtle::aig_network distinct{};
    const auto              e = distinct.create_pi();
    const auto              f = distinct.create_pi();
    distinct.create_po(distinct.create_or(e, f));
    return equivalence_checking(spec, distinct) == eq_type::NO ? 0 : 2;
}
