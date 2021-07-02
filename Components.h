#ifndef __Component_h__
#define __Component_h__
#include <string>


class Component{
public:
    Component(int comp_id = -1, double val =-1, int anode = -1, int cathode = -1, 
        const std::string& name = "" )
        :  _component_id(comp_id), _value(val), _anode(anode), _cathode(cathode), _name(name) {}

    virtual int get_relative_id() const = 0;
    int get_component_id() const {
        return _component_id;
    }

    double get_value() const{
        return _value;
    }

    int get_anode() const{
        return _anode;
    }

    int get_cathode() const{
        return _cathode;
    }
    const std::string& get_name() const{
        return _name;
    }

private:
    int _component_id;
    double _value;
    int _anode;
    int _cathode;
    std::string _name;
};

class VoltageSource : public Component {
public:
    VoltageSource(int comp_id = -1, int rel_id = -1, double val =-1, int anode = -1, int cathode = -1, 
        const std::string& name = "")
        : Component(comp_id, val, anode, cathode, name), _voltage_source_id(rel_id) {}

    int get_relative_id() const{
        return _voltage_source_id;
    }

private:
   int _voltage_source_id;

};

class CurrentSource : public Component {
public:
    CurrentSource(int comp_id = -1, int rel_id = -1, double val =-1, int anode = -1, int cathode = -1, 
        const std::string& name = "")
        : Component(comp_id, val, anode, cathode, name), _current_source_id(rel_id) {}

    int get_relative_id() const{
        return _current_source_id;
    }
    
private:
   int _current_source_id;

};

class Resistor : public Component {
public:
    Resistor(int comp_id = -1, int rel_id = -1, double val =-1, int anode = -1, int cathode = -1, 
        const std::string& name = "")
        : Component(comp_id, val, anode, cathode, name), _resistor_id(rel_id) {}

    int get_relative_id() const{
        return _resistor_id;
    }
private:
   int _resistor_id;

};

#endif