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
        std::string _body;
        std::fstream _myfile;
        std::fstream _outf;
        std::string  _out;
        std::string _connection;
    public:
        cgi();
        cgi(s_requestInfo req, std::string fileName, std::string cgiExt, std::string connection);
        ~cgi();
        void setEnvironment();
        void executeFile();
        std::pair<std::string, std::string> parseCgiOutput();
};  

#endif
