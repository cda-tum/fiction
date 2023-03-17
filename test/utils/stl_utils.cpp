//
// Created by marcel on 07.02.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/graph/generate_edge_intersection_graph.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/routing_utils.hpp>
#include <fiction/utils/stl_utils.hpp>

#include <array>
#include <iterator>
#include <vector>

using namespace fiction;

TEST_CASE("Test find_first_two_of with vector input", "[find_first_two_of]")
{
    static const std::vector<int> v1{0, 1, 1, 2, 3, 3};
    static const std::vector<int> v2{1, 2, 3, 3};

    auto it = find_first_two_of(v1.begin(), v1.end(), v2.begin(), v2.end());
    CHECK(*it == 1);
    CHECK(*(it + 1) == 2);

    static const std::vector<int> v3{1, 2, 4, 3};
    it = find_first_two_of(v1.begin(), v1.end(), v3.begin(), v3.end());
    CHECK(it == std::next(v1.begin(), 2));
}

TEST_CASE("Test find_first_two_of with array input", "[find_first_two_of]")
{
    static constexpr const std::array a1{0, 1, 1, 2, 3, 3};
    static constexpr const std::array a2{1, 2, 3, 3};

    auto it = find_first_two_of(std::begin(a1), std::end(a1), std::begin(a2), std::end(a2));
    CHECK(*it == 1);
    CHECK(*(std::next(it, 1)) == 2);

    static constexpr const std::array a3{1, 2, 4, 3};
    it = find_first_two_of(std::begin(a1), std::end(a1), std::begin(a3), std::end(a3));
    CHECK(it == std::next(std::begin(a1), 2));
}

TEST_CASE("Test find_first_two_of with different iterator types", "[find_first_two_of]")
{
    static const std::vector<int>     v1{0, 1, 1, 2, 3, 3};
    static constexpr const std::array a2{1, 2, 3, 3};

    auto it = find_first_two_of(v1.begin(), v1.end(), std::begin(a2), std::end(a2));
    CHECK(*it == 1);
    CHECK(*(it + 1) == 2);

    static constexpr const std::array a3{1, 2, 4, 3};
    it = find_first_two_of(v1.begin(), v1.end(), std::begin(a3), std::end(a3));
    CHECK(it == std::next(v1.begin(), 2));
}

TEST_CASE("Test find_first_two_of with layout_coordinate_paths", "[first_first_two_of]")
{
    static const layout_coordinate_path<cart_gate_clk_lyt> p1{{1, 1}, {2, 1}};
    static const layout_coordinate_path<cart_gate_clk_lyt> p2{{0, 1}, {1, 1}, {2, 1}};

    // regular iterators
    const auto it1 = find_first_two_of(p1.begin(), p1.end(), p2.begin(), p2.end());
    const auto it2 = find_first_two_of(p2.begin(), p2.end(), p1.begin(), p1.end());

    CHECK(it1 == p1.begin());
    CHECK(it2 == std::next(p2.begin(), 1));

    // const iterators via std::cbegin and std::cend
    const auto it3 = find_first_two_of(std::cbegin(p1), std::cend(p1), std::cbegin(p2), std::cend(p2));
    const auto it4 = find_first_two_of(std::cbegin(p2), std::cend(p2), std::cbegin(p1), std::cend(p1));

    CHECK(it3 == p1.begin());
    CHECK(it4 == std::next(p2.begin(), 1));
}
