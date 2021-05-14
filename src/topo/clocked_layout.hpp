//
// Created by marcel on 14.05.21.
//

#ifndef FICTION_CLOCKED_LAYOUT_HPP
#define FICTION_CLOCKED_LAYOUT_HPP

#include "clocking_scheme.hpp"

namespace fiction
{

template <typename TileBasedLayout>
class clocked_layout : public TileBasedLayout
{
  public:
#pragma region Types and constructors

    using tile = typename TileBasedLayout::tile;

    using clocking_scheme_t = clocking_scheme<tile>;

    struct clocked_layout_storage
    {
        explicit clocked_layout_storage(const clocking_scheme_t& scheme) noexcept :
                clocking{std::make_shared<clocking_scheme_t>(scheme)}
        {}

        std::shared_ptr<clocking_scheme_t> clocking;
    };

    using base_type = clocked_layout;

    using storage = std::shared_ptr<clocked_layout_storage>;

    explicit clocked_layout(const typename TileBasedLayout::aspect_ratio& aspect_ratio) :
            TileBasedLayout(aspect_ratio),
            strg{std::make_shared<clocked_layout_storage>(open_4_clocking)}
    {}

    clocked_layout(const typename TileBasedLayout::aspect_ratio& aspect_ratio, const clocking_scheme_t& scheme) :
            TileBasedLayout(aspect_ratio),
            strg{std::make_shared<clocked_layout_storage>(scheme)}
    {}

    explicit clocked_layout(std::shared_ptr<clocked_layout_storage> storage) : strg{std::move(storage)} {}

#pragma endregion

#pragma region clocking

    void assign_clocking(const tile& t, const typename clocking_scheme_t::zone z) noexcept
    {
        strg->clocking->override_clock_zone(t, z);
    }

    [[nodiscard]] typename clocking_scheme_t::zone tile_clocking(const tile& t) const noexcept
    {
        return (*strg->clocking)(t);
    }

    [[nodiscard]] typename clocking_scheme_t::number num_clocks() const noexcept
    {
        return strg->clocking->num_clocks;
    }

    [[nodiscard]] bool is_regularly_clocked() const noexcept
    {
        return strg->clocking->is_regular();
    }

    [[nodiscard]] bool is_clocking(std::string&& name) const noexcept
    {
        return *strg->clocking == name;
    }

    [[nodiscard]] bool is_incoming_clocked(const tile& t1, const tile& t2) const noexcept
    {
        if (t1 == t2)
            return false;

        return static_cast<typename clocking_scheme_t::zone>(
                   (tile_clocking(t2) + static_cast<typename clocking_scheme_t::zone>(1)) % num_clocks()) ==
               tile_clocking(t1);
    }

    [[nodiscard]] bool is_outgoing_clocked(const tile& t1, const tile& t2) const noexcept
    {
        if (t1 == t2)
            return false;

        return static_cast<typename clocking_scheme_t::zone>(
                   (tile_clocking(t1) + static_cast<typename clocking_scheme_t::zone>(1)) % num_clocks()) ==
               tile_clocking(t2);
    }

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_CLOCKED_LAYOUT_HPP
