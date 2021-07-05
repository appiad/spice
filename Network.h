#ifndef __Network_h__
#define __Network_h__
#include <Eigen/Dense>
#include "Components.h"
#include "Bimap.h"
#include "utils.h"
using Eigen::MatrixXd;



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
	int _num_nodes, _num_components, _num_voltage_sources;
    
};

Network::Network(){
	_num_nodes = 0;
	_num_components = 0;
	_num_voltage_sources = 0;
}

// get num nodes, volt_sources, components
/*todo: use 1 component name set
    - properly handle node 0
    - check stirng.char comparison
*/
void Network::preprocess_netlist(std::ifstream& net_file){
    std::string line;
    unsigned line_num = 1;
    std::vector<std::string> tokens;
	std::unordered_set<std::string> component_names;
    std::unordered_set<std::string> node_names;

	std::string component_name, anode_name, cathode_name;
	double value;

    while (std::getline(net_file, line)){
        // skip first line which is model description
        if (line_num == 1){
            ++line_num;
            continue;
        }
        get_tokens_from_line(line, tokens);
        assert(tokens.size() == 4);
		component_name = tokens[0];
		anode_name = tokens[1];
		cathode_name = tokens[2];
		value = std::stod(tokens[3]);

        if (component_name[0] == 'V' || component_name[0] == 'v'){
            ++_num_voltage_sources;
            ++_num_components;
            component_names.insert(component_name);
        }
        else if (component_name[0] == 'I' || component_name[0] == 'i'){
            ++_num_components;
            component_names.insert(component_name);
        }
        else if (component_name[0] == 'R' || component_name[0] == 'r'){
            ++_num_components;
            component_names.insert(component_name);
        }
        else{
            std::cout << "failed to find component type" << std::endl;
        }
        if (anode_name != "0")
            node_names.insert(anode_name);
        if (cathode_name != "0")
            node_names.insert(cathode_name);
        _num_nodes = node_names.size();
        tokens.clear();
        ++line_num;
    }
	net_file.clear();
	net_file.seekg(0);
}

void Network::parse_netlist(std::ifstream& net_file){
	// component id to Component
    std::unordered_map<int, std::shared_ptr<Component>> components;

    // bidirectional node id/name hashmaps
    std::unordered_map<int, std::string> node_id_to_name;
    std::unordered_map<std::string, int> node_name_to_id;

	//bidrec comp id/name

	node_id_to_name[0] = "0";
	node_name_to_id["0"] = 0;

    std::unordered_map<int, std::string>::iterator node_id_itr;
    std::unordered_map<std::string, int>::iterator node_name_itr;

    std::vector<std::string> tokens;
    std::shared_ptr<Component> component_ptr;
	unsigned line_num = 1;
	std::string line, component_name, component_type, anode_name, cathode_name;
    int anode_id, cathode_id;
    double value;

	_incidence_matrix = MatrixXd (_num_nodes, _num_components);
    _conductance_matrix = MatrixXd (_num_voltage_sources+_num_nodes,_num_voltage_sources+_num_nodes);
	_z_matrix = MatrixXd (_num_voltage_sources+_num_nodes,1);
	_result_matrix = MatrixXd (_num_voltage_sources+_num_nodes,1);

	_incidence_matrix.fill(0);
    _conductance_matrix.fill(0);
	_z_matrix.fill(0);
	_result_matrix.fill(0);
	
    while (std::getline(net_file, line)){
		std::cout << "line no: " << line_num << '\n';
		// skip first line which is model description
        if (line_num == 1){
            ++line_num;
            continue;
        }
    	get_tokens_from_line(line, tokens);
        assert(tokens.size() == 4);
        component_name = tokens[0];
		anode_name = tokens[1];
		cathode_name = tokens[2];
        value = std::stod(tokens[3]);
       
        // Node id initlialization 

        node_name_itr = node_name_to_id.find(anode_name);
        if (node_name_itr == node_name_to_id.end() ){
            anode_id = generate_node_id();
            node_name_to_id[anode_name] = anode_id;
            node_id_to_name[anode_id] = anode_name;
        }
        else{
            anode_id = node_name_itr->second;
        }

        node_name_itr = node_name_to_id.find(cathode_name);
        if (node_name_itr == node_name_to_id.end() ){
            cathode_id = generate_node_id();
            node_name_to_id[cathode_name] = cathode_id;
            node_id_to_name[cathode_id] = cathode_name;
        }
        else{
            cathode_id = node_name_itr->second;
        }
        
        if (component_name[0] == 'V' || component_name[0] == 'v'){
            component_type = 'v';
            component_ptr = std::make_shared<Component> (
                Component(generate_component_id(), generate_voltage_source_id(), value, anode_id, 
                        cathode_id, component_name, component_type));

			// add incidence of voltage sources to conductance_matrix
			int conductance_col = _num_components - _num_voltage_sources + (component_ptr->get_type_id()-1);
			int z_row = _num_nodes + (component_ptr->get_type_id()-1);
			if (anode_id != 0){
				_conductance_matrix(anode_id-1,conductance_col) = 1;
				//transpose
				_conductance_matrix(conductance_col, anode_id-1) = 1;
			}
			
			if (cathode_id!= 0){
				_conductance_matrix(cathode_id-1,conductance_col) = -1;
				//transpose
				_conductance_matrix(conductance_col, cathode_id-1) = -1;
			}

			_z_matrix(z_row, 0) = component_ptr->get_value();
        }
        else if (component_name[0] == 'I' || component_name[0] == 'i'){
            component_type = 'i';
            component_ptr = std::make_shared<Component> (
                Component(generate_component_id(), generate_current_source_id(), value, anode_id, 
                        cathode_id, component_name, component_type));
        }
        else if (tokens[0][0] == 'R' || tokens[0][0] == 'r'){
            component_type = 'r';
            component_ptr = std::make_shared<Component> (
                Component(generate_component_id(), generate_resistor_id(), value, anode_id, 
                        cathode_id, component_name, component_type)
            );
        
            
            if (anode_id != 0){
                // node 1 diagonal 
                _conductance_matrix(anode_id-1,anode_id-1) += 1/component_ptr->get_value();
                //node 1 off-diagonal
                if (cathode_id != 0)
                    _conductance_matrix(anode_id-1, cathode_id-1) -= 1/component_ptr->get_value();
            }
            
            if (cathode_id != 0){
                // node 2 diagonal
                _conductance_matrix(cathode_id-1,cathode_id-1) += 1/component_ptr->get_value();
                // node 2 off-diagonal
                if (anode_id != 0)
                    _conductance_matrix(cathode_id-1, anode_id-1) -= 1/component_ptr->get_value();
            }
        }
        // todo: do this check and exit before all the others
        else{
            std::cout << "failed to find component type" << std::endl;
        }
        //Incidence
        if (anode_id != 0){
            _incidence_matrix(anode_id-1,component_ptr->get_component_id()-1) = 1;
        }
        if (cathode_id != 0){
            _incidence_matrix(cathode_id-1,component_ptr->get_component_id()-1) = -1;
        }
		++line_num;
        components[component_ptr->get_component_id()] = component_ptr;
        tokens.clear();
    }

	//debug ID's 
	for (node_id_itr = node_id_to_name.begin();node_id_itr != node_id_to_name.end();++node_id_itr){
		std::cout << "nID: " << node_id_itr->first << ", nName:" <<node_id_itr->second << std::endl;
	}
	std::cout << "\n\n";
	for (auto itr = components.begin();itr != components.end();++itr){
		std::cout << "cID: " << itr->first << ", c: " 
			<< itr->second->get_type() << itr->second->get_type_id() << std::endl;
	}
}

void Network::calculate(){
	_result_matrix = _conductance_matrix.colPivHouseholderQr().solve(_z_matrix);
}

#endif