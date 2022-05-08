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
        int             _port;
        std::string     _data;
        s_requestInfo   _rqst;
        bool            _headersComplete;
        bool            _bodyComplete;
      //  std::fstream    my_file;
        bool            _isChunked;
        bool            _isBodyExcpected;
        size_t          _contentLength;
       // serverInfo      _server;

        std::fstream     tmpFile;

        size_t          _originContentLength;

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
        
    public:
        request();
        size_t convertHexToDecimal(std::string value);
        void deleteOptionalWithespaces(std::string &fieldValue); 
        //request(serverInfo server);
        request(const request& obj);
        request& operator=(const request& obj);

        request(char *buffer, int rBytes);
        void setBuffer(char *buff);
        ~request();

        void parse(char *buffer, size_t n);
        s_requestInfo getRequest();
        bool isComplete();
        void forceStopParsing();
        int getPort();

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