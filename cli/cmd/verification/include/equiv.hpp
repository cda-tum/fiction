/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 04.03.20.
//

#pragma once

#include <fiction/verification/equivalence_checking.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

namespace alice
{

/**
 * Performs equality checks on logic networks; either between specification and gate layout or between two gate
 * layouts.
 */
class equiv_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit equiv_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the equivalence check call. Fetches the layout(s) and calls the checker.
     */
    void execute() override;

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the equivalence checking process.
     */
    nlohmann::json log() const override;

  private:
    fiction::verification::equivalence_checking_stats result{};

    template <typename NtkOrLytVariant1, typename NtkOrLytVariant2>
    void equivalence_checking(const NtkOrLytVariant1& ntk_or_lyt_variant1,
                              const NtkOrLytVariant2& ntk_or_lyt_variant_2);
};

}  // namespace alice
