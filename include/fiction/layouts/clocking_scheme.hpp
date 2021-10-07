//
// Created by marcel on 12.05.21.
//

#ifndef FICTION_CLOCKING_SCHEME_HPP
#define FICTION_CLOCKING_SCHEME_HPP

#include "../traits.hpp"
#include "coordinates.hpp"

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
    using clock_function = std::function<clock_number(clock_zone)>;

    explicit clocking_scheme(const std::string& n, clock_function f, const clock_number cn = 4,
                             const bool r = true) noexcept :
            name{n},
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
static constexpr const char* open3       = "OPEN3";
static constexpr const char* open4       = "OPEN4";
static constexpr const char* twoddwave3  = "2DDWAVE3";
static constexpr const char* twoddwave4  = "2DDWAVE4";
static constexpr const char* use         = "USE";
static constexpr const char* res         = "RES";
static constexpr const char* bancs       = "BANCS";
static constexpr const char* topolinano3 = "TOPOLINANO3";
static constexpr const char* topolinano4 = "TOPOLINANO4";
}  // namespace clock_name

/**
 * Representing an irregular clocking that always returns the standard clock. It is intended to be overridden.
 */
template <typename Lyt>
static auto open_3_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function open_3_clock_function =
        []([[maybe_unused]] const clock_zone<Lyt>& cz) noexcept
    { return typename clocking_scheme<clock_zone<Lyt>>::clock_number{}; };

    return clocking_scheme{clock_name::open3, open_3_clock_function, 3u, false};
}
/**
 * Pre-defined open clocking with 4 clock phases.
 */
template <typename Lyt>
static auto open_4_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function open_4_clock_function =
        []([[maybe_unused]] const clock_zone<Lyt>& cz) noexcept
    { return typename clocking_scheme<clock_zone<Lyt>>::clock_number{}; };

    return clocking_scheme{clock_name::open4, open_4_clock_function, 4u, false};
}
/**
 * Representing a 3-phase adoption of the 2DDWave clocking as defined in "Clocking and Cell Placement for QCA" by
 * V. Vankamamidi, M. Ottavi, and F. Lombardi in IEEE Conference on Nanotechnology 2006.
 */
template <typename Lyt>
static auto twoddwave_3_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function twoddwave_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 3u> cutout{
            {{{0, 1, 2}}, {{1, 2, 0}}, {{2, 0, 1}}}};

        return cutout[cz.y % 3ul][cz.x % 3ul];
    };

    return clocking_scheme{clock_name::twoddwave3, twoddwave_3_clock_function, 3u, true};
}
/**
 * Representing the original 2DDWave clocking as defined in "Clocking and Cell Placement for QCA" by V. Vankamamidi,
 * M. Ottavi, and F. Lombardi in IEEE Conference on Nanotechnology 2006.
 */
template <typename Lyt>
static auto twoddwave_4_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function twoddwave_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 2, 3}}, {{1, 2, 3, 0}}, {{2, 3, 0, 1}}, {{3, 0, 1, 2}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::twoddwave4, twoddwave_4_clock_function, 4u, true};
}
/**
 * Representing the USE clocking as defined in "USE: A Universal, Scalable, and Efficient Clocking Scheme for QCA"
 * by Caio Araujo T. Campos, Abner L. Marciano, Omar P. Vilela Neto, and Frank Sill Torres in TCAD 2015.
 */
template <typename Lyt>
static auto use_4_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function use_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 2, 3}}, {{3, 2, 1, 0}}, {{2, 3, 0, 1}}, {{1, 0, 3, 2}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::use, use_4_clock_function, 4u, true};
}
/**
 * Representing the RES clocking as defined in "An efficient clocking scheme for quantum-dot cellular automata" by
 * Mrinal Goswami, Anindan Mondal, Mahabub Hasan Mahalat, Bibhash Sen, and Biplab K. Sikdar in International Journal
 * of Electronics Letters 2019.
 */
template <typename Lyt>
static auto res_4_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function res_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{3, 0, 1, 2}}, {{0, 1, 0, 3}}, {{1, 2, 3, 0}}, {{0, 3, 2, 1}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::res, res_4_clock_function, 4u, true};
}
/**
 * Representing the BANCS clocking as defined in "BANCS: Bidirectional Alternating Nanomagnetic Clocking Scheme" by
 * Ruan Evangelista Formigoni, Omar P. Vilela Neto, and Jose Augusto M. Nacif in SBCCI 2018.
 */
template <typename Lyt>
static auto bancs_3_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function bancs_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 6u> cutout{
            {{{0, 1, 2}}, {{2, 1, 0}}, {{2, 0, 1}}, {{1, 0, 2}}, {{1, 2, 0}}, {{0, 2, 1}}}};

        return cutout[cz.y % 6ul][cz.x % 3ul];
    };

    return clocking_scheme{clock_name::bancs, bancs_3_clock_function, 3u, true};
}
/**
 * Representing a 3-phase adaption of the clocking originally introduced in "A device architecture for computing
 * with quantum dots" by C. S. Lent and P. D. Tougaw in the Proceedings of the IEEE 1997. As it is used in
 * "ToPoliNano" (https://topolinano.polito.it/), it is referred to by that name in fiction to differentiate it
 * from 2DDWave.
 */
template <typename Lyt>
static auto topolinano_3_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function topolinano_3_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 3u>, 3u> cutout{
            {{{0, 1, 2}}, {{0, 1, 2}}, {{0, 1, 2}}}};

        return cutout[cz.y % 3ul][cz.x % 3ul];
    };

    return clocking_scheme{clock_name::topolinano3, topolinano_3_clock_function, 3u, true};
}
/**
 * Representing a linear 4-phase 1D clocking as originally introduced in "A device architecture for computing with
 * quantum dots" by C. S. Lent and P. D. Tougaw in the Proceedings of the IEEE 1997. As it is used in "ToPoliNano"
 * (https://topolinano.polito.it/), it is referred to by that name in fiction to differentiate it from 2DDWave.
 */
template <typename Lyt>
static auto topolinano_4_clocking() noexcept
{
    static const typename clocking_scheme<clock_zone<Lyt>>::clock_function topolinano_4_clock_function =
        [](const clock_zone<Lyt>& cz) noexcept
    {
        constexpr std::array<std::array<typename clocking_scheme<clock_zone<Lyt>>::clock_number, 4u>, 4u> cutout{
            {{{0, 1, 2, 3}}, {{0, 1, 2, 3}}, {{0, 1, 2, 3}}, {{0, 1, 2, 3}}}};

        return cutout[cz.y % 4ul][cz.x % 4ul];
    };

    return clocking_scheme{clock_name::topolinano4, topolinano_4_clock_function, 4u, true};
}

template <typename Lyt>
std::optional<clocking_scheme<clock_zone<Lyt>>> get_clocking_scheme(const std::string& name) noexcept
{
    static const std::unordered_map<std::string, clocking_scheme<clock_zone<Lyt>>> scheme_lookup{
        {clock_name::open3, open_3_clocking<Lyt>()},
        {clock_name::open4, open_4_clocking<Lyt>()},
        {"OPEN", open_4_clocking<Lyt>()},
        {clock_name::twoddwave3, twoddwave_3_clocking<Lyt>()},
        {"DIAG3", twoddwave_3_clocking<Lyt>()},
        {clock_name::twoddwave4, twoddwave_4_clocking<Lyt>()},
        {"DIAG4", twoddwave_4_clocking<Lyt>()},
        {"2DDWAVE", twoddwave_4_clocking<Lyt>()},
        {clock_name::use, use_4_clocking<Lyt>()},
        {clock_name::res, res_4_clocking<Lyt>()},
        {clock_name::bancs, bancs_3_clocking<Lyt>()},
        {clock_name::topolinano3, topolinano_3_clocking<Lyt>()},
        {"TOPOLINANO", topolinano_3_clocking<Lyt>()},
        {clock_name::topolinano4, topolinano_4_clocking<Lyt>()}};

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
