/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viet <viet@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:51:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/22 07:54:54 by viet             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "configurationReader.hpp"
#include "Request.hpp"
#include <sys/wait.h>

class Response{

    private:
        std::string                     _headers;
        std::string                     _body;
        std::string                     _contentType;
        std::string                     _path;
        std::string                     _connection;
        std::string                     _fileName;
        bool                            _iskeepAlive;
        s_requestInfo                   _reqInfo;
        
    public:
        Response();
        Response(request req);
        ~Response();
        std::pair<std::string, std::string> get_response(configurationReader const &);
        std::string setErrorsHeaders(std::string , std::string);
        void stringfyHeaders(configurationReader const &);
        void errorsResponse(int);
        int getContentlength();
        std::string getBody();
        std::string getHeaders();
        void PostMethod();
        void GetMethod();
        void DeleteMethod();
        bool IsKeepAlive();//! ra definit function hna ghi bach tcompila ta tatzidiha f .cpp 7it makaynach
        int response_size();
        template<class T>
        std::string toString(T);
        bool isFileExist(std::string );
        std::string findMimeType(std::string);
        int fileSize(std::string);

};

#endif