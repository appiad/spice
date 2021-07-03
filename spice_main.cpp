#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <cassert>
#include <memory>

#include <Eigen/Dense>
#include "Components.h"

using Eigen::MatrixXd;

// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// todo: handle unlimited whitespace between tokens
void get_tokens_from_line(std::ifstream& net_file, const std::string& line, std::vector<std::string>& tokens){
    size_t last = 0; 
    size_t next = 0; 
    while ((next = line.find(' ', last)) != std::string::npos){
        tokens.push_back(line.substr(last,next-last));
        last = next + 1;
    }
    tokens.push_back(line.substr(last));
}

// get num nodes, volt_sources, components
/*todo: use 1 component name set
    - properly handle node 0
    - check stirng.char comparison
*/
void preprocess_netlist(std::ifstream& net_file, int& num_nodes, int& num_voltage_sources, 
    int& num_components){
    std::string line;
    int line_num = 0;
    std::vector<std::string> tokens;

    std::unordered_set<std::string> voltage_source_names;
    std::unordered_set<std::string> current_source_names;
    std::unordered_set<std::string> resistor_names;
    std::unordered_set<std::string> node_names;
    while (std::getline(net_file, line)){
        // skip first line which is model description
        if (line_num == 0){
            ++line_num;
            continue;
        }
        get_tokens_from_line(net_file, line, tokens);
        assert(tokens.size() == 4);
        if (tokens[0][0] == 'V' || tokens[0][0] == 'v'){
            ++num_voltage_sources;
            ++num_components;
            voltage_source_names.insert(tokens[0]);
        }
        else if (tokens[0][0] == 'I' || tokens[0][0] == 'i'){
            ++num_components;
            current_source_names.insert(tokens[0]);
        }
        else if (tokens[0][0] == 'R' || tokens[0][0] == 'r'){
            ++num_components;
            resistor_names.insert(tokens[0]);
        }
        else{
            std::cout << "failed to find component type" << std::endl;
        }
        if (tokens[1] != "0")
            node_names.insert(tokens[1]);
        if (tokens[2] != "0")
            node_names.insert(tokens[2]);
        num_nodes = node_names.size();
        tokens.clear();
        ++line_num;
    }
	net_file.clear();
	net_file.seekg(0);
	
}

// fill incidence and conductance matrix
// fill bidrec node id,node name map
// fill bidrec comp id, comp name map
// fill comp id,comp map
// ensure col of incidence mtx is ID of comp
//todo: replace w bidirec hashmap class, use cathode_name, cathode_id local vars
void parse_netlist(std::ifstream& net_file, MatrixXd& conductance_matrix, MatrixXd& incidence_matrix){
    // component id to Component
    std::unordered_map<int, std::shared_ptr<Component>> components;

    // bidirectional node id/name hashmaps
    std::unordered_map<int, std::string> node_id_to_name;
    std::unordered_map<std::string, int> node_name_to_id;

	node_id_to_name[0] = "0";
	node_name_to_id["0"] = 0;
    // do i still need this?
    //std::unordered_set<std::string> component_names;

    std::unordered_map<int, std::string>::iterator node_id_itr;
    std::unordered_map<std::string, int>::iterator node_name_itr;

    std::vector<std::string> tokens;
    std::shared_ptr<Component> component_ptr;
	std::string line, name, type;
	int line_num = 0;
    int anode, cathode;
    double value;

    while (std::getline(net_file, line)){
		std::cout << "line no: " << line_num << '\n';
		// skip first line which is model description
        if (line_num == 0){
            ++line_num;
            continue;
        }
    	get_tokens_from_line(net_file, line, tokens);
        
		// for (auto s: tokens){
		// 	std::cout << s << std::endl;
		// }
        assert(tokens.size() == 4);
        name = tokens[0];
        value = std::stod(tokens[3]);
       
        // Node id initlialization 

        node_name_itr = node_name_to_id.find(tokens[1]);
        if (node_name_itr == node_name_to_id.end() ){
            anode = generate_node_id();
            node_name_to_id[tokens[1]] = anode;
            node_id_to_name[anode] = tokens[1];
        }
        else{
            anode = node_name_itr->second;
        }

        node_name_itr = node_name_to_id.find(tokens[2]);
        if (node_name_itr == node_name_to_id.end() ){
            cathode = generate_node_id();
            node_name_to_id[tokens[2]] = cathode;
            node_id_to_name[cathode] = tokens[2];
        }
        else{
            cathode = node_name_itr->second;
        }
        
        if (name[0] == 'V' || name[0] == 'v'){
            type = 'v';
            component_ptr = std::make_shared<Component> (
                Component(generate_component_id(), generate_voltage_source_id(), value, anode, 
                        cathode, name, type));

        }
        else if (tokens[0][0] == 'I' || tokens[0][0] == 'i'){
            type = 'i';
            component_ptr = std::make_shared<Component> (
                Component(generate_component_id(), generate_current_source_id(), value, anode, 
                        cathode, name, type));
        }
        else if (tokens[0][0] == 'R' || tokens[0][0] == 'r'){
            type = 'r';
            component_ptr = std::make_shared<Component> (
                Component(generate_component_id(), generate_resistor_id(), value, anode, 
                        cathode, name, type)
            );
        
            
            if (anode != 0){
                // node 1 diagonal 
                conductance_matrix(anode-1,anode-1) += component_ptr->get_value();
                //node 1 off-diagonal
                if (cathode != 0)
                    conductance_matrix(anode-1, cathode-1) -= component_ptr->get_value();
            }
            
            if (cathode != 0){
                // node 2 diagonal
                conductance_matrix(cathode-1,cathode-1) += component_ptr->get_value();
                // node 2 off-diagonal
                if (anode != 0)
                    conductance_matrix(cathode-1, anode-1) -= component_ptr->get_value();
            }
            
            
        }
        // todo: do this check and exit before all the others
        else{
            std::cout << "failed to find component type" << std::endl;
        }
        //Incidence
        if (anode != 0){
            incidence_matrix(anode-1,component_ptr->get_component_id()-1) = 1;
        }
        if (cathode != 0){
            incidence_matrix(cathode-1,component_ptr->get_component_id()-1) = -1;
        }
		++line_num;
        components[component_ptr->get_component_id()] = component_ptr;
        tokens.clear();
    }
	for (node_id_itr = node_id_to_name.begin();node_id_itr != node_id_to_name.end();++node_id_itr){
		std::cout << "nID: " << node_id_itr->first << ", nName:" <<node_id_itr->second << std::endl;
	}
	std::cout << "\n\n";
	for (auto itr = components.begin();itr != components.end();++itr){
		std::cout << "cID: " << itr->first << ", c: " 
			<< itr->second->get_type() << itr->second->get_type_id() << std::endl;
	}
    

}



// void calculate(int num_nodes, int num_voltage_sources, int num_components){
    
// }




int main(int argc, char const *argv[])
{
    std::ifstream net_file ("netlist.txt");
    if (!net_file.is_open()){
        std::cout << "Failed to open in file" << std::endl;
        return -1;
    }
    int num_nodes = 0;
    int num_voltage_sources = 0;
    int num_components = 0; 
    preprocess_netlist(net_file, num_nodes, num_voltage_sources, num_components);
	std::cout << "(Preprocessor): nodes: " << num_nodes << " |" << "Vs: " << num_voltage_sources << " | Components:"
		<< num_components << std::endl;

    MatrixXd incidence_matrix (num_nodes, num_components);
    MatrixXd conductance_matrix (num_voltage_sources+num_nodes,num_voltage_sources+num_nodes);
    incidence_matrix.fill(0);
    conductance_matrix.fill(0);
	parse_netlist(net_file, conductance_matrix, incidence_matrix);

	std::cout << "Conductance\n" << conductance_matrix << '\n';
	std::cout << "Incidence\n" << incidence_matrix << '\n';
	net_file.close();
    return 0;
}

// <comp type><name> <+> <-> <value>
// https://lpsa.swarthmore.edu/Systems/Electrical/mna/MNA3.html#Notational_Convention