//
// Created by marcel on 02.08.18.
//

#ifndef FICTION_GRID_TOPOLOGY_H
#define FICTION_GRID_TOPOLOGY_H

#include <memory>
#include <random>
#include <boost/graph/topology.hpp>

template <std::size_t D, class R = std::mt19937>
class grid_topology : public boost::convex_topology<D>
{
    using distribution = std::uniform_int_distribution<std::size_t>;

public:
    using point_type = typename boost::convex_topology<D>::point_type;
    using point_difference_type = typename boost::convex_topology<D>::point_difference_type;

    explicit grid_topology(std::size_t edge_length)
            :
            edge_length(edge_length)
    {
        rand = std::make_shared<R>(std::random_device{}());
        distr = std::make_shared<distribution>(0, edge_length);
    }

    grid_topology(R& generator, std::size_t edge_length)
            :
            edge_length(edge_length)
    {
        rand = std::make_shared<R>(generator);
        distr = std::make_shared<distribution>(0, edge_length);
    }

//    double distance(point_type a, point_type b) const  // euclidean
//    {
//        double sum = 0;
//        for (auto i = 0u; i < D; ++i)
//            sum += std::pow(static_cast<double>(b[i]) - a[i], 2);
//
//        return std::sqrt(sum);
//    }

    double distance(point_type a, point_type b) const  // manhattan
    {
        auto equal = false;
        double sum = 0;
        for (auto i = 0u; i < D; ++i)
        {
            sum += std::abs(static_cast<double>(b[i]) - a[i]);
            if (a[i] == b[i])
                equal = true;
        }

        return equal ? std::numeric_limits<double>::infinity() : sum;  // prevent elements from being too close together
    }

    point_type move_position_toward(point_type a, double fraction, point_type b) const
    {
        point_type p{};
        for (auto i = 0u; i < D; ++i)
            p[i] = a[i] + std::round((b[i] - a[i]) * fraction);

        return p;
    }

    point_type random_point() const
    {
        point_type p{};
        for (auto i = 0u; i < D; ++i)
            p[i] = (*distr)(*rand);

        return p;
    }

    point_type bound(point_type a) const
    {
        point_type p{};
        for (auto i = 0u; i < D; ++i)
            p[i] = std::min(edge_length, std::max(-edge_length, a[i]));

        return p;
    }

    std::size_t distance_from_boundary(point_type a) const
    {
        std::size_t dist = std::abs(edge_length - a[0]);
        for (auto i = 1u; i < D; ++i)
            dist = std::min(dist, std::abs(edge_length - a[i]));

        return dist;
    }

    point_type center() const
    {
        point_type p{};
        for (auto i = 0u; i < D; ++i)
            p[i] = static_cast<std::size_t>(edge_length / 2);

        return p;
    }

    point_type origin() const
    {
        point_type p{};
        for (auto i = 0u; i < D; ++i)
            p[i] = 0;

        return p;
    }

    point_difference_type extent() const
    {
        point_difference_type p{};
        for (auto i = 0u; i < D; ++i)
            p[i] = edge_length;

        return p;
    }

private:
    std::size_t edge_length;
    std::shared_ptr<R> rand = nullptr;
    std::shared_ptr<distribution> distr = nullptr;
};

#endif //FICTION_GRID_TOPOLOGY_H
