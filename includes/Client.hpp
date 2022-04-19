/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:45:25 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/19 01:42:56 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Response.hpp"
#include "Request.hpp"
#include <utility>
#include <map>

class Clients{

    private:

    public:
        std::map<int, std::pair<request, Response> > connections;
        Clients(){}
        Clients(int fd, request req, Response res){
            connections[fd] = std::make_pair(req, res);
        }
        ~Clients(){
            connections.clear();
        }

        void remove_clients(int fd){
            connections.erase(fd);
        }

};

#endif