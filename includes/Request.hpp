#ifndef REQUEST_HPP
# define REQUEST_HPP
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <fstream>
#include <string>
#define CRLF "\r\n\r\n"
typedef struct t_requestInfo
{
    std::string method;
    std::string URI; // queries
    std::string versionHTTP;
    //std::string body;//possibility d overflow
    //! add a file to store a file
    std::string bodyFile ;
    std::map<std::string, std::string> headers;
}               s_requestInfo;

class request
{
    private:
        //char            *_buffer;
      //  int             _rBytes;
        std::string     _data;
        s_requestInfo   _rqst;
        bool            _headersComplete;
        bool            _bodyComplete;
        std::fstream    my_file;
        bool            _isChunked;
        bool            _parsingComplete;

        void requestLine(std::istringstream & istr); // Method URI VERSION
        void getHeaders(std::istringstream & str); //all the available headers in MJS
        void clearRequest(); //! not sure if necessary
        bool expectedHeader(const std::string &str);
        void putBufferIntoFile();
        bool isBodyExpected(); // based on the parsed data
        bool isFieldNameValid(const std::string &str);
        bool endBodyisFound(std::string lastLine);
    public:
        request();
        request(const request& obj);
        request& operator=(const request& obj);

        request(char *buffer, int rBytes);
        void setBuffer(char *buff);
        ~request();
       // s_requestInfo tokenizeRequest();
        void parse(char *buffer, size_t n);
       // bool getRequestStatus();//true if rqst is complete
       s_requestInfo getRequest();
       bool isComplete();

       void print_request();
};

#endif

//TODO

 //! put all of this into a file => function void putBufferIntoFile();

 //! finished when there is a \r\n\r\n at the end
 //! check the body depending on the method
 //! parse status line
 //! get Headers => are they excepted or if anyone is missing
 //! return status code(I don't know yet how)