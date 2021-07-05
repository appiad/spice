#ifndef __Component_h__
#define __Component_h__
#include <string>


class Component{
public:
    Component(int comp_id = -1, int type_id= -1, double val =-1, int anode = -1, int cathode = -1, 
        const std::string& name = "", std::string type = "")
        :  _component_id(comp_id), _type_id(type_id), _value(val), _anode(anode), 
        _cathode(cathode), _name(name), _type(type) {}

    int get_type_id() const{
        return _type_id;
    }
    int get_component_id() const {
        return _component_id;
    }

    double get_value() const{
        return _value;
    }
    const std::string& get_type(){
        return _type;
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
    int _type_id;
    double _value;
    int _anode;
    int _cathode;
    std::string _name;
    std::string _type;

};
#endif