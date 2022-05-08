
#include "../includes/Response.hpp"

Response::Response(): _headers(""), _body(""), _cursor(0) {
}

Response::Response(request req, serverInfo s):  _headers(""), _body(""), _reqInfo(req.getRequest()), _servInfo(s), _cursor(0) {
    _iskeepAlive = true;
    _autoIndex = false;
    _location = "";
    _path = "";
    _root = "";
}

Response::~Response(){

}

std::string Response::setErrorsHeaders(std::string ErrorMsg, std::string cLentgh){
    std::ostringstream headers;

    headers << "HTTP/1.1 "<<  ErrorMsg << "\r\nContent-type: text/html\r\nContent-length: ";
    headers << cLentgh <<  "\r\n\r\n";
    return headers.str();
}

bool Response::isFileExist(std::string pathName){
    int fd = open(pathName.c_str(), O_RDONLY);
    if (fd < 0){
        std::cout << "failed to open " << pathName << std::endl;
        return false;
    }
    close(fd);
    return true;
}

void Response::errorsResponse(int statCode){
    char cwd[256];
    std::string currPath(getcwd(cwd, sizeof(cwd)));
    int ret;
    
    _reqInfo.statusCode = statCode;
    _body = currPath + "/error_pages/" + toString(statCode) + ".html";
    if (isFileExist(_body) == false)
        _body = ""; // 
    ret = fileSize(_body);
    if (_reqInfo.statusCode == 400)
        _headers = setErrorsHeaders("400 Bad Request", toString(ret));
    else if (_reqInfo.statusCode == 403)
        _headers = setErrorsHeaders("403 Forbidden", toString(ret));
    else if (_reqInfo.statusCode == 404)
        _headers = setErrorsHeaders("404 Not Found", toString(ret));
    else if (_reqInfo.statusCode == 405)
        _headers = setErrorsHeaders("405 Not Method Not Allowed", toString(ret));
    else if (_reqInfo.statusCode == 411)
        _headers = setErrorsHeaders("411 Length Required", toString(ret));
    else if (_reqInfo.statusCode == 413)
        _headers = setErrorsHeaders("413 Payload Too Large", toString(ret));
    else if (_reqInfo.statusCode == 500)
        _headers = setErrorsHeaders("500 Internal Server Error", toString(ret));
    else if (_reqInfo.statusCode == 501)
        _headers = setErrorsHeaders("501 Not Implemented", toString(ret));
    else if (_reqInfo.statusCode == 504)
        _headers = setErrorsHeaders("504 gateaway timeout", toString(ret));
    else if (_reqInfo.statusCode == 505)
        _headers = setErrorsHeaders("505 HTTP Version Not Supported", toString(ret));
}


void Response::GetandPostMethod(){
    std::ostringstream headers;
    std::map<std::string, std::string> mimetype(_mime.getTypes());
    std::string mType;
    size_t pos = _reqInfo.URI.find(".");
    if (pos != std::string::npos){
        std::map<std::string, std::string>::iterator it2 = mimetype.find(_reqInfo.URI.substr(pos));
        if (it2 != mimetype.end())
            mType = it2->second;
        else
            mType = "text/html";
    }
    else
        mType = "text/html";
    std::map<std::string, std::string>::iterator it = _reqInfo.headers.find("Connection");
    if( it != _reqInfo.headers.end()){
        std::string connect = it->second;
        if (connect == "keep-alive") {
            connect = "Keep-Alive";
        }
        else {
            connect = "Closed";
            _iskeepAlive = false;
        }
    }
    headers << "HTTP/1.1 200 OK\r\nContent-type: " << mType << "\r\nContent-length: " << fileSize(_body) <<  "\r\n\r\n";
    _headers = headers.str();
}

void Response::DeleteMethod(){
    int fd;
    std::ostringstream headers;
    std::string fileName;
    std::cout << "path to delete " << _path << std::endl;
    fd = unlink(_path.c_str());
    if (fd < 0){
        if (errno == EACCES){
            errorsResponse(403);
            return ;
        }
    }
    else {
        std::string connect =  _reqInfo.headers.find("Connection")->second;
        if (connect == "keep-alive") {
            connect = "Keep-Alive";
        }
        else {
            connect = "Closed";
            _iskeepAlive = false;
        }
        headers << "HTTP/1.1 204 No Content\r\nConnection: " << connect << "\r\n\r\n";
        _body = "";
    }
}



void Response::setResponse(){
    bool isLoc = false;
    DIR *folder;
    for (unsigned long  i = 0; i < _servInfo.location.size(); i++){
        if (_reqInfo.URI == (_servInfo.location[i].uri + "/") || _reqInfo.URI == _servInfo.location[i].uri
            || (_reqInfo.URI.find(_servInfo.location[i].uri + "/") == 0 && _servInfo.location[i].uri.size() > 1)) {
            _root = _servInfo.location[i].root;
            _location = _servInfo.location[i].uri;
            isLoc = true;
            _index = _servInfo.location[i].index;
            if (_servInfo.location[i].autoindex == true)
                _autoIndex = true;
            if (std::find(_servInfo.location[i].allow_methods.begin(), _servInfo.location[i].allow_methods.end(), 
                _reqInfo.method) == _servInfo.location[i].allow_methods.end()){
                    _reqInfo.statusCode = 405;
                    errorsResponse(405);
                    return ;
                }
            break ;
        }
        else{
            _index = _servInfo.index;
            _root = _servInfo.root;
        }
    }
    if (_root == _servInfo.root && _servInfo.autoindex == true)
        _autoIndex = true;
    if (isLoc == true){
        std::string uri = _reqInfo.URI.substr(_location.length());
        _path = _root + uri;;
    }
    else
        _path = _root + _reqInfo.URI;
    if (_reqInfo.method == "GET" || _reqInfo.method == "POST"){
        folder = opendir(_path.c_str());
        if (!folder){
            if (errno == EACCES){
                errorsResponse(403);
                return ;
            }
            else if (errno == ENOENT){
                errorsResponse(404);
                return ;
            }
            else if (errno == ENOTDIR){

                std::map<std::string, std::string> mimetype(_mime.getTypes());
                std::string mType;
                size_t pos = _reqInfo.URI.find(".");
                if (pos != std::string::npos){
                    std::map<std::string, std::string>::iterator it2 = mimetype.find(_reqInfo.URI.substr(pos));
                    if (it2 != mimetype.end())
                        mType = it2->second;
                    else
                        mType = "text/html";
                }
                else
                    mType = "text/html";
                _body = _path;
                _headers = "HTTP/1.1 200 OK\r\nContent-type: " + mType + "\r\nContent-length: " + toString(fileSize(_body));
                if (_autoIndex == true)
                    _headers += "\r\nContent-Disposition: attachment;\r\n\r\n";
                else
                    _headers += "\r\n\r\n";
                return ;
            }
        }
        std::cout << std::boolalpha;
        std::cout << "_autoindex == " << _autoIndex << "     path == " << _path << std::endl;
        if (_autoIndex == false){
            int fd = open((_path+_index[0]).c_str(), O_RDONLY);
            if (!fd){
                errorsResponse(404);
            }
            else{
                _body = _path + _index[0];
                _headers = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body)) +  "\r\n\r\n";
        }
        close(fd);
        return ;
        }
        else {
            autoIndex indx;
            indx.setAutoIndexBody(folder, _path, _root, _location);
            if (indx.isError() == true){
                errorsResponse(indx.getErrorCode());
                return ;
            }
            _body = indx.getBodyName();
            _headers = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body)) +  "\r\n\r\n";
            closedir(folder);
            return ;
        }
    }
    else {
            DeleteMethod();
    }

}

std::pair<std::string, std::string> Response::get_response(){
    if (_reqInfo.statusCode != 200)
        errorsResponse(_reqInfo.statusCode);
    else{
        setResponse();
    }
    std::pair<std::string, std::string> p;
    p.first = _headers;
    p.second = _body;
   return (p);
}

std::string Response::getBody(){
    return (_body);
}

std::string Response::getHeaders(){
    return _headers;
}

bool Response::IsKeepAlive(){
    return _iskeepAlive;
} 


int Response::response_size(){
    return (fileSize(_body) + _headers.size());
}



template<class T>
std::string toString(T i){
    std::ostringstream ret;
    ret  << i;
    return ret.str();
}




int fileSize(std::string fileName){
    int fd = open(fileName.c_str(), O_RDONLY);
    int ret = 0;
    if (fd >= 0){
        ret = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        close (fd);
    }
    return (ret);
}

bool Response::is_complete(int len, std::string filename){

    _cursor += len;
	if ((size_t)_cursor >= fileSize(filename) + _headers.size())
        return true;
	return false;
}

int Response::get_cursor(){
    return _cursor;
}
