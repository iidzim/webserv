// #include "cgi.hpp"

// cgi::cgi(){

// }

// cgi::cgi(Request req, std::string fileName): _req(req), _fileName(fileName){
//     setEnvironment();
// }

// cgi::~cgi(){}
// void cgi::void setEnvironment()
// {
//     if (_req.getRequest().query.size() != 0)
//         setenv("CONTENT_LENGTH", toString(_req.getRequest().query.size()).c_str(), 1);
//     setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
//     setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
//     setenv("QUERY_STRING", _req.getRequest().query.c_str() , 1);
//     setenv("REQUEST_METHOD",_req.getRequest().method.c_str(), 1);
//     setenv("SCRIPT_FILENAME", _fileName.c_str(), 1);
//     setenv("SERVER_SOFTWARE", "YDA", 1);
//     setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
//     setenv("REDIRECT_STATUS", "true", 1);
// }

// void cgi::executeFile()
// {
//     pid_t id;
//     int fd1[2];
//     pipe(fd1);
//     int fd = open("/tmp/test", O_CREAT| O_RDONLY | O_WRONLY , 0644);
//     id = fork();
//     if (id == 0){
//         dup2(fd1[0], 0);
//         write(fd1[1], _req.getRequest().query.c_str() ,_req.getRequest().query.size()); 
//    or instead of dup fd1[0] and write the body in fd1[1] dup2(body_fd, 0)
//         dup2(fd, 1);
//         if (_fileName.find(".py") == fileName.size() - 3)
//         {
//             char *args[3];
//             _commandPath = "/usr/bin/python";
//             args[0] = (char *)_commandPath.c_str();
//             args[1] = (char*)_fileName.c_str();
//             args[2] = NULL;
//             execve(args[0], args, NULL);
//         }
//         else if (_fileName.find(".php") == fileName.size() - 4){
//             char *args[3];
//             _commandPath = "/Users/oel-yous/homebrew/bin/php-cgi";
//             args[0] = (char *)_commandPath.c_str();
//             args[1] = (char*)_filename.c_str();
//             args[2] = NULL;
//             execve(args[0], args, environ);
//         }
//     }
//     wait(NULL);
//     close(fd1[0]);
//     close(fd1[1]);
//     close (fd);
// }