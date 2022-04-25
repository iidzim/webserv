/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-yous <oel-yous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 03:41:20 by oel-yous          #+#    #+#             */
/*   Updated: 2022/04/25 01:37:49 by oel-yous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"

Response::Response(): _headers(""), _body("") {
}

Response::Response(request req, serverInfo s):  _headers(""), _body(""), _reqInfo(req.getRequest()), _servInfo(s) {
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
    if (fd < 0)
        return false;
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
    std::map<std::string, std::string>::iterator it = mimetype.find(".html");
    
    if (it != mimetype.end())
        mType = it->second;
    else
        mType = "text/html";
    std::string connect = _reqInfo.headers.find("Connection")->second;
    if (connect == "keep-alive") {
        connect = "Keep-Alive";
        _iskeepAlive = true;
    }
    else {
        connect = "Closed";
        _iskeepAlive = false;
    }
    headers << "HTTP/1.1 200 OK\r\nContent-type: " << mType << "\r\n\r\nContent-length: " << fileName(_body) <<  "\r\n\r\n";
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
            _iskeepAlive = true;
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
    
    for (int i = 0; i < _servInfo.location.size(); i++){
        if (_reqInfo.URI == _servInfo.location[i].uri || _reqInfo.URI.find(_servInfo.location.uri) == 0){
            root = _servInfo.location[i].root;
            if (std::find(_servInfo.location[i].allow_methods.begin(), _servInfo.location[i].allow_methods, 
                _reqInfo.method) == _servInfo.location[i].method.end()){
                    _servInfo.statusCode = 405;
                    errorsResponse(405);
                    return ;
                }
            break ;
        }
        else
            root = _servInfo.root;
    }
    if (_reqInfo.URI == "/"){
        _fileName = root + "/" + "index.html";
        _body = _fileName;
    }
    else{
        _fileName = root + "/" + _reqInfo.URI;
        _body = _reqInfo.bodyFile;
        
    }
    if (!isFileExist(_fileName)){
        errorsResponse(404);
        return ;
    }
    if (_reqInfo.method == "GET" || _reqInfo.method == "POST")
        GetandPostMethod();
    else
        DeleteMethod();
}

std::pair<std::string, std::string> Response::get_response(){
    if (_reqInfo.statusCode != 200)
        errorsResponse(_reqInfo.statusCode);
    else
        stringfyHeaders();
   return (std::make_pair(_headers, _body));
}

std::string Response::getBody(){
    return (_body);
}

std::string Response::getHeaders(){
    return _headers;
}



int Response::fileSize(std::string fileName){
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