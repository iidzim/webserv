/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:45:25 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/08 17:07:23 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Socket.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <utility>

class Client{

    private:
        int _fd;
        std::pair<Response, Request> _data;


    public:
        Client();
        ~Client();
        

};

#endif