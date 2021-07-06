#include "Components.h"

Component::Component(int comp_id = -1, int type_id= -1, double val =-1, int anode = -1, int cathode = -1, 
        const std::string& name = "", std::string type = "")
        :  _component_id(comp_id), _type_id(type_id), _value(val), _anode(anode), 
        _cathode(cathode), _name(name), _type(type) {}

int Component::get_type_id() const{
    return _type_id;
}

int Component::get_component_id() const {
    return _component_id;
}

double Component::get_value() const{
    return _value;
}

const std::string& Component::get_type() const{
    return _type;
}

int Component::get_anode() const{
    return _anode;
}

int Component::get_cathode() const{
    return _cathode;
}

const std::string& Component::get_name() const{
    return _name;
}