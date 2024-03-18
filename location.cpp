/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayylaaba <ayylaaba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 17:45:30 by ayylaaba          #+#    #+#             */
/*   Updated: 2024/02/17 10:24:50 by ayylaaba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"

location::location(std::map<std::string, std::string> &c, std::vector <std::string> &v_s)
{
    cont_l = c;
    allowed_methods = v_s;
}

int     location::check_exist(std::string path, char ch)
{
    if (ch == 'd')
    {
        DIR* dir = opendir(path.c_str());
        if (!dir)
        {
            return (1);
        }
        return (0);
    }
    else
    {
        if (access(path.c_str(), R_OK))
            return 1;
        return 0;
    }
} 

void            location::print_err(std::string str)
{
    std::cout << str << std::endl;
    exit(1);
}


int     location::check_permi(std::string path)
{
    struct stat inf;
    
    if (stat(path.c_str(), &inf) != 0)
        return (1);
    return (0);
} 

void        location::check_dup()
{
    std::map<std::string, int>::iterator it = duplicate.begin();
    std::map<std::string, int>::iterator ite = duplicate.end();
    for(;it != ite; it++)
    {
        if (!it->first.compare("root"))
        {
            if (it->second != 1)
                print_err("duplicate in directive");
            it->second = 0;
        }
        if (!it->first.compare("allow_methods"))
        {
            if (it->second > 1)
                print_err("duplicate in directive");
            it->second = 0;
        }
        if (!it->first.compare("autoindex"))
        {
            if (it->second > 1)
                print_err("duplicate in directive");
            it->second = 0;
        }
        if (!it->first.compare("index"))
        {
            if (it->second > 1)
                print_err("duplicate in directive");
            it->second = 0;
        }
    } 
}

void        location::handl_loca(std::map<std::string, std::string>& m, std::vector<std::string> &methods, std::string root)
{

    std::map<std::string, std::string>::iterator it = m.begin();
    std::map<std::string, std::string>::iterator ite = m.end();

    while (it != ite)
    {
        if (!(*it).first.compare("root"))
        {
            if (check_exist((*it).second, 'd') || check_permi((*it).second))
                print_err("syntaxt_error on the folder");
        }
        if (!(*it).first.compare("allow_methods"))
        {
            for (size_t i = 1; i < methods.size(); i++)
            {
                if (methods[i].compare("GET") && methods[i].compare("DELET")
                && methods[i].compare("POST"))
                    print_err("syntaxt_error methods");
            }
        }
        if (!(*it).first.compare("autoindex"))
        {
            if ((*it).second.compare("on") && (*it).second.compare("off"))
                print_err("syntaxt_error must be 'on' or 'off'");
        }
        if (!(*it).first.compare("index")) // most be more than index and most check each one i think.
        {
            std::string path = root + "/" + (*it).second;
            if (check_exist(path, 'f') || check_permi(path))
                print_err("error on the file");
        }
        it++;
    }
}

location::location(){
    
}