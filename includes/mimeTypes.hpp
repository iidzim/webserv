#ifndef MIMESTYPES_HPP
#define MIMESTYPES_HPP

#include <iostream>
#include <string>
#include <map>
#include <utility>

class mimeTypes{
    private:
        std::map<std::string, std::string>  _types;

    public:
    mimeTypes();
    ~mimeTypes();
        std::map<std::string, std::string> getTypes();
        void setTypes();
};


#endif