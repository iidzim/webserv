/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/24 04:31:05 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Socket.hpp"

Socket::Socket(void){

    int x = 1;
    _msg = "Socket creation failure";
    check((_socketfd = socket(AF_INET, SOCK_STREAM, 0)), -1);
    _msg = "Address already in use";
    check(setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, (char*)&x, sizeof(x)), _socketfd);
    _msg = "Failed to set non-blocking mode";
    check(fcntl(_socketfd, F_SETFL, O_NONBLOCK), _socketfd);
    memset((char *)&_address, 0, sizeof(_address));
    _address.sin_family = AF_INET;
    _address.sin_port = htons(PORT);
    _address.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (_address.sin_addr.s_addr == INADDR_NONE){
    	memset((char *)&_address, 0, sizeof(_address));
    	_msg = "Invalid address";
    	check(-1, _socketfd);
    }
    _msg = "Failed to bind Socket";
    check(bind(_socketfd, (struct sockaddr *)&_address, sizeof(_address)), _socketfd);
    _msg = "Failed to listen";
    check(listen(_socketfd, BACKLOG), _socketfd);
}

Socket::Socket(int port){

    int x = 1;
    _msg = "Socket creation failure";
    check((_socketfd = socket(AF_INET, SOCK_STREAM, 0)), -1);
    _msg = "Address already in use";
    check(setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, (char*)&x, sizeof(x)), _socketfd);
    _msg = "Failed to set non-blocking mode";
    check(fcntl(_socketfd, F_SETFL, O_NONBLOCK), _socketfd);
    memset((char *)&_address, 0, sizeof(_address));
    _address.sin_family = AF_INET;
    _address.sin_port = htons(port);
    _address.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (_address.sin_addr.s_addr == INADDR_NONE){
    	memset((char *)&_address, 0, sizeof(_address));
    	_msg = "Invalid address";
    	check(-1, _socketfd);
    }
    _msg = "Failed to bind Socket";
    check(bind(_socketfd, (struct sockaddr *)&_address, sizeof(_address)), _socketfd);
    _msg = "Failed to listen";
    check(listen(_socketfd, BACKLOG), _socketfd);
}

struct sockaddr_in Socket::get_address(void){
	return _address;
}

int Socket::get_fd(void){
    return _socketfd;
}

void Socket::check(int res, int fd){

    if (res < 0){
    	if (fd > -1)
    		close(fd);
    	throw Socket::SocketException(_msg);
    }
}

Socket::~Socket(void){}
