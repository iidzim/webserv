/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:45:25 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/10 00:29:22 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

// #include "Socket.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <utility>
#include <map>

class Client{

    private:

    public:
        std::map<int, std::pair<Request, Response> > client;
        Client(){}
        Client(int fd, Request req, Response res){
            client[fd] = std::make_pair(req, res);
        }
        ~Client(){
            client.clear();
        }

};

#endif