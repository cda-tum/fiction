//
// Created by Willem Lambooy on 29/02/2024.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/clustercomplete.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/types.hpp>

#include <mockturtle/utils/stopwatch.hpp>

using namespace fiction;

// TEMPLATE_TEST_CASE(
//     "Ground state space of a 7 DB layout", "[ground-state-space]",
//     (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
//     (charge_distribution_surface<cell_level_layout<sidb_technology,
//     clocked_layout<cartesian_layout<siqad::coord_t>>>>))
//{
//     CHECK(true);
// }

using sidb_lyt = sidb_cell_clk_lyt_siqad;

static bool verify_clustercomplete_result(const charge_distribution_surface<sidb_lyt>&              qe_cl,
                                          const std::vector<charge_distribution_surface<sidb_lyt>>& cc_cls) noexcept
{
    for (const charge_distribution_surface<sidb_lyt>& cc_cl : cc_cls)
    {
        bool is_same = true;

        for (const cell<sidb_lyt>& c : qe_cl.get_sidb_order())
        {
            if (qe_cl.get_charge_state(c) != cc_cl.get_charge_state(c))
            {
                is_same = false;
                break;
            }
        }

        if (is_same)
        {
            return true;
        }
    }

    return false;
}

static void print_test_case(const std::vector<charge_distribution_surface<sidb_lyt>>& cc_cls) noexcept
{
    if (cc_cls.empty())
    {
        std::cout << "no physically valid charge distributions for the given base" << std::endl;
        return;
    }

    std::cout << "using sidb_lyt = sidb_cell_clk_lyt_siqad;\nsidb_lyt lyt{};\n\n";
    for (const cell<sidb_lyt>& c : cc_cls[0].get_sidb_order())
    {
        std::cout << "lyt.assign_cell_type({" << c.x << ", " << c.y << ", " << static_cast<uint64_t>(c.z)
                  << "}, sidb_lyt::cell_type::NORMAL);\n";
    }

    //    std::cout << "\n\n// not needed, \"\n"
    //                 "                 \"but I'm giving all default parameters explicitly for the sake of the
    //                 test\nconst " "sidb_simulation_parameters params{3, -0.32, 5.6, 5.0, 3.84, 7.68, 2.25};\n";

    std::cout << "\ncharge_distribution_surface charge_lyt{lyt};\ncharge_lyt.assign_physical_params(params);\n\n";

    for (const charge_distribution_surface<sidb_lyt>& cl : cc_cls)
    {

        for (const cell<sidb_lyt>& c : cl.get_sidb_order())
        {
            std::cout << "charge_lyt.assign_charge_state({" << c.x << ", " << c.y << ", " << static_cast<uint64_t>(c.z)
                      << "}, sidb_charge_state::"
                      << (cl.get_charge_state(c) == sidb_charge_state::NEGATIVE ? "NEGATIVE" :
                          cl.get_charge_state(c) == sidb_charge_state::POSITIVE ? "POSITIVE" :
                                                                                  "NEUTRAL")
                      << ");\n";
        }

        std::cout << "\ncharge_lyt.update_after_charge_change();\n\nCHECK(charge_lyt.is_physically_valid());\n\n";
    }

    std::cout << "\n\n\nCHECK(quickexact(lyt, quickexact_params{params, "
                 "quickexact_params<sidb_lyt>::automatic_base_number_detection::ON, {}, "
                 "0}).charge_distributions.size() == 0);"
              << std::endl;
}

static void verify_lyts(const std::vector<sidb_lyt>& lyts, const bool three_state = true)
{
    for (uint64_t i = 0; i < lyts.size(); ++i)
    {
        std::cout << "LAYOUT NUMBER: " << i << std::endl;

        //                lyts[i].foreach_cell([](const cell<sidb_lyt>& c) { std::cout << fmt::format("{}", c) <<
        //                std::endl; });

        const sidb_simulation_result<sidb_lyt>& qe_res =
            three_state ? quickexact(lyts[i]) :
                          quickexact(lyts[i], quickexact_params<sidb_lyt>{
                                                  sidb_simulation_parameters{2},
                                                  quickexact_params<sidb_lyt>::automatic_base_number_detection::OFF});

        std::cout << "QUICKEXACT RUNTIME: " << (mockturtle::to_seconds(qe_res.simulation_runtime) * 1000) << " ms"
                  << std::endl;
        std::cout << "PHYSICALLY VALID CHARGE DISTRIBUTIONS: " << qe_res.charge_distributions.size() << std::endl;

        try
        {
            const sidb_simulation_result<sidb_lyt> cc_res =
                three_state ? clustercomplete(lyts[i], 6) : clustercomplete(lyts[i], 6, sidb_simulation_parameters{2});

            std::cout << "CLUSTERCOMPLETE RUNTIME: " << (mockturtle::to_seconds(cc_res.simulation_runtime) * 1000)
                      << " ms" << std::endl;
            std::cout << "PHYSICALLY VALID CHARGE DISTRIBUTIONS: " << cc_res.charge_distributions.size() << std::endl;

            for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
            {
                CHECK(verify_clustercomplete_result(cl, cc_res.charge_distributions));
            }

            if (cc_res.charge_distributions.size() != qe_res.charge_distributions.size())
            {
                print_test_case(qe_res.charge_distributions);
                print_test_case(cc_res.charge_distributions);
                REQUIRE(false);
            }
        }
        catch (std::exception& e)
        {
            std::cout << "clustercomplete CRASHED :(((((((((((((((((\n\n\n" << std::endl;
            REQUIRE(false);
        }

        std::cout << std::endl;
    }
}

TEST_CASE("ClusterComplete verification on random layouts with 20 SiDBs", "[clustercomplete]")
{

    //        const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0},
    //        cell<sidb_lyt>{10, 5}};
    const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{10, 5}};

    generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
    rlg_ps.coordinate_pair                    = layout_dimensions;
    rlg_ps.number_of_sidbs                    = 20;
    rlg_ps.number_of_unique_generated_layouts = 3000;

    const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);

    verify_lyts(lyts);
}

TEST_CASE("ClusterComplete runtime tests on random layouts with 40 SiDBs", "[clustercomplete]")
{

    const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{24, 12}};

    generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
    rlg_ps.coordinate_pair                    = layout_dimensions;
    rlg_ps.number_of_sidbs                    = 40;
    rlg_ps.number_of_unique_generated_layouts = 100;

    const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);

    for (uint64_t i = 0; i < lyts.size(); ++i)
    {
        std::cout << "LAYOUT NUMBER: " << i << std::endl;

        const sidb_simulation_result<sidb_lyt> cc_res = clustercomplete(lyts[i], 8, sidb_simulation_parameters{}, 6);

        std::cout << "CLUSTERCOMPLETE LIM=8 RUNTIME: " << cc_res.simulation_runtime.count() << " s" << std::endl;
        std::cout << "PHYSICALLY VALID CHARGE DISTRIBUTIONS: " << cc_res.charge_distributions.size() << std::endl;

        const sidb_simulation_result<sidb_lyt> cc_res2 = clustercomplete(lyts[i], 12, sidb_simulation_parameters{}, 6);

        std::cout << "CLUSTERCOMPLETE LIM=12 RUNTIME: " << cc_res2.simulation_runtime.count() << " s" << std::endl;
        std::cout << "PHYSICALLY VALID CHARGE DISTRIBUTIONS: " << cc_res2.charge_distributions.size() << '\n'
                  << std::endl;
    }
}

TEST_CASE("Tiny fail 3", "[clustercomplete]")
{
    sidb_lyt lyt{};
    lyt.assign_cell_type({2, 0, 1}, sidb_lyt::cell_type::NORMAL);  //  0    -       0
    lyt.assign_cell_type({4, 0, 1}, sidb_lyt::cell_type::NORMAL);  //  -    -       1
    lyt.assign_cell_type({2, 1, 0}, sidb_lyt::cell_type::NORMAL);  //  -    +       2
    lyt.assign_cell_type({3, 1, 1}, sidb_lyt::cell_type::NORMAL);  //  0    -       3

    // . . . . .
    // . . x . x
    //
    // . . x . .
    // . . . x .

    // . . . . .
    // . . O . -
    //
    // . . - . .
    // . . . O .

    // . . . . .
    // . . - . -
    //
    // . . + . .
    // . . . - .

    //      0       1       2       3
    // 0    0
    // 1    0.2871  0
    // 2    0.4248  0.2265  0
    // 3    0.2522  0.2522  0.5285  0

    // 0.1228 on 2 is 0, 1 neg, 3 pos
    // 1.1798 on 2 is 0, 1, 3 neg

    // -0.024 on 3 is 0, 1 neg, 3 pos
    // 1.0330 on 3 is 0, 1, 2 neg

    // 0.1145 on 0 is 1, 3 neg, 2 pos
    // 0.4597 on 0 is 1, 2 neg, 3 pos

    // 0.2614 on 1 is 0, 2 neg, 3 pos
    // 0.3128 on 1 is 0, 3 neg, 2 pos

    const sidb_simulation_result<sidb_lyt>& qe_res = quickexact(lyt);
    const sidb_simulation_result<sidb_lyt>& cc_res = clustercomplete(lyt);

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        CHECK(verify_clustercomplete_result(cl, cc_res.charge_distributions));
    }

    print_test_case(qe_res.charge_distributions);
    print_test_case(cc_res.charge_distributions);
}

TEST_CASE("Base 2 fail", "[clustercomplete]")
{
    sidb_lyt lyt{};
    lyt.assign_cell_type({0, 1, 1}, sidb_lyt::cell_type::NORMAL);  //  0    -       0
    lyt.assign_cell_type({2, 3, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -       1
    lyt.assign_cell_type({2, 3, 1}, sidb_lyt::cell_type::NORMAL);  //  -    +       2
    lyt.assign_cell_type({6, 3, 1}, sidb_lyt::cell_type::NORMAL);  //  0    -       3

    const sidb_simulation_result<sidb_lyt>& qe_res =
        quickexact(lyt, quickexact_params<sidb_lyt>{sidb_simulation_parameters{2},
                                                    quickexact_params<sidb_lyt>::automatic_base_number_detection::OFF});
    const sidb_simulation_result<sidb_lyt>& cc_res = clustercomplete(lyt, 6, sidb_simulation_parameters{2});

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        CHECK(verify_clustercomplete_result(cl, cc_res.charge_distributions));
    }
}

TEST_CASE("valid?", "[clustercomplete]")
{
    sidb_lyt lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_lyt::cell_type::NORMAL);  // 0
    lyt.assign_cell_type({1, 0, 0}, sidb_lyt::cell_type::NORMAL);  // 1
    lyt.assign_cell_type({2, 0, 1}, sidb_lyt::cell_type::NORMAL);  // 2
    lyt.assign_cell_type({1, 1, 0}, sidb_lyt::cell_type::NORMAL);  // 3
    lyt.assign_cell_type({1, 1, 1}, sidb_lyt::cell_type::NORMAL);  // 4
    lyt.assign_cell_type({0, 2, 0}, sidb_lyt::cell_type::NORMAL);  // 5
    lyt.assign_cell_type({1, 2, 0}, sidb_lyt::cell_type::NORMAL);  // 6
    lyt.assign_cell_type({2, 2, 0}, sidb_lyt::cell_type::NORMAL);  // 7
    lyt.assign_cell_type({0, 3, 0}, sidb_lyt::cell_type::NORMAL);  // 8
    lyt.assign_cell_type({2, 3, 0}, sidb_lyt::cell_type::NORMAL);  // 9

    const sidb_simulation_result<sidb_lyt>& qe_res = quickexact(lyt);
    const sidb_simulation_result<sidb_lyt>& cc_res = clustercomplete(lyt);

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        CHECK(verify_clustercomplete_result(cl, cc_res.charge_distributions));
    }

    sidb_lyt lyt2{};

    lyt2.assign_cell_type({0, 0, 0}, sidb_lyt::cell_type::NORMAL);
    lyt2.assign_cell_type({1, 0, 0}, sidb_lyt::cell_type::NORMAL);
    lyt2.assign_cell_type({2, 0, 1}, sidb_lyt::cell_type::NORMAL);
    lyt2.assign_cell_type({2, 1, 0}, sidb_lyt::cell_type::NORMAL);
    lyt2.assign_cell_type({0, 1, 1}, sidb_lyt::cell_type::NORMAL);
    lyt2.assign_cell_type({1, 1, 1}, sidb_lyt::cell_type::NORMAL);
    lyt2.assign_cell_type({0, 2, 0}, sidb_lyt::cell_type::NORMAL);
    lyt2.assign_cell_type({1, 2, 0}, sidb_lyt::cell_type::NORMAL);
    lyt2.assign_cell_type({2, 2, 0}, sidb_lyt::cell_type::NORMAL);
    lyt2.assign_cell_type({2, 3, 0}, sidb_lyt::cell_type::NORMAL);

    const sidb_simulation_result<sidb_lyt>& qe_res2 = quickexact(lyt2);
    const sidb_simulation_result<sidb_lyt>& cc_res2 = clustercomplete(lyt2);

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res2.charge_distributions)
    {
        CHECK(verify_clustercomplete_result(cl, cc_res2.charge_distributions));
    }
}

TEST_CASE("Base 2 clash")
{
    using sidb_lyt = sidb_cell_clk_lyt_siqad;
    sidb_lyt lyt{};

    sidb_simulation_parameters params{2};

    lyt.assign_cell_type({11, 1, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({15, 1, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({5, 2, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({17, 2, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({20, 3, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({13, 3, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({9, 4, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({2, 5, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({17, 5, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({2, 6, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({9, 6, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({10, 7, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({8, 7, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({17, 9, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({11, 9, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({12, 9, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({5, 10, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({5, 10, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({7, 11, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({13, 11, 1}, sidb_lyt::cell_type::NORMAL);

    charge_distribution_surface charge_lyt{lyt};
    charge_lyt.assign_physical_parameters(params);

    charge_lyt.assign_charge_state({11, 1, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({15, 1, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({5, 2, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({20, 3, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 3, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({2, 5, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({17, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({2, 6, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({9, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 7, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({8, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({11, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({12, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({5, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({5, 10, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({7, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    CHECK(quickexact(lyt, quickexact_params<sidb_lyt>{params,
                                                      quickexact_params<sidb_lyt>::automatic_base_number_detection::OFF,
                                                      {},
                                                      0})
              .charge_distributions.size() == 2);

    charge_lyt.assign_charge_state({11, 1, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({15, 1, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({5, 2, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({20, 3, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 3, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({2, 5, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({17, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({2, 6, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({9, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 7, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({8, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({11, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({12, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({5, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({5, 10, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({7, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({11, 1, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({15, 1, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({5, 2, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({20, 3, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 3, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({2, 5, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({17, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({2, 6, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({9, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 7, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({8, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({11, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({12, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({5, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({5, 10, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({7, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    const sidb_simulation_result<sidb_lyt>& res = clustercomplete(lyt, 6, params);

    CHECK(res.charge_distributions.size() == 2);
}
