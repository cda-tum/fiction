//
// Created by Willem Lambooy on 29/02/2024.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/flitsim.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#incldue < fiction / algorithms / simulation / sidb / sidb_simulation_parameters>
#include <fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/types.hpp>

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

static bool verify_flitsim_result(const charge_distribution_surface<sidb_lyt>&              qe_cl,
                                  const std::vector<charge_distribution_surface<sidb_lyt>>& fs_cls) noexcept
{
    for (const charge_distribution_surface<sidb_lyt>& fs_cl : fs_cls)
    {
        bool is_same = true;

        for (const cell<sidb_lyt>& c : qe_cl.get_sidb_order())
        {
            if (qe_cl.get_charge_state(c) != fs_cl.get_charge_state(c))
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

static void print_test_case(const std::vector<charge_distribution_surface<sidb_lyt>>& fs_cls) noexcept
{
    std::cout << "using sidb_lyt = sidb_cell_clk_lyt_siqad;\nsidb_lyt lyt{};\n\n";
    for (const cell<sidb_lyt>& c : fs_cls[0].get_sidb_order())
    {
        std::cout << "lyt.assign_cell_type({" << c.x << ", " << c.y << ", " << static_cast<uint64_t>(c.z)
                  << "}, sidb_lyt::cell_type::NORMAL);\n";
    }

    std::cout << "\n\nconst sidb_simulation_parameters params{3, -0.32, 5.6, 5.0, 3.84, 7.68, 2.25};  // not needed, "
                 "but I'm giving all default parameters explicitly for the sake of the test\n";

    for (const charge_distribution_surface<sidb_lyt>& cl : fs_cls)
    {
        std::cout << "\ncharge_distribution_layout charge_lyt{lyt};\ncharge_lyt.assign_physical_params(params);\n\n";

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

static void verify_lyts(const std::vector<sidb_lyt>& lyts)
{
    for (uint64_t i = 0; i < lyts.size(); ++i)
    {
        std::cout << "LAYOUT NUMBER: " << i << std::endl;

        //        lyts[i].foreach_cell([](const cell<sidb_lyt>& c) { std::cout << fmt::format("{}", c) << std::endl; });

        const sidb_simulation_result<sidb_lyt>& qe_res = quickexact(lyts[i]);

        std::cout << "QUICKEXACT RUNTIME: " << (qe_res.simulation_runtime.count() * 1000) << " ms" << std::endl;
        std::cout << "PHYSICALLY VALID CHARGE DISTRIBUTIONS: " << qe_res.charge_distributions.size() << std::endl;

        try
        {
            const sidb_simulation_result<sidb_lyt> fs_res = flitsim(lyts[i], 6);

            std::cout << "FLITSIM RUNTIME: " << (fs_res.simulation_runtime.count() * 1000) << " ms" << std::endl;
            std::cout << "PHYSICALLY VALID CHARGE DISTRIBUTIONS: " << fs_res.charge_distributions.size() << std::endl;

            for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
            {
                CHECK(verify_flitsim_result(cl, fs_res.charge_distributions));
            }

            if (fs_res.charge_distributions.size() > 20 && qe_res.charge_distributions.size() == 0)
            {
                print_test_case(fs_res.charge_distributions);
                REQUIRE(false);
            }
        }
        catch (std::exception& e)
        {
            std::cout << "FLITSIM CRASHED :(((((((((((((((((\n\n\n" << std::endl;
            REQUIRE(false);
        }

        std::cout << std::endl;
    }
}

TEST_CASE("flitsim verification on random layouts with 20 SiDBs", "[flitsim]")
{

    const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{24, 12}};

    generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
    rlg_ps.coordinate_pair                    = layout_dimensions;
    rlg_ps.number_of_sidbs                    = 20;
    rlg_ps.number_of_unique_generated_layouts = 1000;

    const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);

    verify_lyts(lyts);
}

TEST_CASE("Tiny fail 3", "[ground-state-space]")
{
    sidb_lyt lyt{};
    lyt.assign_cell_type({2, 0, 1}, sidb_lyt::cell_type::NORMAL);  //  0    -       0
    lyt.assign_cell_type({4, 0, 1}, sidb_lyt::cell_type::NORMAL);  //  -    -       1
    lyt.assign_cell_type({2, 1, 0}, sidb_lyt::cell_type::NORMAL);  //  -    +       2
    lyt.assign_cell_type({3, 1, 1}, sidb_lyt::cell_type::NORMAL);  //  0    -       3

    const sidb_simulation_result<sidb_lyt>& qe_res  = quickexact(lyt);
    const ground_state_space_result&        gss_res = ground_state_space(lyt);
    const sidb_simulation_result<sidb_lyt>& fs_res  = flitsim(lyt);

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        //        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(true);
    }

    //    for (const charge_distribution_surface<sidb_lyt>& cl : fs_res.charge_distributions)
    //    {
    //        const bool verification = verify_ground_state_space_result(cl, top);
    //        CHECK(verification);
    //    }
}

#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/types.hpp>

TEST_CASE("21 Physically valid charge distributions for the same layout not detected by QuickExact", "[quickexact]")
{
    using sidb_lyt = sidb_cell_clk_lyt_siqad;
    sidb_lyt lyt{};

    lyt.assign_cell_type({22, 1, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({24, 2, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({23, 3, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({13, 4, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({10, 4, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({1, 5, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({0, 6, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({1, 6, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({24, 6, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({4, 6, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({3, 7, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({0, 8, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({1, 8, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({9, 9, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({24, 9, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({22, 9, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({13, 10, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({14, 10, 0}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({1, 11, 1}, sidb_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({17, 11, 1}, sidb_lyt::cell_type::NORMAL);

    // not needed, but I'm giving all default parameters explicitly for the sake of the test
    const sidb_simulation_parameters params{3, -0.32, 5.6, 5.0, 3.84, 7.68, 2.25};

    charge_distribution_surface charge_lyt{lyt};
    charge_lyt.assign_physical_parameters(params);

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    charge_lyt.assign_charge_state({22, 1, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 2, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({23, 3, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({13, 4, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 5, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({0, 6, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({1, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({4, 6, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({3, 7, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({0, 8, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 8, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({9, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({24, 9, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({22, 9, 1}, sidb_charge_state::NEUTRAL);
    charge_lyt.assign_charge_state({13, 10, 0}, sidb_charge_state::POSITIVE);
    charge_lyt.assign_charge_state({14, 10, 0}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({1, 11, 1}, sidb_charge_state::NEGATIVE);
    charge_lyt.assign_charge_state({17, 11, 1}, sidb_charge_state::NEGATIVE);

    charge_lyt.update_after_charge_change();

    CHECK(charge_lyt.is_physically_valid());

    // QuickExact finds 0 physically valid charge distributions for this layout
    CHECK(quickexact(lyt, quickexact_params<sidb_lyt>{params,
                                                      quickexact_params<sidb_lyt>::automatic_base_number_detection::ON,
                                                      {},
                                                      0})
              .charge_distributions.size() == 0);
}
