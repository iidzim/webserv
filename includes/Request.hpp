#ifndef REQUEST_HPP
# define REQUEST_HPP
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <fstream>
#include <string>
#include "configurationReader.hpp"
#define CRLF "\r\n\r\n"
#include <cctype>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <ctime>
#include "mimeTypes.hpp"
      // std::cout, std::endl
#include <thread>         // std::this_thread::sleep_for
#include <chrono>
typedef struct t_requestInfo
{
    int         fd;
    std::string method;
    std::string URI;
    std::string query;
    std::string versionHTTP;
    //std::string body;//possibility d overflow
    //! add a file to store a file
    std::string bodyFile;
    int     statusCode;
    std::map<std::string, std::string> headers;
}               s_requestInfo;

class request
{
    private:
        std::time_t    _start;
        bool            _begin;
        int             _port;
        std::string     _host;
        std::string     _data;
        s_requestInfo   _rqst;
        bool            _headersComplete;
        bool            _bodyComplete;
        bool            _isChunked;
        bool            _isBodyExcpected;
        size_t          _contentLength;
        std::string     _contentType;
        std::string     _root;
        std::string     _uploadpath;


        std::fstream     tmpFile;

        size_t          _originContentLength;
        std::vector<serverInfo> servers;
        size_t          _size;
        struct pollfd   _fds[1];
    
        void requestLine(std::istringstream & istr); // Method URI VERSION
        void getHeaders(std::istringstream & str); //all the available headers in MJS
        void clearRequest(); //! not sure if necessary
        bool expectedHeader(const std::string &str);
        void putBufferIntoFile();
        bool isBodyExpected(); // based on the parsed data
        bool isFieldNameValid(const std::string &str);
        //bool endBodyisFound(std::string lastLine);
        void isBodyValid();

        //! Not implemented yet 
        bool isHexadecimalFormat(std::string &number); // use xdigit
        std::string     getMimeType();
        std::string     getContentType();

        
    public:
        request();
        request(std::vector<serverInfo> &servers);
        void BlockMatching(std::vector<serverInfo> server_conf);
        size_t convertHexToDecimal(std::string value);
        void deleteOptionalWithespaces(std::string &fieldValue); 
        //request(serverInfo server);
        request(const request& obj);
        request& operator=(const request& obj);

        request(char *buffer, int rBytes);
        void setBuffer(char *buff);
        ~request();

        void parse(char *buffer, size_t n);
        s_requestInfo   getRequest();
        bool            isComplete();
        void            forceStopParsing();
        int             getPort();
        std::string     getHost();
        void resetTime();
        std::time_t getTime();
        void closefds();
        bool getComplete();

       //! for debugging
       void print_request();
       class  RequestNotValid: public std::exception 
        {
            public:
                const char* what() const throw();
        };
};

#endif


 //TODO for today
 //delete optinal withespaces from field value
 //fix body errors