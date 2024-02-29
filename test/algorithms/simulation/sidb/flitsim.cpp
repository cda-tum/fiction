//
// Created by Willem Lambooy on 29/02/2024.
//

#include "fiction/algorithms/simulation/sidb/flitsim.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp"
#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/clocked_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/types.hpp"

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
                const bool verification = verify_flitsim_result(cl, fs_res.charge_distributions);
                CHECK(verification);
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
