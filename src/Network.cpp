#include "Network.h"
#include "spdlog/spdlog.h"

Network::Network(){
	_num_nodes = 0;
	_num_components = 0;
	_num_voltage_sources = 0;
    _num_resistors = 0;
}


/**
 * Validates netlist and counts number of components, nodes, and voltage sources. Outputs 
 * error and exits program if invalid netlist format is given,
 * Clears file error state and resets stream position to beginning before returning.
 * 
 * @param net_file Stream associated with an open netlist file
 */

void Network::preprocess_netlist(std::ifstream& net_file){
    spdlog::info("Welcome to spdlog!");
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
            ++_num_resistors;
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
/**
 * Parses netlist to populate conducatnce and incidence matrices as well as store
 * associations between node/component id's and their given names from the netlist.
 * 
 * @param net_file Stream associated with an open netlist file.
 */
void Network::parse_netlist(std::ifstream& net_file){
	// component id to Component
    std::unordered_map<int, std::shared_ptr<Component>> components;

    // bidirectional node id/name hashmaps
    Bimap node_id_name_converter;
    Bimap component_id_name_converter;
	//bidrec comp id/name

    // add ground node
    node_id_name_converter.insert(0,"0");

    std::pair<bool,std::pair<int,std::string>> node_id_name;

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
		std::cout << "line no: " << line_num << std::endl;
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
       

        // Generate node id's if we haven't already
        node_id_name = node_id_name_converter.find(anode_name);
        if (!node_id_name.first){
            anode_id = node_id_gen.generate();
            node_id_name_converter.insert(anode_id, anode_name);
        }
        else{
            anode_id = node_id_name.second.first;
        }

        node_id_name = node_id_name_converter.find(cathode_name);
        if (!node_id_name.first){
            cathode_id = node_id_gen.generate();
            node_id_name_converter.insert(cathode_id,cathode_name);
        }
        else{
            cathode_id = node_id_name.second.first;
        }
        
        // Determine if the component is a voltage source, resistor, or current source
        if (component_name[0] == 'V' || component_name[0] == 'v'){
            component_type = 'v';
            component_ptr = std::make_shared<Component> (
                Component(component_id_gen.generate(), voltage_source_id_gen.generate(), value, anode_id, 
                        cathode_id, component_name, component_type));

			// add incidence of voltage sources to conductance_matrix
			int conductance_col = _conductance_matrix.cols() - _num_voltage_sources 
                + (component_ptr->get_type_id()-1);
			int z_row = _num_nodes + (component_ptr->get_type_id()-1);
            std::cout << anode_id <<_conductance_matrix.rows()<< "   " 
                << conductance_col << _conductance_matrix.cols() << std::endl;
			if (anode_id != 0){
				_conductance_matrix(anode_id-1,conductance_col) = 1;
				//transpose
                std::cout << "ba" << std::endl;
				_conductance_matrix(conductance_col, anode_id-1) = 1;
			}
			std::cout << "b" << std::endl;
			if (cathode_id!= 0){
				_conductance_matrix(cathode_id-1,conductance_col) = -1;
				//transpose
				_conductance_matrix(conductance_col, cathode_id-1) = -1;
			}
            // add voltage value to other side of equation for this node
			_z_matrix(z_row, 0) = component_ptr->get_value();
            std::cout << "c" << std::endl;
        }
        else if (component_name[0] == 'I' || component_name[0] == 'i'){
            component_type = 'i';
            component_ptr = std::make_shared<Component> (
                Component(component_id_gen.generate(), current_source_id_gen.generate(), value, anode_id, 
                        cathode_id, component_name, component_type));
            // add current value to other side of equation for this node
            if (cathode_id != 0){
                _z_matrix(cathode_id-1,0) = component_ptr->get_value();
            }
            if (anode_id != 0){
                _z_matrix(anode_id-1,0) = -component_ptr->get_value();
            }
        }
        else if (tokens[0][0] == 'R' || tokens[0][0] == 'r'){
            component_type = 'r';
            component_ptr = std::make_shared<Component> (
                Component(component_id_gen.generate(), resistor_id_gen.generate(), value, anode_id, 
                        cathode_id, component_name, component_type)
            );
        
            // Add the effect of the resistor for each nodes conducatnce
            if (anode_id != 0){
                _conductance_matrix(anode_id-1,anode_id-1) += 1/component_ptr->get_value();
                if (cathode_id != 0)
                    _conductance_matrix(anode_id-1, cathode_id-1) -= 1/component_ptr->get_value();
            }
            
            if (cathode_id != 0){
                _conductance_matrix(cathode_id-1,cathode_id-1) += 1/component_ptr->get_value();
                if (anode_id != 0)
                    _conductance_matrix(cathode_id-1, anode_id-1) -= 1/component_ptr->get_value();
            }
        }
        // todo: do this check and exit before all the others
        else{
            std::cout << "failed to find component type" << std::endl;
        }

        // Add the nodes the component is connected to (excluding ground) to the incidence matrix
        if (anode_id != 0){
            _incidence_matrix(anode_id-1,component_ptr->get_component_id()-1) = 1;
        }
        if (cathode_id != 0){
            _incidence_matrix(cathode_id-1,component_ptr->get_component_id()-1) = -1;
        }
        components[component_ptr->get_component_id()] = component_ptr;
        component_id_name_converter.insert(component_ptr->get_component_id(), component_name);
        tokens.clear();
        ++line_num;
    }

	//debug ID's 
	// for (node_id_itr = node_id_to_name.begin();node_id_itr != node_id_to_name.end();++node_id_itr){
	// 	std::cout << "nID: " << node_id_itr->first << ", nName:" <<node_id_itr->second << std::endl;
	// }
	// std::cout << "\n\n";
	// for (auto itr = components.begin();itr != components.end();++itr){
	// 	std::cout << "cID: " << itr->first << ", c: " 
	// 		<< itr->second->get_type() << itr->second->get_type_id() << std::endl;
	// }
}

/**
 * Solves for node voltages and current through voltage sources.
 * 
 * @param net_file Stream associated with an open netlist file
 */
void Network::calculate(){
	_result_matrix = _conductance_matrix.colPivHouseholderQr().solve(_z_matrix);
}