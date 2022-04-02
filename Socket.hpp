/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:25:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/02 20:38:02 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
// #include <sys/select.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#define PORT 80

namespace ft{

	class Socket{
	
	  private:
		std::vector<int> _socket_fd;
		struct sockaddr_in _address;
		int _addrlen;

	  public:

		//? Default Constructor
		Socket(void){

	//! warning .c_str()
			std::string msg;
			int socket_fd;
			memset((char *)&_address, 0, sizeof(_address));
			_address.sin_family = AF_INET;
			_address.sin_addr.s_addr = htonl(INADDR_ANY);
			_address.sin_port = htons(PORT);
			msg = "socket creation failure";
			check((socket_fd = socket(AF_INET, SOCK_STREAM, 0)), msg.c_str(), -1);
			msg = "Failed to bind socket";
			check(bind(socket_fd, (struct sockaddr *)&_address, sizeof(_address)), msg.c_str(), socket_fd);
			msg = "Failed to listen";
			check(listen(socket_fd, 3), msg.c_str(), socket_fd);
			_addrlen = sizeof(_address);
			_socket_fd.push_back(socket_fd);
			accept_connection();
			// socketio();
		}

		//? Parametrized Constructor
		Socket(int domain, int type, int protocol, int port, unsigned int interface, int backlog){

			std::string msg;
			int socket_fd;
			memset((char *)&_address, 0, sizeof(_address));
			_address.sin_family = domain;
			_address.sin_addr.s_addr = htonl(interface);
			_address.sin_port = htons(port);
			msg = "socket creation failure";
			check((socket_fd = socket(domain, type, protocol)), msg.c_str(), -1);
			msg = "Failed to bind socket";
			check(bind(socket_fd, (struct sockaddr *)&_address, sizeof(_address)), msg.c_str(), socket_fd);
			msg = "Failed to listen";
			check(listen(socket_fd, backlog), msg.c_str(), socket_fd);
			_addrlen = sizeof(_address);
			_socket_fd.push_back(socket_fd);
			accept_connection();
			// socketio();
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


		// socket I/O
		void socketio(){

			int timeout, p, r, s, new_socket;
			// int nfds = _socket_fd.size();
			std::vector<struct pollfd> fds;
			// struct pollfd fds[1024];
			std::string msg;
			char buffer[100000] = {0};

			//? Set up the initial listening socket for connections
			for (int i = 0; i < _socket_fd.size(); i++){
				struct pollfd fd;
				fd.fd = _socket_fd[i];
				fd.events = POLLIN;
				fds.push_back(fd);
			}

			// // ! can't work with vector of struct pollfd
			// for (int i = 0; i < _socket_fd.size(); i++){
				// fds[i].fd = _socket_fd[i];
				// fds[i].events = POLLIN;
			// }

			//? Initialize the timeout to 1 minute (60,000 milliseconds)
			//? If no activity after 3 minutes this program will end
			timeout = 60 * 1000;
			//? Loop waiting for incoming connects or for incoming data on any of the connected sockets
			while (1){

				std::cout << "Polling ..." << std::endl;
				p = poll(&(fds.front()), fds.size(), timeout);
				if (p < 0){
					std::cout << "Poll failed: Unexpected event occured" << std::endl;
					continue;
				}
				if (p == 0){
					std::cout << "No new connection - Timeout" << std::endl;
					continue;
				}
				for (int i = 0; i < fds.size(); i++){

					if (!fds[i].revents)
						continue;
					if (fds[i].revents & POLLIN){

						if (fds[i].fd == _socket_fd[i]){

							new_socket = accept(fds[i].fd, (struct sockaddr *) &_address, (socklen_t*) &_addrlen);
							msg = "Failed to connect";
							check(new_socket, msg.c_str(), new_socket);
							struct pollfd new_fd;
							new_fd.fd = new_socket;
							new_fd.events = POLLIN;
							fds.push_back(new_fd);
						}
						else{
							r = recv(new_socket, buffer, sizeof(buffer), 0);
							if (r <= 0)
								fds.erase(fds.begin() + i);
						}
					}
					if (fds[i].revents & POLLOUT){
						//? Data may be written on device number i.
						std::cout << "POLLOUT event" << std::endl;
						std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h2>succes</h2></body></html>";
						s = send(new_socket, resp.c_str(), strlen(resp.c_str()), 0);
						if (s <= 0){
							fds.erase(fds.begin() + i);
						}
					}
				}
				// close();
			}
		}

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

		// 		// if (p == 0)
		// 		// 	std::cout << ">> No new connection" << std::endl; //time out
		// 		// else if (fds[0].revents & POLLIN){
		// 		// 	r = recv(new_socket, buffer, sizeof(buffer), 0);
		// 		// 	if (r < 0)
		// 		// 		perror("read");
		// 		// 	else
		// 		// 		std::cout << ">> New connection from " << inet_ntoa(_address.sin_addr) << std::endl;
		// 		// }
		// 		// else if (fds[1].revents & POLLOUT){
		// 		// 	std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/html\n";//\n<html><body><h2>succes</h2></body></html>";
		// 		// 	s = send(new_socket, resp.c_str(), strlen(resp.c_str()), 0);
		// 		// 	if (s < 0)
		// 		// 		perror("write");
		// 		// }
		// 		//? Receive the request from the client
		// 		// char buffer[100000] = {0};
		// 		// int s;
		// 		// r = recv(new_socket, buffer, sizeof(buffer), 0);
		// 		// check if (r <= 0)
		// 		// std::cout << "**********\n" << buffer << "\n****************\n" << std::endl;

		// 		// //todo: Send the response to the client
		// 		// std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h2>succes</h2></body></html>";
		// 		// send(new_socket, resp.c_str(), strlen(resp.c_str()), 0);

		// 		close(new_socket);
		// 	}
		// }
		//? poll returns: count of ready descriptors, 0 on timeout, –1 on error

/*
 struct pollfd pfds[1]; // More if you want to monitor more

    pfds[0].fd = 0;          // Standard input
    pfds[0].events = POLLIN; // Tell me when ready to read

    // If you needed to monitor other things, as well:
    // pfds[1].fd = some_socket; // Some socket descriptor
    // pfds[1].events = POLLIN;  // Tell me when ready to read

    printf("Hit RETURN or wait 2.5 seconds for timeout\n");

    int num_events = poll(pfds, 1, 2500); // 2.5 second timeout

    if (num_events == 0) {
        printf("Poll timed out!\n");
    } else {
        int pollin_happened = pfds[0].revents & POLLIN;

        if (pollin_happened) {
            printf("File descriptor %d is ready to read\n", pfds[0].fd);
        } else {
            printf("Unexpected event occurred: %d\n", pfds[0].revents);
        }
    }
*/

/*
	//? https://pubs.opengroup.org/onlinepubs/009696799/functions/poll.html
	#include <stropts.h>
	#include <poll.h>
	...
	struct pollfd fds[2];
	int timeout_msecs = 500;
	int ret;
	    int i;
	
	
	// Open STREAMS device.
	fds[0].fd = open("/dev/dev0", ...);
	fds[1].fd = open("/dev/dev1", ...);
	fds[0].events = POLLOUT | POLLWRBAND;
	fds[1].events = POLLOUT | POLLWRBAND;
	
	
	ret = poll(fds, 2, timeout_msecs);
	
	
	if (ret > 0) {
	    // An event on one of the fds has occurred.
	    for (i=0; i<2; i++) {
	        if (fds[i].revents & POLLWRBAND) {
	        // Priority data may be written on device number i.
	...
	        }
	        if (fds[i].revents & POLLOUT) {
	        // Data may be written on device number i.
	...
	        }
	        if (fds[i].revents & POLLHUP) {
	        // A hangup has occurred on device number i.
	...
	        }
	    }
	}
*/

		void check(int res, const char* msg, int fd){

			if (res < 0){
				perror(msg);
				if (fd > -1)
					close(fd);
				exit(EXIT_FAILURE);
			}
		}

		//? Destructor
		~Socket(void){}


	};
}

#endif