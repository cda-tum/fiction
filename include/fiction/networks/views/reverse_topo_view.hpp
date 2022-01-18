//
// Created by marcel on 18.01.22.
//

#ifndef FICTION_REVERSE_TOPO_VIEW_HPP
#define FICTION_REVERSE_TOPO_VIEW_HPP

#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/immutable_view.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>

namespace fiction
{

/**
 * A variation of mockturtle::topo_view that computes the reverse topological order. Unfortunately, this view could not
 * just extend mockturtle::topo_view and override its update_topo function because the necessary data members are
 * private instead of protected...
 *
 * @tparam Ntk mockturtle network type.
 * @tparam sorted Flag that determines whether Ntk is already wrapped in a topo_view.
 */
template <class Ntk, bool sorted = mockturtle::is_topologically_sorted_v<Ntk>>
class reverse_topo_view
{};

template <typename Ntk>
class reverse_topo_view<Ntk, false> : public mockturtle::immutable_view<Ntk>
{
  public:
    using storage = typename Ntk::storage;
    using node    = typename Ntk::node;
    using signal  = typename Ntk::signal;

    /*! \brief Default constructor.
     *
     * Constructs topological view on another network.
     */
    explicit reverse_topo_view(Ntk const& ntk) : mockturtle::immutable_view<Ntk>(ntk)
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_size_v<Ntk>, "Ntk does not implement the size function");
        static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant function");
        static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi function");
        static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po function");
        static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function");
        static_assert(mockturtle::has_incr_trav_id_v<Ntk>, "Ntk does not implement the incr_trav_id function");
        static_assert(mockturtle::has_set_visited_v<Ntk>, "Ntk does not implement the set_visited function");
        static_assert(mockturtle::has_trav_id_v<Ntk>, "Ntk does not implement the trav_id function");
        static_assert(mockturtle::has_visited_v<Ntk>, "Ntk does not implement the visited function");

        update_topo();
    }

    /*! \brief Default constructor.
     *
     * Constructs topological view, but only for the transitive fan-in starting
     * from a given start signal.
     */
    reverse_topo_view(Ntk const& ntk, typename Ntk::signal const& strt_sgnl) :
            mockturtle::immutable_view<Ntk>(ntk),
            start_signal(strt_sgnl)
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_size_v<Ntk>, "Ntk does not implement the size function");
        static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant function");
        static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi function");
        static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po function");
        static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function");
        static_assert(mockturtle::has_incr_trav_id_v<Ntk>, "Ntk does not implement the incr_trav_id function");
        static_assert(mockturtle::has_set_visited_v<Ntk>, "Ntk does not implement the set_visited function");
        static_assert(mockturtle::has_trav_id_v<Ntk>, "Ntk does not implement the trav_id function");
        static_assert(mockturtle::has_visited_v<Ntk>, "Ntk does not implement the visited function");

        update_topo();
    }

    /*! \brief Reimplementation of `size`. */
    [[nodiscard]] auto size() const
    {
        return static_cast<uint32_t>(topo_order.size());
    }

    /*! \brief Reimplementation of `num_gates`. */
    [[nodiscard]] auto num_gates() const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        return static_cast<uint32_t>(topo_order.size() - offset);
    }

    /*! \brief Reimplementation of `node_to_index`. */
    [[nodiscard]] uint32_t node_to_index(node const& n) const
    {
        return static_cast<uint32_t>(
            std::distance(std::begin(topo_order), std::find(std::begin(topo_order), std::end(topo_order), n)));
    }

    /*! \brief Reimplementation of `index_to_node`. */
    [[nodiscard]] node index_to_node(uint32_t index) const
    {
        return topo_order.at(index);
    }

    /*! \brief Reimplementation of `foreach_node`. */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.begin(), topo_order.end(), fn);
    }

    /*! \brief Reimplementation of `foreach_gate`. */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.begin(), topo_order.end() - offset, fn);
    }

    /*! \brief Reimplementation of `foreach_po`.
     *
     * If `start_signal` is provided in constructor, only this is returned as
     * primary output, otherwise reverts to original `foreach_po` implementation.
     */
    template <typename Fn>
    void foreach_po(Fn&& fn) const
    {
        if (start_signal)
        {
            std::vector<signal> signals(1, *start_signal);
            mockturtle::detail::foreach_element(signals.begin(), signals.end(), fn);
        }
        else
        {
            Ntk::foreach_po(fn);
        }
    }

    [[nodiscard]] uint32_t num_pos() const
    {
        return start_signal ? 1 : Ntk::num_pos();
    }

    void update_topo()
    {
        topo_order.clear();

        this->incr_trav_id();
        this->incr_trav_id();
        topo_order.reserve(this->size());

        /* constants and PIs */
        const auto c0 = this->get_node(this->get_constant(false));
        topo_order.push_back(c0);
        this->set_visited(c0, this->trav_id());

        if (const auto c1 = this->get_node(this->get_constant(true)); this->visited(c1) != this->trav_id())
        {
            topo_order.push_back(c1);
            this->set_visited(c1, this->trav_id());
        }

        this->foreach_ci(
            [this](auto n)
            {
                if (this->visited(n) != this->trav_id())
                {
                    topo_order.push_back(n);
                    this->set_visited(n, this->trav_id());
                }
            });

        if (start_signal)
        {
            if (this->visited(this->get_node(*start_signal)) == this->trav_id())
                return;
            create_topo_rec(this->get_node(*start_signal));
        }
        else
        {
            Ntk::foreach_co(
                [this](auto f)
                {
                    /* node was already visited */
                    if (this->visited(this->get_node(f)) == this->trav_id())
                        return;

                    create_topo_rec(this->get_node(f));
                });
        }

        // reverse the order; this is the only difference to topo_view
        std::reverse(topo_order.begin(), topo_order.end());
    }

  private:
    void create_topo_rec(node const& n)
    {
        /* is permanently marked? */
        if (this->visited(n) == this->trav_id())
            return;

        /* ensure that the node is not temporarily marked */
        assert(this->visited(n) != this->trav_id() - 1);

        /* mark node temporarily */
        this->set_visited(n, this->trav_id() - 1);

        /* mark children */
        this->foreach_fanin(n, [this](signal const& f) { create_topo_rec(this->get_node(f)); });

        /* mark node n permanently */
        this->set_visited(n, this->trav_id());

        /* visit node */
        topo_order.push_back(n);
    }

  private:
    std::vector<node>     topo_order;
    std::optional<signal> start_signal;
};

template <typename Ntk>
class reverse_topo_view<Ntk, true> : public Ntk
{
  public:
    explicit reverse_topo_view(Ntk const& ntk) : Ntk(ntk) {}
};

template <class T>
reverse_topo_view(T const&) -> reverse_topo_view<T>;

template <class T>
reverse_topo_view(T const&, typename T::signal const&) -> reverse_topo_view<T>;

}  // namespace fiction

#endif  // FICTION_REVERSE_TOPO_VIEW_HPP
