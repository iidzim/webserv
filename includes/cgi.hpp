#ifndef CGI_HPP
#define CGI_HPP

#include "webserv.hpp"

// extern char **environ;

class cgi{
    private:
        std::string _commandPath;
        s_requestInfo _req;
        std::string _fileName;
        std::string _cgiExtention;
    public:
        cgi();
        cgi(s_requestInfo req, std::string fileName, std::string cgiExt);
        ~cgi();
        void setEnvironment();
        void executeFile();
};  

#endif
