//
// Created by Jan Drewniok on 26.03.24.
//
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/technology/sidb_nanometer_distance.hpp>
#include <fiction/types.hpp>

#include <cmath>

using namespace fiction;
using namespace Catch::Matchers;

TEMPLATE_TEST_CASE("SiDB nanometer distance", "[distance]", sidb_cell_clk_lyt_siqad, sidb_100_cell_clk_lyt_siqad)
{
    const TestType layout{};

    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 0}, {0, 0}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({1, 0}, {1, 0}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 1}, {0, 1}), WithinAbs(0.0, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<TestType>({-3, 0}, {-3, 0}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({0, -5}, {0, -5}), WithinAbs(0.0, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 1, 1}, {0, 1, 1}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 0}, {1, 0}), WithinAbs(sidb_100_lattice::LAT_A * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 0}, {0, 1}), WithinAbs(sidb_100_lattice::LAT_B * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 0}, {0, 0, 1}),
               WithinAbs(sidb_100_lattice::LAT_C.second * 0.1, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 0}, {-1, 0}), WithinAbs(sidb_100_lattice::LAT_A * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 0}, {0, -1}), WithinAbs(sidb_100_lattice::LAT_B * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 0}, {0, 0, -1}),
               WithinAbs(sidb_100_lattice::LAT_C.second * 0.1, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 0}, {0, 2, 1}),
               WithinAbs(sidb_100_lattice::LAT_B * 0.2 + sidb_100_lattice::LAT_C.second * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 0}, {0, -2, 1}),
               WithinAbs(sidb_100_lattice::LAT_B * 0.2 - sidb_100_lattice::LAT_C.second * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<TestType>({0, -2, 1}, {0, 0}),
               WithinAbs(sidb_100_lattice::LAT_B * 0.2 - sidb_100_lattice::LAT_C.second * 0.1, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<TestType>({0, 2, 1}, {-5, 1, 0}),
               WithinAbs(std::hypot(sidb_100_lattice::LAT_A * 0.5,
                                    sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1),
                         0.00001));
}

TEST_CASE("SiDB nanometer distance for Si-111 lattice orientation", "[distance]")
{
    using lattice = sidb_111_cell_clk_lyt_siqad;
    const lattice layout{};

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 0}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({1, 0}, {1, 0}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 1}, {0, 1}), WithinAbs(0.0, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({-3, 0}, {-3, 0}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, -5}, {0, -5}), WithinAbs(0.0, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 1, 1}, {0, 1, 1}), WithinAbs(0.0, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {1, 0}), WithinAbs(sidb_111_lattice::LAT_A * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 1}), WithinAbs(sidb_111_lattice::LAT_B * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 0, 1}),
               WithinAbs(std::hypot(sidb_111_lattice::LAT_C.first, sidb_111_lattice::LAT_C.second) * 0.1, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {-1, 0}), WithinAbs(sidb_111_lattice::LAT_A * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, -1}), WithinAbs(sidb_111_lattice::LAT_B * 0.1, 0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 0, -1}),
               WithinAbs(std::hypot(sidb_111_lattice::LAT_C.first, sidb_111_lattice::LAT_C.second) * 0.1, 0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, 2, 1}),
               WithinAbs(std::hypot(sidb_111_lattice::LAT_C.first,
                                    2 * sidb_111_lattice::LAT_B + sidb_111_lattice::LAT_C.second) *
                             0.1,
                         0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 0}, {0, -2, 1}),
               WithinAbs(std::hypot(sidb_111_lattice::LAT_C.first,
                                    -2 * sidb_111_lattice::LAT_B + sidb_111_lattice::LAT_C.second) *
                             0.1,
                         0.00001));
    CHECK_THAT(sidb_nanometer_distance<lattice>({0, -2, 1}, {0, 0}),
               WithinAbs(std::hypot(sidb_111_lattice::LAT_C.first,
                                    -2 * sidb_111_lattice::LAT_B + sidb_111_lattice::LAT_C.second) *
                             0.1,
                         0.00001));

    CHECK_THAT(sidb_nanometer_distance<lattice>({0, 2, 0}, {-5, 1, 0}),
               WithinAbs(std::hypot(sidb_111_lattice::LAT_A * 0.5, sidb_111_lattice::LAT_B * 0.1), 0.00001));
}
