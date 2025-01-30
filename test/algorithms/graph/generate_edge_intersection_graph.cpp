//
// Created by marcel on 03.02.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/graph/generate_edge_intersection_graph.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("EPG on 2x2 layouts", "[generate-edge-intersection-graph]")
{
    using gate_lyt = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    generate_edge_intersection_graph_stats st{};

    SECTION("2DDWave")
    {
        const gate_lyt layout{{1, 1}, twoddwave_clocking<gate_lyt>()};

        SECTION("Disjoint paths")
        {
            // (0,0) --> (0,1), (1,0) --> (1,1)
            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 0}, {0, 1}}, {{1, 0}, {1, 1}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 2);

            CHECK(graph.size_vertices() == 2);
            CHECK(graph.size_edges() == 0);

            CHECK(graph.at_vertex(0) == layout_coordinate_path<gate_lyt>{{{0, 0}, {0, 1}}});
            CHECK(graph.at_vertex(1) == layout_coordinate_path<gate_lyt>{{{1, 0}, {1, 1}}});
        }
        SECTION("Non-disjoint paths")
        {
            // (0,0) --> (1,1), (1,0) --> (1,1)
            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 0}, {1, 1}}, {{1, 0}, {1, 1}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 2);

            CHECK(graph.size_vertices() == 3);
            CHECK(graph.size_edges() == 2);
        }
        SECTION("Unsatisfiable objective")
        {
            // (0,0) --> (1,1), (1,1) --> (0,0)
            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 0}, {1, 1}}, {{1, 1}, {0, 0}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 1);
            CHECK(st.cliques.size() == 1);

            CHECK(graph.size_vertices() == 2);
            CHECK(graph.size_edges() == 1);
        }
    }
    SECTION("USE")
    {
        gate_lyt layout{{1, 1}, use_clocking<gate_lyt>()};

        SECTION("Disjoint paths")
        {
            // (0,0) --> (1,0), (1,1) --> (0,1)
            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 0}, {1, 0}}, {{1, 1}, {0, 1}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 2);

            CHECK(graph.size_vertices() == 2);
            CHECK(graph.size_edges() == 0);

            CHECK(graph.at_vertex(0) == layout_coordinate_path<gate_lyt>{{{0, 0}, {1, 0}}});
            CHECK(graph.at_vertex(1) == layout_coordinate_path<gate_lyt>{{{1, 1}, {0, 1}}});
        }
        SECTION("Non-disjoint paths")
        {
            // (0,0) --> (0,1), (1,0) --> (1,1)
            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 0}, {0, 1}}, {{1, 0}, {1, 1}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 2);

            CHECK(graph.size_vertices() == 2);
            CHECK(graph.size_edges() == 1);
        }
        SECTION("Unsatisfiable objective")
        {
            layout.resize(aspect_ratio<gate_lyt>{0, 2});
            // (0,2) --> (0,0), (0,0) --> (0,2)
            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 2}, {0, 0}}, {{0, 0}, {0, 2}}};

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
    using gate_lyt = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    generate_edge_intersection_graph_stats st{};

    SECTION("2DDWave")
    {
        const gate_lyt layout{{2, 2}, twoddwave_clocking<gate_lyt>()};

        SECTION("(0,1) to (2,1) and (1,0) to (1,2)")
        {
            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 1}, {2, 1}}, {{1, 0}, {1, 2}}};

            SECTION("without crossings")
            {
                // disable crossings
                const generate_edge_intersection_graph_params ps{false};

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
                const generate_edge_intersection_graph_params ps{true};

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
    using gate_lyt = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    generate_edge_intersection_graph_stats st{};

    SECTION("2DDWave")
    {
        const gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) without obstruction")
        {
            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 0}, {3, 3}}};

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
        const gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) without obstruction")
        {
            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 0}, {3, 3}}};

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
    using gate_lyt = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    generate_edge_intersection_graph_stats st{};

    SECTION("2DDWave")
    {
        gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with obstruction")  // 19 valid paths
        {
            // create a PI as obstruction
            layout.create_pi("obstruction", {3, 0});  // blocks 1 path

            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 0}, {3, 3}}};

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
        gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with obstruction")  // 1 valid path
        {
            // create a PI as obstruction
            layout.create_pi("obstruction", {3, 0});  // blocks 3 paths

            const std::vector<routing_objective<gate_lyt>> objectives{{{0, 0}, {3, 3}}};

            const auto graph = generate_edge_intersection_graph(layout, objectives, {}, &st);

            CHECK(st.number_of_unroutable_objectives == 0);
            CHECK(st.cliques.size() == 1);
            CHECK(st.cliques.front().size() == 1);

            CHECK(graph.size_vertices() == 1);  // 1 valid paths == 1 vertex
            CHECK(graph.size_edges() == 0);     // a complete graph has (n(n-1))/2 edges
        }
    }
}
