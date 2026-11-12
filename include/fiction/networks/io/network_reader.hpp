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
 * @brief Reads combinational logic networks from files and directories.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/networks/io/detail/network_reader_parsers.hpp"

#include <fmt/format.h>
#include <lorina/common.hpp>
#include <lorina/diagnostics.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace fiction::networks::io
{

/** @brief Failure to read a network or list a network directory. */
class network_read_error : public std::runtime_error
{
  public:
    /** @brief Construct an error with the input path and failure reason. */
    using std::runtime_error::runtime_error;
};

/** @brief A file that could not be read during a directory scan. */
struct network_read_failure
{
    /** @brief Path to the failed file. */
    std::filesystem::path path;
    /** @brief Diagnostic with the failure reason. */
    std::string message;
};

/**
 * @brief Results from reading a directory. Each vector follows filename order.
 * @tparam Ntk Logic network type.
 */
template <typename Ntk>
struct network_read_batch
{
    /** @brief Successfully parsed networks. */
    std::vector<Ntk> networks;
    /** @brief Failed files. */
    std::vector<network_read_failure> failures;
};

namespace detail
{

/** @brief Represent a native path as UTF-8 for names and diagnostics. */
[[nodiscard]] inline std::string network_path_text(const std::filesystem::path& path)
{
    const auto utf8 = path.u8string();
    return {utf8.begin(), utf8.end()};
}

/** @brief Report a read failure with the source path. */
[[noreturn]] inline void fail_network_read(const std::filesystem::path& path, const std::string_view reason)
{
    throw network_read_error{fmt::format("could not parse '{}': {}", network_path_text(path), reason)};
}

/** @brief Resolve a format token or extension using ASCII case-insensitive matching. */
[[nodiscard]] inline network_format parse_network_format(std::string_view format)
{
    if (format.starts_with('.'))
    {
        format.remove_prefix(1);
    }
    const auto matches = [format](const std::string_view expected)
    {
        return std::ranges::equal(format, expected, [](const char input, const char lower)
                                  { return input == lower || input == lower - ('a' - 'A'); });
    };
    if (matches("v"))
    {
        return network_format::VERILOG;
    }
    if (matches("aig"))
    {
        return network_format::AIGER;
    }
    if (matches("blif"))
    {
        return network_format::BLIF;
    }
    return network_format::UNKNOWN;
}

/** @brief Validate an explicit format override before accessing the filesystem. */
[[nodiscard]] inline network_format requested_network_format(const std::filesystem::path& path,
                                                             const std::string_view       format)
{
    if (path.native().find(std::filesystem::path::value_type{}) != std::filesystem::path::string_type::npos)
    {
        throw network_read_error{"could not parse a path containing a null character"};
    }
    const auto parsed = parse_network_format(format);
    if (!format.empty() && parsed == network_format::UNKNOWN)
    {
        fail_network_read(path, fmt::format("unsupported network format '{}'", format));
    }
    return parsed;
}

/** @brief Resolve a candidate's format from its extension when the caller did not specify one. */
[[nodiscard]] inline network_format file_network_format(const std::filesystem::path& path, const network_format format)
{
    return format == network_format::UNKNOWN ? parse_network_format(network_path_text(path.extension())) : format;
}

/** @brief A directory candidate and the information needed to read it. */
struct network_candidate
{
    /** @brief Candidate path. */
    std::filesystem::path path;
    /** @brief Resolved input format. */
    network_format format;
    /** @brief Error from checking the candidate's filesystem status, if any. */
    std::error_code status_error;
};

/** @brief List supported regular files in filename order. */
[[nodiscard]] inline std::vector<network_candidate> network_candidates(const std::filesystem::path& directory,
                                                                       const network_format         format)
{
    std::vector<network_candidate> candidates{};
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator{directory})
        {
            const auto resolved = file_network_format(entry.path(), format);
            if (resolved == network_format::UNKNOWN)
            {
                continue;
            }

            std::error_code error{};
            if (entry.is_regular_file(error) || (error && error != std::errc::no_such_file_or_directory))
            {
                candidates.push_back({.path = entry.path(), .format = resolved, .status_error = error});
            }
        }
    }
    catch (const std::filesystem::filesystem_error& error)
    {
        fail_network_read(directory, error.code().message());
    }
    std::ranges::sort(candidates, {}, &network_candidate::path);
    return candidates;
}

/** @brief Collect Lorina diagnostics without writing to the terminal. */
class network_diagnostics : public lorina::diagnostic_consumer
{
  public:
    /** @brief Append a diagnostic for a failed parse. */
    void handle_diagnostic(const lorina::diagnostic_level level, const std::string& message) const override
    {
        if (level != lorina::diagnostic_level::ignore)
        {
            if (!messages.empty())
            {
                messages += '\n';
            }
            messages += message;
        }
    }

    /** @brief Diagnostics in emission order. */
    mutable std::string messages{};
};

/** @brief Parse and clean up one candidate before exposing its network. */
template <typename Ntk>
[[nodiscard]] Ntk read_network_file(const network_candidate& candidate)
{
    const auto& path = candidate.path;
    if (candidate.status_error)
    {
        fail_network_read(path, candidate.status_error.message());
    }
    if (candidate.format == network_format::UNKNOWN)
    {
        fail_network_read(path, "unsupported network extension");
    }

    std::ifstream input{path, std::ios::binary};
    if (!input)
    {
        fail_network_read(path, "could not open file");
    }

    Ntk                       network{};
    network_diagnostics       diagnostics{};
    lorina::diagnostic_engine engine{&diagnostics};
    try
    {
        input.exceptions(std::ios::badbit);
        if (parse_network(input, candidate.format, network, engine) != lorina::return_code::success)
        {
            fail_network_read(path, diagnostics.messages.empty() ? "parsing error" :
                                                                   "parsing error: " + diagnostics.messages);
        }
    }
    catch (const std::invalid_argument& error)
    {
        fail_network_read(path, fmt::format("parsing error: {}", error.what()));
    }
    catch (const std::out_of_range& error)
    {
        fail_network_read(path, fmt::format("parsing error: {}", error.what()));
    }
    catch (const std::ios_base::failure& error)
    {
        fail_network_read(path, fmt::format("read error: {}", error.what()));
    }

    if (!network.is_combinational())
    {
        fail_network_read(path, "the parsed network is not combinational");
    }
    if constexpr (mockturtle::has_set_network_name_v<Ntk>)
    {
        network.set_network_name(network_path_text(path.stem()));
    }
    return mockturtle::cleanup_dangling(network);
}

}  // namespace detail

/**
 * @brief Read a file or the first readable network in a directory in filename order.
 * @tparam Ntk Logic network type.
 * @param path File or directory to read.
 * @param format Explicit format (v, aig, or blif), or empty to infer it from the extension.
 * @return The parsed combinational network.
 * @throws network_read_error If no network can be read.
 */
template <typename Ntk>
[[nodiscard]] Ntk read_network(const std::filesystem::path& path, const std::string_view format = {})
{
    const auto      requested = detail::requested_network_format(path, format);
    std::error_code error{};
    const auto      status = std::filesystem::status(path, error);
    if (error)
    {
        detail::fail_network_read(path, error.message());
    }
    if (std::filesystem::is_regular_file(status))
    {
        return detail::read_network_file<Ntk>(
            {.path = path, .format = detail::file_network_format(path, requested), .status_error = {}});
    }
    if (!std::filesystem::is_directory(status))
    {
        detail::fail_network_read(path, "expected a regular file or directory");
    }

    std::string first_failure{};
    for (const auto& candidate : detail::network_candidates(path, requested))
    {
        try
        {
            return detail::read_network_file<Ntk>(candidate);
        }
        catch (const network_read_error& failure)
        {
            if (first_failure.empty())
            {
                first_failure = failure.what();
            }
        }
    }
    detail::fail_network_read(path, first_failure.empty() ? "no supported network files" : first_failure);
}

/**
 * @brief Read all supported files in a directory without recursing.
 * @tparam Ntk Logic network type.
 * @param directory Directory to read.
 * @param format Explicit format (v, aig, or blif) for all regular files, or empty to infer it from each extension.
 * @return Successful networks and file failures, each in filename order.
 * @throws network_read_error If the format is unknown or the directory cannot be listed.
 */
template <typename Ntk>
[[nodiscard]] network_read_batch<Ntk> read_networks(const std::filesystem::path& directory,
                                                    const std::string_view       format = {})
{
    const auto              requested = detail::requested_network_format(directory, format);
    network_read_batch<Ntk> result{};
    for (const auto& candidate : detail::network_candidates(directory, requested))
    {
        try
        {
            result.networks.push_back(detail::read_network_file<Ntk>(candidate));
        }
        catch (const network_read_error& failure)
        {
            result.failures.push_back({.path = candidate.path, .message = failure.what()});
        }
    }
    return result;
}

}  // namespace fiction::networks::io
