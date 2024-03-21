#include "request.hpp"

// not_found fun 

void            request::parse_req(std::string   rq, server &server)
{
    parse_header(rq, server);
    std::string line;
    size_t last = rq.find("\r\n");
    std::cout << "request line 'first line' " << rq.substr(0, last) << "\n";
    std::vector<std::string> vec = server.isolate_str(rq.substr(0, last) , ' ');
    std::string     extention_type;

    method        = vec[0];
    path          = vec[1];
    http_version  = vec[2];


    std::cout << "method = " << method <<  " path " << path << ", http_v = " << http_version << std::endl;
    
    uri = get_full_uri(path, server); //
   
    if (!uri.compare("error"))
    {
        std::cout << " ^-^ $ '_' $ ^-^ ERROR  == " << uri << "\n";
    }
    std::cout << "Full Path = " << uri << std::endl;
}

std::string     request::get_full_uri(std::string path, server &server)
{
    std::string loca_fldr;
    std::string rest_fldr;
    std::string full_path;
    size_t      pos;
    
    check = false;
    path = path.substr(1);
    pos = path.find("/");
    loca_fldr = path.substr(0, pos); // folder that isolat from request line
    rest_fldr = path.substr(pos + 1); // chyata lib9at mn wrat folder dyal request line.

    std::cout << "first_fldr = " << loca_fldr << "\n";
    std::cout << "rest of folder = " << rest_fldr << "\n";

    for (size_t i = 0; i < server.s.size(); i++)
    {
        for (size_t j = 0; j < server.s[i]->l.size(); j++)
        {
            std::map<std::string, std::string>::iterator      ite = server.s[i]->l[j]->cont_l.end();
            for (std::map<std::string, std::string>::iterator itb = server.s[i]->l[j]->cont_l.begin(); itb != ite; itb++)
            {
                if (!(*itb).first.compare("location") &&  !(*itb).second.substr(1).compare(loca_fldr))
                {
                    std::cout << "check location == " << (*itb).first << " check Second == " << (*itb).second << "\n";
                    auto_index_stat = check_autoindex(server.s[i]->l[j]->cont_l);
                    std::map<std::string, std::string>::iterator it_e = server.s[i]->l[j]->cont_l.end();
                    for (std::map<std::string, std::string>::iterator it_b = server.s[i]->l[j]->cont_l.begin(); it_b != it_e; it_b++)
                    {
                        if (!(*it_b).first.compare("root"))
                        {
                            if (!rest_fldr.empty()) // rest 3amr
                            {
                                full_path = (*it_b).second + "/" + rest_fldr; 
                                check = 1;
                                break;
                            }
                            else
                            {
                                std::map<std::string, std::string>::iterator indx = server.s[i]->l[j]->cont_l.find("index");
                                if (indx != server.s[i]->l[j]->cont_l.end())
                                {
                                    full_path = (*it_b).second + "/" + (*indx).second; 
                                    check = 1;
                                    break;
                                }
                                else
                                    full_path = (*it_b).second + "/"; 
                            }
                        }
                    }
                    if (check)
                        break;
                }
                else if (!loca_fldr.compare("favicon.ico"))
                {
                    full_path = "error";
                    check = 1;
                    break;
                }
            }
            if (check)
                break;
        }
    }
    return (full_path);
}

bool            request::check_autoindex(std::map<std::string, std::string> loca_map)
{
    std::map<std::string, std::string>::iterator it_e = loca_map.end();
    for (std::map<std::string, std::string>::iterator it_b = loca_map.begin(); it_b != it_e; it_b++)
    {
        if (!(*it_b).first.compare("autoindex"))
        {
            if (!(*it_b).second.compare("on"))
                auto_index_stat = true;
            break;
        }
    }
    return (auto_index_stat);
}

std::string     request::get_delet_resp(std::string path, int stat)
{
    std::stringstream   StringSize;
    std::string         extention_type;
    std::string         response = "HTTP/1.1 200 OK\r\n";
    std::streampos      fileSize = get_fileLenth(path);

    StringSize << fileSize;
    std::cout << "-------------- sizeOFtheFile that We Want To Delete == " << StringSize.str() << "\n";
    if (stat == 1)
    {
        extention_type = get_exten_type(uri); 
        response += "Content-Type:" + extention_type + "\r\n" +  "Content-Length:" + StringSize.str() + "\n\r\n\r";
        response += "<h1> IS DELETE SUCCESSFUL </h1>";
    }
    else if (stat == 4)
    {
        extention_type = get_exten_type(uri); 
        response = "HTTP/1.1 404 Not Found\r\n";
        response += "Content-Type:" + extention_type + "\r\n" +  "Content-Length:" +  StringSize.str() + "\n\r\n\r";
        response += "<h1>THE FILE NOT FOUND</h1>";
    }
    return (response);
}

std::string     request::delet_method(std::string path, server &server)
{
    std::string line;
    std::string path_;

    std::cout << "Path == " << path << "\n";

    struct stat path_stat;
    int stat_ = stat(path.c_str(), &path_stat);
    std::string res;

    if(stat_)
    {
        res = get_delet_resp(path, 4);
    }
    if (!S_ISDIR(path_stat.st_mode))
    {
        if (!remove(path.c_str()))
        {
            res = get_delet_resp(path, 1);
            std::cout << "haa9 Mcha ----> " << "Path =" << path << "\n";
            return (res);
        }
        else
        {
            res = get_delet_resp(path, 4);
            std::cout << "haa9 Mcha ----> " << "Path =" << path << "\n";
            std::cout << "\nMabghach Ya Mchi \n";
            return (res);
        }
    }
    else
    {
        DIR *direct = opendir(path.c_str());
        if (!direct)
        {
            perror("Folder does'nt exist");
            exit (1);
        }
        else
        {
            std::cout << "Here \n";
            struct dirent* entry = NULL;
            while ((entry = readdir(direct)))
            {
                std::cout << "Here 2 " << entry->d_name << "\n";
                if (std::string(entry->d_name).compare("..") && std::string(entry->d_name).compare("."))
                {
                    std::cout << "check  < -- > : " << path << "\n"; 
                    if (path[path.length() - 1] != '/')
                    {
                        std::cout << "entre 1"<< "\n";
                        line = path + "/" + entry->d_name;
                        delet_method(line, server);
                    }
                    else
                    {
                        std::cout << "entre 2"<< "\n";
                        line = path + entry->d_name;
                        delet_method(line, server);
                    }
                }
            }
        }
        closedir(direct);
        if (!remove(path.c_str()))
        {
            std::cout << " path "  << path <<" Is Deleted Successfull !\n";
            return ( get_delet_resp(path,1));
        }
        else
        {
            std::cout <<  " path " << path <<"Is Not Deleted Successfull !\n";
            return ( get_delet_resp(path,4));
        }
    }
    return (" -- ");
}

void        request::fill_extentions()
{   
    // i think you need more than that 
    extentions["html"] = "text/html"; 
    extentions["txt"]  = "text/plain"; 
    extentions["jpg"] = "image/jpg"; 
    extentions["jpeg"] = "image/jpeg";
    extentions["png"] = "image/png";
    extentions["mp3"] = "audio/mpeg";
    extentions["mp4"] = "video/mp4";
    extentions["webm"] = "video/webm";
    extentions["pdf"] = "application/pdf";
    extentions["zip"] = "application/zip";
    extentions["woff"] = "application/font-woff";
}

std::string     request::get_exten_type(std::string path)
{
    std::string exten;
    size_t      pos = path.find_last_of(".");

    exten = path.substr(pos + 1);
    std::map<std::string, std::string>::iterator b = extentions.find(exten);
    if (b != extentions.end())
    {
        return ((*b).second);
    }
    return ("");
}

std::streampos  request::get_fileLenth(std::string path)
{
    std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return -1; // Return -1 to indicate error
    }
    file.seekg(0, std::ios::end);
    std::streampos file_Size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::cout << "===== Be Attention Here === \n" ;
    std::cout << "Size === " << to_string(file_Size) << "\n";
    file.close();
    return file_Size;
}

request::request(/* args */){
    fill_extentions();
}

request::~request(){
}
