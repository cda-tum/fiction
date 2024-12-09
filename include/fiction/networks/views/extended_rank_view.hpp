//
// Created by benjamin on 18.06.24.
//

#ifndef FICTION_EXTENDED_RANK_VIEW_HPP
#define FICTION_EXTENDED_RANK_VIEW_HPP

#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/networks/events.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace fiction
{
/**
 * @class extended_rank_view. Extends the mockturtle::rank_view
 *
 * Provides a view with node ranks for given networks. It adds functionalities to modify ranks. Most importantly, the
 * new `init_ranks()` function allows an array of nodes to be provided, which sets the levels and ranks of the nodes in
 * the network. Additionally, the `modify_rank()` function allows to pass a vector to assign the ranks of nodes in one
 * specific level.
 *
 * This class template is specialized depending on whether the provided network has a rank interface or not. The rank
 * interface is detected using the type traits defined in mockturtle. Specifically,
 * - has_rank_position_v<Ntk>
 * - has_at_rank_position_v<Ntk>
 * - has_swap_v<Ntk>
 * - has_width_v<Ntk>
 * - has_foreach_node_in_rank_v<Ntk>
 * - has_foreach_gate_in_rank_v<Ntk>
 *
 * @tparam Ntk The network type.
 * @tparam has_rank_interface Boolean flag checked compile-time, determines if the provided Ntk supports the rank
 * interface
 */
template <class Ntk, bool has_rank_interface =
                         mockturtle::has_rank_position_v<Ntk> && mockturtle::has_at_rank_position_v<Ntk> &&
                         mockturtle::has_swap_v<Ntk> && mockturtle::has_width_v<Ntk> &&
                         mockturtle::has_foreach_node_in_rank_v<Ntk> && mockturtle::has_foreach_gate_in_rank_v<Ntk>>
class extended_rank_view
{};

/**
 * @class extended_rank_view<Ntk, true>
 *
 * If already a rank_interface exists only the depth_view constructor gets called.
 *
 * @tparam Ntk The network type.
 */
template <class Ntk>
class extended_rank_view<Ntk, true> : public mockturtle::depth_view<Ntk>
{
  public:
    explicit extended_rank_view(const Ntk& ntk) : mockturtle::depth_view<Ntk>(ntk) {}
};

/**
 * @class extended_rank_view<Ntk, false>
 *
 * If no rank_interface exists, inherits from mockturtle::depth_view<Ntk> and initializes ranks for the network.
 *
 * @tparam Ntk The network type.
 */
template <class Ntk>
class extended_rank_view<Ntk, false> : public mockturtle::depth_view<Ntk>
{
  public:
    static constexpr bool is_topologically_sorted = true;
    using storage                                 = typename Ntk::storage;
    using node                                    = typename Ntk::node;
    using signal                                  = typename Ntk::signal;

    /**
     * Default constructor.
     * Constructs an empty extended_rank_view object, initializes base class and class variables, and verifies that the
     * Network Type (Ntk) needs to support certain methods.
     */
    explicit extended_rank_view() :
            mockturtle::depth_view<Ntk>(),
            rank_pos{*this},
            ranks{},
            max_rank_width{0},
            add_event(Ntk::events().register_add_event([this](auto const& n) { on_add(n); }))
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_foreach_node_v<Ntk>, "Ntk does not implement the foreach_node method");
        static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node method");
        static_assert(mockturtle::has_num_pis_v<Ntk>, "Ntk does not implement the num_pis method");
        static_assert(mockturtle::has_is_ci_v<Ntk>, "Ntk does not implement the is_ci method");
        static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant method");
    }

    /**
     * Constructs an extended_rank_view from an existing network.
     * Calls the base class constructor with the provided network, initializes class members, and registers network
     * events. Also, it ensures that the Network Type (Ntk) needs to support certain methods.
     *
     * @param ntk Reference to the network.
     */
    explicit extended_rank_view(const Ntk& ntk) :
            mockturtle::depth_view<Ntk>{ntk},
            rank_pos{ntk},
            ranks{this->depth() + 1},
            max_rank_width{0},
            add_event(Ntk::events().register_add_event([this](auto const& n) { on_add(n); }))
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_foreach_node_v<Ntk>, "Ntk does not implement the foreach_node method");
        static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node method");
        static_assert(mockturtle::has_num_pis_v<Ntk>, "Ntk does not implement the num_pis method");
        static_assert(mockturtle::has_is_ci_v<Ntk>, "Ntk does not implement the is_ci method");
        static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant method");

        init_ranks();
    }

    /**
     * Constructs an extended_rank_view from an existing network and a specific initial rank configuration.
     *
     * @param ntk Reference to the network.
     * @param ranks A vector of vectors specifying initial ranks for the nodes within the network.
     */
    explicit extended_rank_view(const Ntk& ntk, const std::vector<std::vector<node>>& ranks) :
            mockturtle::depth_view<Ntk>{ntk},
            rank_pos{ntk},
            ranks{this->depth() + 1},
            max_rank_width{0},
            add_event(Ntk::events().register_add_event([this](auto const& n) { on_add(n); }))
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_foreach_node_v<Ntk>, "Ntk does not implement the foreach_node method");
        static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node method");
        static_assert(mockturtle::has_num_pis_v<Ntk>, "Ntk does not implement the num_pis method");
        static_assert(mockturtle::has_is_ci_v<Ntk>, "Ntk does not implement the is_ci method");
        static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant method");

        init_ranks(ranks);
    }

    /**
     * Copy constructor creates a new extended_rank_view by copying the content of another extended_rank_view.
     *
     * @param other The other extended_rank_view object to be copied.
     */
    extended_rank_view(const extended_rank_view<Ntk, false>& other) :
            mockturtle::depth_view<Ntk>(other),
            rank_pos{other.rank_pos},
            ranks{other.ranks},
            max_rank_width{other.max_rank_width},
            add_event(Ntk::events().register_add_event([this](auto const& n) { on_add(n); }))
    {}

    /**
     * Overloaded assignment operator for copying `extended_rank_view` content of another `extended_rank_view` object.
     *
     * @param other The source `extended_rank_view` object whose contents are being copied.
     * @return A reference to the current object, enabling chain assignments.
     */
    extended_rank_view<Ntk, false>& operator=(const extended_rank_view<Ntk, false>& other)
    {
        // Check for self-assignment
        if (this == &other)
        {
            return *this;
        }

        /* delete the event of this network */
        Ntk::events().release_add_event(add_event);

        /* update the base class */
        this->_storage = other._storage;
        this->_events  = other._events;

        /* copy */
        rank_pos       = other.rank_pos;
        ranks          = other.ranks;
        max_rank_width = other.max_rank_width;

        /* register new event in the other network */
        add_event = Ntk::events().register_add_event([this](auto const& n) { on_add(n); });

        return *this;
    }

    /**
     * Destructor for extended_rank_view.
     */
    ~extended_rank_view()
    {
        Ntk::events().release_add_event(add_event);
    }
    /**
     * Returns the rank position of a node.
     *
     * @param n Node to get the rank position of.
     * @return Rank position of node `n`.
     */
    uint32_t rank_position(const node& n) const noexcept
    {
        assert(!this->is_constant(n) && "node must not be constant");

        return rank_pos[n];
    }

    /**
     * Verifies the validity of ranks and rank positions within the extended_rank_view context.
     *
     * @return A boolean indicating whether the ranks and rank positions are valid (true) or not (false).
     */
    [[nodiscard]] bool check_validity() const noexcept
    {
        for (std::size_t i = 0; i < ranks.size(); ++i)
        {
            const auto& rank              = ranks[i];
            uint32_t    expected_rank_pos = 0;
            for (const auto& n : rank)
            {
                // Check if the level is different from the rank level
                if (this->level(n) != i)
                {
                    return false;
                }
                // Check if the rank_pos is not in ascending order
                if (rank_pos[n] != expected_rank_pos)
                {
                    return false;
                }
                ++expected_rank_pos;  // Increment the expected rank_pos
            }
        }
        return true;
    }

    /**
     * Updates the nodes and associated rank positions for a specific level within the rank.
     *
     * @param level Level at which to replace nodes.
     * @param nodes The new nodes to be set at the given level.
     */
    void modify_rank(const uint32_t level, const std::vector<node>& nodes)
    {
        auto& rank = ranks[level];
        assert(rank.size() == nodes.size());
        rank = nodes;
        std::for_each(rank.cbegin(), rank.cend(), [this, i = 0u](auto const& n) mutable { rank_pos[n] = i++; });
    }

    /**
     * Fetches a node at a specific rank position
     *
     * @param level The level in the network, from which the node is to be fetched.
     * @param pos The position within the rank from which to obtain the node.
     * @return The node that resides at the `pos` in the `level`.
     */
    [[nodiscard]] node at_rank_position(const uint32_t level, const uint32_t pos) const noexcept
    {
        assert(level < ranks.size() && "level must be less than the number of ranks");
        assert(pos < ranks[level].size() && "pos must be less than the number of nodes in rank");

        return ranks[level][pos];
    }
    /**
     * Returns the width of the widest rank in the network.
     *
     * @return Width of the widest rank in the network.
     */
    [[nodiscard]] uint32_t width() const noexcept
    {
        return max_rank_width;
    }
    /**
     * Returns the width of the rank.
     *
     * @return Width of the rank.
     */
    uint32_t rank_width(const uint32_t level) noexcept
    {
        auto& rank = ranks[level];
        return static_cast<uint32_t>(rank.size());
    }
    /**
     * Swaps the positions of two nodes in the same rank.
     *
     * @param n1 First node to swap.
     * @param n2 Second node to swap.
     */
    void swap(const node& n1, const node& n2) noexcept
    {
        assert(this->level(n1) == this->level(n2) && "nodes must be in the same rank");

        auto& pos1 = rank_pos[n1];
        auto& pos2 = rank_pos[n2];

        std::swap(ranks[this->level(n1)][pos1], ranks[this->level(n2)][pos2]);
        std::swap(pos1, pos2);
    }
    /**
     * Sorts the given rank according to a comparator.
     *
     * @tparam Cmp Functor type that compares two nodes. It needs to fulfill the requirements of `Compare` (named C++
     * requirement).
     * @param level The level of the rank to sort.
     * @param cmp The comparator to use.
     */
    template <typename Cmp>
    void sort_rank(const uint32_t level, const Cmp& cmp)
    {
        // level must be less than the number of ranks
        if (level < ranks.size())
        {
            auto& rank = ranks[level];

            std::sort(rank.begin(), rank.end(), cmp);
            std::for_each(rank.cbegin(), rank.cend(), [this, i = 0u](auto const& n) mutable { rank_pos[n] = i++; });
        }
    }
    /**
     * Applies a given function to each node in the rank level in order.
     *
     * @tparam Fn Functor type.
     * @param level The rank to apply fn to.
     * @param fn The function to apply.
     */
    template <typename Fn>
    void foreach_node_in_rank(const uint32_t level, Fn&& fn) const
    {
        // level must be less than the number of ranks
        if (level < ranks.size())
        {
            auto const& rank = ranks[level];

            mockturtle::detail::foreach_element(rank.cbegin(), rank.cend(), std::forward<Fn>(fn));
        }
    }
    /**
     * Applies a given function to each node in rank order.
     *
     * This function overrides the `foreach_node` method of the base class.
     *
     * @tparam Fn Functor type.
     * @param fn The function to apply.
     */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        for (auto l = 0; l < ranks.size(); ++l)
        {
            foreach_node_in_rank(l, std::forward<Fn>(fn));
        }
    }
    /**
     * Applies a given function to each gate in the rank level in order.
     *
     * @tparam Fn Functor type.
     * @param level The rank to apply fn to.
     * @param fn The function to apply.
     */
    template <typename Fn>
    void foreach_gate_in_rank(const uint32_t level, Fn&& fn) const
    {
        // level must be less than the number of ranks
        if (level < ranks.size())
        {
            auto const& rank = ranks[level];

            mockturtle::detail::foreach_element_if(
                rank.cbegin(), rank.cend(), [this](auto const& n) { return !this->is_ci(n); }, std::forward<Fn>(fn));
        }
    }
    /**
     * Applies a given function to each gate in rank order.
     *
     * This function overrides the `foreach_gate` method of the base class.
     *
     * @tparam Fn Functor type.
     * @param fn The function to apply.
     */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        for (auto l = 0; l < ranks.size(); ++l)
        {
            foreach_gate_in_rank(l, std::forward<Fn>(fn));
        }
    }
    /**
     * Applies a given function to each PI in rank order.
     *
     * This function overrides the `foreach_pi` method of the base class.
     *
     * @tparam Fn Functor type.
     * @param fn The function to apply.
     */
    template <typename Fn>
    void foreach_pi(Fn&& fn) const
    {
        std::vector<node> pis{};
        pis.reserve(this->num_pis());

        mockturtle::depth_view<Ntk>::foreach_pi([&pis](auto const& pi) { pis.push_back(pi); });
        std::sort(pis.begin(), pis.end(),
                  [this](auto const& n1, auto const& n2) { return rank_pos[n1] < rank_pos[n2]; });
        mockturtle::detail::foreach_element(pis.cbegin(), pis.cend(), std::forward<Fn>(fn));
    }
    /**
     * Applies a given function to each CI in rank order.
     *
     * This function overrides the `foreach_ci` method of the base class.
     *
     * @tparam Fn Functor type.
     * @param fn The function to apply.
     */
    template <typename Fn>
    void foreach_ci(Fn&& fn) const
    {
        std::vector<node> pis{};
        pis.reserve(this->num_pis());

        mockturtle::depth_view<Ntk>::foreach_ci([&pis](auto const& pi) { pis.push_back(pi); });
        std::sort(pis.begin(), pis.end(),
                  [this](auto const& n1, auto const& n2) { return rank_pos[n1] < rank_pos[n2]; });
        mockturtle::detail::foreach_element(pis.cbegin(), pis.cend(), std::forward<Fn>(fn));
    }
    /**
     * Overrides the base class method to also call the add_event on create_pi().
     *
     * @return Newly created PI signal.
     */
    signal create_pi()
    {
        auto const n = mockturtle::depth_view<Ntk>::create_pi();
        this->resize_levels();
        on_add(this->get_node(n));
        return n;
    }

  private:
    mockturtle::node_map<uint32_t, Ntk> rank_pos;
    std::vector<std::vector<node>>      ranks;
    uint32_t                            max_rank_width;

    std::shared_ptr<typename mockturtle::network_events<Ntk>::add_event_type> add_event;

    /**
     * Inserts a node into the rank and updates the rank position, ranks, and max rank width accordingly.
     *
     * @param n The node to insert into the rank.
     */
    void insert_in_rank(const node& n)
    {
        auto& rank  = ranks[this->level(n)];
        rank_pos[n] = static_cast<uint32_t>(rank.size());
        rank.push_back(n);
        max_rank_width = std::max(max_rank_width, static_cast<uint32_t>(rank.size()));
    }

    /**
     * Adds a new node to the skip list.
     *
     * @param n The node to be added.
     */
    void on_add(const node& n) noexcept
    {
        if (this->level(n) >= ranks.size())
        {
            // add sufficient ranks to store the new node
            ranks.insert(ranks.end(), this->level(n) - ranks.size() + 1, {});
        }
        rank_pos.resize();

        insert_in_rank(n);
    }

    /**
     * Initializes the ranks for the given network. It traverses the nodes in the network using a depth-first search and
     * inserts each non-constant node into the rank.
     *
     * This function is noexcept.
     */
    void init_ranks() noexcept
    {
        mockturtle::depth_view<Ntk>::foreach_node(
            [this](auto const& n)
            {
                if (!this->is_constant(n))
                {
                    insert_in_rank(n);
                }
            });
    }

    /**
     * Implements a node into the rank list at the given rank level.
     *
     * @param n The node to be inserted.
     * @param rank_level The level at which the node should be inserted in the rank.
     */
    void insert_in_rank(const node& n, std::size_t rank_level)
    {
        assert(rank_level < ranks.size());
        auto& rank  = ranks[rank_level];
        rank_pos[n] = static_cast<uint32_t>(rank.size());
        rank.push_back(n);
        max_rank_width = std::max(max_rank_width, static_cast<uint32_t>(rank.size()));
    }

    /**
     * Initializes the ranks with the given array of nodes.
     *
     * @param input_ranks The input vector of ranks.
     */
    void init_ranks(const std::vector<std::vector<node>>& input_ranks)
    {
        for (std::size_t i = 0; i < input_ranks.size(); ++i)
        {
            auto const& rank_nodes = input_ranks[i];
            for (auto const& n : rank_nodes)
            {
                if (!this->is_constant(n))
                {
                    insert_in_rank(n, i);
                }
            }
        }
    }
};

/**
 * Deduction guide for `extended_rank_view'
 *
 * @tparam T Network type deduced from the construction context of `extended_rank_view`.
 */
template <class T>
extended_rank_view(const T&) -> extended_rank_view<T>;

/**
 * Deduction guide for `extended_rank_view` with two constructor arguments
 *
 * @tparam T Network type deduced from the construction context of `extended_rank_view`.
 */
template <class T>
extended_rank_view(const T&, std::vector<std::vector<typename T::node>>) -> extended_rank_view<T>;

}  // namespace fiction

#endif  // FICTION_EXTENDED_RANK_VIEW_HPP
