
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
    headers << Connection(0);
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
    else if (_reqInfo.statusCode == 502)
        _headers = setErrorsHeaders("502 Bad Gateaway", toString(ret));
    else if (_reqInfo.statusCode == 505)
        _headers = setErrorsHeaders("505 HTTP Version Not Supported", toString(ret));
}

std::string Response::Connection(int flag){
    std::string ret = "\r\nConnection: ";
    std::map<std::string, std::string>::iterator it = _reqInfo.headers.find("connection");
    if( it != _reqInfo.headers.end()){
        std::string connect = it->second;
        if (connect == "keep-alive") {
            connect = "Keep-Alive";
        }
        else {
            connect = "Closed";
            _iskeepAlive = false;
        }
        ret += connect;
    }
    else
        ret += "Keep-alive";
    if (flag == 0)
        ret +="\r\n\r\n";
    return ret;
}

void Response::DeleteMethod(){
    int fd;
    std::ostringstream headers;
    std::cout << "file to delete ==== " << _path << std::endl;
    fd = unlink(_path.c_str());
    if (fd < 0){
        std::cout << "fd <<<<<<<<<<<<<<<<< 0" << std::endl;
        if (errno == EACCES)
            errorsResponse(403);
        return ;
    }
    else {
        headers << "HTTP/1.1 204 No Content";
        headers << Connection(0);
        _body = "";
        std::cout << "body is supposed to be empty == |" << _body << "|" <<std::endl; 
    }
    _headers = headers.str();
}

void Response::setOkHeaders(std::string mType, std::string body){
    _headers = "HTTP/1.1 200 OK\r\nContent-type: " + mType + "\r\nContent-length: " + toString(fileSize(body));
    _headers += Connection(0);
}

void Response::setResponse(){
    bool isLoc = false;
    DIR *folder;
    std::string cgiExt ="";
    std::pair<std::string, std::string> redirect;
    for (unsigned long  i = 0; i < _servInfo.location.size(); i++){
        if (_reqInfo.URI == (_servInfo.location[i].uri + "/") || _reqInfo.URI == _servInfo.location[i].uri
            || (_reqInfo.URI.find(_servInfo.location[i].uri + "/") == 0 && _servInfo.location[i].uri.size() > 1)) {
            _root = _servInfo.location[i].root;
            _location = _servInfo.location[i].uri;
            isLoc = true;
            _index = _servInfo.location[i].index;
            cgiExt = _servInfo.location[i].cgi;
            std::cout << _servInfo.location[i].redirect.first << "|||||||||||||||||" <<_servInfo.location[i].redirect.second << std::endl;
            redirect = _servInfo.location[i].redirect;
            // 
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
    if (cgiExt.length() > 0 && cgiExt != ".py" && cgiExt != ".php"){
        errorsResponse(502);
        return ;
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

    if (redirect.second.length() != 0){
        if (_path == (_root + redirect.first)){
            _body = _path;
            _headers = "HTTP/1.1 301 Moved Permanently\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body));
            _headers += "\r\nLocation: " + redirect.second + "/";
            _headers += Connection(0);
            return ;
        }
    }
    // if (_reqInfo.method == "DELETE"){
    //     DeleteMethod();
    //     return;
    // }
    folder = opendir(_path.c_str());
    if (!folder){
        std::cout << "path  ==== " << _path << std::endl;
        if (errno == EACCES)
            errorsResponse(403);
        else if (errno == ENOENT)
            errorsResponse(404);
        else if (errno == ENOTDIR){
            if (_reqInfo.method == "DELETE"){
                DeleteMethod();
                return ;
            }
            if (cgiExt.length()){
                if (cgiExt == _path.substr(_path.length() - cgiExt.length())){
                    std::pair<std::string, std::string> cgiOut;
                    cgi CGI(_reqInfo, _path, cgiExt);
                    CGI.executeFile();
                    cgiOut = CGI.parseCgiOutput();
                    _headers = "HTTP/1.1 200 OK" + Connection(1) + "\r\n" + cgiOut.first;
                    _body = cgiOut.second;
                }
                else // cgiExt != _path.substr(_path.length() - cgiExt.length())
                    errorsResponse(502);
                return ;
            }
            else{ // cgiExt.length  == 0
                // std::map<std::string, std::string> mimetype(_mime.getTypes());
                // std::string mType;
                // size_t pos = _reqInfo.URI.find(".");
                // if (pos != std::string::npos){
                //     std::map<std::string, std::string>::iterator it2 = mimetype.find(_reqInfo.URI.substr(pos));
                //     if (it2 != mimetype.end())
                //         mType = it2->second;
                //     else // mime type not found
                //         mType = "text/html";
                // }
                // else //  mime type not found
                //     mType = "text/html";
                
                _body = _path;
                _headers = "HTTP/1.1 200 OK\r\nContent-type: " + _mime.getType(_reqInfo.URI) + "\r\nContent-length: " + toString(fileSize(_body));
                if (_autoIndex == true)
                    _headers += "\r\nContent-Disposition: attachment";
                _headers += Connection(0);
            }
            return ;
        }
    }
    else { // folder opened 
        if (cgiExt.length() == 0){
            if (_path[_path.length() - 1] != '/'){
                _path += "/";
                _body = _path + _index[0];
                _headers = "HTTP/1.1 302 Found\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body));
                _headers += "\r\nLocation: " + _reqInfo.URI + "/";
                _headers += Connection(0);
                return ;
            }
            int fd =-1;
            size_t i = 0;
            for (; i < _index.size(); i++){
                fd = open((_path+ _index[i]).c_str(), O_RDONLY);
                if (fd >= 0){
                    std::cout << "reqInfomethod ==== " << _reqInfo.method << std::endl;
                    if (_reqInfo.method == "DELETE"){
                        _path = _path + _index[i];
                        DeleteMethod();
                        return ;
                    }
                    _body = _path + _index[i];
                    setOkHeaders("text/html", _body);
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
                    else { // indx.isError == false
                        _body = indx.getBodyName();
                        setOkHeaders("text/html", _body);

                        closedir(folder);
                    }
                }
                else //autoindex == false
                    errorsResponse(404);
                return ;
            }
        }
        else { // cgi.length != 0
            for (size_t i = 0; i < _index.size(); i++){
                if (_index[i].substr(_index[i].length() - cgiExt.length()) == cgiExt){
                    std::pair<std::string, std::string> cgiOut;
                    cgi CGI(_reqInfo, _path + "/" +_index[i], cgiExt);
                    CGI.executeFile();
                    cgiOut = CGI.parseCgiOutput();
                    _headers = "HTTP/1.1 200 OK" + Connection(1) + "\r\n" + cgiOut.first;
                    _body = cgiOut.second;
                    return ;
                }
            }
              // no cgi file with cgiExt is found in path
            int opn = -1;
            for (size_t i = 0; i < _index.size(); i++){
                opn = open((_path + '/' + _index[i]).c_str(), O_RDONLY);
                if (opn != -1){
                    _body = _path + "/" + _index[i];
                     setOkHeaders("text/html", _body);
                    close(opn);
                    break;
                }
                close(opn);
            }
            if (opn == -1) { 
                if (_autoIndex == true) {
                    autoIndex indx;
                    indx.setAutoIndexBody(folder, _path, _root, _location);
                    if (indx.isError() == true)
                        errorsResponse(indx.getErrorCode());
                    else { // no error in autoindex
                        _body = indx.getBodyName();
                         setOkHeaders("text/html", _body);
                        closedir(folder);
                    }
                }
                else // autoindex is false
                    errorsResponse(404);
            }
        }   
    }

}

std::pair<std::string, std::string> Response::get_response(){

    std::cout << "-----------------" <<  _reqInfo.statusCode <<"--------------------" << std::endl;
    if (_reqInfo.statusCode != 200)
        errorsResponse(_reqInfo.statusCode);
    else{
        setResponse();
    }
    std::pair<std::string, std::string> p;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << _headers << std::endl;
    std::cout << _body << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
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
    std::cout << "sending .... " << _cursor << std::endl;
	if ((size_t)_cursor >= fileSize(filename) + _headers.size())
        return true;
	return false;
}

int Response::get_cursor(){
    return _cursor;
}
