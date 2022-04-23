/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_sock.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/23 23:17:05 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NEW_SOCKET_HPP
# define NEW_SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include <exception>
#include <stdexcept>
#include <fstream>

#include "Client.hpp"
#define PORT 8080
#define BACKLOG 1024

class new_sock{

	private:
		int					_socketfd;
		struct sockaddr_in	_address;
		std::string			_msg;

        void check(int res, int fd){

	        if (res < 0){
	        	if (fd > -1)
	        		close(fd);
	        	throw new_sock::SocketException(_msg);
	        }
        }

	public:

		new_sock(void){

	        int x = 1;
	        _msg = "socket creation failure";
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
	        _msg = "Failed to bind socket";
	        check(bind(_socketfd, (struct sockaddr *)&_address, sizeof(_address)), _socketfd);

	        //+ Set the listen back log to 1024
	        _msg = "Failed to listen";
	        check(listen(_socketfd, BACKLOG), _socketfd);
        }

		new_sock(int port){

            int x = 1;
	        _msg = "socket creation failure";
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
	        _msg = "Failed to bind socket";
	        check(bind(_socketfd, (struct sockaddr *)&_address, sizeof(_address)), _socketfd);

	        //+ Set the listen back log to 1024
	        _msg = "Failed to listen";
	        check(listen(_socketfd, BACKLOG), _socketfd);
        }

        class SocketException : public std::exception{

			private:
				std::string _msg;
			public:
				SocketException(std::string const &msg) throw(): _msg(msg){}
				virtual ~SocketException() throw(){}
				virtual const char* what() const throw(){
					return _msg.c_str();
				}
		};

        struct sockaddr_in get_address(void){
        	return _address;
        }

        int get_fd(void){
            return _socketfd;
        }

		~new_sock(void){}

};

#endif