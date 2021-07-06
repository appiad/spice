#ifndef __utils_h_h
#define __utils_h_h
#include <fstream>
#include <vector>
#include <string>

// todo: handle unlimited whitespace between tokens
void get_tokens_from_line(const std::string& line, std::vector<std::string>& tokens);
#endif