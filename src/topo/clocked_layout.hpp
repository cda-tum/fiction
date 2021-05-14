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

    struct clocked_layout_storage
    {
        template <typename ClockingScheme>
        explicit clocked_layout_storage(const ClockingScheme& scheme) noexcept :
                clocking{std::make_shared<ClockingScheme>(scheme)}
        {}

        std::shared_ptr<clocking_scheme<tile>> clocking;
    };

    using base_type = clocked_layout;

    using storage = std::shared_ptr<clocked_layout_storage>;

    explicit clocked_layout(const typename TileBasedLayout::aspect_ratio& aspect_ratio) :
            TileBasedLayout(aspect_ratio),
            strg{std::make_shared<clocked_layout_storage>(open_4_clocking)}
    {}

    clocked_layout(const typename TileBasedLayout::aspect_ratio& aspect_ratio, const clocking_scheme<tile>& scheme) :
            TileBasedLayout(aspect_ratio),
            strg{std::make_shared<clocked_layout_storage>(scheme)}
    {}

    explicit clocked_layout(std::shared_ptr<clocked_layout_storage> storage) : strg{std::move(storage)} {}

#pragma endregion

#pragma region clocking

    void assign_clocking(const tile& t, const typename clocking_scheme<tile>::zone z) noexcept
    {
        strg->clocking->override_clock_zone(t, z);
    }

    [[nodiscard]] typename clocking_scheme<tile>::zone tile_clocking(const tile& t) const noexcept
    {
        return (*strg->clocking)(t);
    }

    [[nodiscard]] typename clocking_scheme<tile>::number num_clocks() const noexcept
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

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_CLOCKED_LAYOUT_HPP
