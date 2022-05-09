
#include "../includes/Response.hpp"

Response::Response(): _headers(""), _body(""), _cursor(0) {
}

Response::Response(request req, serverInfo s):  _headers(""), _body(""), _reqInfo(req.getRequest()), _servInfo(s), _cursor(0) {
    // std::cout << "_servInfo ================ " << _servInfo.serverName << std::endl;
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
    headers << cLentgh;
    headers << Connection();
    return headers.str();
}

bool Response::isFileExist(std::string pathName){
    int fd = open(pathName.c_str(), O_RDONLY);
    if (fd < 0)
        return false;
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

std::string Response::Connection(){
    std::string ret = "\r\nConnection: ";
    std::map<std::string, std::string>::iterator it = _reqInfo.headers.find("connection");
    if( it != _reqInfo.headers.end()){
        std::string connect = it->second;
        if (connect == "keep-alive") {
            connect = "Keep-Alive\r\n\r\n";
        }
        else {
            connect = "Closed\r\n\r\n";
            _iskeepAlive = false;
        }
        ret += connect;
    }
    else
        ret += "Keep-alive\r\n\r\n";
    return ret;
}

void Response::DeleteMethod(){
    int fd;
    std::ostringstream headers;
    std::string fileName;
    fd = unlink(_path.c_str());
    if (fd < 0){
        if (errno == EACCES){
            errorsResponse(403);
            return ;
        }
    }
    else {
        headers << "HTTP/1.1 204 No Content";
        headers << Connection();
        _body = "";
    }
    _headers = headers.str();
}



void Response::setResponse(){
    bool isLoc = false;
    DIR *folder;
    std::string cgi ="";
    for (unsigned long  i = 0; i < _servInfo.location.size(); i++){
        if (_reqInfo.URI == (_servInfo.location[i].uri + "/") || _reqInfo.URI == _servInfo.location[i].uri
            || (_reqInfo.URI.find(_servInfo.location[i].uri + "/") == 0 && _servInfo.location[i].uri.size() > 1)) {
            _root = _servInfo.location[i].root;
            _location = _servInfo.location[i].uri;
            isLoc = true;
            _index = _servInfo.location[i].index;
            cgi = _servInfo.location[i].cgi;
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
    }
    if (isLoc == false || _root == "" || _index.size() == 0){
        if (_index.size() == 0)
            _index = _servInfo.index;
        if (_root == "")
            _root = _servInfo.root;
        if (_reqInfo.method != "GET" && isLoc == false){
            errorsResponse(405);
            return ;
        }
    }
    if (_root == _servInfo.root && _servInfo.autoindex == true)
        _autoIndex = true;
    if (isLoc == true){
        std::string uri = _reqInfo.URI.substr(_location.length());
        _path = _root + uri;
    }
    else{
        _path = _root + _reqInfo.URI;
    }
    if (_reqInfo.method == "GET" || _reqInfo.method == "POST"){
        folder = opendir(_path.c_str());
        if (!folder){
            if (errno == EACCES)
                errorsResponse(403);
            else if (errno == ENOENT)
                errorsResponse(404);
            else if (errno == ENOTDIR){
                // if (cgi.length()){
                //     if (cgi == _path.substr(_path.length() - cgi.length())){
                //         execute_cgi (_path);
                //     }
                //     else
                //         errorResponse(502);
                // }
                // else{
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
                        _headers += "\r\nContent-Disposition: attachment";
                    _headers += Connection();
                }
            return ;
            }
        else {
            // if (cgi.length() == 0){
                if (_path[_path.length() - 1] != '/'){
                    _path += "/";
                    _body = _path + _index[0];
                    _headers = "HTTP/1.1 302 Found\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body));
                    _headers += "\r\nLocation: " + _reqInfo.URI + "/";
                    _headers += Connection();
                    return ;
                }
                int fd =-1;
                size_t i = 0;
                for (; i < _index.size(); i++){
                    fd = open((_path+ _index[i]).c_str(), O_RDONLY);
                    if (fd >= 0){
                         _body = _path + _index[i];
                        _headers = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body));
                        _headers += Connection();
                        close(fd);
                        return ;
                    }
                    close(fd);
                }
                if (fd < 0){
                    if (_autoIndex == true){
                        autoIndex indx;
                        indx.setAutoIndexBody(folder, _path, _root, _location);
                        if (indx.isError() == true)
                            errorsResponse(indx.getErrorCode());
                        else {
                            _body = indx.getBodyName();
                            _headers = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body));
                            _headers += Connection();
                            closedir(folder);
                        }
                    }
                    else
                        errorsResponse(404);
                    return ;
                }
        //  }
        //  else {
        //         iterator it = find(index.(cgi) in  _index)
        //         if (it != _index.end())
        //             execute_cgi(*it)
        //         else{
        //             int opn = -1;
        //             try to open the first file that will open in _index vector;(opn = open(_index[i])) (break when opn != -1)
        //             if opn >= 0 : _body = _index[i];
        //             else {
        //                 if _autoIndex =true
        //                     autoindex;
        //                 else
        //                     errorResponse(404);
        //             }
        //         }
        //     }
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
    // std::cout << "-----------------------------------------------" << std::endl;
    // std::cout << _headers << std::endl;
    // std::cout << "-----------------------------------------------" << std::endl;
    // std::cout << "-----------------------------------------------" << std::endl;
    // std::cout << _body << std::endl;
    // std::cout << "-----------------------------------------------" << std::endl;
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
