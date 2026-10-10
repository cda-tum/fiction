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
 * @brief Cell writers propagate terminal-list allocation failures.
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/allocation_failure.hpp"

#include <fiction/technology/fcn/io/write_qll_layout.hpp>
#include <fiction/technology/inml/io/write_qcc_layout.hpp>
#include <fiction/technology/inml/layout.hpp>

#include <array>
#include <new>
#include <sstream>
#include <string>

using namespace fiction;
using namespace fiction::fcn::io;
using namespace fiction::inml;
using namespace fiction::inml::io;
using namespace fiction::test;

/** @brief QCC and QLL propagate allocation failures while preparing either terminal list. */
TEST_CASE("Cell writers propagate terminal-list allocation failures", "[cell-writer-allocation]")
{
    require_allocation_failure_support();
    for (const bool qcc : std::array{true, false})
    {
        for (const auto type : std::array{magnet_type::INPUT, magnet_type::OUTPUT})
        {
            DYNAMIC_SECTION((qcc ? "QCC" : "QLL") << " " << (type == magnet_type::INPUT ? "PI" : "PO"))
            {
                /** @brief Single border terminal accepted by either format. */
                layout lyt{{1, 1}};
                lyt.assign_cell_type({0, 0}, type);
                lyt.assign_cell_name({0, 0}, "terminal");
                /** @brief Destination prepared before fault injection. */
                std::ostringstream stream{};
                /** @brief Records an allocation failure propagated through the public writer. */
                bool propagated{};
                try
                {
                    allocation_budget = 0;
                    if (qcc)
                    {
                        write_qcc_layout(lyt, stream);
                    }
                    else
                    {
                        write_qll_layout(lyt, stream);
                    }
                    allocation_budget.reset();
                }
                catch (const std::bad_alloc&)
                {
                    allocation_budget.reset();
                    propagated = true;
                }
                catch (...)
                {
                    allocation_budget.reset();
                    throw;
                }
                CHECK(propagated);

                if (qcc)
                {
                    CHECK_NOTHROW(write_qcc_layout(lyt, stream));
                }
                else
                {
                    CHECK_NOTHROW(write_qll_layout(lyt, stream));
                }
                CHECK(stream.str().find("name=\"terminal\"") != std::string::npos);
            }
        }
    }
}
