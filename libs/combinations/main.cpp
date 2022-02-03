
#include "combinations.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <cstdint>
#include <cassert>

// print out a range separated by commas,
//    return number of values printed.
template <class It>
unsigned
display(It begin, It end)
{
    unsigned r = 0;
    if (begin != end)
    {
        std::cout << *begin;
        ++r;
        for (++begin; begin != end; ++begin)
        {
            std::cout << ", " << *begin;
            ++r;
        }
    }
    return r;
}

// functor called for each permutation
class f
{
    unsigned len;
    std::uint64_t count;
public:
    explicit f(unsigned l) : len(l), count(0) {}

    template <class It>
        bool operator()(It first, It last)  // called for each permutation
        {
            // count the number of times this is called
            ++count;
            // print out [first, mid) surrounded with [ ... ]
            std::cout << "[ ";
            unsigned r = display(first, last);
            // If [mid, last) is not empty, then print it out too
            //     prefixed by " | "
            if (r < len)
            {
                std::cout << " | ";
                display(last, std::next(last, len - r));
            }
            std::cout << " ]\n";
            return false;  // Don't break out of the loop
        }

    operator std::uint64_t() const {return count;}
};

int main()
{
    const int r = 3;
    const int n = 5;
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 0);
    std::uint64_t count = for_each_permutation(v.begin(),
                                               v.begin() + r,
                                               v.end(),
                                               f(v.size()));
    // print out "---" to the correct length for the above output
    unsigned e = 3 * r + 2;
    if (r < v.size())
        e += 1 + 3 * (v.size() - r);
    for (unsigned i = 0; i < e; ++i)
        std::cout << '-';
    // print out the permuted vector to show that it has the original order
    std::cout << "\n[ ";
    display(v.begin(), v.end());
    std::cout << " ]\n";
    // sanity check
    assert(count == count_each_permutation(v.begin(), v.begin() + r, v.end()));
    // print out summary of what has happened,
    //   using 'count' from functor state returned from for_each_permutation algorithm.
    std::cout << "Found " << count << " permutations of " << v.size()
              << " objects taken " << r << " at a time.\n";
}
