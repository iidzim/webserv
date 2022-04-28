/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 03:41:20 by oel-yous          #+#    #+#             */
/*   Updated: 2022/04/28 02:45:06 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"

Response::Response(): _headers(""), _body("") {
}

Response::Response(request req, serverInfo s): _headers(""), _body(""), _reqInfo(req.getRequest()), _servInfo(s), _cursor(0) {
    _iskeepAlive = true;
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
    
    _body = currPath + "/../error_pages/" + toString(statCode) + ".html";
    if (!isFileExist(_body))
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
    else if (_reqInfo.statusCode == 500)
        _headers = setErrorsHeaders("500 Internal Server Error", toString(ret));
    else if (_reqInfo.statusCode == 501)
        _headers = setErrorsHeaders("501 Not Implemented", toString(ret));
    else if (_reqInfo.statusCode == 504)
        _headers = setErrorsHeaders("504 gateaway timeout", toString(ret));
    else if (_reqInfo.statusCode == 505)
        _headers = setErrorsHeaders("505 HTTP Version Not Supported", toString(ret));
    _iskeepAlive = true; // connection closed
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

    fd = unlink(_fileName.c_str());
    if (fd < 0){
        if (errno == EACCES){
            errorsResponse(403);
            return ;
        }
    }
    else {
        std::string connect =  _reqInfo.headers.find("Connection")->second;;
        if (connect == "keep-alive") {
            connect = "Keep-Alive";
        }
        else {
            connect = "Closed";
            _iskeepAlive = false;
        }
        headers << "HTTP/1.1 204 No Content\r\nConnection: " << connect << "\r\n\r\n";
    }
}



void Response::stringfyHeaders(){
    std::string root;
    
    for (unsigned long  i = 0; i < _servInfo.location.size(); i++){
        if (_reqInfo.URI == _servInfo.location[i].uri || _reqInfo.URI.find(_servInfo.location[i].uri) == 0){
            root = _servInfo.location[i].root;
            if (std::find(_servInfo.location[i].allow_methods.begin(), _servInfo.location[i].allow_methods.end(), 
                _reqInfo.method) == _servInfo.location[i].allow_methods.end()){
                    _reqInfo.statusCode = 405;
                    errorsResponse(405);
                    return ;
                }
            break ;
        }
        else
            root = _servInfo.root;
    }
    if (_reqInfo.URI == "/"){
        _fileName = root + "/" + toString("index.html");
        _body = _fileName;
    }
    else{
        _fileName = root +"/" + toString("index.html");
        _body = _reqInfo.bodyFile;
    }

    if (isFileExist(_fileName) == false){
        std::cout << "hereee " << std::endl;
        errorsResponse(404);
        return ;
    }
    if (_reqInfo.method == "GET" || _reqInfo.method == "POST"){
        GetandPostMethod();
    }
    else
        DeleteMethod();

}

std::pair<std::string, std::string> Response::get_response(){
    if (_reqInfo.statusCode != 200)
        errorsResponse(_reqInfo.statusCode);
    else{
        stringfyHeaders();
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

template<class T>
std::string Response::toString(T i){
    std::ostringstream ret;
    ret  << i;
    return ret.str();
}


int Response::response_size(){
    return (fileSize(_body) + _headers.size());
}


bool Response::IsKeepAlive(){
    return _iskeepAlive;
}


//!!!!!!!!!!!!
bool Response::is_complete(int len, std::string filename){

    _cursor += len;
    std::cout << "sending ... " << _cursor << std::endl;
	if ((size_t)_cursor >= fileSize(filename) + _headers.size())
        return true;
	return false;
}

int Response::get_cursor(){
    return _cursor;
}
