/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:51:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/18 16:50:29 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

class Response{

    private:
        std::string _response;
        std::string _headers; //= start_line + headers
        int _body_file; //= file descriptor of the body file
    public:
        Response(){}
        ~Response(){}
        std::string get_response(){
            return _response;
        }
        std::string get_headers(){
            return _headers;
        }
        int get_body_file(){
            return _body_file;
        }
};

#endif