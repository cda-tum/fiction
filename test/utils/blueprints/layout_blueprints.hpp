//
// Created by marcel on 24.05.21.
//

#ifndef FICTION_LAYOUT_BLUEPRINTS_HPP
#define FICTION_LAYOUT_BLUEPRINTS_HPP

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/traits.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/traits.hpp>

namespace blueprints
{

template <typename GateLyt>
GateLyt straight_wire_gate_layout() noexcept
{
    GateLyt layout{{2, 2}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 1});
    const auto w1 = layout.create_buf(x1, {1, 1});
    layout.create_po(w1, "f1", {2, 1});

    return layout;
}

template <typename GateLyt>
GateLyt three_wire_paths_gate_layout() noexcept
{
    GateLyt layout{{4, 4}, fiction::use_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {0, 2});
    const auto x3 = layout.create_pi("x3", {0, 4});

    const auto p1 = layout.create_buf(layout.create_buf(layout.create_buf(x1, {1, 0}), {2, 0}), {3, 0});
    const auto p2 = layout.create_buf(layout.create_buf(layout.create_buf(x2, {1, 2}), {2, 2}), {3, 2});
    const auto p3 = layout.create_buf(layout.create_buf(layout.create_buf(x3, {1, 4}), {2, 4}), {3, 4});

    layout.create_po(p1, "f1", {4, 0});
    layout.create_po(p2, "f2", {4, 2});
    layout.create_po(p3, "f3", {4, 4});

    return layout;
}

template <typename GateLyt>
GateLyt xor_maj_gate_layout() noexcept
{
    REQUIRE(mockturtle::has_create_node_v<GateLyt>);

    GateLyt layout{typename GateLyt::aspect_ratio{3, 2, 0}, fiction::open_clocking<GateLyt>()};

    layout.assign_clock_number({2, 0}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({1, 1}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({3, 1}, static_cast<typename GateLyt::clock_number_t>(0));

    layout.assign_clock_number({1, 0}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({2, 1}, static_cast<typename GateLyt::clock_number_t>(1));

    layout.assign_clock_number({0, 0}, static_cast<typename GateLyt::clock_number_t>(2));
    layout.assign_clock_number({2, 2}, static_cast<typename GateLyt::clock_number_t>(2));

    const auto a = layout.create_pi("a", {1, 1});
    const auto b = layout.create_pi("b", {2, 0});
    const auto c = layout.create_pi("c", {3, 1});

    kitty::dynamic_truth_table tt_maj(3u), tt_xor(2u);
    kitty::create_from_hex_string(tt_maj, "e8");
    kitty::create_from_hex_string(tt_xor, "6");

    const auto n_maj = layout.create_node({a, b, c}, tt_maj, {2, 1});
    const auto n_xor = layout.create_node({a, b}, tt_xor, {1, 0});

    layout.create_po(n_maj, "f1", {2, 2});
    layout.create_po(n_xor, "f2", {0, 0});

    return layout;
}

template <typename GateLyt>
GateLyt and_or_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{3, 1, 0}, fiction::open_clocking<GateLyt>()};

    layout.assign_clock_number({2, 0}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({1, 0}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({0, 0}, static_cast<typename GateLyt::clock_number_t>(2));

    layout.assign_clock_number({1, 1}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({2, 1}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({3, 1}, static_cast<typename GateLyt::clock_number_t>(2));

    const auto x1 = layout.create_pi("x1", {2, 0});
    const auto x2 = layout.create_pi("x2", {1, 1});
    const auto a  = layout.create_and(x1, x2, {1, 0});
    const auto o  = layout.create_or(x2, x1, {2, 1});
    layout.create_po(a, "f1", {0, 0});
    layout.create_po(o, "f2", {3, 1});

    return layout;
}

template <typename GateLyt>
GateLyt and_not_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{3, 1, 0}, fiction::open_clocking<GateLyt>()};

    layout.assign_clock_number({2, 0}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({1, 0}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({0, 0}, static_cast<typename GateLyt::clock_number_t>(2));

    layout.assign_clock_number({1, 1}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({2, 1}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({3, 1}, static_cast<typename GateLyt::clock_number_t>(2));

    const auto x1 = layout.create_pi("x1", {2, 0});
    const auto x2 = layout.create_pi("x2", {1, 1});
    const auto a1 = layout.create_and(x1, x2, {1, 0});
    const auto n1 = layout.create_not(x2, {2, 1});
    layout.create_po(a1, "f1", {0, 0});
    layout.create_po(n1, "f2", {3, 1});

    return layout;
}

template <typename GateLyt>
GateLyt or_not_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{2, 2, 0}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {1, 0});
    const auto x2 = layout.create_pi("x2", {0, 1});
    const auto o1 = layout.create_or(x1, x2, {1, 1});
    const auto n1 = layout.create_not(o1, {1, 2});
    layout.create_po(n1, "f1", {2, 2});

    return layout;
}

template <typename GateLyt>
GateLyt use_and_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{3, 3, 0}, fiction::use_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 1});
    const auto x2 = layout.create_pi("x2", {3, 3});

    const auto w1 = layout.create_buf(x1, {0, 0});
    const auto w2 = layout.create_buf(w1, {1, 0});
    const auto w3 = layout.create_buf(w2, {1, 1});

    const auto w4 = layout.create_buf(x2, {2, 3});
    const auto w5 = layout.create_buf(w4, {1, 3});
    const auto w6 = layout.create_buf(w5, {0, 3});
    const auto w7 = layout.create_buf(w6, {0, 2});

    const auto a1 = layout.create_and(w3, w7, {1, 2});

    const auto w8 = layout.create_buf(a1, {2, 2});

    layout.create_po(w8, "f1", {3, 2});

    return layout;
}

template <typename GateLyt>
GateLyt res_maj_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{2, 2, 0}, fiction::res_clocking<GateLyt>()};
    layout.assign_clock_number({0, 0}, static_cast<typename GateLyt::clock_number_t>(0));

    const auto x1 = layout.create_pi("x1", {0, 1});
    const auto x2 = layout.create_pi("x2", {1, 0});
    const auto x3 = layout.create_pi("x3", {2, 1});

    const auto maj = layout.create_maj(x1, x2, x3, {1, 1});

    layout.create_po(maj, "f1", {1, 2});

    return layout;
}

template <typename GateLyt>
GateLyt single_input_tautology_gate_layout() noexcept
{
    REQUIRE(mockturtle::has_create_node_v<GateLyt>);

    GateLyt layout{typename GateLyt::aspect_ratio{2, 0, 0}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 0});

    kitty::dynamic_truth_table tt_t(1u);
    kitty::create_from_hex_string(tt_t, "3");

    const auto n = layout.create_node({x1}, tt_t, {1, 0});

    layout.create_po(n, "f1", {2, 0});

    return layout;
}

template <typename GateLyt>
GateLyt tautology_gate_layout() noexcept
{
    REQUIRE(mockturtle::has_create_node_v<GateLyt>);

    GateLyt layout{typename GateLyt::aspect_ratio{2, 2, 1}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {1, 0});
    const auto x2 = layout.create_pi("x2", {0, 1});

    kitty::dynamic_truth_table tt_t(2u);
    kitty::create_from_hex_string(tt_t, "F");

    const auto n_xor = layout.create_node({x1, x2}, tt_t, {1, 1});

    layout.create_po(n_xor, "f1", {2, 1});

    return layout;
}

template <typename GateLyt>
GateLyt res_tautology_gate_layout() noexcept
{
    REQUIRE(mockturtle::has_create_node_v<GateLyt>);

    GateLyt layout{typename GateLyt::aspect_ratio{2, 2, 0}, fiction::res_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 1});
    const auto x2 = layout.create_pi("x2", {1, 0});
    const auto x3 = layout.create_pi("x3", {2, 1});

    kitty::dynamic_truth_table tt_t(3u);
    kitty::create_from_hex_string(tt_t, "FF");

    const auto n = layout.create_node({x1, x2, x3}, tt_t, {1, 1});

    layout.create_po(n, "f1", {1, 2});

    return layout;
}

template <typename GateLyt>
GateLyt open_tautology_gate_layout() noexcept
{
    REQUIRE(mockturtle::has_create_node_v<GateLyt>);

    GateLyt layout{typename GateLyt::aspect_ratio{2, 2, 0}, fiction::open_clocking<GateLyt>()};

    layout.assign_clock_number({0, 0}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({1, 0}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({0, 2}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({1, 2}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({1, 1}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({2, 1}, static_cast<typename GateLyt::clock_number_t>(2));

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {1, 0});
    const auto x3 = layout.create_pi("x3", {0, 2});
    const auto x4 = layout.create_pi("x4", {1, 2});

    kitty::dynamic_truth_table tt_t(4u);
    kitty::create_from_hex_string(tt_t, "FFFF");

    const auto n = layout.create_node({x1, x2, x3, x4}, tt_t, {1, 1});

    layout.create_po(n, "f1", {2, 1});

    return layout;
}

template <typename GateLyt>
GateLyt crossing_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{3, 2, 1}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {1, 0});
    const auto x2 = layout.create_pi("x2", {0, 1});
    const auto x3 = layout.create_pi("x3", {2, 0});
    const auto x4 = layout.create_pi("x4", {0, 2});

    const auto a1 = layout.create_and(x1, x2, {1, 1});

    const auto b1 = layout.create_buf(x3, {2, 1});
    const auto b2 = layout.create_buf(x4, {1, 2});

    const auto a2 = layout.create_and(b1, b2, {2, 2});

    const auto c = layout.create_buf(a1, {2, 1, 1});

    layout.create_po(c, "f1", {3, 1});
    layout.create_po(a2, "f2", {3, 2});

    return layout;
}

template <typename GateLyt>
GateLyt fanout_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{2, 2, 1}};

    layout.assign_clock_number({0, 0}, static_cast<typename GateLyt::clock_number_t>(3));
    layout.assign_clock_number({0, 1}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({0, 2}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({1, 0}, static_cast<typename GateLyt::clock_number_t>(2));
    layout.assign_clock_number({1, 1}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({1, 2}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({2, 0}, static_cast<typename GateLyt::clock_number_t>(3));
    layout.assign_clock_number({2, 1}, static_cast<typename GateLyt::clock_number_t>(2));
    layout.assign_clock_number({2, 2}, static_cast<typename GateLyt::clock_number_t>(3));

    const auto x1 = layout.create_pi("x1", {0, 1});

    const auto f1 = layout.create_buf(x1, {1, 1});
    const auto f2 = layout.create_buf(f1, {2, 1});
    const auto w1 = layout.create_buf(f2, {2, 2});

    layout.create_po(f1, "f1", {1, 0});
    layout.create_po(f2, "f2", {2, 0});
    layout.create_po(w1, "f3", {1, 2});

    return layout;
}

template <typename GateLyt>
GateLyt unbalanced_and_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{3, 2, 0}};

    layout.assign_clock_number({1, 0}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({2, 0}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({3, 0}, static_cast<typename GateLyt::clock_number_t>(2));

    layout.assign_clock_number({0, 2}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({1, 2}, static_cast<typename GateLyt::clock_number_t>(2));
    layout.assign_clock_number({2, 1}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({2, 2}, static_cast<typename GateLyt::clock_number_t>(3));

    const auto x1 = layout.create_pi("x1", {0, 2});
    const auto x2 = layout.create_pi("x1", {1, 0});

    const auto w1 = layout.create_buf(x1, {1, 2});
    const auto w2 = layout.create_buf(w1, {2, 2});
    const auto w3 = layout.create_buf(w2, {2, 1});

    const auto a1 = layout.create_and(w3, x2, {2, 0});

    layout.create_po(a1, "f1", {3, 0});

    return layout;
}

template <typename GateLyt>
GateLyt non_structural_all_function_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{2, 4, 0}, fiction::open_clocking<GateLyt>()};

    layout.assign_clock_number({0, 0}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({1, 0}, static_cast<typename GateLyt::clock_number_t>(0));
    layout.assign_clock_number({2, 0}, static_cast<typename GateLyt::clock_number_t>(0));

    layout.assign_clock_number({0, 1}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({1, 1}, static_cast<typename GateLyt::clock_number_t>(1));
    layout.assign_clock_number({2, 1}, static_cast<typename GateLyt::clock_number_t>(1));

    layout.assign_clock_number({0, 2}, static_cast<typename GateLyt::clock_number_t>(2));
    layout.assign_clock_number({1, 2}, static_cast<typename GateLyt::clock_number_t>(2));
    layout.assign_clock_number({2, 2}, static_cast<typename GateLyt::clock_number_t>(3));

    layout.assign_clock_number({0, 3}, static_cast<typename GateLyt::clock_number_t>(3));
    layout.assign_clock_number({1, 3}, static_cast<typename GateLyt::clock_number_t>(3));
    layout.assign_clock_number({2, 3}, static_cast<typename GateLyt::clock_number_t>(3));

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {1, 0});
    const auto x3 = layout.create_pi("x3", {2, 0});

    layout.create_and(x1, x2, {0, 1});
    layout.create_or(x1, x2, {1, 1});
    layout.create_xor(x1, x2, {2, 1});

    const auto m = layout.create_maj(x1, x2, x3, {0, 2});
    const auto f = layout.create_buf(m, {1, 2});
    const auto w = layout.create_buf(f, {2, 2});

    layout.create_not(w, {2, 3});
    layout.create_po(f, "po", {1, 3});

    layout.create_nand(x1, x2, {0, 4});
    layout.create_nor(x1, x2, {1, 4});

    return layout;
}

template <typename GateLyt>
GateLyt se_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{2, 1, 0}, fiction::twoddwave_clocking<GateLyt>()};

    layout.assign_synchronization_element({1, 0}, 1);

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {2, 0});

    const auto w1 = layout.create_buf(x1, {0, 1});
    const auto w2 = layout.create_buf(x2, {1, 0});

    const auto o1 = layout.create_or(w1, w2, {1, 1});
    layout.create_po(o1, "f1", {2, 1});

    return layout;
}

template <typename GateLyt>
GateLyt shifted_cart_and_or_inv_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{4, 2, 0},
                   fiction::columnar_clocking<GateLyt>(fiction::num_clks::THREE)};

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {0, 1});
    const auto x3 = layout.create_pi("x3", {0, 2});

    const auto a1 = layout.create_and(x1, x2, {1, 0});

    const auto w1 = layout.create_buf(x3, {1, 1});
    const auto w2 = layout.create_buf(a1, {2, 0});
    const auto i1 = layout.create_not(w1, {2, 1});

    const auto o1 = layout.create_or(w2, i1, {3, 0});

    layout.create_po(o1, "f1", {4, 0});

    return layout;
}

template <typename GateLyt>
GateLyt row_clocked_and_xor_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{2, 3, 0}, fiction::row_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {1, 0});
    const auto x3 = layout.create_pi("x3", {2, 0});

    const auto a1 = layout.create_and(x1, x2, {1, 1});

    const auto w1 = layout.create_buf(x3, {2, 1});

    const auto xor1 = layout.create_xor(w1, a1, {1, 2});

    layout.create_po(xor1, "f1", {1, 3});

    return layout;
}

template <typename GateLyt>
GateLyt unclockable_gate_layout() noexcept
{
    GateLyt layout{typename GateLyt::aspect_ratio{2, 2, 0}, fiction::open_clocking<GateLyt>()};

    const auto x0  = layout.create_pi("x0", {0, 0});
    const auto fo  = layout.create_buf(x0, {0, 1});
    const auto inv = layout.create_not(fo, {0, 2});
    const auto w   = layout.create_buf(inv, {1, 2});
    const auto a   = layout.create_and(fo, w, {1, 1});
    layout.create_po(a, "f", {2, 1});

    return layout;
}

template <typename GateLyt>
GateLyt optimization_layout() noexcept
{
    GateLyt layout{{2, 3, 1}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {2, 0});

    const auto w1 = layout.create_buf(x1, {0, 1});
    const auto w2 = layout.create_buf(w1, {0, 2});
    const auto w3 = layout.create_buf(w2, {1, 2});
    const auto w4 = layout.create_buf(x2, {2, 1});

    const auto and1 = layout.create_and(w3, w4, {2, 2});

    layout.create_po(w2, "f1", {0, 3});
    layout.create_po(and1, "f2", {2, 3});

    return layout;
}

template <typename GateLyt>
GateLyt optimization_layout_corner_case_outputs_1() noexcept
{
    GateLyt layout{{2, 3, 1}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {0, 1});

    const auto not1 = layout.create_not(x1, {1, 0});
    const auto w1   = layout.create_buf(x2, {1, 1});
    const auto w2   = layout.create_buf(not1, {1, 1, 1});
    const auto not2 = layout.create_not(w2, {1, 2});

    layout.create_po(not2, "f1", {1, 3});
    layout.create_po(w1, "f2", {2, 1});

    return layout;
}

template <typename GateLyt>
GateLyt optimization_layout_corner_case_outputs_2() noexcept
{
    GateLyt layout{{3, 2, 1}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {1, 0});

    const auto not1 = layout.create_not(x1, {0, 1});
    const auto w1   = layout.create_buf(not1, {1, 1});
    const auto not2 = layout.create_not(w1, {2, 1});
    const auto w2   = layout.create_buf(x2, {1, 1, 1});

    layout.create_po(not2, "f1", {3, 1});
    layout.create_po(w2, "f2", {1, 2});

    return layout;
}

template <typename GateLyt>
GateLyt planar_unoptimized_layout() noexcept
{
    GateLyt layout{{4, 4, 0}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {2, 0});
    const auto x2 = layout.create_pi("x2", {0, 2});

    const auto w1   = layout.create_buf(x1, {2, 1});
    const auto w2   = layout.create_buf(x2, {1, 2});
    const auto and1 = layout.create_and(w1, w2, {2, 2});
    const auto w3   = layout.create_buf(and1, {3, 2});
    const auto not2 = layout.create_not(w3, {4, 2});
    const auto w4   = layout.create_buf(not2, {4, 3});

    layout.create_po(w4, "f1", {4, 4});

    return layout;
}

template <typename GateLyt>
GateLyt planar_optimization_layout() noexcept
{
    GateLyt layout{{2, 2, 1}, fiction::twoddwave_clocking<GateLyt>()};

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto x2 = layout.create_pi("x2", {0, 1});
    layout.create_pi("x3", {0, 2});
    layout.create_pi("x4", {2, 0});

    const auto w1   = layout.create_buf(x1, {1, 0});
    const auto w2   = layout.create_buf(x2, {1, 1});
    const auto w3   = layout.create_buf(w1, {1, 1, 1});
    const auto not1 = layout.create_not(w3, {1, 2});

    layout.create_po(w2, "f1", {2, 1});
    layout.create_po(not1, "f2", {2, 2});

    return layout;
}

template <typename CellLyt>
CellLyt single_layer_qca_and_gate() noexcept
{
    CellLyt layout{{4, 4}, "AND"};

    layout.assign_cell_type({0, 2}, fiction::qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 4}, fiction::qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 0}, fiction::qca_technology::cell_type::CONST_0);
    layout.assign_cell_type({2, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 3}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 2}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 2}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 2}, fiction::qca_technology::cell_type::OUTPUT);

    layout.assign_cell_name({0, 2}, "a");
    layout.assign_cell_name({2, 4}, "b");
    layout.assign_cell_name({4, 2}, "f");

    return layout;
}

template <typename CellLyt>
CellLyt two_layer_qca_wire_crossing() noexcept
{
    CellLyt layout{{4, 4, 1}, "Crossover"};

    layout.assign_cell_type({0, 2}, fiction::qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 0}, fiction::qca_technology::cell_type::INPUT);

    layout.assign_cell_type({2, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 3}, fiction::qca_technology::cell_type::NORMAL);

    layout.assign_cell_type({0, 2, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 2, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 2, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 2, 1}, fiction::qca_technology::cell_type::NORMAL);

    layout.assign_cell_mode({0, 2}, fiction::qca_technology::cell_mode::VERTICAL);
    layout.assign_cell_mode({0, 2, 1}, fiction::qca_technology::cell_mode::CROSSOVER);
    layout.assign_cell_mode({1, 2, 1}, fiction::qca_technology::cell_mode::CROSSOVER);
    layout.assign_cell_mode({2, 2, 1}, fiction::qca_technology::cell_mode::CROSSOVER);
    layout.assign_cell_mode({3, 2, 1}, fiction::qca_technology::cell_mode::CROSSOVER);
    layout.assign_cell_mode({4, 2, 1}, fiction::qca_technology::cell_mode::CROSSOVER);
    layout.assign_cell_mode({4, 2}, fiction::qca_technology::cell_mode::VERTICAL);

    layout.assign_cell_type({4, 2}, fiction::qca_technology::cell_type::OUTPUT);
    layout.assign_cell_type({2, 4}, fiction::qca_technology::cell_type::OUTPUT);

    layout.assign_cell_name({0, 2}, "a");
    layout.assign_cell_name({2, 0}, "b");
    layout.assign_cell_name({4, 2}, "a'");
    layout.assign_cell_name({2, 4}, "b'");

    return layout;
}

template <typename CellLyt>
CellLyt single_layer_inml_maj_gate() noexcept
{
    CellLyt layout{{4, 4}, "MAJ"};

    layout.assign_cell_type({0, 0}, fiction::inml_technology::cell_type::INPUT);
    layout.assign_cell_type({0, 2}, fiction::inml_technology::cell_type::INPUT);
    layout.assign_cell_type({0, 4}, fiction::inml_technology::cell_type::INPUT);
    layout.assign_cell_type({1, 0}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 2}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 4}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 0}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 1}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 3}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 4}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 2}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 2}, fiction::inml_technology::cell_type::OUTPUT);

    layout.assign_cell_name({0, 0}, "a");
    layout.assign_cell_name({0, 2}, "b");
    layout.assign_cell_name({0, 4}, "c");
    layout.assign_cell_name({4, 2}, "f");

    return layout;
}

template <typename CellLyt>
CellLyt single_layer_inml_coupler_with_inverter() noexcept
{
    CellLyt layout{{11, 4}, "Coupler with inverter"};

    layout.assign_cell_type({0, 2}, fiction::inml_technology::cell_type::INPUT);
    layout.assign_cell_type({1, 2}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 1}, fiction::inml_technology::cell_type::FANOUT_COUPLER_MAGNET);
    layout.assign_cell_type({3, 2}, fiction::inml_technology::cell_type::FANOUT_COUPLER_MAGNET);
    layout.assign_cell_type({3, 3}, fiction::inml_technology::cell_type::FANOUT_COUPLER_MAGNET);
    layout.assign_cell_type({4, 1}, fiction::inml_technology::cell_type::FANOUT_COUPLER_MAGNET);
    layout.assign_cell_type({4, 3}, fiction::inml_technology::cell_type::FANOUT_COUPLER_MAGNET);
    layout.assign_cell_type({5, 1}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({6, 1}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({5, 3}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({6, 3}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({7, 1}, fiction::inml_technology::cell_type::INVERTER_MAGNET);
    layout.assign_cell_type({8, 1}, fiction::inml_technology::cell_type::INVERTER_MAGNET);
    layout.assign_cell_type({9, 1}, fiction::inml_technology::cell_type::INVERTER_MAGNET);
    layout.assign_cell_type({10, 1}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({7, 3}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({8, 3}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({9, 3}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({10, 3}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({11, 1}, fiction::inml_technology::cell_type::OUTPUT);
    layout.assign_cell_type({11, 3}, fiction::inml_technology::cell_type::OUTPUT);

    layout.assign_cell_name({0, 2}, "a");
    layout.assign_cell_name({11, 1}, "not a");
    layout.assign_cell_name({11, 3}, "a");

    return layout;
}

template <typename CellLyt>
CellLyt single_layer_inml_crosswire() noexcept
{
    CellLyt layout{{5, 2}, "Crosswire"};

    layout.assign_cell_type({0, 0}, fiction::inml_technology::cell_type::INPUT);
    layout.assign_cell_type({0, 2}, fiction::inml_technology::cell_type::INPUT);
    layout.assign_cell_type({1, 0}, fiction::inml_technology::cell_type::CROSSWIRE_MAGNET);
    layout.assign_cell_type({1, 2}, fiction::inml_technology::cell_type::CROSSWIRE_MAGNET);
    layout.assign_cell_type({2, 1}, fiction::inml_technology::cell_type::CROSSWIRE_MAGNET);
    layout.assign_cell_type({3, 0}, fiction::inml_technology::cell_type::CROSSWIRE_MAGNET);
    layout.assign_cell_type({3, 2}, fiction::inml_technology::cell_type::CROSSWIRE_MAGNET);
    layout.assign_cell_type({4, 0}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 2}, fiction::inml_technology::cell_type::NORMAL);
    layout.assign_cell_type({5, 0}, fiction::inml_technology::cell_type::OUTPUT);
    layout.assign_cell_type({5, 2}, fiction::inml_technology::cell_type::OUTPUT);

    layout.assign_cell_name({0, 0}, "a");
    layout.assign_cell_name({0, 2}, "b");
    layout.assign_cell_name({5, 0}, "b'");
    layout.assign_cell_name({5, 2}, "a'");

    return layout;
}
/**
 * This layout represents a BDL wire comprising of 3 BDL pairs and an output perturber SiDB.
 *
 */
template <typename Lyt>
Lyt bdl_wire() noexcept
{
    static_assert(fiction::is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(fiction::has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    Lyt lyt{{24, 0}, "BDL wire"};

    lyt.assign_cell_type({0, 0, 0}, Lyt::cell_type::INPUT);
    lyt.assign_cell_type({3, 0, 0}, Lyt::cell_type::INPUT);

    lyt.assign_cell_type({6, 0, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, Lyt::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, Lyt::cell_type::OUTPUT);

    // output perturber
    lyt.assign_cell_type({24, 0, 0}, Lyt::cell_type::NORMAL);

    return lyt;
};
/**
 * This layout represents the AND Gate, as proposed in the paper
 * titled \"SiQAD: A Design and Simulation Tool for Atomic Silicon Quantum Dot Circuits\" by Samuel Sze Hang Ng, Jacob
 * Retallick, Hsi Nien Chiu, Robert Lupoiu, Lucian Livadaru, Taleana Huff, Mohammad Rashidi, Wyatt Vine, Thomas Dienel,
 * Robert A. Wolkow, and Konrad Walus in IEEE TRANSACTIONS ON NANOTECHNOLOGY, Volume 19, 2020.
 */
template <typename Lyt>
Lyt siqad_and_gate() noexcept
{
    static_assert(fiction::is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(fiction::has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    Lyt lyt{};

    lyt.assign_cell_type({0, 0, 1}, Lyt::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, Lyt::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, Lyt::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 1}, Lyt::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, Lyt::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, Lyt::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 9, 1}, Lyt::cell_type::NORMAL);

    return lyt;
};
/**
 * This layout represents the AND Gate, as proposed in the paper
 * titled \"Hexagons are the Bestagons: Design Automation for Silicon Dangling Bond Logic\" by
 * Marcel Walter, Samuel Sze Hang Ng, Konrad Walus, and Robert Wille in DAC 2022.
 *
 * (https://github.com/cda-tum/mnt-bestagon-library/blob/main/bestagon-gates/2i1o_and/21_hex_inputsdbp_and_v19.sqd)
 */
template <typename Lyt>
Lyt bestagon_and_gate() noexcept
{
    static_assert(fiction::is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(fiction::has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(fiction::has_siqad_coord_v<Lyt>, "Lyt is not an SiDB layout");

    Lyt lyt{};

    lyt.assign_cell_type({36, 1, 0}, Lyt::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, Lyt::cell_type::INPUT);

    lyt.assign_cell_type({38, 0, 0}, Lyt::cell_type::INPUT);
    lyt.assign_cell_type({0, 0, 0}, Lyt::cell_type::INPUT);

    lyt.assign_cell_type({23, 9, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({18, 11, 1}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({18, 9, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({19, 8, 0}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({20, 14, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({19, 13, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({32, 2, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({26, 4, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({24, 5, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({32, 18, 0}, Lyt::cell_type::OUTPUT);
    lyt.assign_cell_type({30, 17, 0}, Lyt::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, Lyt::cell_type::NORMAL);

    return lyt;
};
/**
 * This layout represents the AND Gate implemented on the H-Si(111)-1x1 surface, as proposed in the paper
 * titled \"Unlocking Flexible Silicon Dangling Bond Logic Designs on Alternative Silicon Orientations\" by
 * Samuel Sze Hang Ng, Jan Drewniok, Marcel Walter, Jacob Retallick, Robert Wille, and Konrad Walus.
 *
 * (https://github.com/samuelngsh/si-111-paper-supplementary/blob/main/bestagon-111-gates/gates/AND_mu_032_0.sqd)
 */
template <typename Lyt>
Lyt and_gate_111() noexcept
{
    static_assert(fiction::is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(fiction::has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(fiction::is_sidb_lattice_111_v<Lyt>, "Lyt should have 111 as lattice orientation");
    static_assert(fiction::has_siqad_coord_v<Lyt>, "Lyt is not an SiDB layout");

    Lyt lyt{};

    lyt.assign_cell_type({0, 0, 0}, Lyt::cell_type::INPUT);
    lyt.assign_cell_type({1, 1, 1}, Lyt::cell_type::INPUT);

    lyt.assign_cell_type({25, 0, 0}, Lyt::cell_type::INPUT);
    lyt.assign_cell_type({23, 1, 1}, Lyt::cell_type::INPUT);

    lyt.assign_cell_type({4, 4, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({21, 4, 0}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({5, 5, 1}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({19, 5, 1}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({17, 8, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({8, 8, 0}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({9, 9, 1}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({15, 9, 1}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({13, 17, 0}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({16, 18, 0}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({10, 18, 0}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({15, 21, 1}, Lyt::cell_type::NORMAL);
    lyt.assign_cell_type({17, 23, 0}, Lyt::cell_type::NORMAL);

    lyt.assign_cell_type({19, 25, 1}, Lyt::cell_type::OUTPUT);
    lyt.assign_cell_type({21, 27, 0}, Lyt::cell_type::OUTPUT);

    lyt.assign_cell_type({23, 29, 1}, Lyt::cell_type::NORMAL);

    return lyt;
};

}  // namespace blueprints

#endif  // FICTION_LAYOUT_BLUEPRINTS_HPP
