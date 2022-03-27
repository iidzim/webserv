/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simple_server.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 15:22:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/03/26 16:50:45 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void){

	int socket_fd, new_sock_fd, r, backlog = 5, port = 8080;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	//! creates a stream socket in the Internet domain
	//* int server_fd = socket(domain, type, protocol);
		//? domain or address family = AF_INET(IPv4)
		//? type : SOCK_STREAM(virtual circuit service) means that it is a TCP socket.
		//? protocol :If the protocol is unspecified (a value of 0), the system selects
			//? a protocol that supports the requested socket type
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("cannot create socket");
		return (0);
	}
	//? htonl converts a long integer (e.g. address) to a network representation 
	//? htons converts a short integer (e.g. port) to a network representation
	memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));
	//! allows a process to specify the local address of the socket
	//* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	if (bind(socket_fd, (struct sockaddr *) &address, sizeof(address)) < 0){
		perror("failed to bind");
		return (0);
	}
	//! listen system call tells a socket that it should be capable of accepting incoming connections
		//? backlog = maximum number of pending connections that can be queued up before connections are refused
	if (listen(socket_fd, backlog) < 0){
		perror("listen failed");
		return (0);
	}
	//! connect : accept system call grabs the first connection request on the queue of pending
	//! connections (set up in listen) and creates a new socket for that connection.
	while (1){

		std::cout << " Waiting for new connections ..." << std::endl;
		if ((new_sock_fd = accept(socket_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0){
			perror("connect failed");
			return (0);
		}
		std::string buffer[1024] = {0};
		//* size_t send(s, const void* buf, sizeof buf, int flags);
		//* size_t recv(s, const void* buf, sizeof buf, int flags);
			//? flags can be specified as a nonzero value if one or more of the following is required
			//? MSG_OOB - MSG_PEEK - MSG_DONTROUTE
		r = recv(new_sock_fd, buffer, sizeof(buffer), 0);
		std::cout << buffer << std::endl;
		// char *hola = "Hello from server";
		// send(new_sock_fd, hola, strlen(hola), 0);
		close(new_sock_fd);
	}


	return (0);
}