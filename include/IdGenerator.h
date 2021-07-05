#ifndef __Id_Generator_h__
#define __Id_Generator_h__
class IdGenerator
{
public:
	IdGenerator();
	int generate();
private:
	int _id;
	
};

IdGenerator::IdGenerator(){
	_id = 1;
}

int IdGenerator::generate(){
	return _id++;
}
#endif