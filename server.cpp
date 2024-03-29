/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayylaaba <ayylaaba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/05 14:46:12 by ayylaaba          #+#    #+#             */
/*   Updated: 2024/02/15 16:28:31 by ayylaaba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int i = 0;

server::server()
{
    duplicate["index"]          = 0;
    duplicate["root"]           = 0;
    duplicate["allow_methods"]  = 0;
    duplicate["autoindex"]      = 0;
}

server::server(std::map<std::string, std::string> &cont_s, std::vector<location*> &l_)
{
    cont = cont_s;
    l = l_;  
}

std::string     server::strtrim(std::string &str)
{
    size_t  start;
    size_t  end;
    
    start = str.find_first_not_of(" ");
    if (start != std::string::npos)
        str = str.substr(start);
    end = str.find_last_not_of(" ");
    if (end != std::string::npos)
        str = str.substr(0, end + 1);
    return (str);
}

void            server::print_err(std::string str)
{
    std::cout << str << std::endl;
    exit(1);
}

std::vector<std::string>    server::isolate_str(std::string s, char deli)
{
    std::vector<std::string> vec;
    std::string              words;
    
    strtrim(s);
    std::istringstream       stream(s);
    while (std::getline(stream, words, deli))
    {
        if (!words.empty())
            vec.push_back(words);
    }
    return (vec);
}


void        server::mange_file(const char* file)
{
    // lfkra = had lfile rdi t9rah fkola server radi parsih plus t stori cola sserver f string ao diro f vector
    std::ifstream   rd_content(file);
    s_token = 0;
    obj.l_token = 0;

    while (std::getline(rd_content, str)) // loop to get lines
    {
        str = strtrim(str);
        if (str.empty())
            return ;
        if (str.compare("server"))
            print_err("syntaxt_error server");
        std::getline(rd_content, str); // store all servers
        str = strtrim(str);
        if (!str.compare("{"))
        {
            s_token++;
            parse_both(rd_content,str);
            if ((!str.compare("}") && s_token == 1 ))
            {
                s.push_back(new server(cont, l));
                cont.clear();
                l.clear();
                s_token = 0;
            }
        }
    }
}


int        server::parse_loca(std::ifstream& rd_cont, std::string &str_)
{
    std::vector<std::string>    v_s;
    while (std::getline(rd_cont, str)) // loop to iterate inside location
    {
        check = "off";
        str_ = strtrim(str_);
        l_vec = isolate_str(str_, ' ');
        std::cout << "str --> " << str << "<--\n";
        if (!l_vec[0].compare("root") || !l_vec[0].compare("index") 
            || !l_vec[0].compare("limit_except") || !l_vec[0].compare("allow_methods") 
            || !l_vec[0].compare("autoindex") || !l_vec[0].compare("upload") 
            ||  !l_vec[0].compare("upload_path"))
            {
                check_size(l_vec, 'l'); 
                if (!l_vec[0].compare("allow_methods"))
                    v_s = l_vec;
                if (!l_vec[0].compare("root"))
                    _root = l_vec[1];
                stor_values(l_vec, 'l');
            }
        else if (!l_vec[0].compare("}"))
        {
            obj.l_token++;
            if (obj.l_token == 2)
            {
                handl_loca(cont_l, v_s, _root);
                l.push_back(new location(cont_l, v_s));
                check_dup();
                cont_l.clear();
                break ;
            }
        }
        else
        {
            print_err("syntaxt_error unkown directive");
        }
    }
    if (obj.l_token == 2)
    {
        obj.l_token = 0;
        std::getline(rd_cont, str_l);
        str_l = strtrim(str_l);
        str_l_vec = isolate_str(str_l, ' ');
        if(!str_l_vec[0].compare("location"))
        {
            check_size(str_l_vec, 'l');
            loca_path[str_l_vec[0]] = str_l_vec[1].substr(0, str_l_vec[1].size());
            cont_l[str_l_vec[0]]    = str_l_vec[1].substr(0, str_l_vec[1].size()); // store location with its path
            return 1 ;
        }
        else
        {
            check = "on";
            return 0;
        }
    }
    return 1;
    
}

int     server::is_num(std::string s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if (!std::isdigit(s[i]))
            return 1;
    }
    return 0;
}

int     server::valid_range(std::string s)
{
    int num;
    num = std::atoi(s.c_str());

    if (num < 1 || num > 65535)
        return 1;
    return 0;
}

int     server::check_exist(std::string path, char ch)
{
    if (ch == 'd')
    {
        DIR* dir = opendir(path.c_str());
        if (!dir)
        {
            closedir(dir);
            return (1);
        }
        closedir(dir);
        return (0);
    }
    else
    {
        std::cout << "path == " << path << std::endl;
        std::FILE* file = std::fopen(path.c_str(), "r"); // most change the function
        if (file && path[path.length() - 1] != '/')
        {
            std::fclose(file);
            return 0;
        }
        // std::fclose(file); // segv ??
        return 1;
    }
} 

int     server::check_permi(std::string path)
{
    struct stat inf;
    
    if (stat(path.c_str(), &inf) != 0)
        return (1);
    return (0);
} 

int    server::parse_both(std::ifstream& rd_cont, std::string &str_)
{
    while (std::getline(rd_cont, str_)) // loop to iterate inside server
    {
        str_ = strtrim(str_);
        s_vec = isolate_str(str_, ' ');
        if (s_token == 1 && !s_vec[0].compare("}"))
            break;
        else if (s_vec[0].compare("listen")&& s_vec[0].compare("error_page") 
        && s_vec[0].compare("client_max_body_size") && s_vec[0].compare("host") 
        && s_vec[0].compare("server_name") && s_vec[0].compare("index")
        &&s_vec[0].compare("root"))
        {
            if (s_vec[0].compare("location") && s_vec[0].compare("{"))
                print_err("syntaxt_error");    
            else if (!s_vec[0].compare("location") || !s_vec[0].compare("{"))
            {
                if (s_vec[0].compare("{")) // you mean that is not { then it is a location
                {
                    check_size(s_vec, 'l'); // check first loca's path
                    std::cout << "loca ----------- " << s_vec[0] << " first root ------- " << s_vec[1] << "\n"; 
                    loca_path[s_vec[0]] = s_vec[1];
                    cont_l[s_vec[0]] = s_vec[1];
                    std::getline(rd_cont, str_);
                    str = strtrim(str);
                }
                if (!str_.compare("{"))
                {
                    obj.l_token++;
                    if (parse_loca(rd_cont, str_) == 1)
                        continue;
                    else
                        return 1;
                }
            }
        }
        else 
        {
            check_size(s_vec, 's');
            if (!s_vec[0].compare("error_page"))
            {
                if (!check_exist(s_vec[2], 'f') && !check_stat(s_vec[1])) // check also stat lik 404 301 ...
                    err_page[s_vec[1]] = s_vec[2];
                else
                    print_err("syntaxt_error on the error_page");
            }
            handl_serv(s_vec); 
            stor_values(s_vec, 's');
        }
    }
    return (0);
}

int        server::check_stat(std::string &stat_error)
{
    if (stat_error.compare("403") && stat_error.compare("404") && 
    stat_error.compare("301")) // you should add more i think ...
        return 1;
    return 0;
}

void        server::check_size(std::vector<std::string> &s, char c)
{
    if (c == 's')
    {
        if (!s_vec[0].compare("root"))
            s_root = s_vec[1];    
        if (!s[0].compare("listen") || !s[0].compare("root")
        || !s[0].compare("client_max_body_size") || !s[0].compare("host") 
        || !s[0].compare("server_name"))
        {
            if (s.size() != 2)
                print_err("syntaxt_error " + s[0]);
        }
        else if (!s[0].compare("error_page") ){
            if (s.size() != 3)
                print_err("syntaxt_error " + s[0]);
        }
    }
    else
    {
        if ((!s[0].compare("root") || !s[0].compare("index") 
        || !s[0].compare("limit_except") || !s[0].compare("autoindex") 
        || !s[0].compare("upload"))){
            if (s.size() != 2)
                print_err("syntaxt_error on " + s[0]);
        }
        if (!s[0].compare("allow_methods") ){
            if (s.size() < 2 || s.size() > 4)
                print_err("syntaxt_error on allow_methods");
        }
        if (!s[0].compare("location"))
        {
            if (s.size() < 2 || s.size() > 2)
                print_err("syntaxt_error on location");
        } 
    }
}


void      server::handl_serv(std::vector<std::string> s)
{
    if (!s[0].compare("listen"))
    {   
        if (is_num(s[1]) || valid_range(s[1]))
            print_err("syntaxt_error on port");
    }
    else if (!s[0].compare("root"))
    {
        if (check_exist(s[1], 'd') || check_permi(s[1]))
            print_err("syntaxt_error on the path");
    }
    else if (!s[0].compare("error_page"))
    {
        std::cout << "s[1] ===> " << s[1] << "<====\n";
        if (check_exist(s[2], 'f') || check_permi(s[2]))
            print_err("syntaxt_error on the file");
    }
    else if (!s[0].compare("index"))
    {
        for (size_t i = 1; i < s.size(); i++)
        {
            indexs.push_back(s[i]);
            std::cout << "----->>>> " << s[i] << "<<<<-------\n";
            s[1] = s_root + "/" + s[i];
            if (check_exist(s[1], 'f') || check_permi(s[1]))
                print_err("syntaxt_error on index");
        }
    }
    else if (!s[0].compare("host"))
    {
        if (check_ip(s[1]) )
            print_err("syntaxt_error on ip");
    }
}

int      server::check_ip_nbr(std::string nbr)
{
    int number = std::atoi(nbr.c_str());
    if (number < 0 || number > 255)
        return 1;
    return 0;
}

int      server::check_ip(std::string ip)
{
    int count = 0;
    for (size_t i = 0; i < ip.size(); i++)
    {
        if (ip[i] == '.')
            count++;
    }
    if (count != 3)
        return 1;
    std::vector<std::string> ip_nmbr = isolate_str(ip, '.');
    for (size_t i = 0; i < ip_nmbr.size(); i++)
    {
        if (is_num(ip_nmbr[i]) || check_ip_nbr(ip_nmbr[i]))
            return 1;
    }
    return 0;
}

void        server::stor_values(std::vector<std::string> s, char ch)
{
    if (ch == 's')
    {
        if (s[0].compare("error_page"))
        {
            std::map<std::string, std::string>::iterator it = cont.find(s[0]);
            if (it != cont.end())
                print_err("Duplicat on directive's server");
        }
        if (s[0].compare("error_page"))
            handl_serv(s);
        if (!s[0].compare("listen"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("root"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("error_page"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("host"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("server_name"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("client_max_body_size"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("index"))
            cont[s[0]] = s[1].substr(0, s[1].size());
    }
    else
    {
        // std::map<std::string, std::string>::iterator it = cont.find(s[0]);
        // if (it != cont.end())
        //     print_err("Duplicat on directive's location");
        duplicate.find(s[0])->second++;
        if (!s[0].compare("index"))            
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("root"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("limit_except"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("allow_methods"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("autoindex"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("upload"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
    }
}
