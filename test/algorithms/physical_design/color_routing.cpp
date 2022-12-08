//
// Created by marcel on 07.02.22.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/color_routing.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/routing_utils.hpp>

using namespace fiction;

template <typename Spec, typename Impl>
void check_color_routing(const Spec& spec, Impl& impl, const std::vector<routing_objective<Impl>>& objectives,
                         color_routing_params ps = {})
{
    const auto success = color_routing(impl, objectives, ps);

    CHECK(success);

    check_eq(spec, impl);
}

TEST_CASE("Simple wire connection", "[color-routing]")
{
    auto spec_layout = blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>();

    const auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    color_routing_params ps{};

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}

TEST_CASE("Two paths wire connections", "[color-routing]")
{
    auto spec_layout = blueprints::unbalanced_and_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::unbalanced_and_layout<cart_gate_clk_lyt>();

    const auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    color_routing_params ps{};

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}

TEST_CASE("Three paths wire connections", "[color-routing]")
{
    auto spec_layout = blueprints::three_wire_paths_gate_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::three_wire_paths_gate_layout<cart_gate_clk_lyt>();

    const auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    color_routing_params ps{};

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}

TEST_CASE("Direct gate connections", "[color-routing]")
{
    auto spec_layout = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();

    const auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    color_routing_params ps{};

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}

TEST_CASE("Partial routing", "[color-routing]")
{
    auto spec_layout = blueprints::use_and_gate_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::use_and_gate_layout<cart_gate_clk_lyt>();

    auto objectives = extract_routing_objectives(impl_layout);
    objectives.push_back({{0, 3}, {3, 0}});  // additional unsatisfiable objective

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    color_routing_params ps{};
    ps.conduct_partial_routing = true;

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}

TEST_CASE("Routing with crossings", "[color-routing]")
{
    auto spec_layout = blueprints::crossing_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::crossing_layout<cart_gate_clk_lyt>();

    auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    color_routing_params ps{};
    ps.crossings = true;

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}
