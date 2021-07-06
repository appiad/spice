#ifndef __Network_h__
#define __Network_h__
#include <Eigen/Dense>
#include "Components.h"
#include "Bimap.h"
#include "IdGenerator.h"
#include "utils.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <memory>
using Eigen::MatrixXd;

/* Todo
    - seperate classes to .h and .cpp
    - error checking in preprocess
    - output volts/current in table 
    - make converters their own class (and bimap subclass) and make them private member vars

*/

/**
 * Represents a circuit network
 */
class Network{
public:
    Network();
	void preprocess_netlist(std::ifstream& net_file);
	void parse_netlist(std::ifstream& net_file);
	void calculate();
	const MatrixXd& get_conductance_matrix() const {return _conductance_matrix;}
	const MatrixXd& get_incidence_matrix() const {return _incidence_matrix;}
	const MatrixXd& get_result_matrix() const {return _result_matrix;}
	const MatrixXd& get_z_matrix() const {return _z_matrix;}

private:
	MatrixXd _incidence_matrix;
	MatrixXd _conductance_matrix;
	MatrixXd _z_matrix;
	MatrixXd _result_matrix;
    Bimap node_ids_names;
    IdGenerator node_id_gen, component_id_gen, voltage_source_id_gen, 
        current_source_id_gen, resistor_id_gen;
	int _num_nodes, _num_components, _num_voltage_sources, _num_resistors;
    
};
#endif