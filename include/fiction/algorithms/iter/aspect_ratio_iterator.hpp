//
// Created by marcel on 06.01.20.
//

#ifndef FICTION_ASPECT_RATIO_ITERATOR_HPP
#define FICTION_ASPECT_RATIO_ITERATOR_HPP

#include <cmath>
#include <cstdint>
#include <iterator>
#include <vector>

namespace fiction
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
     * The value `n` represents the amount of faces in the desired aspect ratios. For example, \f$n = 1\f$ will
     * yield aspect ratios with exactly \f$1\f$ face, i.e. \f$1 \times 1\f$ which is equal to `ucoord_t{0, 0}`. If
     * \f$n = 2\f$, the aspect ratios \f$1 \times 2\f$ and \f$2 \times 1\f$ will result, which are equal to
     * `ucoord_t{0, 1}` and `ucoord_t{1, 0}`. Both examples with `AspectRatio == offset::ucoord_t`.
     *
     * @param n Starting value of the aspect ratio iteration.
     */
    explicit aspect_ratio_iterator([[maybe_unused]] uint64_t n = 0ul) noexcept : num{n != 0ul ? n - 1 : 0ul}
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
    aspect_ratio_iterator& operator++() noexcept
    {
        ++it;

        // end of factors: compute next ones
        if (it == factors.end())
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
    aspect_ratio_iterator operator++(int) noexcept
    {
        auto result{*this};

        ++(*this);

        return result;
    }

    [[nodiscard]] AspectRatio operator*() const
    {
        return *it;
    }

    [[nodiscard]] bool operator==(const uint64_t m) const noexcept
    {
        return num == m;
    }

    [[nodiscard]] bool operator==(const aspect_ratio_iterator& other) const
    {
        return (num == other.num) && (*it == *(other.it));
    }

    [[nodiscard]] bool operator!=(const uint64_t m) const noexcept
    {
        return num != m;
    }

    [[nodiscard]] bool operator!=(const aspect_ratio_iterator& other) const
    {
        return !(*this == other);
    }

    [[nodiscard]] bool operator<(const uint64_t m) const noexcept
    {
        return num < m;
    }

    [[nodiscard]] bool operator<(const aspect_ratio_iterator& other) const
    {
        return (num < other.num) || (num == other.num && *it < *(other.it));
    }

    [[nodiscard]] bool operator<=(const uint64_t m) const noexcept
    {
        return num <= m;
    }

    [[nodiscard]] bool operator<=(const aspect_ratio_iterator& other) const
    {
        return (num <= other.num) || (num == other.num && *it <= *(other.it));
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
     * Iterator pointing to current factor.
     */
    typename std::vector<AspectRatio>::iterator it;

    /**
     * Factorizes the current `num` into all possible factors \f$(x, y)\f$ with \f$x \cdot y = num\f$. The result is
     * stored as a vector of `AspectRatio` objects in the attribute factors.
     */
    void factorize() noexcept
    {
        factors.clear();

        for (auto i = 1u; i <= std::sqrt(num); ++i)
        {
            if (num % i == 0)
            {
                const auto x = i - 1;
                const auto y = num / i - 1;

                factors.emplace_back(x, y);
                if (x != y)
                {
                    factors.emplace_back(y, x);
                }
            }
        }

        // let it point to the first element of the sequence
        it = factors.begin();
    }
    /**
     * Computes the next possible `num` where a factorization \f$(x, y)\f$ with \f$x \cdot y = num\f$ exists.
     */
    void next() noexcept
    {
        do {
            ++num;
            factorize();
        } while (factors.empty());
    }
};

}  // namespace fiction

// make `aspect_ratio_iterator` compatible with STL iterator categories
namespace std
{
template <typename AspectRatio>
struct iterator_traits<fiction::aspect_ratio_iterator<AspectRatio>>
{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = AspectRatio;
};
}  // namespace std

#endif  // FICTION_ASPECT_RATIO_ITERATOR_HPP
