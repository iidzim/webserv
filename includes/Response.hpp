/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:51:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/20 07:23:01 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>


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
        void stringfyHeaders();
        void errorsResponse(int statCode);
        int getContentlength();
        std::string getBody();
        std::string getHeaders();
        std::string toString(int);
        void PostMethod();
        void GetMethod();
        void DeleteMethod();
        bool IsKeepAlive(){return true;} //! ra definit function hna ghi bach tcompila ta tatzidiha f .cpp 7it makaynach
        int response_size(){return (_body.size() + _headers.size());} //! zet hadi tahya sorry ;) talmenb3d o 7ydiha
};

#endif