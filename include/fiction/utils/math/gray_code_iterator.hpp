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
 * @brief Iterator over the Gray code representations of consecutive numbers.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <cstdint>
#include <iterator>

namespace fiction::utils::math
{
/**
 * An iterator type that iterates over Gray code representations for decimal numbers.
 *
 * The `gray_code_iterator` class provides an iterator for generating Gray code representations
 * for a range of decimal numbers. It starts from a specified number and produces Gray codes
 * for consecutive decimal numbers using bitwise XOR operations. Position arithmetic wraps modulo 2^64.
 * Dereferencing returns a value. C++20 ranges support constant-time random access; legacy iterator algorithms
 * use the input category because the iterator does not refer to stored sequence elements.
 * Iterator ranges must not wrap, and distances must fit in `int64_t`.
 */
class gray_code_iterator
{
  public:
    /**
     * Constructs a Gray Code Iterator with a specified starting number.
     *
     * Constructs a `gray_code_iterator` that generates Gray codes for decimal numbers
     * starting from the given `start` number.
     *
     * @param start The starting decimal number for the iterator.
     */
    explicit constexpr gray_code_iterator(const uint64_t start = 0) noexcept : current_iteration{start} {}
    /**
     * Dereference operator. Returns the Gray code of the current iteration.
     *
     * @return Current Gray code.
     */
    [[nodiscard]] constexpr uint64_t operator*() const noexcept
    {
        return current_iteration ^ (current_iteration >> 1u);
    }
    /**
     * Prefix increment operator. Sets the number and the corresponding Gray code.
     *
     * @return Reference to `this`.
     */
    constexpr gray_code_iterator& operator++() noexcept
    {
        ++current_iteration;

        return *this;
    }
    /**
     * Equality comparison operator. Compares the current iterator with another iterator.
     *
     * @param other The iterator to compare with.
     * @return `true` if the current iterator is equal to the other iterator, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator==(const gray_code_iterator& other) const noexcept
    {
        return current_iteration == other.current_iteration;
    }
    /**
     * Inequality comparison operator. Compares the current iterator with another iterator.
     *
     * @param other The iterator to compare with.
     * @return `true` if the current iterator is not equal to the other iterator, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator!=(const gray_code_iterator& other) const noexcept
    {
        return !(*this == other);
    }
    /**
     * Less-than comparison operator. Compares the current iterator with another iterator.
     *
     * @param other The iterator to compare with.
     * @return `true` if the current iterator is less than the other iterator, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator<(const gray_code_iterator& other) const noexcept
    {
        return current_iteration < other.current_iteration;
    }
    /**
     * Less-than or equal-to comparison operator. Compares the current iterator with another iterator.
     *
     * @param other The iterator to compare with.
     * @return `true` if the current iterator is less than or equal to the other iterator, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator<=(const gray_code_iterator& other) const noexcept
    {
        return current_iteration <= other.current_iteration;
    }
    /**
     * @brief Compares iterator positions.
     * @param other Iterator to compare with.
     * @return Whether this position is greater than the other position.
     */
    [[nodiscard]] constexpr bool operator>(const gray_code_iterator& other) const noexcept
    {
        return other < *this;
    }
    /**
     * @brief Compares iterator positions.
     * @param other Iterator to compare with.
     * @return Whether this position is greater than or equal to the other position.
     */
    [[nodiscard]] constexpr bool operator>=(const gray_code_iterator& other) const noexcept
    {
        return other <= *this;
    }
    /**
     * Subtraction operator to calculate the difference between two gray_code_iterators.
     *
     * This operator calculates the difference between the current iterator and another
     * gray_code_iterator provided as input. The result is returned as an int64_t representing
     * the number of positions between the iterators. The distance must fit in int64_t.
     *
     * @param other The gray_code_iterator to subtract from the current iterator.
     * @return The difference between the current iterator and the input iterator as int64_t.
     */
    constexpr int64_t operator-(const gray_code_iterator& other) const noexcept
    {
        return static_cast<int64_t>(current_iteration - other.current_iteration);
    }
    /**
     * Postfix increment operator. Sets the next Gray Code.
     *
     * @return Copy of `this` before incrementing.
     */
    constexpr gray_code_iterator operator++(int) noexcept
    {
        auto result{*this};

        ++(*this);

        return result;
    }
    /**
     * Addition operator. Computes the Gray code of the current iterator plus the given integer.
     *
     * @param m The amount of Gray codes to skip.
     * @return Iterator of the current iterator plus the given integer.
     */
    [[nodiscard]] constexpr gray_code_iterator operator+(const int64_t m) const noexcept
    {
        auto result{*this};

        result += m;

        return result;
    }
    /**
     * @brief Advances an iterator by the given offset.
     * @param offset Number of positions to advance.
     * @param it Starting iterator.
     * @return Iterator at the offset position.
     */
    [[nodiscard]] friend constexpr gray_code_iterator operator+(const int64_t             offset,
                                                                const gray_code_iterator& it) noexcept
    {
        return it + offset;
    }
    /**
     * Addition assignment operator. Iterator is increased by given number.
     *
     * @param m The amount of Gray codes to skip.
     * @return Reference to `this`.
     */
    constexpr gray_code_iterator& operator+=(const int64_t m) noexcept
    {
        current_iteration += static_cast<uint64_t>(m);

        return *this;
    }
    /**
     * Subtraction operator. Computes the Gray code of the current iterator minus the given integer.
     *
     * @param m The amount of Gray codes to skip.
     * @return Iterator of the current iterator minus the given integer.
     */
    [[nodiscard]] constexpr gray_code_iterator operator-(const int64_t m) const noexcept
    {
        auto result{*this};

        result -= m;

        return result;
    }
    /**
     * Prefix decrement operator. Sets the previous Gray code.
     *
     * @return Reference to `this`.
     */
    constexpr gray_code_iterator& operator--() noexcept
    {
        --current_iteration;

        return *this;
    }
    /**
     * Postfix decrement operator. Sets the previous Gray Code.
     *
     * @return Copy of `this` before decrementing.
     */
    constexpr gray_code_iterator operator--(int) noexcept
    {
        auto result{*this};

        --(*this);

        return result;
    }
    /**
     * Subtraction assignment operator. Sets a previous Gray code.
     *
     * @param m The amount of Gray codes to skip.
     * @return Reference to `this`.
     */
    constexpr gray_code_iterator& operator-=(const int64_t m) noexcept
    {
        current_iteration -= static_cast<uint64_t>(m);

        return *this;
    }
    /**
     * Assignment operator. Sets the current number to the given integer.
     *
     * @param m The number to set.
     */
    constexpr gray_code_iterator& operator=(const uint64_t m) noexcept
    {
        current_iteration = m;
        return *this;
    }
    /**
     * Subscript operator. Returns the Gray code at an offset from the current position.
     *
     * @param index The offset from the current position.
     * @return The Gray code at the specified position.
     */
    constexpr uint64_t operator[](const int64_t index) const noexcept
    {
        return *(*this + index);
    }
    /**
     * Equality operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is equal to `m`, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator==(const uint64_t m) const noexcept
    {
        return current_iteration == m;
    }
    /**
     * Inequality operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is not equal to `m`, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator!=(const uint64_t m) const noexcept
    {
        return current_iteration != m;
    }
    /**
     * Less-than operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is less than `m`, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator<(const uint64_t m) const noexcept
    {
        return current_iteration < m;
    }
    /**
     * Less-or-equal-than operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is less than or equal to `m`, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator<=(const uint64_t m) const noexcept
    {
        return current_iteration <= m;
    }
    /**
     * Greater-than operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is greater than `m`, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator>(const uint64_t m) const noexcept
    {
        return current_iteration > m;
    }
    /**
     * Greater-or-equal-than operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is greater than or equal to `m`, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator>=(const uint64_t m) const noexcept
    {
        return current_iteration >= m;
    }

  private:
    /**
     * Current number (i.e., current iteration number).
     */
    uint64_t current_iteration;
};

}  // namespace fiction::utils::math
// make `gray_code_iterator` compatible with STL iterator categories
namespace std
{
/** @brief Standard iterator traits for the specialized iterator. */
template <>
struct iterator_traits<fiction::utils::math::gray_code_iterator>
{
    /** @brief C++20 iterator traversal concept. */
    using iterator_concept = std::random_access_iterator_tag;
    /** @brief Legacy traversal category for generated values. */
    using iterator_category = std::input_iterator_tag;
    /** @brief Value produced by dereferencing the iterator. */
    using value_type = uint64_t;
    /** @brief Value returned by dereferencing the iterator. */
    using reference = uint64_t;
    /** @brief No pointer into stored sequence elements is available. */
    using pointer = void;
    /** @brief Signed type for iterator distances. */
    using difference_type = int64_t;
};
}  // namespace std
