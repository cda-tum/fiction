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
 * @brief Writes files transactionally and checks stream failures.
 * @author OpenAI (Codex)
 */

#pragma once

#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace fiction::detail
{
/**
 * Serialize into a sibling temporary file and replace the destination after a successful close.
 * @tparam Writer Callable accepting an output stream.
 * @param filename Destination file.
 * @param writer Serialization callback.
 * @throws std::filesystem::filesystem_error If the destination cannot be replaced.
 * @throws std::ios_base::failure If writing, flushing, or closing fails.
 */
template <typename Writer>
void atomic_write(const std::string_view filename, const Writer& writer)
{
    const std::filesystem::path destination{filename};
    std::filesystem::path       directory{};
    for (auto attempt = 0u; attempt < 16u; ++attempt)
    {
        directory = destination.parent_path() / (".fiction-" + std::to_string(std::random_device{}()));
        if (std::filesystem::create_directory(directory))
        {
            break;
        }
        directory.clear();
    }
    if (directory.empty())
    {
        throw std::runtime_error("could not create a temporary output directory");
    }
    try
    {
        const auto    temporary = directory / destination.filename();
        std::ofstream stream{};
        stream.exceptions(std::ios::badbit | std::ios::failbit);
        stream.open(temporary, std::ios::out | std::ios::binary);
        writer(stream);
        stream.flush();
        stream.close();
        std::filesystem::rename(temporary, destination);
    }
    catch (...)
    {
        std::error_code ignored{};
        std::filesystem::remove_all(directory, ignored);
        throw;
    }
    std::error_code ignored{};
    std::filesystem::remove_all(directory, ignored);
}
}  // namespace fiction::detail
