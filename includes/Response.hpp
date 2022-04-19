/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:51:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/19 03:00:48 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Request.hpp"

class Response{

    private:
        std::string _response;
        std::string _headers; //= start_line + headers
        int _body_file; //= file descriptor of the body file
        
    public:
        Response(){}
        Response(int fd): _body_file(fd) {}
        Response(request req) {
            (void)req;
        }
        ~Response(){}
        std::pair<std::string, std::string> get_response(){
            return std::make_pair(_headers, _response);
        }
        std::string get_headers(){
            return _headers;
        }
        int get_body_file(){
            return _body_file;
        }
};

#endif