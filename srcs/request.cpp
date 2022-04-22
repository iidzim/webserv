#include "../includes/Request.hpp"

// ! recipients SHOULD ignore unrecognized header fields.
// ! rfc 7230 section 3.2.1

//! The order in which header fields with differing field names are received is not significant
//! rfc 7230 section 3.2.2

//! A server MUST reject any received request message that contains whitespace between a header field-name and colon with 400
//! rfc 7230 section 3.2.4

//! The presence of a message body in a request is signaled by a content-length or transfer-Encoding header field 
//! rfc 7230 section 3.3

 //! when a messaege does not have a transfer-Encoding header fields, a contentlength header field can provide the anticipated
 //! size as a decimal number of octets
 //! rfc 7230 section 3.2.2


 //! Any cotent-length field value greater than or eual to zero is valid[...] prevent parsing errors due to integer conversion overlows
 //! rfc 7230 section 3.3.2

//! if a message is received without transfer-Encoding and content length , the transfer-encoding overrides the content length.

//! if a message is received without transefer-Encoding and with multiple content-length => BAD-REQUEST
//! if a valid content-length header field is present without transfer-encoding , the value defines 
    //! the expected message body length

//! if this is a request message and none f the above are true => then message body length is zero

//! if there is a message body  without content-length => response 411 (length required)

//! A message body that uses the chunked transfer coding is incomplete if the zero-sized chunk that terminates the encoding has not been received.
//! A message that uses a valid Content-Length is incomplete if the size of the message body received (in octets) is less than the value given by Content-Length.

// request::request(){}

request::request():  _headersComplete(false), _bodyComplete(false), _isChunked(false), _isBodyExcpected(false)//, _server(server)
{
    _rqst.method = "";
    _rqst.URI = "";
    _rqst.versionHTTP = "";
    _rqst.query = "";
    _rqst.statusCode = 200;
    _contentLength = 0;
    
    std::cout<<"request called !"<<std::endl;
}

request::request(const request& obj)
{
    *this = obj;
}

request& request::operator=(const request& obj)
{
    _rqst = obj._rqst;
    _headersComplete = obj._headersComplete;
    _bodyComplete = obj._bodyComplete;
    _isChunked = obj._isChunked;
    _isBodyExcpected = obj._isBodyExcpected;
    _contentLength = obj._contentLength;
    // _server = obj._server;
    _data = obj._data;
    return *this;
}

request::~request(){}

void request::deleteOptionalWithespaces(std::string & fieldValue)
{
    while (std::isspace(fieldValue[0]))
        fieldValue.erase(0, 1);
    while (std::isspace(fieldValue[fieldValue.size()-1]))
        fieldValue.erase(fieldValue.size()-1);

   // std::cout<<"["<<fieldValue<<"]"<<std::endl;
}

bool request::isFieldNameValid(const std::string &str)
{
    for (size_t i = str.size() - 1; i != 0; i--)
    {
        if (std::isspace(str[i]))
            return false;
    }
    return true;
}

void    request::requestLine(std::istringstream &istr)
{
    //! I should convert basic_string to basic_istream to use getline

    std::string line;
    
    getline(istr, line);
   
    //! split first line by spaces
   
    std::vector<std::string> words;
    size_t pos = 0;

    if (line.empty() || line[line.size() - 1] != '\r')
    {
        _rqst.statusCode = 400;
        throw request::RequestNotValid();
    }
    while ((pos=line.find(" ")) != std::string::npos)
    {
        words.push_back(line.substr(0, pos));
        line.erase(0, pos+1);
    }
    words.push_back(line);
    if (words.size() != 3)//! should only be one space between them
    {
        _rqst.statusCode = 400;
        throw request::RequestNotValid();
    }
    if (words[0] != "GET" && words[0] != "POST" && words[0] != "DELETE")
    {
        _rqst.statusCode = 501; //! 501 method not implemented || 405 method not allowed
        throw request::RequestNotValid();
    }
    _rqst.method = words[0];
    if (_rqst.method == "POST")
    {
        srand(time(0));
        std::stringstream str;
        std::string st = "bodyFile";
        _rqst.bodyFile  = st + std::to_string(rand()) +".txt";
        my_file.open(_rqst.bodyFile, std::ios::out);
        _isBodyExcpected = true;
    }
    pos = words[1].find("?");
    if (pos != std::string::npos)
    {
        _rqst.URI = words[1].substr(0, pos);
        _rqst.query = words[1].substr(pos+1, words[1].size()-1);
    }
    else
        _rqst.URI = words[1];

    //* http version
    //! erase \r at the end of the line
    words[2].erase(words[2].end() - 1);
    if (words[2] != "HTTP/1.1") //! rfc 
    {
        _rqst.statusCode = 505;
        throw request::RequestNotValid();
    }
    _rqst.versionHTTP = words[2]; 
}

bool isNumber(std::string value)
{
    for (size_t i = 0 ; i < value.size();i++)
    {
        if (!std::isdigit(value[i]))
            return false;
    }
    return true;
}

void    request::getHeaders(std::istringstream & istr)
{
    std::string line;
    //! I should erase \r at the end of each line
    std::vector<std::string> tmp;

    while (getline(istr, line))
    {
        if (line[0] == '\r') //the end of the headers
            return;
        if (line[line.size() - 1] != '\r')
        {
            _rqst.statusCode = 400;
            throw request::RequestNotValid();
        }
        line.erase(line.end()-1);

        //! put the expected headers in the right position
        //! ignore the duplication
        //! content-length && transfer-Encoding
        
        //! split by :
        size_t pos = line.find(":");

        if (pos != std::string::npos)
        {
            std::string fieldName = line.substr(0, pos);
            //! check if the header is valid
            if (isFieldNameValid(fieldName))
            {
                std::string fieldValue = line.substr(pos+1, line.size()-1);
                deleteOptionalWithespaces(fieldValue);
                if (fieldName == "Transfer-Encoding" && fieldValue == "chunked")
                    _isChunked = true;
               (_rqst.headers).insert(std::pair<std::string, std::string>(fieldName, fieldValue)) ;
            }
            else
            {
                _rqst.statusCode = 400;
              throw request::RequestNotValid();
            }
        }
        else
        {
            _rqst.statusCode = 400;
            throw request::RequestNotValid();
        }   
    }
    if (!_isChunked)
    {
        std::cout<<"Is not chunked ! I should test negative value of content-length"<<std::endl;
        if (_rqst.headers.find("content-length") == _rqst.headers.end() || !isNumber(_rqst.headers.find("content-length")->second))
        {
            std::cout<<" ttttetsvfegrer  "<<_rqst.headers.find("content-length")->second<<std::endl;
            _rqst.statusCode = 411; // length required
            throw request::RequestNotValid();
        }
    }
}



const char* request::RequestNotValid::what()const throw()
{
    return "Request Not Valid !";
}

bool request::endBodyisFound(std::string lastLine)
{
    std::string line;
    (void)lastLine;
    my_file.close();
    my_file.open(_rqst.bodyFile, std::ios::in);
    while (std::getline(my_file, line))
    {
        if (line.find("\r") != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

s_requestInfo request::getRequest()
{
    return _rqst;
}

void request::parse(char *buffer, size_t r)
{
   // std::cout<<"parsing called !"<<std::endl;
    size_t i ;
    if (!_headersComplete)
    {
        i = 0;
        while (i < r)
        {
            _data+=buffer[i];i++;
        }
    }
    else if (_headersComplete && _isBodyExcpected && !_isChunked)
    {
        size_t i = 0;
        while (i < r && _contentLength < stoul(_rqst.headers["content-length"]))
        {
            _contentLength++;
            my_file<<buffer[i];i++;
        }
    }
    size_t pos = 0;
    if ((pos = _data.find("\r\n\r\n")) != std::string::npos && !_headersComplete) 
    {
            std::istringstream istr(_data);
            _headersComplete = true;
            requestLine(istr);
            getHeaders(istr);
            if (_isBodyExcpected && !_isChunked)
            {
                std::string leftdata = _data.substr(pos+4, _data.size()-1);
                //compare contentlength
                size_t i = 0;
                while (i < leftdata.size() && _contentLength < stoul(_rqst.headers["content-length"]))
                {
                    _contentLength++;
                    my_file<<leftdata[i];
                    i++;
                }
            }
            _data.clear();
            print_request();
    }
    //if body excpected of course
    if (_headersComplete && _isBodyExcpected && !_bodyComplete)
    {
        if (_contentLength >= stoul(_rqst.headers["content-length"]) && !_isChunked)
        {
            //I should erase the unwanted data
            std::cout<<"end body found"<<std::endl;
            _bodyComplete = true;
        }
        else if (_isChunked && endBodyisFound("0\r\n\r\n"))
        {
            ///check the size && put the rest in a file
            _bodyComplete = true;
        }
    }
}

void request::print_request()
{
    std::cout<<"Method : "<<_rqst.method<<std::endl;
    std::cout<<"URI : "<<_rqst.URI<<std::endl;
    std::cout<<"Query "<<_rqst.query<<std::endl;
    std::cout<<"http version : "<<_rqst.versionHTTP<<std::endl;

     //! print the map
    std::map<std::string, std::string>::iterator it = _rqst.headers.begin();
    std::map<std::string, std::string>::iterator ite = _rqst.headers.end();
    while (it != ite)
    {
        std::cout<<it->first<<" "<<it->second<<std::endl;
        it++;
    }
    //! print the body
}

bool request::isComplete()
{
    if (_headersComplete && (_bodyComplete || !_isBodyExcpected))
    {
        my_file.close();
        return true;
    }
    return false;
}

void request::forceStopParsing()
{
    _headersComplete = true;
    if (_isBodyExcpected)
        _bodyComplete = true;
}

// void        request::clearRequest()
// {
//     _rqst.method = "";
//     _rqst.URI = "";
//     _rqst.versionHTTP = "";
//     _rqst.headers.clear(); //clear the map => size = 0 | elements are destroyed
// }

// s_requestInfo request::tokenizeRequest()
// {
//     //! istream getline(istream& is, string& str, char delim)

//     return _rqst;
// }