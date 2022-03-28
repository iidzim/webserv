/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/03/28 18:12:25 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#define PORT 80

namespace ft{

	class Socket{
	
	  private:
		int _socket_fd;
		struct sockaddr_in _address;
		int _addrlen;

	  public:

		//? Default Constructor
		Socket(void){

	//! warning .c_str()
			std::string msg;
			memset((char *)&_address, 0, sizeof(_address));
			_address.sin_family = AF_INET;
			_address.sin_addr.s_addr = htonl(INADDR_ANY);
			_address.sin_port = htons(PORT);
			msg = "socket creation failure";
			check((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)), msg.c_str());
			msg = "Failed to bind socket";
			check(bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)), msg.c_str());
			msg = "Failed to listen";
			check(listen(_socket_fd, 3), msg.c_str());
			_addrlen = sizeof(_address);
			accept_connection();
		}

		//? Parametrized Constructor
		Socket(int domain, int type, int protocol, int port, unsigned int interface, int backlog){

			std::string msg;
			memset((char *)&_address, 0, sizeof(_address));
			_address.sin_family = domain;
			_address.sin_addr.s_addr = htonl(interface);
			_address.sin_port = htons(port);
			msg = "socket creation failure";
			check((_socket_fd = socket(domain, type, protocol)), msg.c_str());
			msg = "Failed to bind socket";
			check(bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)), msg.c_str());
			msg = "Failed to listen";
			check(listen(_socket_fd, backlog), msg.c_str());
			_addrlen = sizeof(_address);
			accept_connection();
		}

		//? Copy Constructor
		Socket(const Socket& sock){ *this = sock; }

		//? Assignment operator
		Socket& operator=(const Socket& sock){

			_socket_fd = sock._socket_fd;
			_address = sock._address;
			_addrlen = sock._addrlen;
			return (*this);
		}

		//? Accept method
		void accept_connection(){

			int new_socket, r;
			std::cout << ">> Waiting for new connections ..." << std::endl;
			while(1){
				if ((new_socket = accept(_socket_fd, (struct sockaddr *) &_address, (socklen_t*) &_addrlen)) < 0){
					perror("Failed to connect");
					exit(EXIT_FAILURE);
				}
				//? Receive the request from the client
				char buffer[100000] = {0};
				r = recv(new_socket, buffer, sizeof(buffer), 0);
				// check if (r <= 0)
				std::cout << "**********\n" << buffer << "\n****************\n" << std::endl;

				//todo: Send the response to the client
				std::string resp = "hello from the other side hihihihih";
				send(new_socket, resp.c_str(), strlen(resp.c_str()), 0);

				close(new_socket);
			}
		}

		void check(int res, const char* msg){

			if (res < 0){
				perror(msg);
				exit(EXIT_FAILURE);
			}
		}

		//? Destructor
		~Socket(void){}


	};
}

#endif