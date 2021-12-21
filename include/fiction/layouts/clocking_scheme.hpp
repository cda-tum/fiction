//
// Created by marcel on 12.05.21.
//

#ifndef FICTION_CLOCKING_SCHEME_HPP
#define FICTION_CLOCKING_SCHEME_HPP

#include "fiction/layouts/coordinates.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace fiction
{
template <typename ClockZone>
class clocking_scheme
{
  public:
    using clock_zone     = ClockZone;
    using clock_number   = uint8_t;
    using degree         = uint8_t;
    using clock_function = std::function<clock_number(clock_zone)>;

    explicit clocking_scheme(const std::string& n, clock_function f, const degree in_deg, const degree out_deg,
                             const clock_number cn = 4, const bool r = true) noexcept :
            name{n},
            max_in_degree{in_deg},
            max_out_degree{out_deg},
            num_clocks{cn},
            regular{r},
            fn{std::move(f)}
    {}

    clock_number operator()(clock_zone cz) const noexcept
    {
        if (regular)
            return fn(cz);

        if (auto it = override.find(cz); it != override.end())
        {
            return it->second;
        }

        return fn(cz);
    }

    bool operator==(const std::string& n) const noexcept
    {
        return name == n;
    }

    [[nodiscard]] bool is_regular() const noexcept
    {
        return regular;
    }

    void override_clock_number(const clock_zone& cz, const clock_number cn) noexcept
    {
        regular = false;

        override[cz] = cn % num_clocks;
    }
    /**
     * Name of the clocking scheme.
     */
    const std::string name;
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
    using clocking_map = std::unordered_map<clock_zone, clock_number>;
    /**
     * Stores mappings clock_zone -> clock_number to override clock zones.
     */
    clocking_map override{};
};

namespace clock_name
{
static constexpr const char* open          = "OPEN";
static constexpr const char* columnar      = "COLUMNAR";
static constexpr const char* row           = "ROW";
static constexpr const char* twoddwave     = "2DDWAVE";
static constexpr const char* twoddwave_hex = "2DDWAVEHEX";
static constexpr const char* use           = "USE";
static constexpr const char* res           = "RES";
static constexpr const char* esp           = "ESP";
static constexpr const char* bancs         = "BANCS";
}  // namespace clock_name

enum class num_clks
{
    THREE,
    FOUR
};

/**
 * Returns an irregular clocking that always returns the standard clock. It is intended to be overridden.
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
                clock_name::open, open_clock_function, Lyt::max_fanin_size, Lyt::max_fanin_size, 3u, false};
        }
        case num_clks::FOUR:
        {
            return clocking_scheme{
                clock_name::open, open_clock_function, Lyt::max_fanin_size, Lyt::max_fanin_size, 4u, false};
        }
    }

    // fix -Wreturn-type warning
    return clocking_scheme{clock_name::open, open_clock_function, Lyt::max_fanin_size, Lyt::max_fanin_size, 4u, false};
}
/**
 * Returns a linear 1D clocking as originally introduced in "A device architecture for computing with
 * quantum dots" by C. S. Lent and P. D. Tougaw in the Proceedings of the IEEE 1997.
 */
template <typename Lyt>
static auto columnar_clocking(const num_clks& n = num_clks::FOUR) noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function columnar_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 3u> cutout{
            {{{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}}};

        return cutout[cz.y % 3ul][cz.x % 3ul];
    };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function columnar_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 2, 3}}, {{0, 1, 2, 3}}, {{0, 1, 2, 3}}, {{0, 1, 2, 3}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    switch (n)
    {
        case num_clks::THREE:
        {
            return clocking_scheme{
                clock_name::columnar, columnar_3_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 3u, true};
        }
        case num_clks::FOUR:
        {
            return clocking_scheme{
                clock_name::columnar, columnar_4_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 4u, true};
        }
    }

    // fix -Wreturn-type warning
    return clocking_scheme{
        clock_name::columnar, columnar_4_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 4u, true};
}
/**
 * Returns a 90° rotated linear 1D clocking based on the one originally introduced in "A device architecture for
 * computing with quantum dots" by C. S. Lent and P. D. Tougaw in the Proceedings of the IEEE 1997.
 */
template <typename Lyt>
static auto row_clocking(const num_clks& n = num_clks::FOUR) noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function row_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 3u> cutout{
            {{{0, 0, 0}}, {{1, 1, 1}}, {{2, 2, 2}}}};

        return cutout[cz.y % 3ul][cz.x % 3ul];
    };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function row_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 0, 0, 0}}, {{1, 1, 1, 1}}, {{2, 2, 2, 2}}, {{3, 3, 3, 3}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    switch (n)
    {
        case num_clks::THREE:
        {
            return clocking_scheme{
                clock_name::row, row_3_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 3u, true};
        }
        case num_clks::FOUR:
        {
            return clocking_scheme{
                clock_name::row, row_4_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 4u, true};
        }
    }

    // fix -Wreturn-type warning
    return clocking_scheme{clock_name::row, row_4_clock_function, std::min(Lyt::max_fanin_size, 3u), 2u, 4u, true};
}
/**
 * Returns the 2DDWave clocking as defined in "Clocking and Cell Placement for QCA" by V. Vankamamidi, M. Ottavi,
 * and F. Lombardi in IEEE Conference on Nanotechnology 2006.
 */
template <typename Lyt>
static auto twoddwave_clocking(const num_clks& n = num_clks::FOUR) noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function twoddwave_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 3u> cutout{
            {{{0, 1, 2}}, {{1, 2, 0}}, {{2, 0, 1}}}};

        return cutout[cz.y % 3ul][cz.x % 3ul];
    };

    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function twoddwave_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 2, 3}}, {{1, 2, 3, 0}}, {{2, 3, 0, 1}}, {{3, 0, 1, 2}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    switch (n)
    {
        case num_clks::THREE:
        {
            return clocking_scheme{
                clock_name::twoddwave, twoddwave_3_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 3u, true};
        }
        case num_clks::FOUR:
        {
            return clocking_scheme{
                clock_name::twoddwave, twoddwave_4_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 4u, true};
        }
    }

    // fix -Wreturn-type warning
    return clocking_scheme{
        clock_name::twoddwave, twoddwave_4_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 4u, true};
}
/**
 * Returns a hexagonal variation of the 2DDWave clocking as originally defined in "Clocking and Cell Placement for QCA"
 * by V. Vankamamidi, M. Ottavi, and F. Lombardi in IEEE Conference on Nanotechnology 2006.
 */
template <typename Lyt>
static auto twoddwave_hex_clocking(const num_clks& n = num_clks::FOUR) noexcept
{
    static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 6u>
        odd_3_cutout{{{{0, 1, 2}}, {{1, 2, 0}}, {{1, 2, 0}}, {{2, 0, 1}}, {{2, 0, 1}}, {{0, 1, 2}}}};

    static constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 6u>
        even_3_cutout{{{{0, 1, 2}}, {{0, 1, 2}}, {{1, 2, 0}}, {{1, 2, 0}}, {{2, 0, 1}}, {{2, 0, 1}}}};

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
                    return clocking_scheme{clock_name::twoddwave_hex,
                                           odd_row_twoddwave_hex_3_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           3u,
                                           true};
                }
                case num_clks::FOUR:
                {
                    return clocking_scheme{clock_name::twoddwave_hex,
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
                    return clocking_scheme{clock_name::twoddwave_hex,
                                           even_row_twoddwave_hex_3_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           3u,
                                           true};
                }
                case num_clks::FOUR:
                {
                    return clocking_scheme{clock_name::twoddwave_hex,
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
                    return clocking_scheme{clock_name::twoddwave_hex,
                                           odd_column_twoddwave_hex_3_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           3u,
                                           true};
                }
                case num_clks::FOUR:
                {
                    return clocking_scheme{clock_name::twoddwave_hex,
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
                    return clocking_scheme{clock_name::twoddwave_hex,
                                           even_column_twoddwave_hex_3_clock_function,
                                           std::min(Lyt::max_fanin_size, 2u),
                                           2u,
                                           3u,
                                           true};
                }
                case num_clks::FOUR:
                {
                    return clocking_scheme{clock_name::twoddwave_hex,
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
 * Returns the USE clocking as defined in "USE: A Universal, Scalable, and Efficient Clocking Scheme for QCA"
 * by Caio Araujo T. Campos, Abner L. Marciano, Omar P. Vilela Neto, and Frank Sill Torres in TCAD 2015.
 */
template <typename Lyt>
static auto use_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function use_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 2, 3}}, {{3, 2, 1, 0}}, {{2, 3, 0, 1}}, {{1, 0, 3, 2}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::use, use_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 4u, true};
}
/**
 * Returns the RES clocking as defined in "An efficient clocking scheme for quantum-dot cellular automata" by
 * Mrinal Goswami, Anindan Mondal, Mahabub Hasan Mahalat, Bibhash Sen, and Biplab K. Sikdar in International Journal
 * of Electronics Letters 2019.
 */
template <typename Lyt>
static auto res_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function res_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{3, 0, 1, 2}}, {{0, 1, 0, 3}}, {{1, 2, 3, 0}}, {{0, 3, 2, 1}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::res, res_clock_function, std::min(Lyt::max_fanin_size, 3u), 3u, 4u, true};
}
/**
 * Returns the ESP (Zig-Zag) clocking as defined in "Regular Clocking based Emerging Technique in QCA Targeting Low
 * Power Nano Circuit" by Jayanta Pal, Amit Kumar Pramanik, Mrinal Goswami, Apu Kumar Saha, and Bibhash Sen in
 * International Journal of Electronics 2021.
 */
template <typename Lyt>
static auto esp_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function esp_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{3, 0, 1, 2}}, {{0, 1, 2, 3}}, {{1, 2, 3, 0}}, {{0, 3, 2, 1}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::esp, esp_clock_function, std::min(Lyt::max_fanin_size, 3u), 3u, 4u, true};
}
/**
 * Returns the BANCS clocking as defined in "BANCS: Bidirectional Alternating Nanomagnetic Clocking Scheme" by
 * Ruan Evangelista Formigoni, Omar P. Vilela Neto, and Jose Augusto M. Nacif in SBCCI 2018.
 */
template <typename Lyt>
static auto bancs_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function bancs_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 6u> cutout{
            {{{0, 1, 2}}, {{2, 1, 0}}, {{2, 0, 1}}, {{1, 0, 2}}, {{1, 2, 0}}, {{0, 2, 1}}}};

        return cutout[cz.y % 6ul][cz.x % 3ul];
    };

    return clocking_scheme{clock_name::bancs, bancs_clock_function, std::min(Lyt::max_fanin_size, 2u), 2u, 3u, true};
}
/**
 * Returns a smart pointer to the given scheme.
 *
 * @tparam Lyt Layout type.
 * @param scheme Universal reference to a clocking scheme.
 * @return A shared pointer to the given scheme.
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
 * @return true iff scheme is listed as one of the linear clocking schemes.
 */
template <typename Lyt>
bool is_linear_scheme(const clocking_scheme<clock_zone<Lyt>>& scheme) noexcept
{
    static constexpr const std::array<const char*, 4> linear_schemes{
        {clock_name::columnar, clock_name::row, clock_name::twoddwave, clock_name::twoddwave_hex}};

    return std::any_of(linear_schemes.cbegin(), linear_schemes.cend(),
                       [&scheme](const auto& name) { return scheme == name; });
}
/**
 * Returns a clocking scheme by name.
 *
 * @tparam Lyt Layout type.
 * @param name Name of the desired clocking scheme.
 * @return Clocking scheme object that matches the given name or std::nullopt if no clocking scheme by the given name
 * exists.
 */
template <typename Lyt>
std::optional<clocking_scheme<clock_zone<Lyt>>> get_clocking_scheme(const std::string& name) noexcept
{
    static const std::unordered_map<std::string, clocking_scheme<clock_zone<Lyt>>> scheme_lookup{
        {clock_name::open, open_clocking<Lyt>(num_clks::FOUR)},
        {"OPEN3", open_clocking<Lyt>(num_clks::THREE)},
        {"OPEN4", open_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::columnar, columnar_clocking<Lyt>(num_clks::FOUR)},
        {"COLUMNAR3", columnar_clocking<Lyt>(num_clks::THREE)},
        {"COLUMNAR4", columnar_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::row, row_clocking<Lyt>(num_clks::FOUR)},
        {"ROW3", row_clocking<Lyt>(num_clks::THREE)},
        {"ROW4", row_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::twoddwave, twoddwave_clocking<Lyt>(num_clks::FOUR)},
        {"2DDWAVE3", twoddwave_clocking<Lyt>(num_clks::THREE)},
        {"2DDWAVE4", twoddwave_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::twoddwave_hex, twoddwave_hex_clocking<Lyt>(num_clks::FOUR)},
        {"2DDWAVEHEX3", twoddwave_hex_clocking<Lyt>(num_clks::THREE)},
        {"2DDWAVEHEX4", twoddwave_hex_clocking<Lyt>(num_clks::FOUR)},
        {clock_name::use, use_clocking<Lyt>()},
        {clock_name::res, res_clocking<Lyt>()},
        {clock_name::esp, esp_clocking<Lyt>()},
        {clock_name::bancs, bancs_clocking<Lyt>()}};

    auto upper_name = name;
    std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);

    if (auto it = scheme_lookup.find(upper_name); it != scheme_lookup.end())
    {
        return it->second;
    }
    else
    {
        return std::nullopt;
    }
}

}  // namespace fiction

#endif  // FICTION_CLOCKING_SCHEME_HPP
