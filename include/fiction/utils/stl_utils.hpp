//
// Created by marcel on 15.02.22.
//

#ifndef FICTION_STL_UTILS_HPP
#define FICTION_STL_UTILS_HPP

#include <functional>
#include <iterator>
#include <queue>
#include <type_traits>
#include <vector>

namespace fiction
{

/**
 * A derivative of `std::find_first_of` that uses the example implementation given at
 * https://en.cppreference.com/w/cpp/algorithm/find_first_of.
 *
 * This version searches the range `[first, last)` for any two consecutive elements of the elements in the range
 * `[s_first, s_last)`, i.e., for a 2-element sub-sequence shared between the ranges.
 *
 * Example: Let two ranges be `[0, 1, 1, 2, 3, 3]` and `[1, 2, 3, 3]`. A call to `find_first_two_of` would return an
 * iterator to index 2, i.e., the second `1` in the first list, because the 2-element sub-sequence `[1,2]` is shared
 * between the two ranges.
 *
 * @tparam InputIt must meet the requirements of `LegacyInputIterator`.
 * @tparam ForwardIt must meet the requirements of `LegacyForwardIterator`.
 * @param first Begin of the range to examine.
 * @param last End of the range to examine.
 * @param s_first Begin of the range to search for.
 * @param s_last End of the range to search for.
 * @return Iterator in the range `[first, last)` to the first position of the first 2-element sub-sequence shared
 * between the two ranges, or `last` if no such shared sub-sequence exists.
 */
template <class InputIt, class ForwardIt>
InputIt find_first_two_of(InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last) noexcept
{
    static_assert(std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>,
                  "InputIt must meet the requirements of LegacyInputIterator");
    static_assert(
        std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<ForwardIt>::iterator_category>,
        "ForwardIt must meet the requirements of LegacyForwardIterator");

    for (; first != last - 1; ++first)
    {
        for (ForwardIt it = s_first; it != s_last - 1; ++it)
        {
            if (*first == *it && *(first + 1) == *(it + 1))
            {
                return first;
            }
        }
    }

    return last;
}
/**
 * An extension of `std::priority_queue` that allows searching the underlying container. The implementation is based on
 * https://stackoverflow.com/questions/16749723/how-i-can-find-value-in-priority-queue.
 *
 * @tparam T The type of the stored elements.
 * @tparam Container The type of the underlying container.
 * @tparam Compare A Compare type providing a strict weak ordering.
 */
template <class T, class Container = std::vector<T>, class Compare = std::less<typename Container::value_type>>
class searchable_priority_queue : public std::priority_queue<T, Container, Compare>
{
  public:
    using iterator       = typename std::priority_queue<T, Container, Compare>::container_type::iterator;
    using const_iterator = typename std::priority_queue<T, Container, Compare>::container_type::const_iterator;

    iterator begin() noexcept
    {
        return this->c.begin();
    }
    const_iterator begin() const noexcept
    {
        return this->c.begin();
    }
    iterator end() noexcept
    {
        return this->c.end();
    }
    const_iterator end() const noexcept
    {
        return this->c.end();
    }
    const_iterator cbegin() const noexcept
    {
        return this->c.cbegin();
    }
    const_iterator cend() const noexcept
    {
        return this->c.cend();
    }
    /**
     * Returns an iterator to the provided value if it is contained in the priority queue. Returns an iterator to
     * the end of the stored container otherwise.
     *
     * @param val Value to search for.
     * @return Iterator to the stored value or to the end of the container if `val` is not contained.
     */
    iterator find(const T& val) noexcept
    {
        auto first = begin(), last = end();

        while (first != last)
        {
            if (*first == val)
            {
                return first;
            }

            ++first;
        }

        return last;
    }
    /**
     * Returns a `const_iterator` to the provided value if it is contained in the priority queue. Returns an iterator to
     * the end of the stored container otherwise.
     *
     * @param val Value to search for.
     * @return Iterator to the stored value or to the end of the container if `val` is not contained.
     */
    const_iterator find(const T& val) const noexcept
    {
        auto       first = cbegin();
        const auto last  = cend();

        while (first != last)
        {
            if (*first == val)
            {
                return first;
            }

            ++first;
        }

        return last;
    }
    /**
     * Returns `true` if the provided value is stored in the queue and `false` otherwise.
     *
     * @param val Value to search for.
     * @return `true` iff `val` is contained in the priority queue.
     */
    bool contains(const T& val) const noexcept
    {
        return find(val) != this->c.cend();
    }
};

}  // namespace fiction

#endif  // FICTION_STL_UTILS_HPP
