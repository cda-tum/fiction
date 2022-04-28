//
// Created by marcel on 04.02.22.
//

#include "catch.hpp"

#include <fiction/algorithms/graph/graph_coloring.hpp>

#include <algorithm>
#include <cstdint>
#include <vector>

#include <combinations.h>
#include <undirected_graph.h>
#include <undirected_pair.h>

using namespace fiction;

template <typename Graph>
void check_coloring_engines(const Graph& graph, const std::size_t expected_chromatic_number,
                            std::vector<typename Graph::vertex_id_type> clique = {})
{
    determine_vertex_coloring_stats pst{};

    SECTION("MCS")
    {
        const auto coloring = determine_vertex_coloring(graph, {graph_coloring_engine::MCS, {}, {}, true}, &pst);

        REQUIRE(pst.coloring_verified.has_value());
        CHECK(pst.coloring_verified.value() == true);
        CHECK(pst.chromatic_number == expected_chromatic_number);
        CHECK(pst.color_frequency > 0);
    }
    SECTION("DSATUR")
    {
        const auto coloring = determine_vertex_coloring(graph, {graph_coloring_engine::DSATUR, {}, {}, true}, &pst);

        REQUIRE(pst.coloring_verified.has_value());
        CHECK(pst.coloring_verified.value() == true);
        CHECK(pst.chromatic_number == expected_chromatic_number);
        CHECK(pst.color_frequency > 0);
    }
    SECTION("LmXRLF")
    {
        const auto coloring = determine_vertex_coloring(graph, {graph_coloring_engine::LMXRLF, {}, {}, true}, &pst);

        REQUIRE(pst.coloring_verified.has_value());
        CHECK(pst.coloring_verified.value() == true);
        CHECK(pst.chromatic_number >= expected_chromatic_number);  // randomized, could be non-optimal
        CHECK(pst.color_frequency > 0);
    }
    SECTION("TabuCol")
    {
        const auto coloring = determine_vertex_coloring(
            graph, {graph_coloring_engine::TABUCOL, {}, {expected_chromatic_number}, true}, &pst);

        REQUIRE(pst.coloring_verified.has_value());
        CHECK(pst.coloring_verified.value() == true);
        CHECK(pst.chromatic_number == expected_chromatic_number);
        CHECK(pst.color_frequency > 0);
    }
    SECTION("SAT")
    {
        determine_vertex_coloring_sat_params<Graph> sat_params{};
        sat_params.cliques = {{clique}};

        SECTION("linear ascending search")
        {
            sat_params.sat_search_tactic = graph_coloring_sat_search_tactic::LINEARLY_ASCENDING;

            const auto coloring =
                determine_vertex_coloring(graph, {graph_coloring_engine::SAT, sat_params, {}, true}, &pst);

            REQUIRE(pst.coloring_verified.has_value());
            CHECK(pst.coloring_verified.value() == true);
            CHECK(pst.chromatic_number == expected_chromatic_number);
            CHECK(pst.color_frequency > 0);
        }
        SECTION("linear descending search")
        {
            sat_params.sat_search_tactic = graph_coloring_sat_search_tactic::LINEARLY_DESCENDING;

            const auto coloring =
                determine_vertex_coloring(graph, {graph_coloring_engine::SAT, sat_params, {}, true}, &pst);

            REQUIRE(pst.coloring_verified.has_value());
            CHECK(pst.coloring_verified.value() == true);
            CHECK(pst.chromatic_number == expected_chromatic_number);
            CHECK(pst.color_frequency > 0);
        }
        SECTION("binary search")
        {
            sat_params.sat_search_tactic = graph_coloring_sat_search_tactic::BINARY_SEARCH;

            const auto coloring =
                determine_vertex_coloring(graph, {graph_coloring_engine::SAT, sat_params, {}, true}, &pst);

            REQUIRE(pst.coloring_verified.has_value());
            CHECK(pst.coloring_verified.value() == true);
            CHECK(pst.chromatic_number == expected_chromatic_number);
            CHECK(pst.color_frequency > 0);
        }
    }
}

TEST_CASE("Petersen graph", "[graph-coloring]")
{
    // using a graph data structure with dummy values for its associated types
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;

    // The Peterson graph is 3-chromatic
    graph petersen_graph{};

    // create 10 vertices
    for (std::size_t i = 0; i < 10; i++)
    {
        petersen_graph.insert_vertex(i, {});
    }

    petersen_graph.insert_edge(0, 1, {});
    petersen_graph.insert_edge(0, 5, {});
    petersen_graph.insert_edge(1, 2, {});
    petersen_graph.insert_edge(1, 6, {});
    petersen_graph.insert_edge(2, 3, {});
    petersen_graph.insert_edge(2, 7, {});
    petersen_graph.insert_edge(3, 4, {});
    petersen_graph.insert_edge(3, 8, {});
    petersen_graph.insert_edge(4, 0, {});
    petersen_graph.insert_edge(4, 9, {});
    petersen_graph.insert_edge(5, 7, {});
    petersen_graph.insert_edge(6, 8, {});
    petersen_graph.insert_edge(7, 9, {});
    petersen_graph.insert_edge(8, 5, {});

    SECTION("with clique information")
    {
        check_coloring_engines(petersen_graph, 3, {0, 1});
    }
    SECTION("without clique information")
    {
        check_coloring_engines(petersen_graph, 3, {});
    }
}

TEST_CASE("Golomb graph", "[graph-coloring]")
{
    // using a graph data structure with dummy values for its associated types
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;

    // The Golomb graph is 4-chromatic
    graph golomb_graph{};

    // create 10 vertices
    for (std::size_t i = 0; i < 10; i++)
    {
        golomb_graph.insert_vertex(i, {});
    }

    golomb_graph.insert_edge(0, 1, {});
    golomb_graph.insert_edge(0, 6, {});
    golomb_graph.insert_edge(0, 9, {});
    golomb_graph.insert_edge(1, 2, {});
    golomb_graph.insert_edge(1, 9, {});
    golomb_graph.insert_edge(2, 3, {});
    golomb_graph.insert_edge(2, 7, {});
    golomb_graph.insert_edge(2, 9, {});
    golomb_graph.insert_edge(3, 4, {});
    golomb_graph.insert_edge(3, 9, {});
    golomb_graph.insert_edge(4, 5, {});
    golomb_graph.insert_edge(4, 8, {});
    golomb_graph.insert_edge(4, 9, {});
    golomb_graph.insert_edge(5, 0, {});
    golomb_graph.insert_edge(5, 9, {});
    golomb_graph.insert_edge(6, 7, {});
    golomb_graph.insert_edge(7, 8, {});
    golomb_graph.insert_edge(8, 6, {});

    SECTION("with clique information")
    {
        check_coloring_engines(golomb_graph, 4, {0, 1});
    }
    SECTION("without clique information")
    {
        check_coloring_engines(golomb_graph, 4);
    }
}

TEST_CASE("Moser spindle", "[graph-coloring]")
{
    // using a graph data structure with dummy values for its associated types
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;

    // The Moser spindle is 4-chromatic
    graph moser_spindle{};

    // create 7 vertices
    for (std::size_t i = 0; i < 7; i++)
    {
        moser_spindle.insert_vertex(i, {});
    }

    moser_spindle.insert_edge(0, 1, {});
    moser_spindle.insert_edge(0, 5, {});
    moser_spindle.insert_edge(0, 6, {});
    moser_spindle.insert_edge(1, 2, {});
    moser_spindle.insert_edge(1, 5, {});
    moser_spindle.insert_edge(2, 3, {});
    moser_spindle.insert_edge(2, 5, {});
    moser_spindle.insert_edge(3, 4, {});
    moser_spindle.insert_edge(3, 6, {});
    moser_spindle.insert_edge(4, 0, {});
    moser_spindle.insert_edge(4, 6, {});

    SECTION("with clique information")
    {
        check_coloring_engines(moser_spindle, 4, {0, 1, 5});
    }
    SECTION("without clique information")
    {
        check_coloring_engines(moser_spindle, 4);
    }
}

template <typename Graph>
Graph create_complete_graph(std::vector<typename Graph::vertex_id_type>& vertices) noexcept
{
    Graph k{};

    std::for_each(vertices.cbegin(), vertices.cend(), [&k](const auto v) { k.insert_vertex(v, {}); });
    combinations::for_each_combination(vertices.begin(), vertices.begin() + 2, vertices.end(),
                                       [&k](const auto begin, [[maybe_unused]] const auto end)
                                       {
                                           k.insert_edge(*begin, *(begin + 1), {});
                                           return false;  // keep looping
                                       });

    return k;
}

TEST_CASE("K", "[graph-coloring]")
{
    // using a graph data structure with dummy values for its associated types
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;

    // Ki is i-chromatic

    SECTION("2")
    {
        std::vector<std::size_t> vertices{{0, 1}};

        check_coloring_engines(create_complete_graph<graph>(vertices), 2, {{0, 1}});
    }
    SECTION("3")
    {
        std::vector<std::size_t> vertices{{0, 1, 2}};

        check_coloring_engines(create_complete_graph<graph>(vertices), 3, {{0, 1, 2}});
    }
    SECTION("4")
    {
        std::vector<std::size_t> vertices{{0, 1, 2, 3}};

        check_coloring_engines(create_complete_graph<graph>(vertices), 4, {{0, 1, 2, 3}});
    }
    SECTION("5")
    {
        std::vector<std::size_t> vertices{{0, 1, 2, 3, 4}};

        check_coloring_engines(create_complete_graph<graph>(vertices), 5, {{0, 1, 2, 3, 4}});
    }
    SECTION("6")
    {
        std::vector<std::size_t> vertices{{0, 1, 2, 3, 4, 5}};

        check_coloring_engines(create_complete_graph<graph>(vertices), 6, {{0, 1, 2, 3, 4, 5}});
    }
    SECTION("10")
    {
        std::vector<std::size_t> vertices{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

        check_coloring_engines(create_complete_graph<graph>(vertices), 10, {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}});
    }
}

TEST_CASE("String IDs (mostly a compile test)", "[graph-coloring]")
{
    // using a graph data structure with dummy values for its associated types
    using graph = undirected_graph<std::string, uint8_t, undirected_pair<std::string>, uint8_t>;

    // C5 is 3-chromatic
    graph c5{};

    // create 5 vertices
    for (std::size_t i = 0; i < 5; i++)
    {
        c5.insert_vertex(std::to_string(i), {});
    }

    c5.insert_edge("0", "1", {});
    c5.insert_edge("1", "2", {});
    c5.insert_edge("2", "3", {});
    c5.insert_edge("3", "4", {});
    c5.insert_edge("4", "0", {});

    check_coloring_engines(c5, 3, {"0", "1"});
}
