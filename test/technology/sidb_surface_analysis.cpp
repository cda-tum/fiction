//
// Created by marcel on 07.03.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/utils/truth_table_utils.hpp>
#include <fiction/technology/fcn/cell_technologies.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/sidb/primitives/defect_surface.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/technology/sidb_surface_analysis.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <kitty/dynamic_truth_table.hpp>

using namespace fiction;

/**
 * This is simply a test library, its gates do not represent real entities that would work when fabricated.
 */
class dummy_gate_library : public fcn::gate_library<sidb::technology, 3, 3>
{
  public:
    dummy_gate_library() = delete;

    static gate_functions get_functional_implementations() noexcept
    {
        static const gate_functions implementations{
            {{networks::utils::create_id_tt(), {LINE}}, {networks::utils::create_and_tt(), {Y}}}};

        return implementations;
    }

    static gate_ports<fcn::port_position> get_gate_ports() noexcept
    {
        static const gate_ports<fcn::port_position> ports{
            {{LINE, {{{fcn::port_position(0, 1)}, {fcn::port_position(2, 1)}}}},
             {Y, {{{fcn::port_position(0, 0), fcn::port_position(0, 2)}, {fcn::port_position(2, 1)}}}}}};

        return ports;
    }

  private:
    static constexpr const fcn_gate LINE{cell_list_to_gate<char>({{
        {' ', 'x', ' '},
        {' ', 'x', ' '},
        {' ', 'x', ' '},
    }})};

    static constexpr const fcn_gate Y{cell_list_to_gate<char>({{
        {'x', ' ', 'x'},
        {' ', 'x', ' '},
        {' ', 'x', ' '},
    }})};
};

template <typename TTMap>
static bool contains_tt(const TTMap& m, const kitty::dynamic_truth_table& elem) noexcept
{
    return m.count(elem) > 0;
}

TEST_CASE("Dummy gate library traits", "[sidb-surface-analysis]")
{
    REQUIRE(has_get_functional_implementations_v<dummy_gate_library>);
    REQUIRE(has_get_gate_ports_v<dummy_gate_library>);
}

TEST_CASE("Dummy gate library simple defects", "[sidb-surface-analysis]")
{
    static const cart_gate_clk_lyt     gate_lyt{{3, 2}};   // 4 x 3 tiles of size 3 x 3 cells each
    static const sidb_100_cell_clk_lyt cell_lyt{{11, 8}};  // makes for 12 x 9 cells

    static const fcn::port_list<fcn::port_position> line_ports{{fcn::port_position(0, 1)}, {fcn::port_position(2, 1)}};
    static const fcn::port_list<fcn::port_position> y_ports{{fcn::port_position(0, 0), fcn::port_position(0, 2)},
                                                            {fcn::port_position(2, 1)}};

    sidb::primitives::defect_surface defect_layout{cell_lyt};

    SECTION("defect-free")
    {
        static const auto black_list = sidb_surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.empty());
    }
    SECTION("single charged defect")
    {
        defect_layout.assign_defect({6, 3}, sidb::model::defect{sidb::model::defect_type::SI_VACANCY});

        static const auto black_list = sidb_surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 12);

        REQUIRE(black_list.at({1, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 0}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 0}), networks::utils::create_and_tt()));
        CHECK(black_list.at({1, 0}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({1, 0}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({2, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 0}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 0}), networks::utils::create_and_tt()));
        CHECK(black_list.at({2, 0}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({2, 0}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({1, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 1}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 1}), networks::utils::create_and_tt()));
        CHECK(black_list.at({1, 1}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({1, 1}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({2, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 1}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 1}), networks::utils::create_and_tt()));
        CHECK(black_list.at({2, 1}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({2, 1}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({3, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({3, 1}), networks::utils::create_and_tt()));
        CHECK(black_list.at({3, 1}).at(networks::utils::create_and_tt()).front() == y_ports);
    }
    SECTION("single uncharged defect")
    {
        defect_layout.assign_defect({1, 1}, sidb::model::defect{sidb::model::defect_type::SILOXANE});

        static const auto black_list = sidb_surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.at({0, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_and_tt()));
        CHECK(black_list.at({0, 0}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({0, 0}).at(networks::utils::create_and_tt()).front() == y_ports);
    }
    SECTION("multi-defect")
    {
        defect_layout.assign_defect({6, 2}, sidb::model::defect{sidb::model::defect_type::DB});
        defect_layout.assign_defect({6, 3}, sidb::model::defect{sidb::model::defect_type::DB});
        defect_layout.assign_defect({3, 6}, sidb::model::defect{sidb::model::defect_type::ETCH_PIT});
        defect_layout.assign_defect({7, 5}, sidb::model::defect{sidb::model::defect_type::RAISED_SI});
        defect_layout.assign_defect({7, 6}, sidb::model::defect{sidb::model::defect_type::RAISED_SI});

        static const auto black_list = sidb_surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 12);

        REQUIRE(black_list.at({1, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 0}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 0}), networks::utils::create_and_tt()));
        CHECK(black_list.at({1, 0}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({1, 0}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({2, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 0}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 0}), networks::utils::create_and_tt()));
        CHECK(black_list.at({2, 0}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({2, 0}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({3, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({3, 0}), networks::utils::create_and_tt()));
        CHECK(black_list.at({3, 0}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({1, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 1}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 1}), networks::utils::create_and_tt()));
        CHECK(black_list.at({1, 1}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({1, 1}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({2, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 1}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 1}), networks::utils::create_and_tt()));
        CHECK(black_list.at({2, 1}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({2, 1}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({3, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({3, 1}), networks::utils::create_and_tt()));
        CHECK(black_list.at({3, 1}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({0, 2}).size() == 2);
        CHECK(contains_tt(black_list.at({0, 2}), networks::utils::create_and_tt()));
        CHECK(black_list.at({0, 2}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({1, 2}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 2}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 2}), networks::utils::create_and_tt()));
        CHECK(black_list.at({1, 2}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({1, 2}).at(networks::utils::create_and_tt()).front() == y_ports);

        REQUIRE(black_list.at({2, 2}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 2}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 2}), networks::utils::create_and_tt()));
        CHECK(black_list.at({2, 2}).at(networks::utils::create_id_tt()).front() == line_ports);
        CHECK(black_list.at({2, 2}).at(networks::utils::create_and_tt()).front() == y_ports);
    }
}

TEST_CASE("SiDB Bestagon gate library with simple defects", "[sidb-surface-analysis]")
{
    static const hex_even_col_gate_clk_lyt gate_lyt{
        aspect_ratio<hex_even_col_gate_clk_lyt>{0, 0}};     // 1 x 1 tiles of size 60 x 46 cells each
    static const sidb_100_cell_clk_lyt cell_lyt{{59, 45}};  // makes for exactly one gate of the Bestagon library

    sidb::primitives::defect_surface defect_layout{cell_lyt};

    SECTION("defect-free")
    {
        static const auto black_list = sidb_surface_analysis<sidb_bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.empty());
    }
    SECTION("single charged defect")
    {
        defect_layout.assign_defect({30, 45}, sidb::model::defect{sidb::model::defect_type::SI_VACANCY});

        static const auto black_list = sidb_surface_analysis<sidb_bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.at({0, 0}).size() == 8);
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_not_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_and_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_or_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_nand_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_nor_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_xor_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_xnor_tt()));
    }
    SECTION("single uncharged defect")
    {
        defect_layout.assign_defect({30, 45}, sidb::model::defect{sidb::model::defect_type::SILOXANE});

        static const auto black_list = sidb_surface_analysis<sidb_bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.at({0, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_not_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_id_tt()));
    }
    SECTION("multi-defect")
    {
        defect_layout.assign_defect({16, 43}, sidb::model::defect{sidb::model::defect_type::DB});
        defect_layout.assign_defect({30, 40}, sidb::model::defect{sidb::model::defect_type::DB});
        defect_layout.assign_defect({41, 32}, sidb::model::defect{sidb::model::defect_type::ETCH_PIT});
        defect_layout.assign_defect({45, 33}, sidb::model::defect{sidb::model::defect_type::RAISED_SI});
        defect_layout.assign_defect({45, 34}, sidb::model::defect{sidb::model::defect_type::RAISED_SI});

        static const auto black_list = sidb_surface_analysis<sidb_bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.at({0, 0}).size() == 8);
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_id_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_not_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_or_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_xor_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), networks::utils::create_xnor_tt()));
    }
}
