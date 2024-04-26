//
// Created by Jan Drewniok on 30.08.23.
//

#ifndef FICTION_GRAY_CODE_ITERATOR_HPP
#define FICTION_GRAY_CODE_ITERATOR_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>

namespace fiction
{
/**
 * An iterator type that iterates over Gray code representations for decimal numbers.
 *
 * The `gray_code_iterator` class provides an iterator for generating Gray code representations
 * for a range of decimal numbers. It starts from a specified number and produces Gray codes
 * in ascending order based on bitwise XOR operations.
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
    explicit constexpr gray_code_iterator(const uint64_t start) noexcept :
            start_number{start},
            current_iteration{start},
            current_gray_code{start}
    {
        binary_to_gray();
    };
    /**
     * Dereference operator. Returns a reference to the Gray code of the current iteration.
     *
     * @return Reference to the current Gray code.
     */
    [[nodiscard]] constexpr const uint64_t& operator*() const noexcept
    {
        return current_gray_code;
    }
    /**
     * Prefix increment operator. Sets the number and the corresponding Gray code.
     *
     * @return Reference to `this`.
     */
    constexpr gray_code_iterator& operator++() noexcept
    {
        ++current_iteration;
        binary_to_gray();

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
        return (current_gray_code == other.current_gray_code);
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
        return current_gray_code < other.current_gray_code;
    }
    /**
     * Less-than or equal-to comparison operator. Compares the current iterator with another iterator.
     *
     * @param other The iterator to compare with.
     * @return `true` if the current iterator is less than or equal to the other iterator, `false` otherwise.
     */
    [[nodiscard]] constexpr bool operator<=(const gray_code_iterator& other) const noexcept
    {
        return current_gray_code <= other.current_gray_code;
    }
    /**
     * Subtraction operator to calculate the difference between two gray_code_iterators.
     *
     * This operator calculates the difference between the current iterator and another
     * gray_code_iterator provided as input. The result is returned as an int64_t representing
     * the number of positions between the iterators.
     *
     * @param other The gray_code_iterator to subtract from the current iterator.
     * @return The difference between the current iterator and the input iterator as int64_t.
     */
    constexpr int64_t operator-(const gray_code_iterator& other) const noexcept
    {
        return static_cast<int64_t>(current_iteration) - static_cast<int64_t>(other.current_iteration);
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
    [[nodiscard]] constexpr gray_code_iterator operator+(const int m) const noexcept
    {
        auto result{*this};

        result += m;

        return result;
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    /**
     * Addition assignment operator. Iterator is increased by given number.
     *
     * @param m The amount of Gray codes to skip.
     * @return Reference to `this`.
     */
    constexpr gray_code_iterator& operator+=(const int m) noexcept
    {
        current_iteration += m;
        binary_to_gray();

        return *this;
    }
#pragma GCC diagnostic pop
    /**
     * Subtraction operator. Computes the Gray code of the current iterator minus the given integer.
     *
     * @param m The amount of Gray codes to skip.
     * @return Iterator of the current iterator minus the given integer.
     */
    [[nodiscard]] constexpr gray_code_iterator operator-(const int m) const noexcept
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

        binary_to_gray();

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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    /**
     * Subtraction assignment operator. Sets a previous Gray code.
     *
     * @param m The amount of Gray codes to skip.
     * @return Reference to `this`.
     */
    constexpr gray_code_iterator& operator-=(const int m) noexcept
    {
        current_iteration -= m;

        binary_to_gray();

        return *this;
    }
#pragma GCC diagnostic pop
    /**
     * Assignment operator. Sets the current number to the given integer.
     *
     * @param m The number to set.
     */
    constexpr gray_code_iterator& operator=(const uint64_t m) noexcept
    {
        current_iteration = m;
        binary_to_gray();
        return *this;
    }
    /**
     * Subscript operator. Returns the Gray code at a specific position in the iteration range.
     *
     * @param index The position in the iteration range.
     * @return The Gray code at the specified position.
     */
    constexpr uint64_t operator[](std::size_t index) const noexcept
    {
        // Calculate the Gray code at the specified position
        uint64_t result = start_number + index;
        result ^= (result >> 1u);
        return result;
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
     * Start number of the iteration.
     */
    const uint64_t start_number;
    /**
     * Current number (i.e., current iteration number).
     */
    uint64_t current_iteration;
    /**
     * Current Gray Code.
     */
    uint64_t current_gray_code;
    /**
     * Converts the current decimal number into its corresponding Gray code representation.
     *
     * This function operates on the current decimal number and produces its Gray code
     * representation using the bitwise XOR operation. Gray code is a binary numeral system
     * in which two successive values differ in only one bit.
     *
     * The result is stored in the 'current_gray_code' variable.
     */
    constexpr void binary_to_gray() noexcept
    {
        current_gray_code = current_iteration ^ (current_iteration >> 1u);
    }
};

}  // namespace fiction

// make `gray_code_iterator` compatible with STL iterator categories
namespace std
{
template <>
struct iterator_traits<fiction::gray_code_iterator>
{
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = uint64_t;
    using difference_type   = int64_t;
};
}  // namespace std

#endif  // FICTION_GRAY_CODE_ITERATOR_HPP
