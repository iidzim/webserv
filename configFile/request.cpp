#include "request.hpp"

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


request::request(): _headersComplete(false), _bodyComplete(false), _isChunked(false)
{
    _rqst.method = "";
    _rqst.URI = "";
    _rqst.versionHTTP = "";
    //generate random name for the file
    srand(time(0));
    std::stringstream str;
    std::string st = "bodyFile";
    _rqst.bodyFile  = st + std::to_string(rand()) +".txt";
    my_file.open(_rqst.bodyFile, std::ios::out);
    //open it
}

request::~request(){}

// void request::setBuffer(char *buff)
// {
//     _buffer = buff;
// }

bool request::isFieldNameValid(const std::string &str)
{
    //! f there is spaces between field name and :
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
        std::cout<<"400 Bad Request !"<<std::endl; //! throw an exception instead
        exit(EXIT_FAILURE);
    }
    while ((pos=line.find(" ")) != std::string::npos)
    {
        words.push_back(line.substr(0, pos));
        line.erase(0, pos+1);
    }
    words.push_back(line);
    if (words.size() != 3)//! should only be one space between them
    {
        std::cout<<"400 Bad Request!"<<std::endl;
        exit(EXIT_FAILURE);
    }
    if (words[0] != "GET" && words[0] != "POST" && words[0] != "DELETE")
    {
        std::cout<<"Request Not Valid\nError this method is not handled !"<<std::endl;
        exit(EXIT_FAILURE);
    }
    _rqst.method = words[0];
    _rqst.URI = words[1]; //!parse queries

    //* http version
    //! erase \r at the end of the line
    words[2].erase(words[2].end() - 1);
    if (words[2] != "HTTP/1.1")
    {
        std::cout<<"400 Bad  Request !"<<std::endl;
        exit(EXIT_FAILURE);
    }

    _rqst.versionHTTP = words[2]; 
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
            std::cout<<"400 Bad Request"<<std::endl;
            exit(EXIT_FAILURE);
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
                if (fieldName == "Transfer-Encoding" && fieldValue == "chunked")
                //! could be several values separated by a comma
                    _isChunked = true;
               (_rqst.headers).insert(std::pair<std::string, std::string>(fieldName, fieldValue)) ;
            }
            else
            {
                std::cout<<"400 Bad Request !"<<std::endl; exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::cout<<"400 Bad Request"<<std::endl; exit(EXIT_FAILURE);
        }   
    }
}

s_requestInfo request::parse(char *buffer, size_t r)
{
    //* look for \r\n\r\n => if found and _headersComplete == false
    //* if not found => put the values into the string caractere by character

    //! for (size_t i = 0; i < 20; i++)
    // !    std::cout<<_buffer[i]; std::string = char * => if there is a \0 it stops there

  
    if (_headersComplete && _bodyComplete)
    {
        my_file.close();
        return _rqst;
    } //_reuestiscomplete

    if (!_headersComplete)
    {
        int i = 0;
        while (i < r)
        {
            _data+=buffer[i];i++;
        }
    }
    
    size_t pos = 0;
    if ((pos = _data.find("\r\n\r\n")) != std::string::npos && !_headersComplete) 
    {
            std::istringstream istr(_data);
            _headersComplete = true;
            requestLine(istr);
            getHeaders(istr);
          //  print_request();
            //erase all from 0 to pos
            //_data.erase(0, pos);
    }
    if (_headersComplete && !_bodyComplete)
    {
        int i = 0;
        while (i < r)
        {   
            my_file<<buffer[i]; i++;
        }
    }
    if (_headersComplete)
    {
        if (!_isChunked && _data.find("\r\n\r\n"))
        {
            _bodyComplete = true;
            return _rqst;
        }
        else if (_isChunked && _data.find("0\r\n\r\n"))
        {
            _bodyComplete = true;
            std::cout<<"check if length = octets received !"<<std::endl;
        }
        
    }
    //body excpected => methode = POST
    //if the body is expected then parse it
    //if hcunked look for 0\r\n\r\n
    //if not hcunked \r\n\r\n
    return _rqst;
}

void request::print_request()
{
    std::cout<<"Method : "<<_rqst.method<<std::endl;
    std::cout<<"URI : "<<_rqst.URI<<std::endl;
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
//bool request::getRequestStatus(){return is_complete;}


//! istream getline(istream& is, string& str, char delim)

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