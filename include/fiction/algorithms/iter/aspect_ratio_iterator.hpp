//
// Created by marcel on 06.01.20.
//

#ifndef FICTION_ASPECT_RATIO_ITERATOR_HPP
#define FICTION_ASPECT_RATIO_ITERATOR_HPP

#include <cmath>
#include <vector>

namespace fiction
{

template <typename AspectRatio>
class aspect_ratio_iterator
{
  public:
    /**
     * Standard constructor. Takes a starting value and computes an initial factorization.
     *
     * @param n Starting value of the dimension iteration.
     */
    explicit aspect_ratio_iterator(uint64_t n = 0ul) noexcept : num{n ? n - 1 : 0}
    {
        next();
    }
    /**
     * Lets the iterator point to the next dimension of the current factorization. If there are no next factors,
     * num is incremented and the next factors are computed.
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
     * factors, num is incremented and the next factors are computed.
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

    AspectRatio operator*() const
    {
        return *it;
    }

    bool operator==(const uint64_t m) const noexcept
    {
        return num == m;
    }

    bool operator==(const aspect_ratio_iterator& other) const
    {
        return (num == other.num) && (*it == *(other.it));
    }

    bool operator!=(const uint64_t m) const noexcept
    {
        return num != m;
    }

    bool operator!=(const aspect_ratio_iterator& other) const
    {
        return !(*this == other);
    }

    bool operator<(const uint64_t m) const noexcept
    {
        return num < m;
    }

    bool operator<(const aspect_ratio_iterator& other) const
    {
        return (num < other.num) || (num == other.num && *it < *(other.it));
    }

    bool operator<=(const uint64_t m) const noexcept
    {
        return num <= m;
    }

    bool operator<=(const aspect_ratio_iterator& other) const
    {
        return (num <= other.num) || (num == other.num && *it <= *(other.it));
    }

  private:
    /**
     * Number to factorize into dimensions.
     */
    uint64_t num;
    /**
     * Factors of num.
     */
    std::vector<AspectRatio> factors;
    /**
     * Iterator pointing to current factor.
     */
    typename std::vector<AspectRatio>::iterator it;

    /**
     * Factorizes the current num into all possible factors (x, y) with x * y = num. The result is returned as a vector
     * of AspectRatio objects.
     *
     * @return All possible aspect ratios with num faces.
     */
    void factorize() noexcept
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

        // let it point to the first element of the sequence
        it = factors.begin();
    }
    /**
     * Computes the next possible num where a factorization (x, y) with x * y = num exists.
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

// make aspect_ratio_iterator compatible with STL iterator categories
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
