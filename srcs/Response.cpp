/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-yous <oel-yous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 03:41:20 by oel-yous          #+#    #+#             */
/*   Updated: 2022/04/20 05:00:33 by oel-yous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(): _headers(""), _body(""), _contentlength(0) {
}

Response::Response(request req):  _headers(""), _body(""), _reqInfo(req.getRequest()), _contentlength(0) {

}

Response::~Response(){

}

int Response::getContentlength(){
    return _contentlength;
}

void Response::errorsResponse(int statCode){
    char cwd[256];
    std::string heads;
    std::string currPath(getcwd(cwd, sizeof(cwd)));

    _body = currPath + "/../error_pages/" + toString(statCode) + ".html"; 
    heads = "HTTP/1.1 ER\r\nContent-type: text/html\r\nContent-length: XX\r\n\r\n";
    if (_reqInfo.statusCode == 400){
        _headers = heads.replace(heads.find("ER",0), 2,"400 Bad Request");
        _headers = _headers.replace(_headers.find("XX",0), 2,"968");
        _contentlength = 968;
    }
    else if (_reqInfo.statCode == 404){
        _headers = heads.replace(heads.find("ER",0), 2,"404 Not Found");
        _headers = _headers.replace(_headers.find("XX",0), 2,"1011");
        _contentlength = 1011;
    }
    // else if (_reqInfo.statCode == 500){
    //     _headers = heads.replace(heads.find("ER",0), 2,"500 Internal Server Error");
    //     _headers = headers.replace(_headers.find("XX",0), 2,"1022");
    //     _contentlength = 1022;
    // }
    else if (_reqInfo.statCode == 501){
        _headers = heads.replace(heads.find("ER",0), 2,"501 Not Implemented");
        _headers = _headers.replace(_headers.find("XX",0), 2,"1014");
        _contentlength = 1014;
    }
    else if (_reqInfo.statCode == 505){
        _headers = heads.replace(heads.find("ER",0), 2,"505 HTTP Version Not Supported");
        _headers = _headers.replace(_headers.find("XX",0), 2,"993");
        _contentlength = 993;
    }
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
    if (_reqInfo.statusCode != 200){
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

std::string Response::toString(int i){
    std::stringstream ret;
    ret  << i;
    return ret.str();
}