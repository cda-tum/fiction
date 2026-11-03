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
 * @brief Allocation failures in cell writers and gate-orientation diagnostics.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/allocation_failure.hpp"

#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/fcn/io/write_qll_layout.hpp>
#include <fiction/technology/inml/io/write_qcc_layout.hpp>
#include <fiction/technology/inml/layout.hpp>

#include <array>
#include <new>
#include <sstream>
#include <string>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::fcn::io;
using namespace fiction::inml;
using namespace fiction::inml::io;
using namespace fiction::test;

/** @brief Copying gate-orientation diagnostics propagates allocation failure and preserves the source. */
TEST_CASE("Gate-orientation diagnostics propagate port-copy allocation failures", "[fcn-gate-library]")
{
    require_allocation_failure_support();
    /** @brief Coordinate and port payload of an unsupported gate orientation. */
    using orientation_exception = unsupported_gate_orientation_exception<layout::coordinate, port_position>;
    /** @brief Independently supplied gate coordinate. */
    const layout::coordinate coordinate{1, 2};
    /** @brief Nonempty input and output lists with distinct terminal flags. */
    const port_list<port_position> ports{{port_position{0u, 0u, true, false}}, {port_position{3u, 2u, false, true}}};
    /** @brief Diagnostic prepared before arming allocation failure. */
    const orientation_exception error{coordinate, ports};

    SECTION("constructor")
    {
        /** @brief Whether the payload-copy failure reaches its caller. */
        bool propagated{};
        try
        {
            allocation_budget = 0;
            const orientation_exception copy{coordinate, ports};
            allocation_budget.reset();
            CHECK(copy.where() == coordinate);
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
    }
    SECTION("accessor")
    {
        /** @brief Whether the returned port-copy failure reaches its caller. */
        bool propagated{};
        try
        {
            allocation_budget = 0;
            const auto copy   = error.which_ports();
            allocation_budget.reset();
            CHECK(copy == ports);
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
    }
    /** @brief A fresh diagnostic proves recovery after the one-shot failure. */
    const orientation_exception recovered_error{coordinate, ports};
    /** @brief Complete port payload returned after recovery. */
    const auto recovered = recovered_error.which_ports();
    CHECK(error.where() == coordinate);
    CHECK(recovered_error.where() == coordinate);
    CHECK(error.which_ports() == ports);
    CHECK(recovered == ports);
    REQUIRE(recovered.inp.size() == 1);
    REQUIRE(recovered.out.size() == 1);
    CHECK(recovered.inp.begin()->pi);
    CHECK_FALSE(recovered.inp.begin()->po);
    CHECK_FALSE(recovered.out.begin()->pi);
    CHECK(recovered.out.begin()->po);
}

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
                // Warm formatting and stream state before arming terminal-list allocation failure.
                if (qcc)
                {
                    write_qcc_layout(lyt, stream);
                }
                else
                {
                    write_qll_layout(lyt, stream);
                }
                /** @brief Complete output used to check recovery after the injected failure. */
                const auto expected = stream.str();
                CHECK(expected.find("name=\"terminal\"") != std::string::npos);
                stream.str(std::string{});
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
                CHECK(stream.str().empty());

                if (qcc)
                {
                    CHECK_NOTHROW(write_qcc_layout(lyt, stream));
                }
                else
                {
                    CHECK_NOTHROW(write_qll_layout(lyt, stream));
                }
                CHECK(stream.str() == expected);
            }
        }
    }
}
