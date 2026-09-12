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
#include <system_error>

#ifndef _WIN32
#include <sys/stat.h>
#endif

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
#ifndef _WIN32
    const auto permissions = std::filesystem::perms::owner_read | std::filesystem::perms::owner_write;
    std::filesystem::permissions(destination, permissions);
#endif

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
#ifndef _WIN32
    CHECK(std::filesystem::status(destination).permissions() == permissions);
#else
    std::filesystem::permissions(destination, std::filesystem::perms::owner_read);
    CHECK_THROWS_AS(detail::atomic_write(destination.string(), [](std::ostream& stream) { stream << "replacement"; }),
                    std::ios_base::failure);
    CHECK(std::distance(std::filesystem::directory_iterator{directory}, std::filesystem::directory_iterator{}) == 1);
    std::filesystem::permissions(destination, std::filesystem::perms::owner_all);
#endif
    std::filesystem::remove_all(directory);
}

TEST_CASE("Transactional output preserves symbolic links", "[atomic-write]")
{
    const auto directory =
        std::filesystem::temp_directory_path() / ("fiction-symlink-test-" + std::to_string(std::random_device{}()));
    REQUIRE(std::filesystem::create_directory(directory));
    const auto destination = directory / "output.svg";
    const auto link        = directory / "latest.svg";
    detail::atomic_write(destination.string(), [](std::ostream& stream) { stream << "original"; });
    std::error_code link_error{};
    std::filesystem::create_symlink(destination.filename(), link, link_error);
#ifdef _WIN32
    if (link_error.value() == 1314)  // Windows requires symbolic-link privileges unless Developer Mode is enabled.
    {
        std::filesystem::remove_all(directory);
        SKIP("Windows did not grant symbolic-link creation privileges");
    }
#endif
    REQUIRE_FALSE(link_error);
    CHECK_THROWS_AS(detail::atomic_write(link.string(),
                                         [](std::ostream& stream)
                                         {
                                             stream << "partial";
                                             throw std::runtime_error("serialization failed");
                                         }),
                    std::runtime_error);
    {
        std::ifstream     stream{destination};
        const std::string content{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
        CHECK(content == "original");
    }
    detail::atomic_write(link.string(), [](std::ostream& stream) { stream << "complete"; });
    CHECK(std::filesystem::is_symlink(link));
    CHECK(std::filesystem::read_symlink(link) == destination.filename());
    {
        std::ifstream     stream{destination};
        const std::string content{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
        CHECK(content == "complete");
    }
    std::filesystem::remove(destination);
    CHECK_THROWS_AS(detail::atomic_write(link.string(), [](std::ostream& stream) { stream << "new"; }),
                    std::ios_base::failure);
    CHECK(std::filesystem::is_symlink(link));
    CHECK_FALSE(std::filesystem::exists(destination));
    std::filesystem::remove_all(directory);
}

#ifndef _WIN32
TEST_CASE("Transactional output rejects named pipes", "[atomic-write]")
{
    const auto destination =
        std::filesystem::temp_directory_path() / ("fiction-fifo-test-" + std::to_string(std::random_device{}()));
    REQUIRE(::mkfifo(destination.c_str(), 0600) == 0);
    CHECK_THROWS_AS(detail::atomic_write(destination.string(), [](std::ostream& stream) { stream << "content"; }),
                    std::ios_base::failure);
    CHECK(std::filesystem::is_fifo(destination));
    std::filesystem::remove(destination);
}
#endif
