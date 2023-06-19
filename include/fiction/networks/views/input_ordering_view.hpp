//
// Created by benjamin on 11.04.23.
//

#ifndef FICTION_INPUT_ORDERING_VIEW_HPP
#define FICTION_INPUT_ORDERING_VIEW_HPP

#include "fiction/utils/network_utils.hpp"

#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/immutable_view.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

namespace fiction
{

/**
 * A variation of mockturtle::topo_view that computes the topological order after sorting the inputs according to the
 * connections of their fanouts. This is part of the Distribution Newtork I: Ordering_Network
 *
 * @tparam Ntk mockturtle network type.
 * @tparam sorted Flag that determines whether Ntk is already wrapped in a topo_view.
 */
template <class Ntk, bool sorted = fiction::is_input_ordered_v<Ntk>>
class input_ordering_view
{};

template <typename Ntk>
class input_ordering_view<Ntk, false> : public mockturtle::immutable_view<Ntk>
{
  public:
    using storage                          = typename Ntk::storage;
    using node                             = typename Ntk::node;
    using signal                           = typename Ntk::signal;
    static constexpr bool is_input_ordered = true;

    /*! \brief Default constructor.
     *
     * Constructs input ordering view on another network.
     */
    explicit input_ordering_view(Ntk const& ntk) : mockturtle::immutable_view<Ntk>(ntk), ntk{ntk}, num_p{ntk.num_pis()}
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

    /**
     * Reimplementation of `size`.
     */
    [[nodiscard]] auto size() const
    {
        return static_cast<uint32_t>(topo_order.size());
    }

    /**
     * Reimplementation of `num_gates`.
     */
    [[nodiscard]] auto num_gates() const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        return static_cast<uint32_t>(topo_order.size() - offset);
    }

    /**
     * Reimplementation of `node_to_index`.
     */
    [[nodiscard]] uint32_t node_to_index(node const& n) const
    {
        return static_cast<uint32_t>(
            std::distance(std::begin(topo_order), std::find(std::begin(topo_order), std::end(topo_order), n)));
    }

    /**
     * Reimplementation of `index_to_node`.
     */
    [[nodiscard]] node index_to_node(uint32_t index) const
    {
        return topo_order.at(index);
    }

    /**
     * Reimplementation of `foreach_node`.
     */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.cbegin(), topo_order.cend(), fn);
    }

    /*! \brief Implementation of `foreach_node` in reverse topological order. */
    template <typename Fn>
    void foreach_node_reverse(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.rbegin(), topo_order.rend(), fn);
    }

    /**
     * Reimplementation of `foreach_gate`.
     */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.cbegin(), topo_order.cend() - offset, fn);
    }

    /*! \brief Implementation of `foreach_gate` in reverse topological order. */
    template <typename Fn>
    void foreach_gate_reverse(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.rbegin(), topo_order.rend() - offset, fn);
    }

    /*! \brief Reimplementation of `foreach_pi`. */
    template <typename Fn>
    void foreach_pi(Fn&& fn) const
    {
        mockturtle::detail::foreach_element_if(
            topo_order.cbegin() + num_c, topo_order.cbegin() + num_c + num_p + num_r,
            [this](auto n) { return ntk.is_pi(n); }, fn);
    }

    /*! \brief Reimplementation of `foreach_ro`. */
    template <typename Fn>
    void foreach_ro(Fn&& fn) const
    {
        mockturtle::detail::foreach_element_if(
            topo_order.cbegin() + num_c, topo_order.cbegin() + num_c + num_p + num_r,
            [this](auto n) { return ntk.is_ro(n); }, fn);
    }

    node ro_at(uint32_t index) const
    {
        std::vector<node> r_os;
        foreach_ro([&](const auto& n) { r_os.emplace_back(n); });
        return r_os[index];
    }

    /**
     * Reimplementation of `foreach_po`.
     */
    template <typename Fn>
    void foreach_po(Fn&& fn) const
    {
        Ntk::foreach_po(fn);
    }

    [[nodiscard]] uint32_t num_pos() const
    {
        return Ntk::num_pos();
    }
    /*! \brief flag indicating inverters with special treatment in the ordering network */
    [[nodiscard]] bool nc_inv_flag() const
    {
        return fo_inv_flag;
    }

    [[nodiscard]] uint64_t nc_inv_num() const
    {
        return fo_inv_flag_num;
    }

    /**
     * Two PIs are related to each other, when they are connected to the same two fan-in gate and between the PI and
     * this gate are only fan-out nodes or inverters.
     */

    /* ! \brief getter for PIs connected to a fan-out and related to two PIs */
    [[nodiscard]] const std::vector<node>& get_fo_two() const
    {
        return wait;
    }
    /* ! \brief getter for PIs connected to fan-out node and related to one PI */
    [[nodiscard]] const std::vector<node>& get_fo_one() const
    {
        return second_wait;
    }
    /* ! \brief getter for PIs related to one PI */
    [[nodiscard]] const std::vector<node>& get_pi_to_pi() const
    {
        return third_wait;
    }

    void update_topo()
    {
        topo_order.clear();

        this->incr_trav_id();
        this->incr_trav_id();
        // reserve vector capacity
        topo_order.reserve(this->size());
        wait.reserve(static_cast<std::uint64_t>(num_p));
        second_wait.reserve(static_cast<std::uint64_t>(num_p));
        third_wait.reserve(static_cast<std::uint64_t>(num_p));

        /* constants and PIs */
        const auto c0 = this->get_node(this->get_constant(false));
        topo_order.push_back(c0);
        ++num_c;
        this->set_visited(c0, this->trav_id());

        if (const auto c1 = this->get_node(this->get_constant(true)); this->visited(c1) != this->trav_id())
        {
            topo_order.push_back(c1);
            this->set_visited(c1, this->trav_id());
            ++num_c;
        }

        this->foreach_ci(
            [this](auto n)
            {
                if (this->visited(n) != this->trav_id())
                {
                    input_sort(n);
                }
            });

        /* The hierarchies of ordered PIs get pushed into topo_view */
        for (std::size_t iter = 0; iter < wait.size(); ++iter)
        {
            topo_order.push_back(wait[iter]);
            this->set_visited(wait[iter], this->trav_id());
        }

        for (std::size_t iter = 0; iter < second_wait.size(); ++iter)
        {
            topo_order.push_back(second_wait[iter]);
            this->set_visited(second_wait[iter], this->trav_id());
        }

        for (std::size_t iter = 0; iter < third_wait.size(); ++iter)
        {
            topo_order.push_back(third_wait[iter]);
            this->set_visited(third_wait[iter], this->trav_id());
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

        Ntk::foreach_co(
            [this](auto f)
            {
                /* node was already visited */
                if (this->visited(this->get_node(f)) == this->trav_id())
                {
                    return;
                }

                create_topo_rec(this->get_node(f));
            });
    }

  private:
    /**
     * creates the topological order starting at a given node
     * @param n starting node
     */
    void create_topo_rec(node const& n)
    {
        /* is permanently marked? */
        if (this->visited(n) == this->trav_id())
        {
            return;
        }

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

    void get_connecting_nodes(std::vector<node>& connecting_node, node& current_node, const node& fon, bool& inv_flag,
                              bool& is_fan_out)
    {
        /* take fan-out as first candidate for connecting_node */
        current_node = fon;

        if (ntk.is_inv(current_node))
        {
            /* skip inverters */
            ntk.foreach_fanout(fon, [&current_node](const auto& fon_inv) { current_node = fon_inv; });
            fo_inv_flag = true;
            ++fo_inv_flag_num;
            inv_flag = true;
        }

        if (const auto fc = fanins(ntk, current_node); fc.fanin_nodes.size() == 2)
        {
            /* current_node is a 2 fan-in node [connecting_node has only one entry] */
            connecting_node.push_back(current_node);
        }
        else if (ntk.is_fanout(current_node))
        {
            /* current_node is a fan-out node (FO) [connecting_node has two entries] */
            is_fan_out = true;

            /* take fan-outs of the FO as candidates for connecting_node */
            connecting_node.clear();
            ntk.foreach_fanout(current_node,
                               [this, &connecting_node](const auto& fon_two)
                               {
                                   if (ntk.is_fanout(fon_two))
                                   {
                                       return;
                                   }
                                   if (ntk.is_inv(fon_two))
                                   {
                                       /*Skip Inverter*/
                                       ntk.foreach_fanout(fon_two,
                                                          [&connecting_node](const auto& fon_inv) {
                                                              connecting_node.insert(connecting_node.cbegin(), fon_inv);
                                                          });
                                   }
                                   else
                                   {
                                       connecting_node.push_back(fon_two);
                                   }
                               });
        }
    }

    void compute_pi_ranking(const std::vector<node>& connecting_node, const bool& is_fan_out, node const& n,
                            const bool& inv_flag)
    {
        bool already_one_pi = false;
        node first_pi;
        for (std::size_t i = 0; i < connecting_node.size(); ++i)
        {
            ntk.foreach_fanin(connecting_node[i],
                              [this, &is_fan_out, &already_one_pi, &first_pi, &n, &inv_flag](const auto& fi)
                              {
                                  auto fin_inp = ntk.get_node(fi);
                                  /* ignore inverters */
                                  if (ntk.is_inv(fin_inp))
                                  {
                                      const auto fis_inv = fanins(ntk, fin_inp);
                                      fin_inp            = fis_inv.fanin_nodes[0];
                                  }
                                  if (fin_inp != n)
                                  {
                                      if (ntk.is_pi(fin_inp))
                                      {
                                          if (is_fan_out)
                                          {
                                              /* when a FO is related to a PI, it can be one or two PIs */
                                              if (already_one_pi)
                                              {
                                                  /* FO related to two incoming PIs */
                                                  /* push PI and PI stored in first_PI */
                                                  auto check_n = second_wait[second_wait.size() - 2];
                                                  second_wait.erase(second_wait.end() - 2);
                                                  second_wait.erase(second_wait.end() - 1);
                                                  if (check_n == n)
                                                  {
                                                      wait.push_back(n);
                                                      this->set_visited(n, this->trav_id());
                                                      wait.push_back(first_pi);
                                                      this->set_visited(n, this->trav_id());
                                                  }
                                                  else
                                                  {
                                                      assert(false);
                                                  }

                                                  if (this->visited(fin_inp) != this->trav_id())
                                                  {
                                                      wait.push_back(fin_inp);
                                                      this->set_visited(fin_inp, this->trav_id());
                                                  }
                                                  already_one_pi = false;
                                              }
                                              else
                                              {
                                                  /* store PI in first_PI, for the case two related PIs */
                                                  /* Store the currently visited node */
                                                  if (this->visited(n) != this->trav_id())
                                                  {
                                                      second_wait.push_back(n);
                                                      this->set_visited(n, this->trav_id());
                                                  }
                                                  /* Store the connected node */
                                                  if (this->visited(fin_inp) != this->trav_id())
                                                  {
                                                      second_wait.push_back(fin_inp);
                                                      this->set_visited(fin_inp, this->trav_id());
                                                  }
                                                  /* Mark FOs with already one related PI */
                                                  already_one_pi = true;
                                                  first_pi       = fin_inp;
                                              }
                                          }
                                          else
                                          {
                                              if (inv_flag)
                                              {
                                                  /* when a PI is related to only one PI it is ranked third */
                                                  if (this->visited(n) != this->trav_id())
                                                  {
                                                      third_wait.push_back(n);
                                                      this->set_visited(n, this->trav_id());
                                                  }
                                                  if (this->visited(fin_inp) != this->trav_id())
                                                  {
                                                      third_wait.push_back(fin_inp);
                                                      this->set_visited(fin_inp, this->trav_id());
                                                  }
                                              }
                                              else
                                              {
                                                  /* when a PI is related to only one PI it is ranked third */
                                                  if (this->visited(fin_inp) != this->trav_id())
                                                  {
                                                      third_wait.push_back(fin_inp);
                                                      this->set_visited(fin_inp, this->trav_id());
                                                  }
                                                  if (this->visited(n) != this->trav_id())
                                                  {
                                                      third_wait.push_back(n);
                                                      this->set_visited(n, this->trav_id());
                                                  }
                                              }
                                          }
                                      }
                                  }
                              });
        }
    }

    /**
     * Sort the PIs. PIs connected to the same two fan-in node are grouped together.
     */
    void input_sort(node const& n)
    {
        /* node(s), which may connect PIs */
        std::vector<node> connecting_node;
        bool              inv_flag = false;

        /* currently viewed node */
        node current_node;

        ntk.foreach_fanout(n,
                           [this, &n, &connecting_node, &current_node, &inv_flag](const auto& fon)
                           {
                               bool is_fan_out = false;

                               /*get nodes with PIs as fan-ins*/
                               get_connecting_nodes(connecting_node, current_node, fon, inv_flag, is_fan_out);

                               /*compute the PI ranking*/
                               compute_pi_ranking(connecting_node, is_fan_out, n, inv_flag);
                           });
    }

    // private
    Ntk               ntk;
    std::vector<node> topo_order{};

    std::vector<node> wait{};         // PI connected to FO and related to 2 PIs
    std::vector<node> second_wait{};  // PI connected to FO and related to 1 PI
    std::vector<node> third_wait{};   // PI related to PI

    bool     fo_inv_flag = false;
    uint64_t fo_inv_flag_num{0u};

    uint32_t num_p;
    uint32_t num_c{0u};
    uint32_t num_r{0u};
};

template <typename Ntk>
class input_ordering_view<Ntk, true> : public Ntk
{
  public:
    explicit input_ordering_view(Ntk const& ntk) : Ntk(ntk) {}
};

template <class T>
input_ordering_view(T const&) -> input_ordering_view<T>;

template <class T>
input_ordering_view(T const&, typename T::signal const&) -> input_ordering_view<T>;

}  // namespace fiction

#endif  // FICTION_INPUT_ORDERING_VIEW_HPP
