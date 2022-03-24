/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/03/24 21:47:05 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8080

namespace ft{

	class Socket{
	
	  private:
		int _socket_fd;
		struct sockaddr_in _address;

	  public:
		Socket(void){

			_address.sin_family = AF_INET;
			_address.sin_addr.s_addr = htonl(INADDR_ANY);
			_address.sin_port = htons(PORT);
			std::cout << "creating socket ..." << std::endl;
			if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0){
				memset((char *)&_address, 0, sizeof(_address));
				std::cout << "binding socket ..." << std::endl;
				if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0){
					perror("Failed to bind");
					exit(EXIT_FAILURE);
				}
				std::cout << "listening socket ..." << std::endl;
				if (listen(_socket_fd, 3) < 0){
					perror("Failed to bind");
					exit(EXIT_FAILURE);
				}
			}
			else{
				perror("socket creation failure");
				exit(EXIT_FAILURE);
			}
		}

		Socket(int domain, int type, int protocol, int port, unsigned int interface, int backlog){

			_address.sin_family = domain;
			_address.sin_addr.s_addr = htonl(interface);
			_address.sin_port = htons(port);
			std::cout << "creating socket ..." << std::endl;
			if ((_socket_fd = socket(domain, type, protocol)) >= 0){
				memset((char *)&_address, 0, sizeof(_address));
				std::cout << "binding socket ..." << std::endl;
				if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0){
					perror("Failed to bind");
					exit(EXIT_FAILURE);
				}
				std::cout << "listening socket ..." << std::endl;
				if (listen(_socket_fd, backlog) < 0){
					perror("Failed to bind");
					exit(EXIT_FAILURE);
				}
			}
			else{
				perror("socket creation failure");
				exit(EXIT_FAILURE);
			}
		}

		Socket(const Socket& sock){ *this = sock; }

		Socket& operator=(const Socket& sock){

			_socket_fd = sock._socket_fd;
			_address = sock._address;
			return (*this);
		}

		~Socket(void){}


	};
}

#endif