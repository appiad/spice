#include <fstream>
#include <vector>
#include <string>

// todo: handle unlimited whitespace between tokens
void get_tokens_from_line(const std::string& line, std::vector<std::string>& tokens){
    size_t last = 0; 
    size_t next = 0; 
    while ((next = line.find(' ', last)) != std::string::npos){
        tokens.push_back(line.substr(last,next-last));
        last = next + 1;
    }
    tokens.push_back(line.substr(last));
}