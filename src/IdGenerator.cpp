#include "IdGenerator.h"

IdGenerator::IdGenerator(){
	_id = 1;
}

int IdGenerator::generate(){
	return _id++;
}