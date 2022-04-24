/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_sock.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/24 00:12:48 by iidzim           ###   ########.fr       */
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


	public:

		new_sock(void);
		new_sock(int port);
		~new_sock(void);
        struct sockaddr_in get_address(void);
        int get_fd(void);
        void check(int res, int fd);

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

};

#endif