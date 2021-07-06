#include "Bimap.h"
void Bimap::insert(int key1, std::string key2){
	_map_t1_t2[key1] = key2;
	_map_t2_t1[key2] = key1;
}

void Bimap::insert(std::string key2, int key1){
	_map_t1_t2[key1] = key2;
	_map_t2_t1[key2] = key1;
}

std::pair<bool,std::pair<int,std::string>> Bimap::find(int key1){
	std::unordered_map<int,std::string>::iterator itr = _map_t1_t2.find(key1);
	if (itr != _map_t1_t2.end()){
		return std::make_pair(true,std::make_pair(key1, itr->second));
	}
	else{
		std::pair<int,std::string> tmp;
		return std::make_pair(false,tmp);
	}
}

std::pair<bool,std::pair<int,std::string>> Bimap::find(const std::string& key2){
	std::unordered_map<std::string,int>::iterator itr = _map_t2_t1.find(key2);
	if (itr != _map_t2_t1.end()){
		return std::make_pair(true,std::make_pair(itr->second, key2));
	}
	else{
		std::pair<int,std::string> tmp;
		return std::make_pair(false,tmp);
	}
}