/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 21:03:58 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/24 00:08:06 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "new_sock.hpp"

class Server{

	private:
		// std::vector<new_sock>			_socket;
		std::vector<int>				_socket_fd;
		std::string						_msg;
		std::vector<struct sockaddr_in>	_address;
		std::vector<struct pollfd>		_fds;

		void socketio(std::vector<serverInfo> server_conf);
		void accept_connection(int i);
		void recv_request(int i, Clients *c);
		bool send_response(int i, Clients *c);
		void close_fd(void);

	public:

		Server(std::vector<new_sock> s, std::vector<serverInfo> server_conf);
		~Server();


};


#endif