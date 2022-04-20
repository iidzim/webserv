/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-yous <oel-yous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:51:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/20 04:53:45 by oel-yous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>


// #include "request.hpp"

class Response{

    private:
        std::string                     _headers;
        // std::string                     _body;
        // std::pair<int, std::string>     _contentlength;
        // std::string                     _contentType;
        // std::string                     _path;
        // std::string                     _connection;

        s_requestInfo                   _reqInfo;
        
    public:
        Response();
        Response(request req);
        ~Response();
        std::pair<std::string, std::to_string> get_response();
        void stringfyHeaders();
        void errorsResponse();
        int getContentlength();
        std::string getBody();
        std::string getHeaders();
        std::string toString(int);
        void PostMethod();
        void GetMethod();
        void DeleteMethod();
        bool isKeepAlivce();
};

#endif