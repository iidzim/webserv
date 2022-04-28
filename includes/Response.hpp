
#ifndef RESPONSE_HPP
# define RESPONSE_HPP


#include "webserv.hpp"

class Response{

    private:
        std::string                         _headers;
        std::string                         _body;
        std::string                         _fileName;
        bool                                _iskeepAlive;
        s_requestInfo                       _reqInfo;
        mimeTypes                            _mime;
        serverInfo                          _servInfo;
        int                                 _cursor;

    public:
        Response();
        Response(request req, serverInfo serv);
        ~Response();
        std::pair<std::string, std::string> get_response();
        std::string setErrorsHeaders(std::string , std::string);
        void stringfyHeaders();
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
        int get_cursor();

};

template<class T>
std::string toString(T);
int fileSize(std::string);

#endif