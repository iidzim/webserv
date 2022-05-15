
#ifndef RESPONSE_HPP
# define RESPONSE_HPP


#include "webserv.hpp"

class Response{

    private:
        std::string                         _headers;
        std::string                         _body;
        std::string                         _path;
        std::string                         _location;
        std::string                         _root;
        std::vector<std::string>            _index;
        bool                                _iskeepAlive;
        s_requestInfo                       _reqInfo;
        mimeTypes                            _mime;
        serverInfo                          _servInfo;
        int                                 _cursor;
        bool                                _autoIndex;
        std::string                         _CurrDirecory;


    public:
        Response();
        Response(request req, std::string pwd);
        Response(request req, serverInfo serv, std::string pwd);
        Response(const Response & src);
        Response & operator=(const Response & obj);
        ~Response();
        std::string gen_random(const int len);
        std::pair<std::string, std::string> get_response();
        std::string setErrorsHeaders(std::string , std::string);
       
        void setResponse();
        void errorsResponse(int);
        int getContentlength();
        std::string getBody();
        std::string getHeaders();
        void GetandPostMethod();
        void DeleteMethod();
        bool IsKeepAlive();
        int response_size();
        bool isFileExist(std::string );
        std::string findMimeType(std::string);
        bool is_complete(int len, std::string filename);
        void setOkHeaders(std::string mType, std::string body);
        int get_cursor();
        std::string Connection(int flag);
        void uploadResponse();

};

template<class T>
std::string toString(T);
int fileSize(std::string);

#endif