
#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP


#include "webserv.hpp"

class autoIndex{
    private:
        std::string _body;
        std::string _bodyName;
        bool _isError;
        int _errorCode;
        std::string _fileSize;
    public:
        autoIndex();
        ~autoIndex();
        std::string getBodyName();
        void setAutoIndexBody(DIR *folder, std::string path, std::string root, std::string location, std::string CurrPath);
        std::string lastTimeModified(std::string fileName);
        bool isError();
        int getErrorCode();

};

#endif