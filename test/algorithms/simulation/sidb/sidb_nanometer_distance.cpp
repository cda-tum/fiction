//
// Created by Jan Drewniok on 26.03.24.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/sidb_nanometer_distance.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("SiDB nanometer distance", "[distance]")
{
    using namespace Catch::Matchers;
    using lattice = sidb_100_cell_clk_lyt_siqad;
    const lattice layout{};

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 0}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({1, 0}, {1, 0}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 1}, {0, 1}), WithinAbs(0.0, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({-3, 0}, {-3, 0}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, -5}, {0, -5}), WithinAbs(0.0, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 1, 1}, {0, 1, 1}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {1, 0}), WithinAbs(sidb_100_lattice::LAT_A * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 1}), WithinAbs(sidb_100_lattice::LAT_B * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 0, 1}),
               WithinAbs(sidb_100_lattice::LAT_C.second * 0.1, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {-1, 0}), WithinAbs(sidb_100_lattice::LAT_A * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, -1}), WithinAbs(sidb_100_lattice::LAT_B * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 0, -1}),
               WithinAbs(sidb_100_lattice::LAT_C.second * 0.1, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 2, 1}),
               WithinAbs(sidb_100_lattice::LAT_B * 0.2 + sidb_100_lattice::LAT_C.second * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, -2, 1}),
               WithinAbs(sidb_100_lattice::LAT_B * 0.2 - sidb_100_lattice::LAT_C.second * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, -2, 1}, {0, 0}),
               WithinAbs(sidb_100_lattice::LAT_B * 0.2 - sidb_100_lattice::LAT_C.second * 0.1, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 2, 1}, {-5, 1, 0}),
               WithinAbs(std::hypot(sidb_100_lattice::LAT_A * 0.5,
                                    sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1),
                         0.00001));
}
