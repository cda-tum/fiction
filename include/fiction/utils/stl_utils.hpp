//
// Created by marcel on 15.02.22.
//

#ifndef FICTION_STL_UTILS_HPP
#define FICTION_STL_UTILS_HPP

namespace fiction
{

/**
 * A derivative of std::find_first_of that uses the example implementation given at
 * https://en.cppreference.com/w/cpp/algorithm/find_first_of.
 *
 * This version searches the range [first, last) for any two consecutive elements of the elements in the range [s_first,
 * s_last), i.e., for a 2-element sub-sequence shared between the ranges.
 *
 * Example: Let two ranges be [0, 1, 1, 2, 3, 3] and [1, 2, 3, 3]. A call to find_first_two_of would return an iterator
 * to index 2, i.e., the second 1 in the first list, because the 2-element sub-sequence [1,2] is shared between the two
 * ranges.
 *
 *
 * @tparam InputIt must meet the requirements of LegacyInputIterator.
 * @tparam ForwardIt must meet the requirements of LegacyForwardIterator.
 * @param first Begin of the range to examine.
 * @param last End of the range to examine.
 * @param s_first Begin of the range to search for.
 * @param s_last End of the range to search for.
 * @return Iterator to the first position of the first shared 2-element sub-sequence shared between the two ranges.
 */
template <class InputIt, class ForwardIt>
InputIt find_first_two_of(InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last) noexcept
{
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

}  // namespace fiction

#endif  // FICTION_STL_UTILS_HPP
