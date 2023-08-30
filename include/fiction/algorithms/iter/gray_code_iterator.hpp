//
// Created by Jan Drewniok on 30.08.23.
//

#ifndef FICTION_GRAY_CODE_ITERATOR_HPP
#define FICTION_GRAY_CODE_ITERATOR_HPP

#include <cstdint>
#include <iterator>
#include <vector>

namespace fiction
{

class gray_code_iterator
{
  public:
    explicit gray_code_iterator(const uint64_t start, const uint64_t end) noexcept :
            start_number{start},
            current_number{start},
            current_gray_code{start},
            final_number{end}
    {
        binary_to_gray();
    };

    /**
     * Prefix increment operator. Sets the number and the corresponding Gray code.
     *
     * @return Reference to `this`.
     */
    gray_code_iterator& operator++() noexcept
    {
        if (has_next())
        {
            ++current_number;
            binary_to_gray();
        }

        return *this;
    }

    /**
     * Addition operator. Computes the Gray code of the current iterator plus the given integer.
     *
     * @param m The amount of Gray codes to skip.
     * @return Iterator of the current iterator plus the given integer.
     */
    [[nodiscard]] gray_code_iterator operator+(const int m) const noexcept
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
    gray_code_iterator& operator+=(const int m) noexcept
    {
        if (current_number + m <= final_number)
        {
            current_number += m;
            binary_to_gray();
        }

        return *this;
    }
#pragma GCC diagnostic pop
    /**
     * Subtraction operator. Computes the Gray code of the current iterator minus the given integer.
     *
     * @param m The amount of Gray codes to skip.
     * @return Iterator of the current iterator minus the given integer.
     */
    [[nodiscard]] gray_code_iterator operator-(const int m) const noexcept
    {
        auto result{*this};

        result -= m;

        return result;
    }
    /**
     * Prefix decrement operator. Sets the previous input state.
     *
     * @return Reference to `this`.
     */
    gray_code_iterator& operator--() noexcept
    {
        --current_number;

        binary_to_gray();

        return *this;
    }
    /**
     * Postfix decrement operator. Sets the previous input state.
     *
     * @return Copy of `this` before decrementing.
     */
    gray_code_iterator operator--(int) noexcept
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
    gray_code_iterator& operator-=(const int m) noexcept
    {
        current_number -= m;

        binary_to_gray();

        return *this;
    }
#pragma GCC diagnostic pop
    /**
     * Assignment operator. Sets the current number to the given integer.
     *
     * @param m The number to set.
     */
    void operator=(const uint64_t m) noexcept
    {
        current_number = m;
        binary_to_gray();
    }
    /**
     * Equality operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is equal to `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator==(const uint64_t m) const noexcept
    {
        return current_number == m;
    }
    /**
     * Inequality operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is not equal to `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator!=(const uint64_t m) const noexcept
    {
        return current_number != m;
    }
    /**
     * Less-than operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is less than `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator<(const uint64_t m) const noexcept
    {
        return current_number < m;
    }
    /**
     * Less-or-equal-than operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is less than or equal to `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator<=(const uint64_t m) const noexcept
    {
        return current_number <= m;
    }
    /**
     * Greater-than operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is greater than `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator>(const uint64_t m) const noexcept
    {
        return current_number > m;
    }
    /**
     * Greater-or-equal-than operator. Compares the current number with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current number is greater than or equal to `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator>=(const uint64_t m) const noexcept
    {
        return current_number >= m;
    }
    /**
     * Returns the start number, i.e., number for which the first Gray code is determined.
     *
     * @return Start number of the iteration.
     */
    [[nodiscard]] uint64_t get_start_number() const
    {
        return start_number;
    }
    /**
     * Returns the current Gray code of the current iteration step.
     *
     * @return Current Gray code.
     */
    [[nodiscard]] uint64_t get_current_gray_code() const
    {
        return current_gray_code;
    }
    /**
     * Returns the final number, i.e., final number for which the Gray code is determined.
     *
     * @return Final number of the iteration.
     */
    [[nodiscard]] uint64_t get_final_number() const
    {
        return final_number;
    }
    /**
     * Determines if the iterator can be increased by one and does still not overcome the final number.
     *
     * @return `true` if the iterator can be increase by one, `false`otherwise.
     */
    [[nodiscard]] bool has_next() const
    {
        return current_number < final_number;
    }

  private:
    /**
     * Start number of the iteration.
     */
    const uint64_t start_number;
    /**
     * Current number (i.e., current iteration number).
     */
    uint64_t current_number;
    /**
     * Current Gray Code.
     */
    uint64_t current_gray_code;
    /**
     * Final number of the iteration.
     */
    const uint64_t final_number;
    /**
     * Converts the current decimal number into its corresponding Gray code representation.
     *
     * This function operates on the current decimal number and produces its Gray code
     * representation using the bitwise XOR operation. Gray code is a binary numeral system
     * in which two successive values differ in only one bit.
     *
     * The result is stored in the 'current_gray_code' variable.
     */
    void binary_to_gray()
    {
        current_gray_code = current_number ^ (current_number >> 1u);
    }
};
}  // namespace fiction

#endif  // FICTION_GRAY_CODE_ITERATOR_HPP
