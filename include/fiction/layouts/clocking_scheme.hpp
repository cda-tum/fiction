//
// Created by marcel on 12.05.21.
//

#ifndef FICTION_CLOCKING_SCHEME_HPP
#define FICTION_CLOCKING_SCHEME_HPP

#include "fiction/layouts/coordinates.hpp"
#include "fiction/traits.hpp"

#include <phmap.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace fiction
{

/**
 * Clocking scheme type that assigns a clock number to each element of the provided type `ClockZone`. Clocking scheme
 * objects are utilized, e.g., in clocked_layout.
 *
 * Usually, a clocking scheme is defined by the means of a cutout that can be seamlessly extended in all directions to
 * provide repeating clock numbers.
 *
 * Many regular clocking schemes have been proposed in the literature. Some are pre-defined below.
 *
 * Clocking schemes are uniquely identified via their name.
 *
 * @tparam ClockZone Clock zone type. Usually, a coordinate type in a layout.
 */
template <typename ClockZone>
class clocking_scheme
{
  public:
    using clock_zone     = ClockZone;
    using clock_number   = uint8_t;
    using degree         = uint8_t;
    using clock_function = std::function<clock_number(clock_zone)>;

    /**
     * Standard constructor.
     *
     * @param n The clocking scheme's name. The name is utilized as the key to uniquely identify a scheme.
     * @param f A function that assigns a clock number to each clock zone.
     * @param in_deg Maximum possible in-degree in the provided scheme.
     * @param out_deg Maximum possible out-degree in the provided scheme.
     * @param cn Number of clock phases that make up one clock cycle, i.e., the number of different clock numbers.
     * @param r Flag to identify the scheme as regular.
     */
    explicit clocking_scheme(std::string_view n, clock_function f, const degree in_deg, const degree out_deg,
                             const clock_number cn = 4, const bool r = true) noexcept :
            name{std::move(n)},
            max_in_degree{in_deg},
            max_out_degree{out_deg},
            num_clocks{cn},
            regular{r},
            fn{std::move(f)}
    {}
    /**
     * Accesses the clock function to determine the clock number of the given clock zone if the scheme is regular.
     * Otherwise, the stored clock map is accessed to look for a manually specified/overwritten clock number. If none is
     * found, the default one, usually 0, is returned.
     *
     * @param cz Clock zone whose clock number is desired.
     * @return Clock number of cz.
     */
    clock_number operator()(clock_zone cz) const noexcept
    {
        if (regular)
        {
            return std::invoke(fn, cz);
        }

        if (const auto it = override.find(cz); it != override.cend())
        {
            return it->second;
        }

        return std::invoke(fn, cz);
    }
    /**
     * Compares the stored name against a given one.
     *
     * @param n Name to compare.
     * @return `true` iff the stored name is equal to n.
     */
    bool operator==(const std::string& n) const noexcept
    {
        return name == n;
    }
    /**
     * Checks for the clocking scheme's regularity.
     *
     * @return `true` iff the clocking scheme is regular.
     */
    [[nodiscard]] bool is_regular() const noexcept
    {
        return regular;
    }
    /**
     * Overrides a clock zone's clock number. The usage of this function immediately labels the clocking scheme as
     * irregular.
     *
     * @param cz Clock zone to override.
     * @param cn Clock number to assign to cz.
     */
    void override_clock_number(const clock_zone& cz, const clock_number cn) noexcept
    {
        regular = false;

        override[cz] = cn % num_clocks;
    }
    /**
     * Name of the clocking scheme.
     */
    const std::string_view name;
    /**
     * Maximum number of inputs the clocking scheme supports per clock zone.
     */
    const degree max_in_degree;
    /**
     * Maximum number of outputs the clocking scheme supports per clock zone.
     */
    const degree max_out_degree;
    /**
     * Number of different clocks in this scheme.
     */
    const clock_number num_clocks;

  private:
    /**
     * Defines the clocking as regular and well-defined by the scheme.
     */
    bool regular;
    /**
     * A function that determines clock numbers for given zones.
     */
    const clock_function fn;
    /**
     * Alias for a hash map that overrides clock zones.
     */
    using clocking_map = phmap::parallel_flat_hash_map<clock_zone, clock_number>;
    /**
     * Stores mappings clock_zone -> clock_number to override clock zones.
     */
    clocking_map override{};
};

namespace clock_name
{

inline constexpr const char* OPEN          = "OPEN";
inline constexpr const char* COLUMNAR      = "COLUMNAR";
inline constexpr const char* ROW           = "ROW";
inline constexpr const char* TWODDWAVE     = "2DDWAVE";
inline constexpr const char* TWODDWAVE_HEX = "2DDWAVEHEX";
inline constexpr const char* USE           = "USE";
inline constexpr const char* RES           = "RES";
inline constexpr const char* ESR           = "ESR";
inline constexpr const char* CFE           = "CFE";
inline constexpr const char* RIPPLE        = "RIPPLE";
inline constexpr const char* BANCS         = "BANCS";

}  // namespace clock_name

enum class num_clks
{
    THREE,
    FOUR
};

/**
 * Returns an irregular clocking that maps every coordinate to the standard clock. It is intended to be overridden.
 *
 * @tparam Lyt Clocked layout type.
 * @param n Number of clocks.
 * @return Irregular clocking scheme.
 */
template <typename Lyt>
static auto open_clocking(const num_clks& n = num_clks::FOUR) noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function open_clock_function =
        []([[maybe_unused]] const clock_zone<Lyt>& cz) noexcept
    { return typename clocking_scheme<clock_zone<Lyt>>::clock_number{}; };

    switch (n)
    {
        case num_clks::THREE:
        {
            return clocking_scheme{
                clock_name::OPEN, open_clock_function, Lyt::max_fanin_size, Lyt::max_fanin_size, 3u, false};
        }
        case num_clks::FOUR:
        {
            return clocking_scheme{
                clock_name::OPEN, open_clock_function, Lyt::max_fanin_size, Lyt::max_fanin_size, 4u, false};
        }
    }

    // fix -Wreturn-type warning
    return clocking_scheme{clock_name::OPEN, open_clock_function, Lyt::max_fanin_size, Lyt::max_fanin_size, 4u, false};
}
/**
 * Returns a linear 1D clocking as originally introduced in \"A device architecture for computing with quantum dots\" by
 * C. S. Lent and P. D. Tougaw in the Proceedings of the IEEE 1997.
 *
 * @tparam Lyt Clocked layout type.
 * @param n Number of clocks.
 * @return Columnar clocking scheme.
 */
template <typename Lyt>
static auto columnar_clocking(const num_clks& n = num_clks::FOUR) noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function columnar_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 3u> cutout{
            {{{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}}};

        return cutout[cz.y % 3ul][cz.x % 3ul];
    };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function columnar_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 2, 3}}, {{0, 1, 2, 3}}, {{0, 1, 2, 3}}, {{0, 1, 2, 3}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    switch (n)
    {
        case num_clks::THREE:
        {
            return clocking_scheme{
                clock_name::COLUMNAR, columnar_3_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 3u, true};
        }
        case num_clks::FOUR:
        {
            return clocking_scheme{
                clock_name::COLUMNAR, columnar_4_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 4u, true};
        }
    }

    // fix -Wreturn-type warning
    return clocking_scheme{
        clock_name::COLUMNAR, columnar_4_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 4u, true};
}
/**
 * Returns a 90° rotated linear 1D clocking based on the one originally introduced in \"A device architecture for
 * computing with quantum dots\" by C. S. Lent and P. D. Tougaw in the Proceedings of the IEEE 1997.
 *
 * @tparam Lyt Clocked layout type.
 * @param n Number of clocks.
 * @return Row-based clocking scheme.
 */
template <typename Lyt>
static auto row_clocking(const num_clks& n = num_clks::FOUR) noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function row_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 3u> cutout{
            {{{0, 0, 0}}, {{1, 1, 1}}, {{2, 2, 2}}}};

        return cutout[cz.y % 3ul][cz.x % 3ul];
    };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function row_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 0, 0, 0}}, {{1, 1, 1, 1}}, {{2, 2, 2, 2}}, {{3, 3, 3, 3}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    switch (n)
    {
        case num_clks::THREE:
        {
            return clocking_scheme{
                clock_name::ROW, row_3_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 3u, true};
        }
        case num_clks::FOUR:
        {
            return clocking_scheme{
                clock_name::ROW, row_4_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 4u, true};
        }
    }

    // fix -Wreturn-type warning
    return clocking_scheme{clock_name::ROW, row_4_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 4u, true};
}
/**
 * Returns the 2DDWave clocking as defined in \"Clocking and Cell Placement for QCA\" by V. Vankamamidi, M. Ottavi,
 * and F. Lombardi in IEEE Conference on Nanotechnology 2006.
 *
 * @tparam Lyt Clocked layout type.
 * @param n Number of clocks.
 * @return 2DDWave clocking scheme.
 */
template <typename Lyt>
static auto twoddwave_clocking(const num_clks& n = num_clks::FOUR) noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function twoddwave_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 3u> cutout{
            {{{0, 1, 2}}, {{1, 2, 0}}, {{2, 0, 1}}}};

        return cutout[cz.y % 3ul][cz.x % 3ul];
    };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function twoddwave_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 2, 3}}, {{1, 2, 3, 0}}, {{2, 3, 0, 1}}, {{3, 0, 1, 2}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    switch (n)
    {
        case num_clks::THREE:
        {
            return clocking_scheme{
                clock_name::TWODDWAVE, twoddwave_3_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 3u, true};
        }
        case num_clks::FOUR:
        {
            return clocking_scheme{
                clock_name::TWODDWAVE, twoddwave_4_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 4u, true};
        }
    }

    // fix -Wreturn-type warning
    return clocking_scheme{
        clock_name::TWODDWAVE, twoddwave_4_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 4u, true};
}
/**
 * Returns a hexagonal variation of the 2DDWave clocking as originally defined in \"Clocking and Cell Placement for
 * QCA\" by V. Vankamamidi, M. Ottavi, and F. Lombardi in IEEE Conference on Nanotechnology 2006.
 *
 * @tparam Lyt Clocked layout type.
 * @param n Number of clocks.
 * @return Hexagonal 2DDWave clocking scheme.
 */
template <typename Lyt>
static auto twoddwave_hex_clocking(const num_clks& n = num_clks::FOUR) noexcept
{
    // clang-format off

    static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 6u>
        odd_3_cutout{{{{0, 1, 2}},
                      {{1, 2, 0}},
                      {{1, 2, 0}},
                      {{2, 0, 1}},
                      {{2, 0, 1}},
                      {{0, 1, 2}}}};

    static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 6u>
        even_3_cutout{{{{0, 1, 2}},
                       {{0, 1, 2}},
                       {{1, 2, 0}},
                       {{1, 2, 0}},
                       {{2, 0, 1}},
                       {{2, 0, 1}}}};

    static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 8u>
        odd_4_cutout{{{{0, 1, 2, 3}},
                      {{1, 2, 3, 0}},
                      {{1, 2, 3, 0}},
                      {{2, 3, 0, 1}},
                      {{2, 3, 0, 1}},
                      {{3, 0, 1, 2}},
                      {{3, 0, 1, 2}},
                      {{0, 1, 2, 3}}}};

    static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 8u>
        even_4_cutout{{{{0, 1, 2, 3}},
                       {{0, 1, 2, 3}},
                       {{1, 2, 3, 0}},
                       {{1, 2, 3, 0}},
                       {{2, 3, 0, 1}},
                       {{2, 3, 0, 1}},
                       {{3, 0, 1, 2}},
                       {{3, 0, 1, 2}}}};

    // clang-format on

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function odd_row_twoddwave_hex_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept { return odd_3_cutout[cz.y % 6ul][cz.x % 3ul]; };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function odd_row_twoddwave_hex_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept { return odd_4_cutout[cz.y % 8ul][cz.x % 4ul]; };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function even_row_twoddwave_hex_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept { return even_3_cutout[cz.y % 6ul][cz.x % 3ul]; };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function even_row_twoddwave_hex_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept { return even_4_cutout[cz.y % 8ul][cz.x % 4ul]; };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function odd_column_twoddwave_hex_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept { return odd_3_cutout[cz.x % 6ul][cz.y % 3ul]; };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function odd_column_twoddwave_hex_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept { return odd_4_cutout[cz.x % 8ul][cz.y % 4ul]; };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function even_column_twoddwave_hex_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept { return even_3_cutout[cz.x % 6ul][cz.y % 3ul]; };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function even_column_twoddwave_hex_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept { return even_4_cutout[cz.x % 8ul][cz.y % 4ul]; };

    if constexpr (is_hexagonal_layout_v<Lyt>)
    {
        if constexpr (has_odd_row_hex_arrangement_v<Lyt>)
        {
            switch (n)
            {
                case num_clks::THREE:
                {
                    return clocking_scheme{clock_name::TWODDWAVE_HEX,
                                           odd_row_twoddwave_hex_3_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           3u,
                                           true};
                }
                case num_clks::FOUR:
                {
                    return clocking_scheme{clock_name::TWODDWAVE_HEX,
                                           odd_row_twoddwave_hex_4_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           4u,
                                           true};
                }
            }
        }
        else if constexpr (has_even_row_hex_arrangement_v<Lyt>)
        {
            switch (n)
            {
                case num_clks::THREE:
                {
                    return clocking_scheme{clock_name::TWODDWAVE_HEX,
                                           even_row_twoddwave_hex_3_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           3u,
                                           true};
                }
                case num_clks::FOUR:
                {
                    return clocking_scheme{clock_name::TWODDWAVE_HEX,
                                           even_row_twoddwave_hex_4_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           4u,
                                           true};
                }
            }
        }
        else if constexpr (has_odd_column_hex_arrangement_v<Lyt>)
        {
            switch (n)
            {
                case num_clks::THREE:
                {
                    return clocking_scheme{clock_name::TWODDWAVE_HEX,
                                           odd_column_twoddwave_hex_3_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           3u,
                                           true};
                }
                case num_clks::FOUR:
                {
                    return clocking_scheme{clock_name::TWODDWAVE_HEX,
                                           odd_column_twoddwave_hex_4_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           4u,
                                           true};
                }
            }
        }
        else if constexpr (has_even_column_hex_arrangement_v<Lyt>)
        {
            switch (n)
            {
                case num_clks::THREE:
                {
                    return clocking_scheme{clock_name::TWODDWAVE_HEX,
                                           even_column_twoddwave_hex_3_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           3u,
                                           true};
                }
                case num_clks::FOUR:
                {
                    return clocking_scheme{clock_name::TWODDWAVE_HEX,
                                           even_column_twoddwave_hex_4_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           4u,
                                           true};
                }
            }
        }

        // not a supported hexagonal orientation; fall back to regular 2DDWave clocking
        return twoddwave_clocking<Lyt>();
    }
    else  // not a hexagonal layout; fall back to regular 2DDWave clocking
    {
        return twoddwave_clocking<Lyt>();
    }
}
/**
 * Returns the USE clocking as defined in \"USE: A Universal, Scalable, and Efficient Clocking Scheme for QCA\"
 * by Caio Araujo T. Campos, Abner L. Marciano, Omar P. Vilela Neto, and Frank Sill Torres in TCAD 2015.
 *
 * @tparam Lyt Clocked layout type.
 * @return USE clocking scheme.
 */
template <typename Lyt>
static auto use_clocking() noexcept
{
    // clang-format off

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function use_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 2, 3}},
             {{3, 2, 1, 0}},
             {{2, 3, 0, 1}},
             {{1, 0, 3, 2}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::USE, use_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 4u, true};

    // clang-format on
}
/**
 * Returns the RES clocking as defined in \"An efficient clocking scheme for quantum-dot cellular automata\" by
 * Mrinal Goswami, Anindan Mondal, Mahabub Hasan Mahalat, Bibhash Sen, and Biplab K. Sikdar in International Journal
 * of Electronics Letters 2019.
 *
 * @tparam Lyt Clocked layout type.
 * @return RES clocking scheme.
 */
template <typename Lyt>
static auto res_clocking() noexcept
{
    // clang-format off

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function res_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{3, 0, 1, 2}},
             {{0, 1, 0, 3}},
             {{1, 2, 3, 0}},
             {{0, 3, 2, 1}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::RES, res_clock_function, std::min(Lyt::max_fanin_size, 3u), 3u, 4u, true};

    // clang-format on
}
/**
 * Returns the ESR clocking as defined in \"An efficient, scalable, regular clocking scheme based on quantum dot
 * cellular automata\" by Jayanta Pal, Amit Kumar Pramanik, Jyotirmoy Sil Sharma, Apu Kumar Saha, and Bibhash Sen in
 * Analog Integrated Circuits and Signal Processing 2021.
 *
 * @tparam Lyt Clocked layout type.
 * @return ESR clocking scheme.
 */
template <typename Lyt>
static auto esr_clocking() noexcept
{
    // clang-format off

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function esr_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{3, 0, 1, 2}},
             {{0, 1, 2, 3}},
             {{1, 2, 3, 0}},
             {{0, 3, 2, 1}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::ESR, esr_clock_function, std::min(Lyt::max_fanin_size, 3u), 3u, 4u, true};

    // clang-format on
}
/**
 * Returns the CFE clocking as defined in \"CFE: a convenient, flexible, and efficient clocking scheme for quantum-dot
 * cellular automata\" by Feifei Deng, Guang-Jun Xie, Xin Cheng, Zhang Zhang, and Yongqiang Zhang in IET Circuits,
 * Devices & Systems 2020.
 *
 * @tparam Lyt Clocked layout type.
 * @return CFE clocking scheme.
 */
template <typename Lyt>
static auto cfe_clocking() noexcept
{
    // clang-format off

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function cfe_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 0, 1}},
             {{3, 2, 3, 2}},
             {{0, 1, 0, 1}},
             {{3, 2, 3, 2}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::CFE, cfe_clock_function, std::min(Lyt::max_fanin_size, 3u), 3u, 4u, true};

    // clang-format on
}
/**
 * Returns the Ripple clocking as defined in \"Ripple Clock Schemes for Quantum-dot Cellular Automata Circuits\" by
 * Prafull Purohit, Master Thesis, Rochester Institute of Technology, 2012.
 *
 * @tparam Lyt Clocked layout type.
 * @return Ripple clocking scheme.
 */
template <typename Lyt>
static auto ripple_clocking() noexcept
{
    // clang-format off

   static const typename clocking_scheme<clock_zone<Lyt>>::clock_function ripple_clock_function =
       [](const clock_zone<Lyt>& cz) noexcept
   {
       static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
           {{{0, 1, 2, 3}},
            {{3, 2, 1, 0}},
            {{0, 1, 2, 3}},
            {{3, 2, 1, 0}}}};

       return cutout[cz.y % 4ul][cz.x % 4ul];
   };

   return clocking_scheme{clock_name::RIPPLE, ripple_clock_function, std::min(Lyt::max_fanin_size, 3u), 3u, 4u, true};

    // clang-format on
}
/**
 * Returns the BANCS clocking as defined in \"BANCS: Bidirectional Alternating Nanomagnetic Clocking Scheme\" by
 * Ruan Evangelista Formigoni, Omar P. Vilela Neto, and Jose Augusto M. Nacif in SBCCI 2018.
 *
 * @tparam Lyt Clocked layout type.
 * @return BANCS clocking scheme.
 */
template <typename Lyt>
static auto bancs_clocking() noexcept
{
    // clang-format off

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function bancs_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 6u> cutout{
            {{{0, 1, 2}},
             {{2, 1, 0}},
             {{2, 0, 1}},
             {{1, 0, 2}},
             {{1, 2, 0}},
             {{0, 2, 1}}}};

        return cutout[cz.y % 6ul][cz.x % 3ul];
    };

    return clocking_scheme{clock_name::BANCS, bancs_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 3u, true};

    // clang-format on
}
/**
 * Returns a smart pointer to the given scheme.
 *
 * @tparam Lyt Layout type.
 * @param scheme Universal reference to a clocking scheme.
 * @return A shared pointer to the given `scheme`.
 */
template <typename Lyt>
std::shared_ptr<clocking_scheme<clock_zone<Lyt>>> ptr(clocking_scheme<clock_zone<Lyt>>&& scheme) noexcept
{
    return std::make_shared<clocking_scheme<clock_zone<Lyt>>>(std::move(scheme));
}
/**
 * Checks whether a given clocking scheme is registered as a cycle-free one. These currently are
 *
 * - COLUMNAR
 * - ROW
 * - 2DDWAVE
 * - 2DDWAVEHEX
 *
 * @tparam Lyt Layout type.
 * @param scheme Clocking scheme to check.
 * @return `true` iff `scheme` is listed as one of the linear clocking schemes.
 */
template <typename Lyt>
bool is_linear_scheme(const clocking_scheme<clock_zone<Lyt>>& scheme) noexcept
{
    static constexpr const std::array<const char*, 4> linear_schemes{
        {clock_name::COLUMNAR, clock_name::ROW, clock_name::TWODDWAVE, clock_name::TWODDWAVE_HEX}};

    return std::any_of(linear_schemes.cbegin(), linear_schemes.cend(),
                       [&scheme](const auto& name) { return scheme == name; });
}
/**
 * Exception to be thrown when an unsupported clocking scheme is requested.
 */
class unsupported_clocking_scheme_exception : public std::exception
{
  public:
    explicit unsupported_clocking_scheme_exception() noexcept : std::exception() {}

    [[nodiscard]] const char* what() const noexcept
    {
        return "given clocking scheme is unsupported";
    }
};
/**
 * Returns a clocking scheme by name.
 *
 * @tparam Lyt Layout type.
 * @param name Name of the desired clocking scheme.
 * @return Clocking scheme object that matches the given `name`, or `std::nullopt` if no clocking scheme by the given
 * `name` exists.
 */
template <typename Lyt>
std::optional<clocking_scheme<clock_zone<Lyt>>> get_clocking_scheme(const std::string_view& name) noexcept
{
    static const phmap::flat_hash_map<std::string, clocking_scheme<clock_zone<Lyt>>> scheme_lookup{
        {clock_name::OPEN, open_clocking<Lyt>(num_clks::FOUR)},
        {"OPEN3", open_clocking<Lyt>(num_clks::THREE)},
        {"OPEN4", open_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::COLUMNAR, columnar_clocking<Lyt>(num_clks::FOUR)},
        {"COLUMNAR3", columnar_clocking<Lyt>(num_clks::THREE)},
        {"COLUMNAR4", columnar_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::ROW, row_clocking<Lyt>(num_clks::FOUR)},
        {"ROW3", row_clocking<Lyt>(num_clks::THREE)},
        {"ROW4", row_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::TWODDWAVE, twoddwave_clocking<Lyt>(num_clks::FOUR)},
        {"2DDWAVE3", twoddwave_clocking<Lyt>(num_clks::THREE)},
        {"2DDWAVE4", twoddwave_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::TWODDWAVE_HEX, twoddwave_hex_clocking<Lyt>(num_clks::FOUR)},
        {"2DDWAVEHEX3", twoddwave_hex_clocking<Lyt>(num_clks::THREE)},
        {"2DDWAVEHEX4", twoddwave_hex_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::USE, use_clocking<Lyt>()},
        {clock_name::RES, res_clocking<Lyt>()},
        {clock_name::ESR, esr_clocking<Lyt>()},
        {clock_name::CFE, cfe_clocking<Lyt>()},
        {clock_name::RIPPLE, ripple_clocking<Lyt>()},
        {clock_name::BANCS, bancs_clocking<Lyt>()}};

    std::string upper_name = name.data();
    std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);

    if (const auto it = scheme_lookup.find(upper_name); it != scheme_lookup.cend())
    {
        return it->second;
    }

    return std::nullopt;
}

}  // namespace fiction

#endif  // FICTION_CLOCKING_SCHEME_HPP
