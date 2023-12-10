//
// Created by benjamin on 11.04.23.
//

#ifndef FICTION_INPUT_ORDERING_VIEW_HPP
#define FICTION_INPUT_ORDERING_VIEW_HPP

#include "fiction/traits.hpp"
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
 * A variation of `mockturtle::topo_view` that computes the topological order after sorting the inputs. This is
 * according to the fan-out connections of their respective inputs. This is part of the Signal Distribution Network I:
 * Input Ordering.
 *
 * In this context, Input Ordering refers to the process of re-arranging the PIs of the network. The new order is based
 * on their interconnections with logic gates within the network. By aligning PIs that are connected to the same gates,
 * wire crossings and area can be decreased during placement and routing.
 *
 * @tparam Ntk `mockturtle` network type. This provides the representation of the digital logic network on which the
 * view operations will be conducted.
 * @tparam sorted An optional flag that indicates whether `Ntk` has already been wrapped in an `input_ordering_view`. If
 * this is the case, the existing order is preserved.
 */
template <typename Ntk, bool sorted = fiction::is_input_ordered_v<Ntk>>
class input_ordering_view
{};

template <typename Ntk>
class input_ordering_view<Ntk, false> : public mockturtle::immutable_view<Ntk>
{
  public:
    using storage = typename Ntk::storage;
    using node    = typename Ntk::node;
    using signal  = typename Ntk::signal;

    static constexpr bool is_input_ordered = true;

    /*! \brief Default constructor.
     *
     * Constructs input ordering view on another network.
     */
    explicit input_ordering_view(Ntk const& ntk) :
            mockturtle::immutable_view<Ntk>(ntk),
            inp_ntk{ntk},
            num_p{ntk.num_pis()}
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

        update_topo_ordered();
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
            std::distance(std::cbegin(topo_order), std::find(std::cbegin(topo_order), std::end(topo_order), n)));
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
        mockturtle::detail::foreach_element(topo_order.crbegin(), topo_order.crend(), fn);
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
        mockturtle::detail::foreach_element(topo_order.crbegin(), topo_order.crend() - offset, fn);
    }

    /*! \brief Reimplementation of `foreach_pi`. */
    template <typename Fn>
    void foreach_pi(Fn&& fn) const
    {
        mockturtle::detail::foreach_element_if(
            topo_order.cbegin() + num_c, topo_order.cbegin() + num_c + num_p + num_r,
            [this](auto n) { return inp_ntk.is_pi(n); }, fn);
    }

    /*! \brief Reimplementation of `foreach_ro`. */
    // This is only for sequential circuits. The SFINAE is used, since foreach_ro will be called inside the equiv_check
    template <typename Fn>
    void foreach_ro(Fn&& fn) const
    {
        if constexpr (mockturtle::has_is_ro_v<Ntk>)
        {
            mockturtle::detail::foreach_element_if(
                topo_order.cbegin() + num_c, topo_order.cbegin() + num_c + num_p + num_r,
                [this](auto n) { return inp_ntk.is_ro(n); }, fn);
        }
        else
        {
            // do nothing
        }
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
    /**
     * Flag indicating an inverter after a PI
     */
    [[nodiscard]] bool pi_inv_flag() const
    {
        return fo_inv_flag;
    }
    /**
     * Number of PIs with inverters as fan-outs
     */
    [[nodiscard]] uint64_t nc_inv_num() const
    {
        return fo_inv_flag_num;
    }

    /**
     * Getter for PIs connected to a fan-out and related to two PIs
     * */
    [[nodiscard]] const std::vector<node>& get_fo_two() const
    {
        return first_rank;
    }
    /**
     * Getter for PIs connected to fan-out node and related to one PI
     */
    [[nodiscard]] const std::vector<node>& get_fo_one() const
    {
        return second_rank;
    }
    /**
     * Getter for PIs related to one PI
     */
    [[nodiscard]] const std::vector<node>& get_pi_to_pi() const
    {
        return third_rank;
    }

    /**
     * Function ordering the PIs and ordering the network topologically afterwards
     */
    void update_topo_ordered()
    {
        topo_order.clear();

        this->incr_trav_id();
        this->incr_trav_id();
        // reserve vector capacity
        topo_order.reserve(this->size());
        first_rank.reserve(static_cast<std::uint64_t>(num_p));
        second_rank.reserve(static_cast<std::uint64_t>(num_p));
        third_rank.reserve(static_cast<std::uint64_t>(num_p));

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
        for (std::size_t iter = 0; iter < first_rank.size(); ++iter)
        {
            topo_order.push_back(first_rank[iter]);
            this->set_visited(first_rank[iter], this->trav_id());
        }

        for (std::size_t iter = 0; iter < second_rank.size(); ++iter)
        {
            topo_order.push_back(second_rank[iter]);
            this->set_visited(second_rank[iter], this->trav_id());
        }

        for (std::size_t iter = 0; iter < third_rank.size(); ++iter)
        {
            topo_order.push_back(third_rank[iter]);
            this->set_visited(third_rank[iter], this->trav_id());
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
     * The input network
     */
    Ntk inp_ntk;
    /**
     * Topological view of the input network
     */
    std::vector<node> topo_order{};
    /**
     * Rank One: Primary Inputs (PIs) connected to Fan-Outs (FOs) and related to two PIs.
     */
    std::vector<node> first_rank{};
    /**
     * Rank Two: PIs connected to FOs and related to one PI.
     */
    std::vector<node> second_rank{};
    /**
     * Rank Three: PIs related to other PIs.
     */
    std::vector<node> third_rank{};
    /**
     * Flag indicating an inverter at the fan-out of PIs
     */
    bool fo_inv_flag = false;
    /**
     * Number of inverters at fan-outs of PIs
     */
    uint64_t fo_inv_flag_num{0u};
    /**
     * Number of PIs.
     */
    uint32_t num_p;
    /**
     * Number of constants.
     */
    uint32_t num_c{0u};
    /**
     * Number of registers.
     */
    uint32_t num_r{0u};
    /**
     * Creates the topological ordering starting at a given node `n`.

     * @param n Starting node.
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

    /**
     * A function that gets the connecting nodes between PIs. A connecting node is defined as a two-input node,
     * that has two PIs as its fan-ins when bypassing inverters and fan-outs.
     *
     * @param connecting_node Node that has two PIs as fan-ins skipping inverter and fan-out nodes.
     * @param current_node Currently visited node.
     * @param fon Fan-out node.
     * @param inv_flag Flag indicating an inverter as fan-out.
     * @param is_fan_out Flag indicating a fan-out node
     */
    void get_connecting_nodes(std::vector<node>& connecting_node, node& current_node, const node& fon, bool& inv_flag,
                              bool& is_fan_out)
    {
        /* take fan-out as first candidate for connecting_node */
        current_node = fon;

        if (inp_ntk.is_inv(current_node))
        {
            /* skip inverters */
            inp_ntk.foreach_fanout(fon, [&current_node](const auto& fon_inv) { current_node = fon_inv; });
            fo_inv_flag = true;
            ++fo_inv_flag_num;
            inv_flag = true;
        }

        if (const auto fc = fanins(inp_ntk, current_node); fc.fanin_nodes.size() == 2)
        {
            /* current_node is a 2 fan-in node [connecting_node has only one entry] */
            connecting_node.push_back(current_node);
        }
        else if (inp_ntk.is_fanout(current_node))
        {
            /* current_node is a fan-out node (FO) [connecting_node has two entries] */
            is_fan_out = true;

            /* take fan-outs of the FO as candidates for connecting_node */
            connecting_node.clear();
            inp_ntk.foreach_fanout(current_node,
                                   [this, &connecting_node](const auto& fon_two)
                                   {
                                       if (inp_ntk.is_fanout(fon_two))
                                       {
                                           return;
                                       }
                                       if (inp_ntk.is_inv(fon_two))
                                       {
                                           /*Skip Inverter*/
                                           inp_ntk.foreach_fanout(
                                               fon_two, [&connecting_node](const auto& fon_inv)
                                               { connecting_node.insert(connecting_node.cbegin(), fon_inv); });
                                       }
                                       else
                                       {
                                           connecting_node.push_back(fon_two);
                                       }
                                   });
        }
    }

    /**
     * Compute the ranking of PIs, also determining the ordering in the network.
     *
     * @param connecting_node Node that has two PIs as fan-ins skipping inverter and fan-out nodes.
     * @param is_fan_out Flag indicating a fan-out node
     * @param n Network node.
     * @param inv_flag Flag indicating an inverter as fan-out.
     *
     */
    void compute_pi_ranking(const std::vector<node>& connecting_node, const bool& is_fan_out, node const& n,
                            const bool& inv_flag)
    {
        bool already_one_pi = false;
        node first_pi;
        for (std::size_t i = 0; i < connecting_node.size(); ++i)
        {
            inp_ntk.foreach_fanin(connecting_node[i],
                                  [this, &is_fan_out, &already_one_pi, &first_pi, &n, &inv_flag](const auto& fi)
                                  {
                                      auto fin_inp = inp_ntk.get_node(fi);
                                      /* ignore inverters */
                                      if (inp_ntk.is_inv(fin_inp))
                                      {
                                          const auto fis_inv = fanins(inp_ntk, fin_inp);
                                          fin_inp            = fis_inv.fanin_nodes[0];
                                      }
                                      if (fin_inp != n)
                                      {
                                          if (inp_ntk.is_pi(fin_inp))
                                          {
                                              if (is_fan_out)
                                              {
                                                  /* when a FO is related to a PI, it can be one or two PIs */
                                                  if (already_one_pi)
                                                  {
                                                      /* FO related to two incoming PIs */
                                                      /* push PI and PI stored in first_PI */
                                                      auto check_n = second_rank[second_rank.size() - 2];
                                                      second_rank.erase(second_rank.end() - 2);
                                                      second_rank.erase(second_rank.end() - 1);
                                                      if (check_n == n)
                                                      {
                                                          first_rank.push_back(n);
                                                          this->set_visited(n, this->trav_id());
                                                          first_rank.push_back(first_pi);
                                                          this->set_visited(n, this->trav_id());
                                                      }
                                                      else
                                                      {
                                                          assert(false);
                                                      }

                                                      if (this->visited(fin_inp) != this->trav_id())
                                                      {
                                                          first_rank.push_back(fin_inp);
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
                                                          second_rank.push_back(n);
                                                          this->set_visited(n, this->trav_id());
                                                      }
                                                      /* Store the connected node */
                                                      if (this->visited(fin_inp) != this->trav_id())
                                                      {
                                                          second_rank.push_back(fin_inp);
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
                                                          third_rank.push_back(n);
                                                          this->set_visited(n, this->trav_id());
                                                      }
                                                      if (this->visited(fin_inp) != this->trav_id())
                                                      {
                                                          third_rank.push_back(fin_inp);
                                                          this->set_visited(fin_inp, this->trav_id());
                                                      }
                                                  }
                                                  else
                                                  {
                                                      /* when a PI is related to only one PI it is ranked third */
                                                      if (this->visited(fin_inp) != this->trav_id())
                                                      {
                                                          third_rank.push_back(fin_inp);
                                                          this->set_visited(fin_inp, this->trav_id());
                                                      }
                                                      if (this->visited(n) != this->trav_id())
                                                      {
                                                          third_rank.push_back(n);
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
     * Sort the PIs. PIs connected to the same two fan-in nodes are grouped together.
     *
     * @param n Starting node.
     */
    void input_sort(node const& n)
    {
        /* node(s), which may connect PIs */
        std::vector<node> connecting_node;
        bool              inv_flag = false;

        /* currently viewed node */
        node current_node;

        inp_ntk.foreach_fanout(n,
                               [this, &n, &connecting_node, &current_node, &inv_flag](const auto& fon)
                               {
                                   bool is_fan_out = false;

                                   /*get nodes with PIs as fan-ins*/
                                   get_connecting_nodes(connecting_node, current_node, fon, inv_flag, is_fan_out);

                                   /*compute the PI ranking*/
                                   compute_pi_ranking(connecting_node, is_fan_out, n, inv_flag);
                               });
    }
};

template <typename Ntk>
class input_ordering_view<Ntk, true> : public Ntk
{
  public:
    input_ordering_view(Ntk const& ntk) : Ntk(ntk) {}
};

template <class T>
input_ordering_view(T const&) -> input_ordering_view<T>;

template <class T>
input_ordering_view(T const&, typename T::signal const&) -> input_ordering_view<T>;

}  // namespace fiction

#endif  // FICTION_INPUT_ORDERING_VIEW_HPP
