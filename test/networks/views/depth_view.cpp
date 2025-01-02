//
// Created by benjamin on 18.06.24.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/depth_view.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/cost_functions.hpp>
#include <memory>

using namespace fiction;

TEST_CASE("Update Levels Technology Network", "[depth-view]")
{
    fiction::depth_view<technology_network> tec_d;

    const auto x1_r = tec_d.create_pi();
    const auto x2_r = tec_d.create_pi();
    const auto a1_r = tec_d.create_and(x1_r, x2_r);
    tec_d.create_po(a1_r);
    tec_d.update_levels();

    CHECK(tec_d.level(4) == 1);
}

template<typename Ntk>
void test_depth_view()
{
    CHECK( mockturtle::is_network_type_v<Ntk> );
    CHECK( !mockturtle::has_depth_v<Ntk> );
    CHECK( !mockturtle::has_level_v<Ntk> );

    using depth_ntk = depth_view<Ntk>;

    CHECK( mockturtle::is_network_type_v<depth_ntk> );
    CHECK( mockturtle::has_depth_v<depth_ntk> );
    CHECK( mockturtle::has_level_v<depth_ntk> );

    using depth_depth_ntk = depth_view<depth_ntk>;

    CHECK( mockturtle::is_network_type_v<depth_depth_ntk> );
    CHECK( mockturtle::has_depth_v<depth_depth_ntk> );
    CHECK( mockturtle::has_level_v<depth_depth_ntk> );
};

TEST_CASE( "create different depth views", "[depth_view]" )
{
    test_depth_view<mockturtle::aig_network>();
    test_depth_view<mockturtle::mig_network>();
    test_depth_view<mockturtle::klut_network>();
}

TEST_CASE( "compute depth and levels for AIG", "[depth_view]" )
{
    mockturtle::aig_network aig;
    const auto a = aig.create_pi();
    const auto b = aig.create_pi();
    const auto f1 = aig.create_nand( a, b );
    const auto f2 = aig.create_nand( a, f1 );
    const auto f3 = aig.create_nand( b, f1 );
    const auto f4 = aig.create_nand( f2, f3 );
    aig.create_po( f4 );

    const depth_view depth_aig{ aig };
    CHECK( depth_aig.depth() == 3 );
    CHECK( depth_aig.level( aig.get_node( a ) ) == 0 );
    CHECK( depth_aig.level( aig.get_node( b ) ) == 0 );
    CHECK( depth_aig.level( aig.get_node( f1 ) ) == 1 );
    CHECK( depth_aig.level( aig.get_node( f2 ) ) == 2 );
    CHECK( depth_aig.level( aig.get_node( f3 ) ) == 2 );
    CHECK( depth_aig.level( aig.get_node( f4 ) ) == 3 );
}

TEST_CASE( "compute depth and levels for AIG with inverter costs", "[depth_view]" )
{
    mockturtle::aig_network aig;
    const auto a = aig.create_pi();
    const auto b = aig.create_pi();
    const auto f1 = aig.create_nand( a, b );
    const auto f2 = aig.create_nand( a, f1 );
    const auto f3 = aig.create_nand( b, f1 );
    const auto f4 = aig.create_nand( f2, f3 );
    aig.create_po( f4 );

    depth_view_params ps;
    ps.count_complements = true;
    const depth_view depth_aig{ aig, {}, ps };
    CHECK( depth_aig.depth() == 6 );
    CHECK( depth_aig.level( aig.get_node( a ) ) == 0 );
    CHECK( depth_aig.level( aig.get_node( b ) ) == 0 );
    CHECK( depth_aig.level( aig.get_node( f1 ) ) == 1 );
    CHECK( depth_aig.level( aig.get_node( f2 ) ) == 3 );
    CHECK( depth_aig.level( aig.get_node( f3 ) ) == 3 );
    CHECK( depth_aig.level( aig.get_node( f4 ) ) == 5 );
}

TEST_CASE( "compute critical path information", "[depth_view]" )
{
    mockturtle::aig_network aig;
    const auto a = aig.create_pi();
    const auto b = aig.create_pi();
    const auto c = aig.create_pi();
    const auto d = aig.create_pi();
    const auto e = aig.create_pi();

    const auto f1 = aig.create_and( a, b );
    const auto f2 = aig.create_and( c, f1 );
    const auto f3 = aig.create_and( d, e );
    const auto f = aig.create_and( f2, f3 );
    aig.create_po( f );

    const depth_view depth_aig{ aig };
    CHECK( !mockturtle::has_is_on_critical_path_v<decltype( aig )> );
    CHECK( mockturtle::has_is_on_critical_path_v<decltype( depth_aig )> );
    CHECK( depth_aig.is_on_critical_path( aig.get_node( a ) ) );
    CHECK( depth_aig.is_on_critical_path( aig.get_node( b ) ) );
    CHECK( !depth_aig.is_on_critical_path( aig.get_node( c ) ) );
    CHECK( !depth_aig.is_on_critical_path( aig.get_node( d ) ) );
    CHECK( !depth_aig.is_on_critical_path( aig.get_node( e ) ) );
    CHECK( depth_aig.is_on_critical_path( aig.get_node( f1 ) ) );
    CHECK( depth_aig.is_on_critical_path( aig.get_node( f2 ) ) );
    CHECK( !depth_aig.is_on_critical_path( aig.get_node( f3 ) ) );
    CHECK( depth_aig.is_on_critical_path( aig.get_node( f ) ) );
}

TEST_CASE( "compute levels during node construction", "[depth_view]" )
{
    depth_view<mockturtle::xag_network> dxag;

    const auto a = dxag.create_pi();
    const auto b = dxag.create_pi();
    const auto c = dxag.create_pi();

    dxag.create_po( dxag.create_xor( b, dxag.create_and( dxag.create_xor( a, b ), dxag.create_xor( b, c ) ) ) );

    dxag.update_levels();

    CHECK( dxag.depth() == 3u );
}

TEST_CASE( "compute levels during node construction with cost function", "[depth_view]" )
{
    const mockturtle::xag_network xag;
    depth_view<mockturtle::xag_network, mockturtle::mc_cost<mockturtle::xag_network>> dxag{ xag };

    const auto a = dxag.create_pi();
    const auto b = dxag.create_pi();
    const auto c = dxag.create_pi();

    dxag.create_po( dxag.create_xor( b, dxag.create_and( dxag.create_xor( a, b ), dxag.create_xor( b, c ) ) ) );

    dxag.update_levels();

    CHECK( dxag.depth() == 1u );
}

TEST_CASE( "compute levels during node construction after copy assignment", "[depth_view]" )
{
    const mockturtle::xag_network xag{};
    depth_view<mockturtle::xag_network> dxag;
    {
        auto tmp = std::make_unique<depth_view<mockturtle::xag_network>>(xag);
        dxag = *tmp; /* copy assignment */
    }

    auto const a = dxag.create_pi();
    auto const b = dxag.create_pi();
    auto const c = dxag.create_pi();
    dxag.create_po( dxag.create_xor( b, dxag.create_and( dxag.create_xor( a, b ), dxag.create_xor( b, c ) ) ) );

    dxag.update_levels();

    CHECK( dxag.depth() == 3u );
}

TEST_CASE( "compute levels during node construction after move assignment", "[depth_view]" )
{
    const mockturtle::xag_network xag{};
    depth_view<mockturtle::xag_network> dxag;
    {
        const std::unique_ptr<depth_view<mockturtle::xag_network>> tmp = std::make_unique<depth_view<mockturtle::xag_network>>(xag);
        dxag = *tmp; /* copy assignment */
    }

    const auto a = dxag.create_pi();
    const auto b = dxag.create_pi();
    const auto c = dxag.create_pi();

    dxag.create_po( dxag.create_xor( b, dxag.create_and( dxag.create_xor( a, b ), dxag.create_xor( b, c ) ) ) );

    dxag.update_levels();

    CHECK( dxag.depth() == 3u );
}
