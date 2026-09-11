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
 * @brief Tests transactional file replacement and serialization failure handling.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/utils/atomic_write.hpp>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <ostream>
#include <random>
#include <stdexcept>
#include <string>

using namespace fiction;

TEST_CASE("Failed serialization preserves the destination", "[atomic-write]")
{
    const auto directory =
        std::filesystem::temp_directory_path() / ("fiction-write-test-" + std::to_string(std::random_device{}()));
    REQUIRE(std::filesystem::create_directory(directory));
    const auto destination = directory / "output.fgl";
    CHECK_THROWS_AS(detail::atomic_write((directory / "missing" / "output.fgl").string(),
                                         [](std::ostream& stream) { stream << "content"; }),
                    std::ios_base::failure);
    CHECK_THROWS_AS(detail::atomic_write(directory.string(), [](std::ostream& stream) { stream << "content"; }),
                    std::ios_base::failure);
    detail::atomic_write(destination.string(), [](std::ostream& stream) { stream << "original"; });

    CHECK_THROWS_AS(detail::atomic_write(destination.string(),
                                         [](std::ostream& stream)
                                         {
                                             stream << "partial";
                                             throw std::runtime_error("serialization failed");
                                         }),
                    std::runtime_error);
    CHECK_THROWS_AS(detail::atomic_write(destination.string(),
                                         [](std::ostream& stream)
                                         {
                                             stream << "partial";
                                             stream.setstate(std::ios::badbit);
                                         }),
                    std::ios_base::failure);
    {
        std::ifstream     stream{destination};
        const std::string content{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
        CHECK(content == "original");
    }
    detail::atomic_write(destination.string(), [](std::ostream& stream) { stream << "complete"; });
    {
        std::ifstream     stream{destination};
        const std::string content{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
        CHECK(content == "complete");
    }
    std::filesystem::remove_all(directory);
}
