#ifndef __Component_h__
#define __Component_h__
#include <string>


class Component{
public:
    Component(int comp_id, int type_id, double val, int anode, int cathode, 
        const std::string& name, std::string type
    );
    int get_type_id() const;
    int get_component_id() const;

    double get_value() const;
    const std::string& get_type() const;

    int get_anode() const;

    int get_cathode() const;
    const std::string& get_name() const;

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