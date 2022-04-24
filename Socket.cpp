/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/20 06:10:21 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/23 23:48:36 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Socket.hpp"

Socket::Socket(){

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
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (address.sin_addr.s_addr == INADDR_NONE){
		memset((char *)&address, 0, sizeof(address));
		_msg = "Invalid address";
		check(-1, socket_fd);
	}
	_msg = "Failed to bind socket";
	check(bind(socket_fd, (struct sockaddr *)&address, sizeof(address)), socket_fd);
	_address.push_back(address);

	//+ Set the listen back log to 1024
	_msg = "Failed to listen";
	check(listen(socket_fd, BACKLOG), socket_fd);
	_socket_fd.push_back(socket_fd);
	//? Set up the initial listening socket for connections
	fill_fds();
}

Socket::Socket(int port){

	struct sockaddr_in address;
	int socket_fd, x = 1;
	_msg = "socket creation failure";
	check((socket_fd = socket(AF_INET, SOCK_STREAM, 0)), -1);
	_msg = "Address already in use";
	check(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&x, sizeof(x)), socket_fd);
	_msg = "Failed to set non-blocking mode";
	check(fcntl(socket_fd, F_SETFL, O_NONBLOCK), socket_fd);
	memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
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
	check(listen(socket_fd, BACKLOG), socket_fd);
	_socket_fd.push_back(socket_fd);
	//? Set up the initial listening socket for connections
	fill_fds();
}

Socket& Socket::operator=(const Socket& sock){

	_socket_fd = sock._socket_fd;
	_msg = sock._msg;
	_address = sock._address;
	_fds = sock._fds;
	return (*this);
}

Socket::~Socket(void){

	_fds.clear();
	_socket_fd.clear();
	_address.clear();
}

void Socket::check(int res, int fd){

	if (res < 0){
		if (fd > -1)
			close(fd);
		throw Socket::SocketException(_msg);
	}
}

void Socket::fill_fds(){

	struct pollfd new_fd;
	for (size_t i = 0; i < _socket_fd.size(); i++){
		new_fd.fd = _socket_fd[i];
		new_fd.events = POLLIN;
		_fds.push_back(new_fd);
	}
}

void Socket::close_fd(void){

	for (size_t i = 0; i < _fds.size(); i++){
		// if (_fds[i].fd >= 0)
		shutdown(_fds[i].fd, 2);
	}
}

void Socket::accept_connection(int i){

	std::cout << "Accepting connection" << std::endl;
	int addrlen = sizeof(_address[i]);
	int accept_fd = accept(_fds[i].fd, (struct sockaddr *)&(_address[i]), (socklen_t*)&addrlen);
	_msg = "Failed to accept connection";
	check(accept_fd, -1); //!!!! do not exit on error
	struct pollfd new_fd;
	new_fd.fd = accept_fd;
	new_fd.events = POLLIN;
	_fds.push_back(new_fd);
}

void Socket::recv_request(int i, Clients *c){

	char _buffer[1024];
	std::cout << "Receiving request" << std::endl;
	int r = recv(_fds[i].fd, _buffer, sizeof(_buffer), 0);
	if (r <= 0){
		c->remove_clients(_fds[i].fd);
		close(_fds[i].fd);
		_fds.erase(_fds.begin() + i);
		return;
	}
	// if (r == 0){
	// 	std::cout << "- Connection closed" << std::endl;
	// 	return;
	// }
	// std::cout << ">>> Received " << r << " bytes" << "\n" << "|" << _buffer << "|" << std::endl;
	//& parse the buffer
	try
	{
		c->connections[_fds[i].fd].first.parse(_buffer, r);
	}
	catch(request::RequestNotValid &e)
	{
		c->connections[_fds[i].fd].first.forceStopParsing();
		std::cout<<e.what()<<std::endl;
	}
	//catch => forceStopParsing => isComplete
	memset(_buffer, 0, sizeof(_buffer));
	//& when the request is complete switch the type of event to POLLOUT
	if (c->connections[_fds[i].fd].first.isComplete()){
		std::cout << "Request is complete " << std::endl;
		_fds[i].events = POLLOUT;
	}
}

void socketio(std::vector<serverInfo> server_conf, std::vector<int>	_socket_fd, std::string	_msg, std::vector<struct sockaddr_in> _address, std::vector<struct pollfd> _fds){

	Clients c;
	size_t j = 0;

	//? Loop waiting for incoming connects or for incoming data on any of the connected sockets
	while (1){

		//+ If the value of timeout is -1, poll() shall block until a requested event occurs or until the call is interrupted.
		for (size_t i = 0; i < _fds.size(); i++){

			std::cout << "\n\nPolling ... " << std::endl;
			int p = poll(&_fds.front(), _fds.size(), -1);
			std::cout << "********************\np = " << p << std::endl;
			if (p < 0){
				// throw SocketException("Poll failed: Unexpected event occured"); // !! do not exit on error
				std::cout << "Poll failed: Unexpected event occured" << std::endl;
				break;
			}
			else if (p == 0){
				std::cout << "No new connection" << std::endl; //!!!!
				break;
			}
			if (!_fds[i].revents){
				std::cout << "No r events - _fds[" << i << "] = " << _fds[i].fd << std::endl; //!!!
				continue;
			}
			else if (_fds[i].revents & POLLIN){

				if (find(_socket_fd.begin() ,_socket_fd.end(), _fds[i].fd) != _socket_fd.end())
					accept_connection(i);
				else{
					j = _fds.size() - _socket_fd.size() + i;
					c.connections.insert(std::make_pair(_fds[i].fd, std::make_pair(request(server_conf[j]), Response())));
					recv_request(i, &c);
				}
			}
			else if (_fds[i].revents & POLLOUT){
				j = _fds.size() - _socket_fd.size() + i;
				c.connections[_fds[i].fd].second = Response(c.connections[_fds[i].fd].first, server_conf[j]);
				send_response(i, &c);
				std::cout << "Done" << std::endl;
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

void Socket::socketio(){

	Clients c;

	//? Set up the initial listening socket for connections
	fill_fds();
	//? Loop waiting for incoming connects or for incoming data on any of the connected sockets
	while (1){

		//+ If the value of timeout is -1, poll() shall block until a requested event occurs or until the call is interrupted.
		for (size_t i = 0; i < _fds.size(); i++){

			std::cout << "\n\nPolling ... " << std::endl;
			int p = poll(&_fds.front(), _fds.size(), -1);
			std::cout << "********************\np = " << p << std::endl;
			if (p < 0){
				// throw SocketException("Poll failed: Unexpected event occured"); // !! do not exit on error
				std::cout << "Poll failed: Unexpected event occured" << std::endl;
				break;
			}
			else if (p == 0){
				std::cout << "No new connection" << std::endl; //!!!!
				break;
			}
			if (!_fds[i].revents){
				std::cout << "No r events - _fds[" << i << "] = " << _fds[i].fd << std::endl; //!!!
				continue;
			}
			else if (_fds[i].revents & POLLIN){

				if (find(_socket_fd.begin() ,_socket_fd.end(), _fds[i].fd) != _socket_fd.end())
					accept_connection(i);
				else{
					c.connections.insert(std::make_pair(_fds[i].fd, std::make_pair(request(), Response())));
					recv_request(i, &c);
				}
			}
			else if (_fds[i].revents & POLLOUT){
				c.connections[_fds[i].fd].second = Response(c.connections[_fds[i].fd].first);
				send_response(i, &c);
				std::cout << "Sent response" << std::endl;
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

// &&&&&&&&&&&&&&&&& static response
//= use this function for simple test 
bool Socket::send_response(int i, Clients *c){

	(void)c;
	std::cout << "Sending response" << std::endl;
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
	_fds[i].events = POLLIN;
	// close(_fds[i].fd);
	// _fds.erase(_fds.begin() + i);
	return true;
}

// &&&&&&&&&&&&&&&&&
// bool Socket::send_response(int i, Clients *c){

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
// 		// close(_fds[i].fd);
// 		// _fds.erase(_fds.begin() + i);
// 		//+ unlink body_file
// 		// if (unlink(rep.second.c_str()))
// 			// std::cout << "Failed to unlink file" << std::endl;
// 	}
// 	return true;
// }

int file_size(std::fstream file){

	std::streampos begin, end;
	begin = file.tellg();
	file.seekg(0, std::ios::end);
	end = file.tellg();
	int size = end - begin;
	file.seekg(0, std::ios::beg);
	return size;
}

// &&&&&&&&&&&&&&&&&
// bool Socket::send_response(int i, Clients *c){

// 	(void)c;
// 	std::cout << "Sending response" << std::endl;
// 	std::fstream file;
// 	file.open("main.cpp", std::ios::in | std::ios::binary);
// 	if (!file.is_open()){
// 		std::cout << "Failed to open file - no such file" << std::endl;
// 		return false;
// 	}
// 	int s, len = 0;
// 	char buff[1024];
// 	std::cout << "File opened" << std::endl;
// 	std::streampos begin, end;
// 	begin = file.tellg();
// 	file.seekg(0, std::ios::end);
// 	end = file.tellg();
// 	int size = end - begin;
// 	file.seekg(0, std::ios::beg);
// 	std::cout << "File size = " << size << std::endl;
// 	while (1){
// 		if (file.eof()){
// 			std::cout << "End Of File" << std::endl;
// 			break;
// 		}
// 		int lps = size - len;
// 		std::cout << "lps >>>>>>>>>> " << lps << " - len = " << len << std::endl;
// 		if ((size_t)lps >= sizeof(buff)){
// 			file.read(buff, sizeof(buff));
// 			s = send(_fds[i].fd, buff, sizeof(buff), 0);
// 			if (s < 0){
// 				close(_fds[i].fd);
// 				_fds.erase(_fds.begin() + i);
// 				std::cout << "hereeee\n";
// 				break;
// 			}
// 		}
// 		else{
// 			std::cout << "Last packet size = " << lps << std::endl;
// 			file.read(buff, lps);
// 			s = send(_fds[i].fd, buff, sizeof(lps), 0);
// 			if (s < 0){
// 				close(_fds[i].fd);
// 				_fds.erase(_fds.begin() + i);
// 				std::cout << "hereeee\n";
// 				break;
// 			}
// 		}
// 		if (s == 0){
// 			std::cout << "- Connection closed" << std::endl;
// 			return true;
// 		}
//         len += s;
//         //+ move cursor to pos[len] and continue reading from that position
//         file.seekg(len, std::ios::beg);
//         // if (len >= c->connections[_fds[i].fd].second.response_size()){
//         if (len >= size){
// 			std::cout << "here\n";
// 			break;
// 		}
// 		memset(buff, 0, sizeof(buff));
// 	}
// 	std::cout << "------------------- len = " << len << std::endl;
// 	file.close();
// 	std::cout << "File closed" << std::endl;
// 	// close the file if keep_alive is false
// 	if (c->connections[_fds[i].fd].second.IsKeepAlive() == false){
// 		std::cout << "Closing socket - keepAlive = false" << std::endl;
// 		_fds.erase(_fds.begin() + i);
// 	}
// 	else // when the request is complete switch the type of event to POLLIN
// 		_fds[i].events = POLLIN;
// 	close(_fds[i].fd);
//     //- remove node client from the map
// 	c->remove_clients(_fds[i].fd);
// 	std::cout << "client removed" << std::endl;
// 	return true;
// }
