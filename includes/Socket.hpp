/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/20 02:55:34 by iidzim           ###   ########.fr       */
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

namespace ft{

	class Socket{

	  private:
		std::vector<int> _socket_fd;
		std::string _msg;
		std::vector<struct sockaddr_in> _address;
		std::vector<struct pollfd> _fds;

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

		void accept_connection(int i){

			int addrlen = sizeof(_address[i]);
			int new_socket = accept(_fds[i].fd, (struct sockaddr *)&(_address[i]), (socklen_t*)&addrlen);
			_msg = "Failed to accept connection";
			check(new_socket, -1); //!!!! do not exit on error
			// _fds.erase(_fds.begin() + i); //======================== do not remove the socket_fd from the pollfd vector and add the accepted_fd
			struct pollfd new_fd;
			new_fd.fd = new_socket;
			new_fd.events = POLLIN;
			_fds.push_back(new_fd);
		}

		void recv_request(int i, Clients *c){

			char _buffer[5];
			std::cout << "Receiving request" << std::endl;
			int r = recv(_fds[i].fd, _buffer, sizeof(_buffer), 0);
			if (r < 0){
				// c->remove_clients(_fds[i].fd);
				close(_fds[i].fd);
				_fds.erase(_fds.begin() + i);
				return;
				//!! update do not throw exception
			}
			if (r == 0){
				// std::cout << "- Connection closed" << std::endl;
				return;
			}
			// std::cout << ">>> Received " << r << " bytes" << "\n" << "|" << _buffer << "|" << std::endl;
			//& parse the buffer
			c->connections[_fds[i].fd].first.parse(_buffer, r);
			memset(_buffer, 0, sizeof(_buffer));
			//& when the request is complete switch the type of event to POLLOUT
			if (c->connections[_fds[i].fd].first.isComplete()){
				std::cout << "Request is complete " << std::endl;
				_fds[i].events = POLLOUT;
			}
		}

		bool send_response(int i, Clients *c){

			(void)c;
			std::cout << "Sending response" << std::endl;
			// std::fstream file;
			// file.open("../test.png", std::ios::in | std::ios::binary);
			// if (!file.is_open()){
			// 	std::cout << "Failed to open file - no such file" << std::endl;
			// 	return false;
			// }
			// int len = 0;
			// std::cout << "File opened" << std::endl;
			// char buff[1024];
			// while (1){
			// 	file.read(buff, sizeof(buff));
			// 	if (file.eof()){
			// 		break;
			// 	}
			// 	int s = send(_fds[i].fd, buff, sizeof(buff), 0);
			// 	if (s < 0){
			// 		close(_fds[i].fd);
			// 		_fds.erase(_fds.begin() + i);
			// 		std::cout << "here\n";
			// 		return false;
			// 	}
			// 	if (s == 0){
			// 		std::cout << "- Connection closed" << std::endl;
			// 		return true;
			// 	}
			// }
			std::string rep = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 37\r\n\r\n<html><body><h2>ok</h2></body></html>";
			int s = send(_fds[i].fd, rep.c_str(), rep.length(), 0);
			if (s <= 0){
				close(_fds[i].fd);
				_fds.erase(_fds.begin() + i);
				std::cout << "here\n";
				return false;
			}
			if (s == 0){
				std::cout << "- Connection closed" << std::endl;
				return true;
			}
			// close the file if keep_alive is false
			// if (c->connections[_fds[i].fd].second.IsKeepAlive() == false){
				// std::cout << "Closing socket - keepAlive = false" << std::endl;
				// close(_fds[i].fd);
				// _fds.erase(_fds.begin() + i);
			// }
			// else // when the request is complete switch the type of event to POLLIN
				_fds[i].events = POLLIN;
			return true;
		}

		// bool send_response(int i, Clients *c){

		// 	// std::cout << "Sending response" << std::endl;
		// 	std::pair<std::string, std::string> rep = c->connections[_fds[i].fd].second.get_response();
		// 	std::string header = rep.first;
		// 	size_t s = send(_fds[i].fd, rep.first.c_str(), rep.first.length(), 0);
		// 	if (s <= 0){
		// 		c->remove_clients(_fds[i].fd);
		// 		close(_fds[i].fd);
		// 		_fds.erase(_fds.begin() + i);
		// 		return false;
		// 	}
		// 	if (s == rep.first.size()){
		// 		//+ when the request is complete switch the type of event to POLLIN
		// 		_fds[i].events = POLLIN;
		// 		//+ unlink body_file
		// 		if (unlink(rep.second.c_str()))
		// 			std::cout << "Failed to unlink file" << std::endl;
		// 	}
		// 	return true;
		// }

		void fill_fds(){

			struct pollfd new_fd;
			for (size_t i = 0; i < _socket_fd.size(); i++){
				new_fd.fd = _socket_fd[i];
				new_fd.events = POLLIN;
				_fds.push_back(new_fd);
			}
		}

		void close_fd(void){

			for (size_t i = 0; i < _fds.size(); i++){
				if (_fds[i].fd >= 0)
					shutdown(_fds[i].fd, 2);
			}
		}

	  public:

		//? Default Constructor
		Socket(void){

			struct sockaddr_in address;
			int socket_fd, x = 1;

			//+ Create an AF_INET4 stream socket to receive incoming connections
			_msg = "socket creation failure";
			check((socket_fd = socket(AF_INET, SOCK_STREAM, 0)), -1);

			//- This helps in manipulating options for the socket referred by the file descriptor socket.
			//- reuse of address and port - Prevents error such as: “address already in use”.
			//- SOL_SOCKET is the socket layer itself

			//& A server can bind() the same port multiple times as long as every invocation usesa different
			//& local IP address and the wildcard address INADDR_ANY is used only one time per port.

			_msg = "Address already in use";
			check(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&x, sizeof(x)), socket_fd);

			//+ Set socket to be nonblocking
			_msg = "Failed to set non-blocking mode";
			check(fcntl(socket_fd, F_SETFL, O_NONBLOCK), socket_fd);

			//+ Bind the socket
			memset((char *)&address, 0, sizeof(address));
			// bzero(&_address[i].sin_addr, sizeof(_address[i].sin_addr));
			
			address.sin_family = AF_INET;
			// address.sin_addr.s_addr = inet_addr("127.0.0.1");
			address.sin_addr.s_addr = INADDR_ANY;
			address.sin_port = htons(PORT);
			_msg = "Failed to bind socket";
			check(bind(socket_fd, (struct sockaddr *)&address, sizeof(address)), socket_fd);
			_address.push_back(address);

			//+ Set the listen back log to 1024
			_msg = "Failed to listen";
			check(listen(socket_fd, BACKLOG), socket_fd);
			_socket_fd.push_back(socket_fd);
			socketio();
		}

		//? Parametrized Constructor
		Socket(int domain, int type, int protocol, int port, int backlog){

			struct sockaddr_in address;
			int socket_fd, x = 1;
			_msg = "socket creation failure";
			check((socket_fd = socket(domain, type, protocol)), -1);
			_msg = "Address already in use";
			check(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&x, sizeof(x)), socket_fd);
			_msg = "Failed to set non-blocking mode";
			check(fcntl(socket_fd, F_SETFL, O_NONBLOCK), socket_fd);
			memset((char *)&address, 0, sizeof(address));
			address.sin_family = domain;
			address.sin_port = htons(port);
			address.sin_addr.s_addr = inet_addr("127.0.0.1");
			if (address.sin_addr.s_addr == INADDR_NONE){
				memset((char *)&address, 0, sizeof(address));
				_msg = "Invalid address";
				check(-1, socket_fd);
			}
			_msg = "Failed to bind socket";
			check(bind(socket_fd, (struct sockaddr *)&address, sizeof(address)), socket_fd);
			_address.push_back(address);
			_msg = "Failed to listen";
			check(listen(socket_fd, backlog), socket_fd);
			_socket_fd.push_back(socket_fd);
			socketio();
		}

		//? Copy Constructor
		Socket(const Socket& sock){ *this = sock; }

		//? Assignment operator
		Socket& operator=(const Socket& sock){

			_socket_fd = sock._socket_fd;
			_msg = sock._msg;
			_address = sock._address;
			_fds = sock._fds;
			return (*this);
		}

		void socketio(){

			Clients c;

			//? Set up the initial listening socket for connections
			fill_fds();
			//? Loop waiting for incoming connects or for incoming data on any of the connected sockets
			while (1){

				std::cout << "Polling ..." << std::endl;
				// std::cout << _fds.size() << std::endl;
				//+ If the value of timeout is -1, poll() shall block until a requested event occurs or until the call is interrupted.
				int p = poll(&_fds.front(), _fds.size(), -1);
				// std::cout << "********************\np = " << p << std::endl;
				if (p < 0)
					throw SocketException("Poll failed: Unexpected event occured"); // !! do not exit on error
				if (p == 0){
					std::cout << "No new connection" << std::endl; //!!!
					break;
				}
				for (size_t i = 0; i < _fds.size(); i++){

					// std::cout << _fds.size() << std::endl;
					if (!_fds[i].revents)
						continue;
					if (_fds[i].revents & POLLIN){

						if (_fds[i].fd == _socket_fd[i])
							accept_connection(i);
						else{
							// if ()
							c.connections.insert(std::make_pair(_fds[i].fd, std::make_pair(request(), Response())));
							recv_request(i, &c);
						}
					}
					else if (_fds[i].revents & POLLOUT){
						c.connections[_fds[i].fd].second = Response(c.connections[_fds[i].fd].first);
						send_response(i, &c);
					}
					else if ((_fds[i].revents & POLLHUP) || (_fds[i].revents & POLLERR) || (_fds[i].revents & POLLNVAL)){
						close(_fds[i].fd);
						_fds.erase(_fds.begin() + i);
						break;
					}
				}
			}
			//? Terminate the connection
			close_fd();
		}

		//- POLLHUP - The connection has been broken, or a connection has been made to a non-blocking socket that has been marked as non-blocking.
		//- POLLERR - An error has occured on the file descriptor.
		//- POLLNVAL - The file descriptor is not open, or the open file has been closed by another process.

		void check(int res, int fd){

			if (res < 0){
				if (fd > -1)
					close(fd);
				throw SocketException(_msg);
			}
		}

		//? Destructor
		~Socket(void){

			_fds.clear();
			_socket_fd.clear();
			_address.clear();
		}
	};
}

#endif

//td -- chunked request/response
//td -- throw exception instead of exit
//td -- send response (headers first + body)
//td -- catch EAGAIN & EWOULDBLOCK (send and recv /non-blocking)
//td -- check if keep-alive is disabled -> turn off connection
//td -- fix functions return value


//= on success, poll() returns a nonnegative value which is the number of elements in the pollfds
//= whose revents fields have been set to a nonzero value


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