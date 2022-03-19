/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/03/19 19:04:37 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_SOCKET_HPP
# define SERVER_SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define port 8080

namespace ft{

	class serverSocket{
	
	  private:
		int _socket_fd;
		struct sockaddr_in _address;

	  public:
		serverSocket(void){

			if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0){
				memset((char *)&_address, 0, sizeof(_address));
				_address.sin_family = AF_INET;
				_address.sin_addr.s_addr = htonl(INADDR_ANY);
				_address.sin_port = htons(port);
				if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
					perror("Failed to bind");
			}
			else
				perror("socket creation failure");
		}

		serverSocket(int domain, int type, int protocol, int pport){

			if ((_socket_fd = socket(domain, type, protocol)) >= 0){
				memset((char *)&_address, 0, sizeof(_address));
				_address.sin_family = domain;
				_address.sin_addr.s_addr = htonl(INADDR_ANY);
				_address.sin_port = htons(pport);
				if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
					perror("Failed to bind");
			}
			else
				perror("socket creation failure");
		}

		serverSocket(const serverSocket& sock){ *this = sock; }

		serverSocket& operator=(const serverSocket& sock){

			_socket_fd = sock._socket_fd;
			_address = sock._address;
			return (*this);
		}

		~serverSocket(void){}

				

	};
}

#endif