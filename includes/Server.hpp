/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 21:03:58 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/29 05:09:51 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#define BUFF_SIZE 1024

class Server{

	private:

		std::vector<int>				_socket_fd;
		std::string						_msg;
		std::vector<struct sockaddr_in>	_address;
		std::vector<struct pollfd>		_fds;

		void socketio(std::vector<serverInfo> server_conf);
		void accept_connection(int i);
		void recv_request(int i, Clients *c);
		void send_response(int i, Clients *c);
		void close_fd(void);

	public:

		Server(std::vector<Socket> s, std::vector<serverInfo> server_conf);
		~Server();

};


#endif