/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 21:03:58 by iidzim            #+#    #+#             */
/*   Updated: 2022/05/11 20:21:04 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(std::vector<Socket> s, std::vector<serverInfo>& server_conf){

	//& initialize class attribute
	size_t size = s.size();
	_socket_fd.resize(size);
	_address.resize(size);
	for (size_t i = 0; i < size; i++){
		_socket_fd[i] = s[i].get_fd();
		_address[i] = s[i].get_address();
	}

	//& fill fds struct
	_fds.resize(size);
	for (size_t i = 0; i < size; i++){
		_fds[i].fd = _socket_fd[i];
		_fds[i].events = POLLIN;
	}
	socketio(server_conf);
}

Server::~Server(void){
	_socket_fd.clear();
	_address.clear();
	_fds.clear();
}

void Server::accept_connection(int i){

	std::cout << "Accepting connection" << std::endl;
	int addrlen = sizeof(_address[i]);
	int accept_fd = accept(_fds[i].fd, (struct sockaddr *)&(_address[i]), (socklen_t*)&addrlen);
	_msg = "Failed to accept connection";
	if (accept_fd < 0 && errno != EWOULDBLOCK){
		throw::Socket::SocketException(_msg);
	}
	struct pollfd new_fd;
	new_fd.fd = accept_fd;
	new_fd.events = POLLIN;
	_fds.push_back(new_fd);
}

void Server::recv_request(int i, Clients *c, std::vector<serverInfo>& server_conf){

	char _buffer[1024];
	std::cout << "Receiving request" << std::endl;
	int r = recv(_fds[i].fd, _buffer, sizeof(_buffer), 0);
	_buffer[r] = '\0';
	if (r <= 0){
		std::cout << errno << "  HERE\n";
		c->remove_clients(_fds[i].fd);
		close(_fds[i].fd);
		_fds.erase(_fds.begin() + i);
		return;
	}
	c->connections.insert(std::make_pair(_fds[i].fd, std::make_pair(request(server_conf), Response())));
	try{
		c->connections[_fds[i].fd].first.parse(_buffer, r);
	}
	catch(request::RequestNotValid &e){
		c->connections[_fds[i].fd].first.forceStopParsing();
	}
	memset(_buffer, 0, sizeof(_buffer));
	// when the request is complete switch the type of event to POLLOUT
	if (c->connections[_fds[i].fd].first.isComplete()){

		std::cout << _fds[i].fd << " - Request is complete " << std::endl;
		_fds[i].events = POLLOUT;
	}
}

void Server::close_fd(void){

	for (size_t i = 0; i < _fds.size(); i++){
		shutdown(_fds[i].fd, 2);
		// close(_fds[i].fd);
	}
}

void Server::brokenPipe(Clients *c, int i){

	close(_fds[i].fd);
	c->remove_clients(_fds[i].fd);
	_fds.erase(_fds.begin() + i);
}

void Server::send_response(int i, Clients *c){

	std::cout << "Sending response" << std::endl;
	std::pair<std::string, std::string> rep = c->connections[_fds[i].fd].second.get_response();
	std::string headers = rep.first;
	std::string filename = rep.second;
	int headers_size = headers.size();

	// std::cout << "*****************\n" << headers << filename << "\n*****************\n" << std::endl;

	char buff[2048*1000];
	int total_size, o, x, s = 0;
	int len = c->connections[_fds[i].fd].second.get_cursor();

	if (len < headers_size){

		std::cout << "sending headers" << std::endl;
		std::string str = headers.substr(len);
		s = send(_fds[i].fd, str.c_str(), str.length(), 0);
		//? check for sigpipe - if global bool = true -> close accept_fd and remove accept_fd from _fds struct , remove client from map
		if (broken_pipe == true){
			std::cout << " broken pipe -headers-\n";
			brokenPipe(c, i);
			return;
		}
	}
	else if (filename.length() != 0){

		std::cout << "sending body ...\n";
		o = open(filename.c_str(), O_RDONLY);
		total_size = fileSize(filename) + headers_size - len;
		lseek(o, len - headers_size, SEEK_SET);
		x = total_size > BUFF_SIZE ? BUFF_SIZE : total_size;
		struct pollfd file[1];
		file[0].fd = o;
		file[0].events = POLLIN;
		int poll_file = poll(file, 1, -1);
		if (poll_file > 0 && (file[0].revents & POLLIN)){
			int r = read(o, buff, x);
			buff[r] = '\0';
			if (r > 0){
				s = send(_fds[i].fd, buff, x, 0);
				if (broken_pipe == true){
					std::cout << " broken pipe -body-\n";
					close(o);
					brokenPipe(c, i);
					return;
				}
			}
			memset(buff, 0, BUFF_SIZE);
		}
		close(o);
	}
	if (s < 0){
		c->remove_clients(_fds[i].fd);
		close(_fds[i].fd);
		_fds.erase(_fds.begin() + i);
		std::cout << errno << "  send failure s < 0\n";
		return;
	}
	if (s == 0)
		std::cout << "send == 0\n";
	if (c->connections[_fds[i].fd].second.is_complete(s, filename)){
		std::cout << _fds[i].fd << " - response is complete\n";
		int file_descriptor = _fds[i].fd;
		if (c->connections[_fds[i].fd].second.IsKeepAlive() == false){
			std::cout << "Closing socket - keepAlive = false" << std::endl;
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i);
		}
		else
			_fds[i].events = POLLIN;
    	//- remove node client from the map
		c->remove_clients(file_descriptor);
		std::cout << file_descriptor << " - removed client\n";
	}
}

void Server::socketio(std::vector<serverInfo>& server_conf){
	Clients c;

	for(;;){

		std::cout << "Polling ........... " << std::endl;
		int p = poll(&_fds.front(), _fds.size(), -1);
		if (p < 0)
			throw::Socket::SocketException("Poll failed: Unexpected event occured");
		if (p == 0){
			std::cout << "Poll failed: No new connection" << std::endl;
			continue;
		}
		// for (size_t i = 0; i < _fds.size(); i++){
		// 	std::cout << _fds[i].fd << " - " << _fds[i].events << std::endl;
		// }

		for (size_t i = 0; i < _fds.size(); i++){

			// std::cout << "fd[" << i << "] === " << _fds[i].fd << std::endl;
			if (!_fds[i].revents){
				std::cout << _fds[i].fd << " -> no revents" << std::endl;
				continue;
			}
			else if (_fds[i].revents & POLLIN){

				if (find(_socket_fd.begin() ,_socket_fd.end(), _fds[i].fd) != _socket_fd.end())
					accept_connection(i);
				else
					recv_request(i, &c, server_conf);
			}
			else if (_fds[i].revents & POLLOUT){

				if (c.connections[_fds[i].fd].second.get_cursor() == 0){

					serverInfo s;
					std::string serv_name = c.connections[_fds[i].fd].first.getHost();
					int port = c.connections[_fds[i].fd].first.getPort();
					for (size_t i = 0; i < server_conf.size(); i++){
						if (serv_name == server_conf[i].serverName && port == server_conf[i].port){
							s = server_conf[i];
							break;
						}
						if (port == server_conf[i].port)
							s = server_conf[i];
					}
					c.connections[_fds[i].fd].second = Response(c.connections[_fds[i].fd].first, s);
				}
				send_response(i, &c);
			}
			else if ((_fds[i].revents & POLLHUP) || (_fds[i].revents & POLLERR) || (_fds[i].revents & POLLNVAL)){

				std::cout << "POLLHUP - POLLERR - POLLNVAL" << std::endl;
				close(_fds[i].fd);
				_fds.erase(_fds.begin() + i);
				c.remove_clients(_fds[i].fd);
				continue;
			}
		}
	}
	//? Terminate the connection
	close_fd();
}
