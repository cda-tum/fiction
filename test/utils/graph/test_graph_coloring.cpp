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
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/allocation_failure.hpp"

#include <fiction/utils/graph/graph_coloring.hpp>

#include <bill/sat/interface/common.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <new>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <combinations.h>
#include <undirected_graph.h>
#include <undirected_pair.h>

using namespace fiction;
using namespace fiction::test;
using namespace fiction::utils::graph;

/**
 * @brief Checks the returned assignment and independently computed coloring statistics.
 * @tparam Graph Graph type.
 * @tparam Color Color identifier type.
 * @param graph Input graph.
 * @param coloring Returned vertex colors.
 * @param pst Reported coloring statistics.
 */
template <typename Graph, typename Color>
void check_coloring(const Graph& graph, const vertex_coloring<Graph, Color>& coloring,
                    const determine_vertex_coloring_stats<Color>& pst)
{
    REQUIRE(pst.coloring_verified.has_value());
    CHECK(pst.coloring_verified.value_or(false));
    REQUIRE(coloring.size() == graph.size_vertices());
    /** @brief Observed multiplicity of each color, independent of its identifier. */
    std::map<Color, std::size_t> frequencies{};
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
    CHECK(frequencies.size() <= pst.chromatic_number);
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
    petersen_graph.insert_edge(9, 6, {});

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

TEST_CASE("TabuCol caps oversized color bounds to the vertex count", "[graph-coloring]")
{
    /** @brief Graph with integral identifiers and unused vertex and edge data. */
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;
    /** @brief Bounds include a value whose low 32 bits are zero. */
    const auto bound = GENERATE(std::uint64_t{2}, std::uint64_t{1} << 32u);
    CAPTURE(bound);
    if (bound > std::numeric_limits<std::size_t>::max())
    {
        SKIP("The color bound exceeds the platform's size_t range");
    }
    /** @brief Isolated vertices require no randomized conflict repair. */
    graph isolated{};
    isolated.insert_vertex(0, {});
    isolated.insert_vertex(1, {});
    /** @brief Statistics checked against the returned assignment. */
    determine_vertex_coloring_stats stats{};
    /** @brief Assignment for the requested representable upper bound. */
    const auto coloring =
        determine_vertex_coloring(isolated,
                                  {.engine           = graph_coloring_engine::TABUCOL,
                                   .heuristic_params = {.k_color_value = static_cast<std::size_t>(bound)},
                                   .verify_coloring_after_computation = true},
                                  &stats);
    check_coloring(isolated, coloring, stats);
}

TEMPLATE_TEST_CASE("Heuristic colorings preserve the full integral vertex ID range", "[graph-coloring]", std::int64_t,
                   std::uint64_t, std::int8_t, std::uint8_t, bool)
{
    /** @brief Graph whose vertex identifiers retain the test type's full range. */
    using graph = undirected_graph<TestType, uint8_t, undirected_pair<TestType>, uint8_t>;
    /** @brief Adjacent vertices use the smallest and largest representable identifiers. */
    graph sparse{};
    sparse.insert_vertex(std::numeric_limits<TestType>::min(), {});
    sparse.insert_vertex(std::numeric_limits<TestType>::max(), {});
    sparse.insert_edge(std::numeric_limits<TestType>::min(), std::numeric_limits<TestType>::max(), {});
    /** @brief Deterministic engines share the integral identifier conversion contract. */
    const auto engine = GENERATE(graph_coloring_engine::MCS, graph_coloring_engine::DSATUR);
    CAPTURE(engine, std::numeric_limits<TestType>::min(), std::numeric_limits<TestType>::max());
    /** @brief Statistics checked against the returned assignment. */
    determine_vertex_coloring_stats stats{};
    /** @brief Assignment with no color-count narrowing or randomized search. */
    const auto coloring =
        determine_vertex_coloring(sparse, {.engine = engine, .verify_coloring_after_computation = true}, &stats);
    check_coloring(sparse, coloring, stats);
}

TEST_CASE("TabuCol rejects an unrepresentable effective color count", "[graph-coloring]")
{
    /** @brief Graph storage for the count-boundary check. */
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;
    /** @brief Graph-count boundary without allocating billions of vertices. */
    struct oversized_graph : graph
    {
        /** @brief Reports the first count outside the vendor's integer range. */
        [[nodiscard]] static unsigned int size_vertices() noexcept
        {
            return static_cast<unsigned int>(std::numeric_limits<int>::max()) + 1u;
        }
    };
    /** @brief Small storage with an intentionally oversized reported count. */
    oversized_graph isolated{};
    isolated.insert_vertex(0, {});
    isolated.insert_vertex(1, {});
    CHECK_THROWS_AS(determine_vertex_coloring(
                        isolated, {.engine           = graph_coloring_engine::TABUCOL,
                                   .heuristic_params = {.k_color_value = std::numeric_limits<std::size_t>::max()}}),
                    std::invalid_argument);
}

TEST_CASE("Heuristic coloring counts wide color classes", "[graph-coloring]")
{
    /** @brief Graph whose isolated vertices can share one color. */
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;
    /** @brief One more vertex than an eight-bit color identifier can count. */
    graph isolated{};
    for (std::size_t id = 0; id < 256; ++id)
    {
        isolated.insert_vertex(id, {});
    }
    /** @brief Frequencies count vertices independently of the color identifier type. */
    determine_vertex_coloring_stats<std::uint8_t> stats{};
    /** @brief Assignment with a narrow color type and a wider color class. */
    const auto coloring = determine_vertex_coloring<graph, std::uint8_t>(
        isolated, {.engine = graph_coloring_engine::MCS, .verify_coloring_after_computation = true}, &stats);

    check_coloring(isolated, coloring, stats);
    CHECK(stats.color_frequency == 256);
}

TEST_CASE("Heuristic coloring reports no colors for empty results", "[graph-coloring]")
{
    /** @brief Graph used for an empty input or an unsuccessful bounded search. */
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;
    /** @brief Empty graph unless the bounded search case adds a vertex. */
    graph input{};
    /** @brief Independent empty-input and unsuccessful-search cases. */
    const auto empty_input = GENERATE(true, false);
    CAPTURE(empty_input);
    /** @brief Engine and verification settings for this independent case. */
    determine_vertex_coloring_params<graph> params{.engine                            = graph_coloring_engine::MCS,
                                                   .verify_coloring_after_computation = true};
    if (!empty_input)
    {
        input.insert_vertex(0, {});
        params.engine = graph_coloring_engine::TABUCOL;
    }
    /** @brief Statistics of an empty returned assignment. */
    determine_vertex_coloring_stats stats{};
    /** @brief Empty assignment for either case. */
    const auto coloring = determine_vertex_coloring(input, params, &stats);

    CHECK(coloring.empty());
    CHECK(stats.chromatic_number == 0);
    CHECK(stats.color_frequency == 0);
    REQUIRE(stats.coloring_verified.has_value());
    CHECK(stats.coloring_verified.value_or(false) == (input.size_vertices() == 0));
}

TEST_CASE("Heuristic coloring propagates allocation failure", "[graph-coloring]")
{
    require_allocation_failure_support();
    /** @brief Wide identifiers exercise decimal conversion as well as graph storage. */
    using graph = undirected_graph<std::uint64_t, uint8_t, undirected_pair<std::uint64_t>, uint8_t>;
    /** @brief Adjacent vertices whose decimal identifiers have twenty digits. */
    graph input{};
    input.insert_vertex(std::numeric_limits<std::uint64_t>::max(), {});
    input.insert_vertex(std::numeric_limits<std::uint64_t>::max() - 1, {});
    input.insert_edge(std::numeric_limits<std::uint64_t>::max(), std::numeric_limits<std::uint64_t>::max() - 1, {});
    /** @brief Confirms that the sweep exercises at least one failed allocation. */
    bool saw_failure{};
    /** @brief Number of successful allocations before the next injected failure. */
    for (std::size_t failure{};; ++failure)
    {
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        /** @brief Statistics checked only after fault injection is disabled. */
        determine_vertex_coloring_stats stats{};
        try
        {
            allocation_budget   = failure;
            const auto coloring = determine_vertex_coloring(
                input, {.engine = graph_coloring_engine::MCS, .verify_coloring_after_computation = true}, &stats);
            allocation_budget.reset();
            check_coloring(input, coloring, stats);
            break;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
            saw_failure = true;
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
    }
    CHECK(saw_failure);
}

TEST_CASE("Coloring rejects unrepresentable integral colors", "[graph-coloring]")
{
    /** @brief Triangle needs more colors than bool can represent. */
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;
    /** @brief Complete graph on three vertices. */
    graph triangle{};
    /** @brief The three vertex identifiers. */
    for (std::size_t id = 0; id < 3; ++id)
    {
        triangle.insert_vertex(id, {});
    }
    triangle.insert_edge(0, 1, {});
    triangle.insert_edge(1, 2, {});
    triangle.insert_edge(0, 2, {});
    /** @brief Heuristic and exact engines share the output color capacity contract. */
    const auto engine = GENERATE(graph_coloring_engine::MCS, graph_coloring_engine::SAT);
    CAPTURE(engine);
    CHECK_THROWS_AS((determine_vertex_coloring<graph, bool>(triangle, {.engine = engine})), std::overflow_error);
}

TEST_CASE("SAT search keeps its working colors independent of the output type", "[graph-coloring]")
{
    /** @brief Three-vertex path needs exactly the two colors representable by bool. */
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;
    /** @brief Path with more vertices than output color values. */
    graph path{};
    /** @brief The three path vertex identifiers. */
    for (std::size_t id = 0; id < 3; ++id)
    {
        path.insert_vertex(id, {});
    }
    path.insert_edge(0, 1, {});
    path.insert_edge(1, 2, {});
    /** @brief Statistics checked against the returned bool assignment. */
    determine_vertex_coloring_stats<bool> stats{};
    /** @brief Descending search begins with three internal colors and ends with two output colors. */
    const auto coloring = determine_vertex_coloring<graph, bool>(
        path,
        {.engine     = graph_coloring_engine::SAT,
         .sat_params = {.sat_search_tactic = graph_coloring_sat_search_tactic::LINEARLY_DESCENDING},
         .verify_coloring_after_computation = true},
        &stats);
    check_coloring(path, coloring, stats);
    CHECK(stats.chromatic_number == 2);
}

TEST_CASE("SAT coloring handles an empty graph before descending search", "[graph-coloring]")
{
    /** @brief Graph with no vertices or edges. */
    using graph = undirected_graph<std::size_t, uint8_t, undirected_pair<std::size_t>, uint8_t>;
    /** @brief Empty input needs no search. */
    const graph input{};
    /** @brief Optional verification remains observable for an empty result. */
    const auto verify = GENERATE(false, true);
    CAPTURE(verify);
    /** @brief Statistics of the empty assignment. */
    determine_vertex_coloring_stats stats{};
    /** @brief Empty coloring from a search tactic whose first bound is zero. */
    const auto coloring = determine_vertex_coloring(
        input,
        {.engine     = graph_coloring_engine::SAT,
         .sat_params = {.sat_search_tactic = graph_coloring_sat_search_tactic::LINEARLY_DESCENDING},
         .verify_coloring_after_computation = verify},
        &stats);
    CHECK(coloring.empty());
    CHECK(stats.chromatic_number == 0);
    CHECK(stats.color_frequency == 0);
    CHECK(stats.coloring_verified.has_value() == verify);
    if (verify)
    {
        CHECK(stats.coloring_verified.value_or(false));
    }
}
