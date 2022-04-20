/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clients.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/20 06:34:34 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/20 06:37:48 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Clients::Clients(){}

Clients::Clients(int fd, request req, Response res){
    connections[fd] = std::make_pair(req, res);
}

Clients::~Clients(){
    connections.clear();
}

void Clients::remove_clients(int fd){
    connections.erase(fd);
}
