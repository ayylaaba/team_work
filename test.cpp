       #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>
       #include <iostream>
       #include <string.h>

int     check_exist(const std::string path) 
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) 
    {
        if (S_ISREG(fileStat.st_mode)) 
            return 1; // Path exists and is a regular file
        if (S_ISDIR(fileStat.st_mode)) 
            return 2; // Path exists and is a directory
    }
    return 0; // Path does not exist
}

int main( ) {
    std::cout << check_exist("/nfs/homes/ayylaaba/Desktop/webserve/website/folder//data") << std::endl;
}