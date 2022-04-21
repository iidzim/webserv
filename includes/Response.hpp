/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viet <viet@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:51:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/21 17:17:07 by viet             ###   ########.fr       */
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

#include "Request.hpp"

class Response{

    private:
        std::string                     _headers;
        std::string                     _body;
        std::string                     _contentType;
        std::string                     _path;
        std::string                     _connection;

        s_requestInfo                   _reqInfo;
        
    public:
        Response();
        Response(request req);
        ~Response();
        std::pair<std::string, std::string> get_response();
        std::string setErrorsHeaders(std::string , std::string);
        void stringfyHeaders();
        void errorsResponse(int statCode);
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
};

#endif