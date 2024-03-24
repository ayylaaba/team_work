# include "multplixing.hpp"

int multplixing::string_to_int(std::string str)
{
    std::istringstream iss(str);
    int num;
    iss >> num;
    return num;
}