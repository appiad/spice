#ifndef __Bimap_h__
#define __Bimap_h__
#include <unordered_map>
#include <utility>
#include <string>

class Bimap{
public:
	void insert(int key1, std::string key2);
	void insert(std::string key2, int key1);
	std::pair<bool,std::pair<int,std::string>> find(int key1);
	std::pair<bool,std::pair<int,std::string>> find(const std::string& key2);
private:
	std::unordered_map<int,std::string> _map_t1_t2;
	std::unordered_map<std::string,int> _map_t2_t1;	
};

#endif