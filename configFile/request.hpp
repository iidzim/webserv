#ifndef REQUEST_HPP
# define REQUEST_HPP
#include <iostream>
#include <vector>
#include <map>
#include "utility.hpp"

typedef struct t_requestInfo
{
    std::string method;
    std::string URI; // queries
    std::string versionHTTP;
    std::string body;//possibility d overflow
    std::map<std::string, std::string> headers;
}               s_requestInfo;

class request
{
    private:
        std::string     _buffer;
        bool            _isComplete;
        s_requestInfo   _rqst;

        void requestLine(std::string line); // Method URI VRSION
        void getHeaders(); //all the available headers in MJS
        void clearRequest(); //! not sure if necessary
        bool expectedHeader(const std::string &str);
        void putBufferIntoFile();
    public:
        request(char *buffer);
        ~request();
        s_requestInfo tokenizeRequest();
        bool getRequestStatus();//true if rqst is complete
};


//TODO

 //! put all of this into a file => function void putBufferIntoFile();

 //! finished when there is a \r\n\r\n at the end
 //! check the body depending on the method
 //! parse status line
 //! get Headers => are they excepted or if anyone is missing
 //! return status code(I don't know yet how)