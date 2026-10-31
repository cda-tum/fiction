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
 * @brief Tests for `fiction/utils/graph/graph_coloring.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/utils/graph/graph_coloring.hpp>

#include <bill/sat/interface/common.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <combinations.h>
#include <undirected_graph.h>
#include <undirected_pair.h>

using namespace fiction;
using namespace fiction::utils::graph;

/**
 * @brief Checks the returned assignment and independently computed coloring statistics.
 * @tparam Graph Graph type.
 * @param graph Input graph.
 * @param coloring Returned vertex colors.
 * @param pst Reported coloring statistics.
 */
template <typename Graph>
void check_coloring(const Graph& graph, const vertex_coloring<Graph, std::size_t>& coloring,
                    const determine_vertex_coloring_stats<>& pst)
{
    REQUIRE(pst.coloring_verified.has_value());
    CHECK(pst.coloring_verified.value_or(false));
    REQUIRE(coloring.size() == graph.size_vertices());
    /** @brief Observed multiplicity of each color, independent of its identifier. */
    std::map<std::size_t, std::size_t> frequencies{};
    for (auto vertex = graph.begin_vertices(); vertex != graph.end_vertices(); ++vertex)
    {
        CAPTURE(vertex->first);
        REQUIRE(coloring.contains(vertex->first));
        ++frequencies[coloring.at(vertex->first)];
    }
    for (auto edge = graph.begin_edges(); edge != graph.end_edges(); ++edge)
    {
        /** @brief Endpoints whose returned colors must differ. */
        const auto& [source, target] = edge->first;
        CAPTURE(source, target);
        CHECK(coloring.at(source) != coloring.at(target));
    }
    REQUIRE_FALSE(frequencies.empty());
    CHECK(frequencies.size() == pst.chromatic_number);
    /** @brief Largest observed color class; ties may use any color identifier. */
    const auto most_frequent =
        std::ranges::max_element(frequencies, [](const auto& a, const auto& b) { return a.second < b.second; });
    CHECK(pst.color_frequency == most_frequent->second);
    REQUIRE(frequencies.contains(pst.most_frequent_color));
    CHECK(frequencies.at(pst.most_frequent_color) == pst.color_frequency);
}

/**
 * @brief Checks each SAT search tactic against the expected chromatic number.
 * @tparam Graph Graph type.
 * @param graph Graph to color.
 * @param expected_chromatic_number Exact chromatic number.
 * @param sat_params SAT backend and clique information.
 */
template <typename Graph>
void check_sat_search_tactics(const Graph& graph, const std::size_t expected_chromatic_number,
                              determine_vertex_coloring_sat_params<Graph> sat_params)
{
    for (const auto& [name, tactic] :
         {std::pair{"linearly ascending search", graph_coloring_sat_search_tactic::LINEARLY_ASCENDING},
          std::pair{"linearly descending search", graph_coloring_sat_search_tactic::LINEARLY_DESCENDING},
          std::pair{"binary search", graph_coloring_sat_search_tactic::BINARY_SEARCH}})
    {
        DYNAMIC_SECTION(name)
        {
            sat_params.sat_search_tactic = tactic;
            /** @brief Statistics from this backend and search tactic. */
            determine_vertex_coloring_stats pst{};
            /** @brief Returned assignment checked independently of the production verifier. */
            const auto coloring = determine_vertex_coloring(graph,
                                                            {.engine     = graph_coloring_engine::SAT,
                                                             .sat_params = sat_params,
                                                             .verify_coloring_after_computation = true},
                                                            &pst);
            check_coloring(graph, coloring, pst);
            CAPTURE(pst.chromatic_number, expected_chromatic_number);
            CHECK((pst.chromatic_number == expected_chromatic_number));
        }
    }
}

/**
 * @brief Checks supported SAT backends with the supplied clique information.
 * @tparam Graph Graph type.
 * @param graph Graph to color.
 * @param expected_chromatic_number Exact chromatic number.
 * @param clique Known clique, or empty to omit clique information.
 */
template <typename Graph>
void check_sat_coloring_engine(const Graph& graph, const std::size_t expected_chromatic_number,
                               std::vector<typename Graph::vertex_id_type> clique = {})
{
    determine_vertex_coloring_sat_params<Graph> sat_params{.cliques = {{clique}}};

    SECTION("SAT")
    {
        SECTION("ghack")
        {
            sat_params.sat_engine = bill::solvers::ghack;

            check_sat_search_tactics(graph, expected_chromatic_number, sat_params);
        }
        SECTION("glucose_41")
        {
            sat_params.sat_engine = bill::solvers::glucose_41;

            check_sat_search_tactics(graph, expected_chromatic_number, sat_params);
        }
#ifndef BILL_WINDOWS_PLATFORM
        SECTION("maple")
        {
            sat_params.sat_engine = bill::solvers::maple;

            check_sat_search_tactics(graph, expected_chromatic_number, sat_params);
        }
        SECTION("bmcg")
        {
            sat_params.sat_engine = bill::solvers::bmcg;

            check_sat_search_tactics(graph, expected_chromatic_number, sat_params);
        }
#endif
    }
}

/** @brief Checks each heuristic coloring engine against the expected chromatic number. @tparam Graph Graph type. */
template <typename Graph>
void check_brian_crites_engine(const Graph& graph, const std::size_t expected_chromatic_number)
{
    determine_vertex_coloring_stats pst{};

    SECTION("MCS")
    {
        const auto coloring = determine_vertex_coloring(
            graph, {.engine = graph_coloring_engine::MCS, .verify_coloring_after_computation = true}, &pst);

        check_coloring(graph, coloring, pst);
        CHECK(pst.chromatic_number == expected_chromatic_number);
    }
    SECTION("DSATUR")
    {
        const auto coloring = determine_vertex_coloring(
            graph, {.engine = graph_coloring_engine::DSATUR, .verify_coloring_after_computation = true}, &pst);

        check_coloring(graph, coloring, pst);
        CHECK(pst.chromatic_number == expected_chromatic_number);
    }
    SECTION("LmXRLF")
    {
        const auto coloring = determine_vertex_coloring(
            graph, {.engine = graph_coloring_engine::LMXRLF, .verify_coloring_after_computation = true}, &pst);

        // randomized, could be non-optimal
        check_coloring(graph, coloring, pst);
        CHECK(pst.chromatic_number >= expected_chromatic_number);
    }
    SECTION("TabuCol")
    {
        const auto coloring =
            determine_vertex_coloring(graph,
                                      {.engine           = graph_coloring_engine::TABUCOL,
                                       .heuristic_params = {.k_color_value = expected_chromatic_number},
                                       .verify_coloring_after_computation = true},
                                      &pst);

        check_coloring(graph, coloring, pst);
        CHECK(pst.chromatic_number == expected_chromatic_number);
    }
}

/** @brief Checks all coloring engines with and without a known clique. @tparam Graph Graph type. */
template <typename Graph>
void check_coloring_engines(const Graph& graph, const std::size_t expected_chromatic_number,
                            const std::vector<typename Graph::vertex_id_type>& clique = {})
{
    check_brian_crites_engine(graph, expected_chromatic_number);

    SECTION("with clique information")
    {
        check_sat_coloring_engine(graph, expected_chromatic_number, clique);
    }
    // shortcut: omit graphs with at least 30 edges without cliques; increase the bound when SAT runtime permits.
    if (graph.size_edges() < 30)
    {
        SECTION("without clique information")
        {
            check_sat_coloring_engine(graph, expected_chromatic_number);
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

    check_coloring_engines(petersen_graph, 3, {0, 1});
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

    check_coloring_engines(golomb_graph, 4, {0, 1});
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

    check_coloring_engines(moser_spindle, 4, {0, 1, 5});
}

/** @brief Builds a complete graph from the given vertex identifiers. @tparam Graph Graph type. */
template <typename Graph>
Graph create_complete_graph(std::vector<typename Graph::vertex_id_type>& vertices)
{
    Graph k{};

    if (vertices.size() >= 2)
    {
        std::for_each(vertices.cbegin(), vertices.cend(), [&k](const auto& v) { k.insert_vertex(v, {}); });
        combinations::for_each_combination(vertices.begin(), vertices.begin() + 2, vertices.end(),
                                           [&k](const auto begin, [[maybe_unused]] const auto end)
                                           {
                                               k.insert_edge(*begin, *(begin + 1), {});
                                               return false;  // keep looping
                                           });
    }

    return k;
}

TEST_CASE("K", "[graph-coloring]")
{
    // using a graph data structure with dummy values for its associated types
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;

    /** @brief Complete-graph sizes, each equal to its chromatic number. */
    const auto size = GENERATE(2u, 3u, 4u, 5u, 6u, 10u);
    CAPTURE(size);
    /** @brief Contiguous vertex identifiers also form a maximum clique. */
    std::vector<std::size_t> vertices(size);
    std::iota(vertices.begin(), vertices.end(), std::size_t{0});
    check_coloring_engines(create_complete_graph<graph>(vertices), size, vertices);
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
