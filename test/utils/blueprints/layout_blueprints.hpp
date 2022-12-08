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

}  // namespace blueprints

#endif  // FICTION_LAYOUT_BLUEPRINTS_HPP
