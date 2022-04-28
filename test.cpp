#include <iostream>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>

int main(void){

    // std::fstream file;
    // char buff[1024], buff2[1024];
    // file.open("test.cpp", std::ios::in | std::ios::binary);
    // if (!file.is_open())
    //     std::cout << "Failed to open file - no such file" << std::endl;
    // file.read(buff, sizeof(buff));
    // file.seekg(0, std::ios::beg);
    // file.read(buff2, sizeof(buff2));
    // if (file.eof())
    //     std::cout << "End Of File" << std::endl;
    // std::cout << buff2 << std::endl;

    char buff[1024], buff2[1024];
    int fd = open("test.cpp", O_RDONLY);
    int r = read(fd, buff, sizeof(buff));
    std::cout << r << std::endl;
    std::cout << buff << "\n****************" << std::endl;
    lseek(fd, 0, SEEK_SET);
    int f = read(fd, buff2, sizeof(buff2));
    if (f <= 0)
        std::cout << "End Of File" << std::endl;
    std::cout << buff2 << std::endl;
    lseek(fd, 0, SEEK_SET);
    memset(buff, 0, sizeof(buff));
    r = read(fd, buff, sizeof(buff));
    std::cout << "\n****************" << buff <<  std::endl;
    
    
    close(fd);
    return 0;
}