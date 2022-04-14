#include "request.hpp"

request::request(char * buffer):_buffer(buffer), _isComplete(true){}

request::~request(){}

bool request::getRequestStatus(){return is_complete;}

void request::requestLine(std::string line)
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    if (line.empty() || line[line.size() - 1] != '\r')
    {
        std::cout<<"Request Not Valid !"<<std::endl; //throw an exception instead
        exit(EXIT_FAILURE);
    }
    while ((pos=line.find(" ")) != std::string::npos)
    {
        words.push_back(line.substr(0, pos));
        line.erase(0, pos+1);
    }
    if (tokens.size() != 3)//should only be one space between them
    {
        std::cout<<"Request Not Valid\nError in request Line!"<<std::endl;
        exit(EXIT_FAILURE);
    }
    if (words[0] != "GET" && words[0] != "POST" && words[0] != "DELETE")
    {
        std::cout<<"Request Not Valid\nError this method is not handled !"<<std::endl;
        exit(EXIT_FAILURE);
    }
    _rqst.method = words[0];
    _rqst.URI = words[1];
    //http version
    if (words[2] != "HTTP/1.1\r")
    {
        std::cout<<"Request Not Valid\nError http version incorrect !"<<std::endl;
        exit(EXIT_FAILURE);
    }
    _rqst.versionHTTP = words[2];
}

void        request::clearRequest()
{
    _rqst.method = "";
    _rqst.URI = "";
    _rqst.versionHTTP = "";
    _rqst.headers.clear(); //clear the map => size = 0 | elements are destroyed
}

s_requestInfo request::tokenizeRequest()
{
    std::string     line;

    if (_isComplete)
    {
        clearRequest();
        getline(_buffer, line);
        requestLine(line);
    }
    //in case is complete or not there will be only the headers
    while (std::getline(_buffer, line))
    {

        std::cout<<"This should only be the headers or body"<<std::endl;
        //switch case would be great
        //read the parsed data to determine if the body is expected !
    }
    //when finished check if the last line is blank if no => _isComplete = false
    return _rqst;
}