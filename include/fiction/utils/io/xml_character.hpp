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
 * @brief Decodes UTF-8 characters permitted by XML 1.0.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>

namespace fiction::utils::io::detail
{

/**
 * @brief Consume one UTF-8 character permitted by XML 1.0.
 * @param remaining UTF-8 text; advances past the consumed character.
 * @pre remaining is nonempty.
 * @return Decoded XML character.
 * @throws std::invalid_argument If the encoding or XML character is invalid.
 * @see https://www.w3.org/TR/xml/#charsets
 * @see https://www.rfc-editor.org/rfc/rfc3629#section-4
 */
[[nodiscard]] inline uint32_t next_xml_character(std::string_view& remaining)
{
    /** @brief First encoded byte of the character. */
    const auto leading = static_cast<unsigned char>(remaining.front());
    remaining.remove_prefix(1);
    /** @brief Decoded Unicode character. */
    uint32_t character{leading};
    /** @brief Smallest character encoded in this sequence length. */
    uint32_t minimum{};
    /** @brief Number of continuation bytes to consume. */
    std::size_t continuations{};
    if (leading >= 0xc2 && leading <= 0xdf)
    {
        character     = leading & 0x1fu;
        minimum       = 0x80;
        continuations = 1;
    }
    else if (leading >= 0xe0 && leading <= 0xef)
    {
        character     = leading & 0x0fu;
        minimum       = 0x800;
        continuations = 2;
    }
    else if (leading >= 0xf0 && leading <= 0xf4)
    {
        character     = leading & 0x07u;
        minimum       = 0x10000;
        continuations = 3;
    }
    else if (leading >= 0x80)
    {
        throw std::invalid_argument("XML text requires valid UTF-8");
    }
    if (remaining.size() < continuations)
    {
        throw std::invalid_argument("XML text requires valid UTF-8");
    }
    /** @brief Index into the encoded continuation bytes. */
    for (std::size_t i = 0; i < continuations; ++i)
    {
        /** @brief Next encoded continuation byte. */
        const auto continuation = static_cast<unsigned char>(remaining.front());
        remaining.remove_prefix(1);
        if ((continuation & 0xc0u) != 0x80u)
        {
            throw std::invalid_argument("XML text requires valid UTF-8");
        }
        character = (character << 6u) | (continuation & 0x3fu);
    }
    if (character < minimum ||
        !(character == 0x9 || character == 0xa || character == 0xd || (character >= 0x20 && character <= 0xd7ff) ||
          (character >= 0xe000 && character <= 0xfffd) || (character >= 0x10000 && character <= 0x10ffff)))
    {
        throw std::invalid_argument("XML text requires XML 1.0 characters encoded as UTF-8");
    }
    return character;
}

}  // namespace fiction::utils::io::detail
