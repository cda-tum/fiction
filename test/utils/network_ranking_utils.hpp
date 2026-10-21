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
 * @brief A test helper that balances and ranks a network for the planarization tests.
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/network_balancing.hpp>

/**
 * Balances the given network with unified outputs, leaves constant outputs unbuffered, and ranks it, without
 * substituting fanouts.
 *
 * @tparam Ntk Network type.
 * @param ntk Network to balance and rank.
 * @return The ranked, balanced technology network.
 */
template <typename Ntk>
fiction::networks::views::mutable_rank_view<fiction::networks::technology_network> rank(const Ntk& ntk)
{
    fiction::synthesis::network_balancing_params ps{};
    ps.unify_outputs           = true;
    ps.buffer_constant_outputs = false;

    return fiction::networks::views::mutable_rank_view{
        fiction::synthesis::network_balancing<fiction::networks::technology_network>(ntk, ps)};
}
