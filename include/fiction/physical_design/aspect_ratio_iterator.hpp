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
 * @brief Iterator over increasingly larger 2D aspect ratios obtained by factorization.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <tuple>
#include <vector>

namespace fiction::physical_design
{
/**
 * An iterator type that iterates over increasingly larger 2D aspect ratios via factorization, starting from a number of
 * faces \f$n\f$. After iterating over all possible factorizations of n, the next step increases \f$n\f$ and
 * continues with the factorization. Thereby, a sequence of aspect ratios starting from \f$n = 4\f$ faces looks like
 * this: \f$1 \times 4, 4 \times 1, 2 \times 2, 1 \times 5, 5 \times 1, 1 \times 6, 6 \times 1, 2 \times 3, 3 \times 2,
 * \dots\f$
 *
 * @tparam AspectRatio Aspect ratio type.
 */
template <typename AspectRatio>
class aspect_ratio_iterator
{
  public:
    /**
     * Standard constructor. Takes a starting value and computes an initial factorization.
     * The value `n` represents the amount of faces in the desired aspect ratios. For example, \f$n = 1\f$
     * yields the size-based extent `1 x 1`. A starting value of `2` yields extents `1 x 2` and `2 x 1`.
     *
     * @param n Starting value of the aspect ratio iteration.
     */
    explicit aspect_ratio_iterator([[maybe_unused]] uint64_t n = 0ul) : num{n != 0ul ? n - 1 : 0ul}
    {
        next();
    }
    /**
     * Lets the iterator point to the next dimension of the current factorization. If there are no next factors,
     * `num` is incremented and the next factors are computed.
     *
     * Prefix version.
     *
     * @return Reference to this.
     */
    aspect_ratio_iterator& operator++()
    {
        ++factor_index;

        // end of factors: compute next ones
        if (factor_index == factors.size())
        {
            next();
        }

        return *this;
    }
    /**
     * Creates a new iterator that points to the next dimension of the current factorization. If there are no next
     * factors, `num` is incremented and the next factors are computed.
     *
     * Postfix version. Less performance than the prefix version due to copy construction.
     *
     * @return Resulting iterator.
     */
    aspect_ratio_iterator operator++(int)
    {
        auto result{*this};

        ++(*this);

        return result;
    }

    /** @brief Return the current extent. */
    [[nodiscard]] AspectRatio operator*() const
    {
        return factors[factor_index];
    }

    /** @brief Compare iterator positions. */
    [[nodiscard]] bool operator==(const uint64_t m) const
    {
        return num == m;
    }

    /** @brief Compare iterator positions. */
    [[nodiscard]] bool operator==(const aspect_ratio_iterator& other) const
    {
        return (num == other.num) && (factor_index == other.factor_index);
    }

    /** @brief Compare iterator positions. */
    [[nodiscard]] bool operator!=(const uint64_t m) const
    {
        return num != m;
    }

    /** @brief Compare iterator positions. */
    [[nodiscard]] bool operator!=(const aspect_ratio_iterator& other) const
    {
        return !(*this == other);
    }

    /** @brief Compare iterator positions. */
    [[nodiscard]] bool operator<(const uint64_t m) const
    {
        return num < m;
    }

    /** @brief Compare iterator positions. */
    [[nodiscard]] bool operator<(const aspect_ratio_iterator& other) const
    {
        return std::tie(num, factor_index) < std::tie(other.num, other.factor_index);
    }

    /** @brief Compare iterator positions. */
    [[nodiscard]] bool operator<=(const uint64_t m) const
    {
        return num <= m;
    }

    /** @brief Compare iterator positions. */
    [[nodiscard]] bool operator<=(const aspect_ratio_iterator& other) const
    {
        return std::tie(num, factor_index) <= std::tie(other.num, other.factor_index);
    }

  private:
    /**
     * Number to factorize into dimensions.
     */
    uint64_t num{};
    /**
     * Factors of num.
     */
    std::vector<AspectRatio> factors;
    /**
     * Index of the current factor.
     */
    std::size_t factor_index{};

    /**
     * Factorizes the current `num` into all possible factors \f$(x, y)\f$ with \f$x \cdot y = num\f$. The result is
     * stored as a vector of `AspectRatio` objects in the attribute factors.
     */
    void factorize()
    {
        factors.clear();

        for (auto i = 1u; i <= std::sqrt(num); ++i)
        {
            if (num % i == 0)
            {
                const auto x = i;
                const auto y = num / i;

                factors.emplace_back(x, y);
                if (x != y)
                {
                    factors.emplace_back(y, x);
                }
            }
        }

        factor_index = 0;
    }
    /**
     * Computes the next possible `num` where a factorization \f$(x, y)\f$ with \f$x \cdot y = num\f$ exists.
     */
    void next()
    {
        ++num;
        factorize();

        while (factors.empty())
        {
            ++num;
            factorize();
        }
    }
};

}  // namespace fiction::physical_design
// make `aspect_ratio_iterator` compatible with STL iterator categories
namespace std
{
template <typename AspectRatio>
struct iterator_traits<fiction::physical_design::aspect_ratio_iterator<AspectRatio>>
{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = AspectRatio;
};
}  // namespace std
