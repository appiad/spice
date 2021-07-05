#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <Eigen/Dense>
#include <cassert>
#include "Components.h"
#include "Network.h"
#include <fstream>
#include <iostream>

using Eigen::MatrixXd;
 
bool test_vs(){
	MatrixXd expected_result (5,1) ;
	expected_result << 24, -8, 20, -4, 1;
	std::ifstream net_file ("./test_net.txt");
    assert(net_file.is_open());
    Network net = Network();
    net.preprocess_netlist(net_file);
    net.parse_netlist(net_file);
    net.calculate();
    net_file.close();
    return expected_result.isApprox(net.get_result_matrix());
}

bool test_cs(){
	MatrixXd expected_result (3,1) ;
	expected_result << 8, -24, -11;
	std::ifstream net_file2 ("./test_net2.txt");
    assert(net_file2.is_open());
    Network net = Network();
    net.preprocess_netlist(net_file2);
    net.parse_netlist(net_file2);
    net.calculate();
    net_file2.close();
    return expected_result.isApprox(net.get_result_matrix());
}


TEST_CASE( "Circuit test" ) {
	REQUIRE(test_vs() == true);
	REQUIRE(test_cs() == true);
}
