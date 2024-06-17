//
// Created by benjamin on 6/17/24.
//

#include "catch2/catch_test_macros.hpp"
#include "fiction/networks/virtual_pi_network.hpp"

using namespace fiction;
//ToDo: Write meaningful test cases
TEST_CASE("Virtual PI view create virtual PI", "[virtual-pi-view]")
{
    technology_network tec{};

    const auto a = tec.create_pi();
    const auto b = tec.create_pi();

    virtual_pi_network vpi{tec};
    const auto c = vpi.create_virtual_pi(a);
    const auto d = vpi.create_virtual_pi(b);

    const auto f1 = vpi.create_and(a, b);
    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    std::cout << "real tec num_gates: " << vpi.size() << std::endl;
    std::cout << "virt tec num_gates: " << vpi.size_virtual() << std::endl;

    vpi.foreach_node([&](const auto& nd)
                     {
                         std::cout << "Nd:" << nd << "\n";
                         vpi.foreach_fanin(nd, [&](const auto& fi)
                                           {
                                               std::cout << "Fis:" << fi << "\n";
                                           });
                         if (vpi.is_virtual_pi(nd)){
                             std::cout << "Is virtual PI \n";
                         }
                     });
    vpi.remove_virtual_input_nodes();

    vpi.foreach_node([&](const auto& nd)
                     {
                         std::cout << "Nd:" << nd << "\n";
                         vpi.foreach_fanin(nd, [&](const auto& fi)
                                           {
                                               std::cout << "Fis:" << fi << "\n";
                                           });
                         if (vpi.is_virtual_pi(nd)){
                             std::cout << "Is virtual PI \n";
                         }
                     });
}
