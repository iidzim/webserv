/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/22 17:16:36 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

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


class Socket{

	private:
		std::vector<int>				_socket_fd;
		std::string						_msg;
		std::vector<struct sockaddr_in> _address;
		std::vector<struct pollfd> 		_fds;

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

		void socketio(void);
		void fill_fds(void);
		void accept_connection(int i);
		void recv_request(int i, Clients *c);
		bool send_response(int i, Clients *c);
		void check(int res, int fd);
		void close_fd(void);

	public:

		Socket(void);
		Socket(int port);
		Socket(const Socket& sock){ *this = sock; }
		Socket& operator=(const Socket& sock);
		~Socket(void);
};

#endif

//td -- chunked request/response
//td -- throw exception instead of exit
//td -- send response (headers first + body)
//td -- catch EAGAIN & EWOULDBLOCK (send and recv /non-blocking)
//td -- check if keep-alive is disabled -> turn off connection


//= on success, poll() returns a nonnegative value which is the number of elements in the pollfds
//= whose revents fields have been set to a nonzero value

//- POLLHUP - The connection has been broken, or a connection has been made to a non-blocking socket that has been marked as non-blocking.
//- POLLERR - An error has occured on the file descriptor.
//- POLLNVAL - The file descriptor is not open, or the open file has been closed by another process.


//! [error] socket: 165031936 address is unavailable.: Can't assign requested address
//! siege aborted due to excessive socket failure; you can change the failure threshold in $HOME/.siegerc
// Transactions:                  16344 hits
// Availability:                  93.97 %
// Elapsed time:                   8.53 secs
// Data transferred:               0.58 MB
// Response time:                  0.01 secs
// Transaction rate:            1916.06 trans/sec
// Throughput:                     0.07 MB/sec
// Concurrency:                   22.14
// Successful transactions:       16344
// Failed transactions:            1048
// Longest transaction:            0.13
// Shortest transaction:           0.00


//* Search for all read/recv/write/send on a socket and check that if an error returned the client is removed
//? accept/poll on a socket and check that if an error returned the client is removed and the socket is closed -> program continues