/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viet <viet@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 03:41:20 by oel-yous          #+#    #+#             */
/*   Updated: 2022/04/21 17:18:02 by viet             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"

Response::Response(): _headers(""), _body("") {
}

Response::Response(request req):  _headers(""), _body(""), _reqInfo(req.getRequest()) {

}

Response::~Response(){

}


std::string Response::setErrorsHeaders(std::string ErrorMsg, std::string cLentgh){
    std::ostringstream headers;

    headers << "HTTP/1.1 ";
    headers << ErrorMsg;
    headers << "\r\nContent-type: text/html\r\nContent-length: ";
    headers << cLentgh;
    headers << "\r\n\r\n";
    return headers.str();
}


void Response::errorsResponse(int statCode){
    char cwd[256];
    std::string heads;
    std::string currPath(getcwd(cwd, sizeof(cwd)));
    off_t ret;
    int fd;

    
    _body = currPath + "/../error_pages/" + toString(statCode) + ".html"; 
    fd = open(_body.c_str(), O_RDONLY);
    ret = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if (_reqInfo.statusCode == 400)
        _headers = setErrorsHeaders("400 Bad Request", toString(ret));
    else if (_reqInfo.statusCode == 404)
        _headers = setErrorsHeaders("404 Not Found", toString(ret));
    else if (_reqInfo.statusCode == 500)
        _headers = setErrorsHeaders("500 Internal Server Error", toString(ret));
    else if (_reqInfo.statusCode == 501)
        _headers = setErrorsHeaders("501 Not Implemented", toString(ret));
    else if (_reqInfo.statusCode == 505)
        _headers = setErrorsHeaders("505 HTTP Version Not Supported", toString(ret));
    close(fd);
}

void Response::PostMethod(){
    
}


void Response::GetMethod(){
}

void Response::DeleteMethod(){

}

void Response::stringfyHeaders(){
    if (_reqInfo.method == "GET")
        GetMethod();
    else if (_reqInfo.method == "POST")
        PostMethod();
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


template<class T>
std::string Response::toString(T i){
    std::stringstream ret;
    ret  << i;
    return ret.str();
}


int Response::response_size(){
    int fd;
    int size;

    fd = open(_body.c_str(), O_RDONLY);
    size = lseek(fd, 0,SEEK_END);
    lseek(fd, 0, SEEK_SET);
    close(fd);
    return (size + _headers.size());
}


bool Response::IsKeepAlive(){
    return true;
} 
