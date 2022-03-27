/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 21:03:58 by iidzim            #+#    #+#             */
/*   Updated: 2022/03/26 15:32:02 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "vector"

namespace ft{

	class Server{

		public:
			Server();
			Server(int domain, int type, int protocol, int port, unsigned int interface, int backlog)
			{
				// _sock = new Socket(domain, type, protocol, port, interface, backlog);
				_sock.push_back(Socket(domain, type, protocol, port, interface, backlog));
				
			}

			Server(const Server& other){ *this = other; }

			Server operator=(const Server& other){

				_sock = other._sock;
				return (*this);
			}

			~Server();

		private:
			// Socket *_sock;
			std::vector<Socket> _sock;
	};

}

//! AvailabilityInternal.h