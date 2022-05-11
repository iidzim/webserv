#include "../includes/cgi.hpp"

cgi::cgi(){

}

cgi::cgi(s_requestInfo req, std::string fileName, std::string cgiExt, std::string connection): _req(req), _fileName(fileName), _cgiExtention(cgiExt), _connection(connection){
    _body = "";
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
        if (it != _req.headers.end()){
            setenv("CONTENT_TYPE", (it->second).c_str(), 1);
        }
        else{
            setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
        }
        std::map<std::string, std::string>::iterator it2 = _req.headers.find("content-length");
        if (it2 != _req.headers.end()){
            // std::cout << "content length == " << it2->second << std::endl;
            setenv("CONTENT_LENGTH", (it2->second).c_str(), 1);
        }
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
    char cwd[256];
    std::string currPath(getcwd(cwd, sizeof(cwd)));
    _body = currPath + "/var/www/html/example.html";
    int fd1 = open(_req.bodyFile.c_str(), O_RDONLY);
    int fd = open(_body.c_str(), O_CREAT| O_RDONLY | O_WRONLY , 0644);
    // std::time_t start = std::time(NULL);
    id = fork();
    if (id == 0){
        dup2(fd1, 0);
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
    // while (std::time(NULL) - start < 60)
        wait(NULL);
    close(fd1);
    close (fd);

}

std::pair<std::string, std::string> cgi::parseCgiOutput()
{
    char cwd[256];
    std::string currPath(getcwd(cwd, sizeof(cwd)));
    _out = currPath+"/var/www/html/form.html";
    _myfile.open(_body, std::ios::in);
    _outf.open(_out, std::ios::out);
    std::string result;

    
    std::string line;

    while (std::getline(_myfile, line))
    {
        if (line == "\r")
        {
            result+=line+"\n";
            while (std::getline(_myfile, line))
                _outf<<line+"\n";
            break;
            
        }
        result+=line+'\n';
    }

    _myfile.close();
    _outf.close();
    std::string r;
    size_t pos = result.find("\r\n");
    if (pos != std::string::npos){
        r = result.substr(0,pos);
        r+= "\r\nContent-length: " ;
        r+= toString(fileSize(_out));
        r+= _connection;
        r+= result.substr(pos);

    }
    else 
        r = result;
    std::remove(_body.c_str());
    return std::make_pair(r, _out);

}