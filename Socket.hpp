/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/03 17:48:35 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
// #include <sys/select.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#define PORT 80

namespace ft{

	class Socket{
	
	  private:
		std::vector<int> _socket_fd;
		std::string _msg;
		std::vector<struct sockaddr_in> _address;
		std::vector<struct pollfd> _fds;

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
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = htonl(INADDR_ANY);
			address.sin_port = htons(PORT);
			_address.push_back(address);
			_msg = "Failed to bind socket";
			check(bind(socket_fd, (struct sockaddr *)&address, sizeof(address)), socket_fd);

			//+ Set the listen back log to 1024
			_msg = "Failed to listen";
			check(listen(socket_fd, 1024), socket_fd);
			_socket_fd.push_back(socket_fd);
			socketio();
		}


		//? Parametrized Constructor
		Socket(int domain, int type, int protocol, int port, unsigned int interface, int backlog){

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
			address.sin_addr.s_addr = htonl(interface);
			address.sin_port = htons(port);
			_address.push_back(address);
			_msg = "Failed to bind socket";
			check(bind(socket_fd, (struct sockaddr *)&address, sizeof(address)), socket_fd);
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

			int timeout, p, r, s, new_socket, addrlen;
			char buffer[100000] = {0};

			//- Set up the initial listening socket for connections
			for (size_t i = 0; i < _socket_fd.size(); i++){
				struct pollfd fd;
				fd.fd = _socket_fd[i];
				fd.events = POLLIN;
				_fds.push_back(fd);
			}

			//- Initialize the timeout to 1 minute (60,000 milliseconds)
			//- If no activity after 1 minute this program will end
			timeout = 60 * 1000;
			//- Loop waiting for incoming connects or for incoming data on any of the connected sockets
			while (1){

				std::cout << "Polling ..." << std::endl;
				//& If the value of timeout is -1, poll() shall block until a requested event occurs or until the call is interrupted.
				p = poll(&_fds.front(), _fds.size(), -1);
				if (p < 0){
					std::cout << "Poll failed: Unexpected event occured" << std::endl;
					break;
				}
				if (p == 0){
					std::cout << "No new connection - Timeout" << std::endl;
					break;
				}
				for (size_t i = 0; i < _fds.size(); i++){

					if (!_fds[i].revents)
						continue;
					if (_fds[i].revents & POLLIN){

						if (_fds[i].fd == _socket_fd[i]){

							addrlen = sizeof(_address[i]);
							new_socket = accept(_fds[i].fd, (struct sockaddr *)&(_address[i]), (socklen_t*)&addrlen);
							_msg = "Failed to accpet";
							check(new_socket, new_socket);
							struct pollfd new_fd;
							new_fd.fd = new_socket;
							new_fd.events = POLLIN;
							_fds.push_back(new_fd);
							std::cout << "New connection accepted" << std::endl;
						}
						else{
							std::cout << "Listening socket is readable" << std::endl;
							// do{
								r = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
								if (r <= 0){
									_fds.erase(_fds.begin() + i);
									break;
								}
								std::cout << ">>> Received " << r << " bytes" << std::endl;
								std::cout << buffer << std::endl;
								memset(buffer, 0, sizeof(buffer));
								//= when the request is complete switch to the next request
								_fds[i].events = POLLOUT;
								// //- Print the received data	
								// std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h2>ok</h2></body></html>";
								// s = send(new_socket, resp.c_str(), strlen(resp.c_str()), 0);
								// if (s <= 0){
								// 	_fds.erase(_fds.begin() + i);
								// 	break;
								// }
								// std::cout << ">>> Sent " << s << " bytes" << std::endl;
							// } while (1);
						}
					}
					else if (_fds[i].revents & POLLOUT){
						std::cout << "Socket is writable" << std::endl;
						std::string resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 37\r\n\r\n<html><body><h2>ok</h2></body></html>";
						s = send(new_socket, resp.c_str(), strlen(resp.c_str()), 0);
						if (s <= 0){
							_fds.erase(_fds.begin() + i);
						}
						//= when the request is complete 
						_fds[i].events = POLLIN;
					}
				}
			}
			//? close all the sockets
			for (size_t i = 0; i < _fds.size(); i++){
				if (_fds[i].fd >= 0)
					close(_fds[i].fd);
			}
		}

		//= POLLHUP - The connection has been broken, or a connection has been made to a non-blocking socket that has been marked as non-blocking.
		//= POLLERR - An error has occured on the file descriptor.
		//= POLLNVAL - The file descriptor is not open, or the open file has been closed by another process.

		// //? Accept method
		// void accept_connection(){
		// 	int new_socket, r, p, s, nbr_fds = 2;
		// 	std::cout << ">> Waiting for new connections ..." << std::endl;
		// 	struct pollfd fds[nbr_fds];
		// 	char buffer[100000] = {0};
		// 	while(1){
		// 		if ((new_socket = accept(_socket_fd, (struct sockaddr *) &_address, (socklen_t*) &_addrlen)) < 0){
		// 			perror("Failed to connect");
		// 			exit(EXIT_FAILURE);
		// 		}
		// 		fds[0].fd = _socket_fd;
		// 		fds[0].events = POLLIN | POLLOUT;
		// 		// p = poll(fds, 1, 1000);
		// 		p = poll(fds, nbr_fds, 1000);
		// 		if (p == 0)
		// 			std::cout << ">> No new connection" << std::endl; //time out
		// 		if (p < 0){
		// 			std::cout << "Unexpected event occured" << std::endl;
		// 			perror("poll");
		// 		}
		// 		for (int i = 0; i < nbr_fds; i++){
		// 			if (!fds[i].revents)
		// 				continue;
		// 			if (fds[i].revents & POLLIN){
		// 				// Check if server socket is ready to read , accept
		// 				// else, recv
		// 				//? Data may be reading on device number i.
		// 				std::cout << "POLLIN event" << std::endl;
		// 				r = recv(new_socket, buffer, sizeof(buffer), 0);
		// 				std::cout << "**********\n" << buffer << "\n****************\n" << std::endl;
		// 				if (r < 0)
		// 					perror("read");
		// 				else
		// 					std::cout << ">> New connection from " << inet_ntoa(_address.sin_addr) << std::endl;
		// 			}
		// 			if (fds[i].revents & POLLOUT){
		// 				//? Data may be written on device number i.
		// 				std::cout << "POLLOUT event" << std::endl;
		// 				std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h2>succes</h2></body></html>";
		// 				s = send(new_socket, resp.c_str(), strlen(resp.c_str()), 0);
		// 				if (s < 0)
		// 					perror("write");
		// 			}
		// 		}
		//  }


		void check(int res, int fd){

			if (res < 0){
				perror(_msg.c_str());
				if (fd > -1)
					close(fd);
				exit(-1);
			}
		}

		//? Destructor
		~Socket(void){}


	};
}

#endif