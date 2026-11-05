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
 * @brief Exclusively creates and cleans a temporary test directory.
 */

#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <system_error>

namespace fiction::test
{

/** @brief Owns an exclusively created directory until destruction. */
class scoped_temp_directory
{
  public:
    /**
     * Creates a directory with a bounded collision retry.
     * @param root Existing parent directory.
     * @param prefix Candidate name prefix.
     * @throws std::filesystem::filesystem_error If creation fails for a reason other than a collision.
     * @throws std::runtime_error If all candidate names are occupied.
     */
    scoped_temp_directory(const std::filesystem::path& root, const std::string& prefix)
    {
        for (auto attempt = 0; attempt < 16; ++attempt)
        {
            directory = root / (prefix + "-" + std::to_string(attempt));
            std::error_code error{};
            if (std::filesystem::create_directory(directory, error))
            {
                return;
            }
            if (error && error != std::errc::file_exists)
            {
                throw std::filesystem::filesystem_error{"Cannot create temporary directory", directory, error};
            }
        }
        throw std::runtime_error{"Cannot create temporary directory: candidates occupied"};
    }
    /** @brief Directory ownership cannot be copied. */
    scoped_temp_directory(const scoped_temp_directory&) = delete;
    /** @brief Directory ownership cannot be copy-assigned. */
    scoped_temp_directory& operator=(const scoped_temp_directory&) = delete;
    /** @brief Directory ownership cannot be moved. */
    scoped_temp_directory(scoped_temp_directory&&) = delete;
    /** @brief Directory ownership cannot be move-assigned. */
    scoped_temp_directory& operator=(scoped_temp_directory&&) = delete;
    /** @brief Removes the directory and its contents without throwing. */
    ~scoped_temp_directory()
    {
        std::error_code ignored{};
        std::filesystem::remove_all(directory, ignored);
    }
    /** @brief Returns the owned directory. @return Directory path. */
    [[nodiscard]] const std::filesystem::path& path() const noexcept
    {
        return directory;
    }

  private:
    /** @brief Exclusively created directory. */
    std::filesystem::path directory{};
};

}  // namespace fiction::test
