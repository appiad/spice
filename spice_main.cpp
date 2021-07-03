#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <cassert>
#include <memory>

#include <Eigen/Dense>
#include "Components.h"
#include "Network.h"
#include "spdlog/spdlog.h"

using Eigen::MatrixXd;

int main(int argc, char const *argv[])
{
    spdlog::info("Welcome to spdlog!");
    std::ifstream net_file ("netlist.txt");
    if (!net_file.is_open()){
        std::cout << "Failed to open in file" << std::endl;
        return -1;
    }
    Network net = Network();
    net.preprocess_netlist(net_file);
    net.parse_netlist(net_file);
    net.calculate();

	std::cout << "Conductance\n" << net.get_conductance_matrix() << '\n';
	std::cout << "Incidence\n" << net.get_incidence_matrix() << '\n';
	std::cout << "z\n" << net.get_z_matrix() << '\n';
	std::cout << "result\n" << net.get_result_matrix() << '\n';
	net_file.close();
    return 0;
}

// <comp type><name> <+> <-> <value>
// https://lpsa.swarthmore.edu/Systems/Electrical/mna/MNA3.html#Notational_Convention
/*todo: 
	- look at initializing node 0 to id -1, gets rid of us having to use offset
*/ 