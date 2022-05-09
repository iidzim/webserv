#include "../includes/cgi.hpp"

cgi::cgi(){

}

cgi::cgi(s_requestInfo req, std::string fileName, std::string cgiExt): _req(req), _fileName(fileName), _cgiExtention(cgiExt){
    // setEnvironment();
}

cgi::~cgi(){}

void cgi::setEnvironment()
{
    if (_req.method == "GET"){
        int s = _req.query.length();
        std::string  size = toString(s);
        if (_req.query.size() != 0)
            setenv("CONTENT_LENGTH",size.c_str(), 1);
        setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
    }
    else if (_req.method == "POST"){
        std::map<std::string, std::string>::iterator it = _req.headers.find("content-type");
        if (it != _req.headers.end())
            setenv("CONTENT_TYPE", (it->second).c_str(), 1);
        else
            setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
        std::map<std::string, std::string>::iterator it2 = _req.headers.find("content-length");
        if (it2 != _req.headers.end())
            setenv("CONTENT_LENGTH", (it2->second).c_str(), 1);
    }
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("QUERY_STRING", _req.query.c_str() , 1);
    setenv("REQUEST_METHOD",_req.method.c_str(), 1);
    setenv("SCRIPT_FILENAME", _fileName.c_str(), 1);
    setenv("SERVER_SOFTWARE", "YDA", 1);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    setenv("REDIRECT_STATUS", "true", 1);
}

void cgi::executeFile()
{
    setEnvironment();
    pid_t id;
    int fd1[2];
    char cwd[256];
    std::string currPath(getcwd(cwd, sizeof(cwd)));
    pipe(fd1);
    int fd = open((currPath + "/var/www/html/example.html").c_str(), O_CREAT| O_RDONLY | O_WRONLY , 0644);
    id = fork();
    if (id == 0){
        dup2(_req.fd, 0);
        // write(fd1[1], _req.getRequest().query.c_str() ,_req.getRequest().query.size()); 
//    or instead of dup fd1[0] and write the body in fd1[1] dup2(body_fd, 0)
        dup2(fd, 1);
        if (_cgiExtention == ".py")
        {
            char *args[3];
            _commandPath = "/usr/bin/python";
            args[0] = (char *)_commandPath.c_str();
            args[1] = (char*)_fileName.c_str();
            args[2] = NULL;
            execve(args[0], args, NULL);
        }
        else if (_cgiExtention == ".php"){
            char *args[3];
            _commandPath = "/Users/oel-yous/homebrew/bin/php-cgi";
            args[0] = (char *)_commandPath.c_str();
            args[1] = (char*)_fileName.c_str();
            args[2] = NULL;
            execve(args[0], args, environ);
        }
    }
    wait(NULL);
    close(fd1[0]);
    close(fd1[1]);
    close (fd);
}