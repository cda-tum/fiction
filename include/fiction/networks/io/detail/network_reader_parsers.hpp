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
 * @brief Adapts Lorina and mockturtle parsers for reading logic networks.
 */

#pragma once

#include <lorina/aiger.hpp>
#include <lorina/blif.hpp>
#include <lorina/common.hpp>
#include <lorina/diagnostics.hpp>
#include <lorina/verilog.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <mockturtle/io/blif_reader.hpp>
#include <mockturtle/io/verilog_reader.hpp>
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace fiction::networks::io::detail
{

/** @brief Input formats supported by the network reader. */
enum class network_format : uint8_t
{
    /** @brief Infer the format from the filename or skip an unsupported extension. */
    UNKNOWN,
    /** @brief Structural Verilog. */
    VERILOG,
    /** @brief Binary AIGER. */
    AIGER,
    /** @brief Berkeley Logic Interchange Format. */
    BLIF
};

/** @brief Parse an entire unsigned integer token without accepting trailing characters. */
[[nodiscard]] inline std::optional<uint64_t> parse_unsigned(const std::string_view token, const int base = 10)
{
    if (token.empty())
    {
        return std::nullopt;
    }
    uint64_t          value{};
    const auto* const end        = std::to_address(token.end());
    const auto [position, error] = std::from_chars(token.data(), end, value, base);
    return error == std::errc{} && position == end ? std::optional{value} : std::nullopt;
}

/** @brief Take a text record from binary AIGER using Lorina's line endings. */
[[nodiscard]] inline std::string_view take_aiger_line(std::string_view& bytes)
{
    const auto end  = bytes.find_first_of("\r\n");
    const auto line = bytes.substr(0, end);
    if (end == std::string_view::npos)
    {
        bytes = {};
        return line;
    }
    const bool crlf = bytes.substr(end).starts_with("\r\n");
    bytes.remove_prefix(end + (crlf ? 2u : 1u));
    return line;
}

/** @brief Validate a required AIGER count or literal before Lorina uses signed conversion. */
[[nodiscard]] inline uint64_t aiger_number(const std::string_view token, const uint64_t maximum)
{
    const auto value = parse_unsigned(token);
    if (!value || *value > maximum)
    {
        throw std::invalid_argument{"invalid AIGER count or literal"};
    }
    return *value;
}

/** @brief Read and bound a gate delta before Lorina's decoder can run past EOF or shift too far. */
[[nodiscard]] inline uint64_t take_aiger_delta(std::string_view& bytes)
{
    uint64_t delta{};
    for (unsigned shift{}; shift < 32u && !bytes.empty(); shift += 7u)
    {
        const auto byte = static_cast<unsigned char>(bytes.front());
        bytes.remove_prefix(1);
        delta |= static_cast<uint64_t>(byte & 0x7fu) << shift;
        if ((byte & 0x80u) == 0 && delta <= std::numeric_limits<uint32_t>::max())
        {
            return delta;
        }
    }
    throw std::invalid_argument{"truncated or oversized AIGER gate delta"};
}

/** @brief Check the index of each symbol that Lorina passes to mockturtle. */
inline void validate_aiger_symbols(std::string_view bytes, const std::array<uint64_t, 9>& counts)
{
    // The first four symbol classes index mockturtle's inputs, latches, and outputs.
    constexpr std::string_view symbols{"ilob"};
    const std::array           limits{counts[1], counts[2], counts[3], counts[5]};
    while (!bytes.empty())
    {
        const auto line = take_aiger_line(bytes);
        if (line == "c")
        {
            break;
        }
        if (line.empty())
        {
            continue;
        }

        const auto kind      = symbols.find(line.front());
        const auto separator = line.find(' ');
        if (kind == std::string_view::npos || separator == std::string_view::npos)
        {
            continue;
        }
        const auto index = line.substr(1, separator - 1);
        if (!index.empty() && index.find_first_not_of("0123456789") == std::string_view::npos)
        {
            const auto value = parse_unsigned(index);
            if (!value || *value >= limits.at(kind))
            {
                throw std::invalid_argument{"invalid AIGER symbol index"};
            }
        }
    }
}

/** @brief Validate records that the pinned binary AIGER parser assumes are well-formed. */
inline void validate_binary_aiger(std::string_view bytes)
{
    auto header = take_aiger_line(bytes);
    if (!header.starts_with("aig "))
    {
        throw std::invalid_argument{"expected a binary AIGER header"};
    }
    header.remove_prefix(4);

    // M I L O A and the optional AIGER 1.9 counts B C J F.
    std::array<uint64_t, 9> counts{};
    std::size_t             field{};
    constexpr auto          max_long  = static_cast<uint64_t>(std::numeric_limits<long>::max());
    constexpr auto          max_count = std::min<uint64_t>(max_long, std::numeric_limits<uint32_t>::max());
    while (!header.empty() && field < counts.size())
    {
        const auto separator = header.find(' ');
        counts.at(field)     = aiger_number(header.substr(0, separator), max_count);
        ++field;
        if (separator == std::string_view::npos)
        {
            header = {};
        }
        else
        {
            header.remove_prefix(separator + 1);
        }
    }
    const auto [variables, inputs, latches, outputs, gates, bad, constraints, justice, fairness] = counts;
    if (field < 5 || !header.empty() || variables != inputs + latches + gates ||
        variables > std::numeric_limits<uint32_t>::max() / 2u)
    {
        throw std::invalid_argument{"inconsistent AIGER header counts"};
    }

    const auto max_literal = std::min(2 * variables + 1, max_long);
    for (uint64_t latch{}; latch < latches; ++latch)
    {
        const auto line      = take_aiger_line(bytes);
        const auto separator = line.find(' ');
        const auto max_next  = std::min<uint64_t>(max_literal, std::numeric_limits<int>::max());
        static_cast<void>(aiger_number(line.substr(0, separator), max_next));
        if (separator != std::string_view::npos)
        {
            const auto reset = aiger_number(line.substr(separator + 1), max_literal);
            if (reset > 1 && reset != 2 * (inputs + latch + 1))
            {
                throw std::invalid_argument{"invalid AIGER latch reset literal"};
            }
        }
    }
    for (uint64_t index{}; index < outputs + bad + constraints; ++index)
    {
        static_cast<void>(aiger_number(take_aiger_line(bytes), max_literal));
    }
    uint64_t justice_literals{};
    for (uint64_t index{}; index < justice; ++index)
    {
        justice_literals += aiger_number(take_aiger_line(bytes), max_count);
        if (justice_literals > bytes.size())
        {
            throw std::invalid_argument{"truncated AIGER justice properties"};
        }
    }
    for (uint64_t index{}; index < justice_literals + fairness; ++index)
    {
        static_cast<void>(aiger_number(take_aiger_line(bytes), max_literal));
    }
    for (auto variable = inputs + latches + 1; variable <= variables; ++variable)
    {
        const auto first_delta  = take_aiger_delta(bytes);
        const auto second_delta = take_aiger_delta(bytes);
        const auto lhs          = 2 * variable;
        if (first_delta == 0 || first_delta > lhs || second_delta > lhs - first_delta)
        {
            throw std::invalid_argument{"invalid AIGER gate literal"};
        }
    }
    validate_aiger_symbols(bytes, counts);
}

/** @brief Validate the same immutable bytes that Lorina parses. */
template <typename Ntk>
[[nodiscard]] lorina::return_code parse_binary_aiger(std::istream& input, Ntk& network, lorina::diagnostic_engine& diag)
{
    std::ostringstream buffer{};
    buffer << input.rdbuf();
    if (buffer.bad())
    {
        throw std::ios_base::failure{"could not read AIGER data"};
    }
    validate_binary_aiger(buffer.view());
    std::istringstream checked{std::move(buffer).str()};
    return lorina::read_aiger(checked, mockturtle::aiger_reader<Ntk>{network}, &diag);
}

/** @brief Validate covers before forwarding them to the mockturtle BLIF reader. */
template <typename Ntk>
class checked_blif_reader final : private mockturtle::blif_reader<Ntk>
{
  public:
    /** @brief Construct a reader that writes to the supplied network. */
    using mockturtle::blif_reader<Ntk>::blif_reader;

    /** @brief Parse BLIF using the checked callbacks. */
    [[nodiscard]] lorina::return_code read(std::istream& input, lorina::diagnostic_engine& diag) const
    {
        return lorina::read_blif(input, *this, &diag);
    }

  private:
    /** @brief Reject latches when the destination cannot represent them. */
    void on_latch(const std::string& input, const std::string& output,
                  const std::optional<lorina::blif_reader::latch_type>& type, const std::optional<std::string>& control,
                  const std::optional<lorina::blif_reader::latch_init_value>& reset) const override
    {
        if constexpr (mockturtle::has_create_ro_v<Ntk> && mockturtle::has_create_ri_v<Ntk>)
        {
            mockturtle::blif_reader<Ntk>::on_latch(input, output, type, control, reset);
        }
        else
        {
            throw std::invalid_argument{"BLIF latches require a network with register support"};
        }
    }

    /** @brief Forward a valid cover or materialize an empty cover as constant false. */
    void on_gate(const std::vector<std::string>& inputs, const std::string& output,
                 const lorina::blif_reader::output_cover_t& cover) const override
    {
        if (cover.empty())
        {
            mockturtle::blif_reader<Ntk>::on_gate({}, output, {});
            return;
        }
        // kitty::cube stores one bit per input in a 32-bit mask.
        if (inputs.size() > 32 || (inputs.empty() && cover.size() != 1))
        {
            throw std::invalid_argument{"unsupported BLIF cover size"};
        }
        for (const auto& [pattern, value] : cover)
        {
            if (pattern.size() != inputs.size() || (value != "0" && value != "1") || value != cover.front().second)
            {
                throw std::invalid_argument{"invalid BLIF cover row"};
            }
        }
        mockturtle::blif_reader<Ntk>::on_gate(inputs, output, cover);
    }
};

/** @brief Skip blank and comment lines before Lorina handles BLIF continuations. */
template <typename Ntk>
[[nodiscard]] lorina::return_code parse_blif_network(std::istream& input, Ntk& network, lorina::diagnostic_engine& diag)
{
    if constexpr (mockturtle::has_create_node_v<Ntk> || mockturtle::has_create_cover_node_v<Ntk>)
    {
        std::string normalized{};
        bool        continuation{};
        for (std::string line{}; std::getline(input, line);)
        {
            constexpr std::string_view whitespace{" \t\r\f\v"};
            const auto                 first = line.find_first_not_of(whitespace);
            if (first != std::string::npos && line[first] != '#')
            {
                continuation = line.at(line.find_last_not_of(whitespace)) == '\\';
                normalized += line;
                normalized += '\n';
            }
        }
        if (continuation)
        {
            throw std::invalid_argument{"unfinished BLIF continuation"};
        }
        if (normalized.empty())
        {
            throw std::invalid_argument{"expected a BLIF network"};
        }
        std::istringstream checked{std::move(normalized)};
        return checked_blif_reader<Ntk>{network}.read(checked, diag);
    }
    else
    {
        throw std::invalid_argument{"BLIF requires a network that supports truth-table nodes"};
    }
}

/** @brief Check vector ranges before mockturtle interprets them as [high:0]. */
template <typename Ntk>
class checked_verilog_reader final : private mockturtle::verilog_reader<Ntk>
{
  public:
    /** @brief Construct a reader for the module named `top`. */
    explicit checked_verilog_reader(Ntk& network) :
            mockturtle::verilog_reader<Ntk>{network, std::string{top_module_name}}
    {}

    /** @brief Parse Verilog using the checked callbacks. */
    [[nodiscard]] lorina::return_code read(std::istream& input, lorina::diagnostic_engine& diag) const
    {
        const auto result = lorina::read_verilog(input, *this, &diag);
        if (result == lorina::return_code::success && !found_top)
        {
            throw std::invalid_argument{"expected a Verilog module named 'top'"};
        }
        return result;
    }

  private:
    /** @brief Name of the module that supplies the network. */
    static constexpr std::string_view top_module_name{"top"};
    /** @brief Whether the active module supplies the network. */
    mutable bool reading_top{};
    /** @brief Whether the file contains the requested module. */
    mutable bool found_top{};

    /** @brief Track which module the following declarations belong to. */
    void on_module_header(const std::string& name, const std::vector<std::string>& ports) const override
    {
        reading_top = name == top_module_name;
        found_top   = found_top || reading_top;
        mockturtle::verilog_reader<Ntk>::on_module_header(name, ports);
    }

    /** @brief Validate the range of an input declaration. */
    void on_inputs(const std::vector<std::string>& names, const std::string& size) const override
    {
        if (reading_top)
        {
            check_range(size);
            mockturtle::verilog_reader<Ntk>::on_inputs(names, size);
        }
    }

    /** @brief Validate the range of an output declaration. */
    void on_outputs(const std::vector<std::string>& names, const std::string& size) const override
    {
        if (reading_top)
        {
            check_range(size);
            mockturtle::verilog_reader<Ntk>::on_outputs(names, size);
        }
    }

    /** @brief Reject ranges that the pinned reader cannot represent. */
    static void check_range(const std::string_view size)
    {
        if (size.empty())
        {
            return;
        }
        if (!size.ends_with(":0"))
        {
            throw std::invalid_argument{"unsupported Verilog port range"};
        }
        const auto high  = size.substr(0, size.size() - 2);
        const auto hex   = high.find("'h");
        auto       value = parse_unsigned(high);
        if (hex != std::string_view::npos)
        {
            const auto bits = parse_unsigned(high.substr(0, hex));
            value           = parse_unsigned(high.substr(hex + 2), 16);
            if (!bits || *bits == 0 || *bits > 64 || (value && *bits < 64 && (*value >> *bits) != 0))
            {
                throw std::invalid_argument{"unsupported Verilog port range"};
            }
        }
        if (!value || *value >= std::numeric_limits<uint32_t>::max())
        {
            throw std::invalid_argument{"unsupported Verilog port range"};
        }
    }
};

/** @brief Dispatch a stream to the parser for its resolved format. */
template <typename Ntk>
[[nodiscard]] lorina::return_code parse_network(std::istream& input, const network_format format, Ntk& network,
                                                lorina::diagnostic_engine& diag)
{
    switch (format)
    {
        case network_format::VERILOG: return checked_verilog_reader<Ntk>{network}.read(input, diag);
        case network_format::AIGER: return parse_binary_aiger(input, network, diag);
        case network_format::BLIF: return parse_blif_network(input, network, diag);
        case network_format::UNKNOWN: return lorina::return_code::parse_error;
    }
    return lorina::return_code::parse_error;
}

}  // namespace fiction::networks::io::detail
