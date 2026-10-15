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
 * @brief Tests for `fiction/physical_design/generate_edge_intersection_graph.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/physical_design/generate_edge_intersection_graph.hpp>
#include <fiction/physical_design/routing_utils.hpp>

#include <algorithm>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;

TEST_CASE("EPG on 2x2 layouts", "[generate-edge-intersection-graph]")
{
    generate_edge_intersection_graph_stats st{};

    SECTION("2DDWave")
    {
        const cartesian_gate_layout layout{{2, 2}, clocking::twoddwave()};

        SECTION("Disjoint paths")
        {
            // (0,0) --> (0,1), (1,0) --> (1,1)
            const std::vector<routing_objective> objectives{{.source = {0, 0}, .target = {0, 1}},
                                                            {.source = {1, 0}, .target = {1, 1}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 2);

            CHECK(graph.size_vertices() == 2);
            CHECK(graph.size_edges() == 0);

            CHECK(graph.at_vertex(0) == layout_coordinate_path{{{0, 0}, {0, 1}}});
            CHECK(graph.at_vertex(1) == layout_coordinate_path{{{1, 0}, {1, 1}}});
        }
        SECTION("Non-disjoint paths")
        {
            // (0,0) --> (1,1), (1,0) --> (1,1)
            const std::vector<routing_objective> objectives{{.source = {0, 0}, .target = {1, 1}},
                                                            {.source = {1, 0}, .target = {1, 1}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 2);

            CHECK(graph.size_vertices() == 3);
            CHECK(graph.size_edges() == 2);
        }
        SECTION("Unsatisfiable objective")
        {
            // (0,0) --> (1,1), (1,1) --> (0,0)
            const std::vector<routing_objective> objectives{{.source = {0, 0}, .target = {1, 1}},
                                                            {.source = {1, 1}, .target = {0, 0}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 1);
            CHECK(st.cliques.size() == 1);

            CHECK(graph.size_vertices() == 2);
            CHECK(graph.size_edges() == 1);
        }
    }
    SECTION("USE")
    {
        cartesian_gate_layout layout{{2, 2}, clocking::use()};

        SECTION("Disjoint paths")
        {
            // (0,0) --> (1,0), (1,1) --> (0,1)
            const std::vector<routing_objective> objectives{{.source = {0, 0}, .target = {1, 0}},
                                                            {.source = {1, 1}, .target = {0, 1}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 2);

            CHECK(graph.size_vertices() == 2);
            CHECK(graph.size_edges() == 0);

            CHECK(graph.at_vertex(0) == layout_coordinate_path{{{0, 0}, {1, 0}}});
            CHECK(graph.at_vertex(1) == layout_coordinate_path{{{1, 1}, {0, 1}}});
        }
        SECTION("Non-disjoint paths")
        {
            // (0,0) --> (0,1), (1,0) --> (1,1)
            const std::vector<routing_objective> objectives{{.source = {0, 0}, .target = {0, 1}},
                                                            {.source = {1, 0}, .target = {1, 1}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 2);

            CHECK(graph.size_vertices() == 2);
            CHECK(graph.size_edges() == 1);
        }
        SECTION("Unsatisfiable objective")
        {
            layout.resize({1, 3});
            // (0,2) --> (0,0), (0,0) --> (0,2)
            const std::vector<routing_objective> objectives{{.source = {0, 2}, .target = {0, 0}},
                                                            {.source = {0, 0}, .target = {0, 2}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 1);
            CHECK(st.cliques.size() == 1);

            CHECK(graph.size_vertices() == 1);
            CHECK(graph.size_edges() == 0);
        }
    }
}

TEST_CASE("EPG on 3x3 layouts", "[generate-edge-intersection-graph]")
{
    generate_edge_intersection_graph_stats st{};

    SECTION("2DDWave")
    {
        const cartesian_gate_layout layout{{3, 3}, clocking::twoddwave()};

        SECTION("(0,1) to (2,1) and (1,0) to (1,2)")
        {
            const std::vector<routing_objective> objectives{{.source = {0, 1}, .target = {2, 1}},
                                                            {.source = {1, 0}, .target = {1, 2}}};

            SECTION("without crossings")
            {
                // disable crossings
                const generate_edge_intersection_graph_params ps{.crossings = false};

                const auto graph = generate_edge_intersection_graph(layout, objectives, ps, &st);

                CHECK(st.cliques.size() == 2);
                CHECK(st.cliques.front().size() == 1);
                CHECK(st.cliques.back().size() == 1);

                CHECK(graph.size_vertices() == 2);  // 2 valid paths == 2 vertices
                CHECK(graph.size_edges() == 1);     // paths are mutual exclusive
            }
            SECTION("with crossings")
            {
                // enable crossings
                const generate_edge_intersection_graph_params ps{.crossings = true};

                const auto graph = generate_edge_intersection_graph(layout, objectives, ps, &st);

                CHECK(st.cliques.size() == 2);
                CHECK(st.cliques.front().size() == 1);
                CHECK(st.cliques.back().size() == 1);

                CHECK(graph.size_vertices() == 2);  // 2 valid paths == 2 vertices
                CHECK(graph.size_edges() == 0);     // paths are NOT mutual exclusive due to crossings
            }
        }
    }
}

TEST_CASE("EPG on 4x4 layouts", "[generate-edge-intersection-graph]")
{
    generate_edge_intersection_graph_stats st{};

    SECTION("2DDWave")
    {
        const cartesian_gate_layout layout{{4, 4}, clocking::twoddwave()};

        SECTION("(0,0) to (3,3) without obstruction")
        {
            const std::vector<routing_objective> objectives{{.source = {0, 0}, .target = {3, 3}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 1);
            CHECK(st.cliques.front().size() == 20);

            CHECK(graph.size_vertices() == 20);  // 20 valid paths == 20 vertices
            CHECK(graph.size_edges() == 190);    // a complete graph has (n(n-1))/2 edges
        }
    }
    SECTION("USE")
    {
        const cartesian_gate_layout layout{{4, 4}, clocking::use()};

        SECTION("(0,0) to (3,3) without obstruction")
        {
            const std::vector<routing_objective> objectives{{.source = {0, 0}, .target = {3, 3}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 1);
            CHECK(st.cliques.front().size() == 4);

            CHECK(graph.size_vertices() == 4);  // 4 valid paths == 4 vertices
            CHECK(graph.size_edges() == 6);     // a complete graph has (n(n-1))/2 edges
        }
    }
}

TEST_CASE("EPG on 4x4 layouts with obstruction", "[generate-edge-intersection-graph]")
{
    generate_edge_intersection_graph_stats st{};

    SECTION("2DDWave")
    {
        cartesian_gate_layout layout{{4, 4}, clocking::twoddwave()};

        SECTION("(0,0) to (3,3) with obstruction")  // 19 valid paths
        {
            // create a PI as obstruction
            layout.create_pi("obstruction", {3, 0});  // blocks 1 path

            const std::vector<routing_objective> objectives{{.source = {0, 0}, .target = {3, 3}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 1);
            CHECK(st.cliques.front().size() == 19);

            CHECK(graph.size_vertices() == 19);  // 19 valid paths == 19 vertices
            CHECK(graph.size_edges() == 171);    // a complete graph has (n(n-1))/2 edges
        }
    }
    SECTION("USE")
    {
        cartesian_gate_layout layout{{4, 4}, clocking::use()};

        SECTION("(0,0) to (3,3) with obstruction")  // 1 valid path
        {
            // create a PI as obstruction
            layout.create_pi("obstruction", {3, 0});  // blocks 3 paths

            const std::vector<routing_objective> objectives{{.source = {0, 0}, .target = {3, 3}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 1);
            CHECK(st.cliques.front().size() == 1);

            CHECK(graph.size_vertices() == 1);  // 1 valid paths == 1 vertex
            CHECK(graph.size_edges() == 0);     // a complete graph has (n(n-1))/2 edges
        }
    }
}

TEST_CASE("Limited paths retain interior conflicts", "[generate-edge-intersection-graph]")
{
    const cartesian_gate_layout          layout{{3, 3, 2}, clocking::twoddwave()};
    const std::vector<routing_objective> objectives{{{0, 1}, {2, 1}}, {{1, 0}, {1, 2}}};
    for (const auto limit : {1u, 2u})
    {
        const auto graph = generate_edge_intersection_graph(layout, objectives, {.path_limit = limit});
        CHECK(graph.size_vertices() == 2);
        CHECK(graph.size_edges() == 1);
    }
}

TEST_CASE("Candidate labels retain their routing objective", "[generate-edge-intersection-graph]")
{
    const cartesian_gate_layout          layout{{2, 2}, clocking::twoddwave()};
    const std::vector<routing_objective> objectives{{{0, 0}, {1, 1}}, {{1, 1}, {0, 0}}, {{0, 0}, {0, 1}}};
    for (const auto limited : {false, true})
    {
        generate_edge_intersection_graph_params params{};
        if (limited)
        {
            params.path_limit = 2;
        }
        generate_edge_intersection_graph_stats st{};
        const auto graph = generate_edge_intersection_graph(layout, objectives, params, &st);
        CHECK(graph.size_vertices() == 3);
        CHECK(st.number_of_unroutable_objectives == 1);
        REQUIRE(st.cliques.size() == 2);
        CHECK(st.cliques[0].size() == 2);
        CHECK(st.cliques[1].size() == 1);
        CHECK(std::count(st.objective_indices.begin(), st.objective_indices.end(), 0) == 2);
        CHECK(std::count(st.objective_indices.begin(), st.objective_indices.end(), 2) == 1);
        for (const auto& clique : st.cliques)
        {
            for (const auto id : clique)
            {
                const auto& objective = objectives.at(st.objective_indices.at(id));
                CHECK(graph.at_vertex(id).source() == objective.source);
                CHECK(graph.at_vertex(id).target() == objective.target);
            }
        }
    }
}
