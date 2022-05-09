/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 21:03:58 by iidzim            #+#    #+#             */
/*   Updated: 2022/05/09 18:40:32 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(std::vector<Socket> s, std::vector<serverInfo> server_conf){

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

Server::~Server(){
	_socket_fd.clear();
	_address.clear();
	_fds.clear();
}

void Server::accept_connection(int i){

	std::cout << "Accepting connection" << std::endl;
	int addrlen = sizeof(_address[i]);
	int accept_fd = accept(_fds[i].fd, (struct sockaddr *)&(_address[i]), (socklen_t*)&addrlen);
	_msg = "Failed to accept connection";
	//!!!! do not exit on error
	// if (errno == EWOULDBLOCK)
		// std::cout << "EWOULDBLOCK failure\n";
	if (accept_fd < 0 && errno != EWOULDBLOCK){
		throw::Socket::SocketException(_msg);
		std::cout << _msg << std::endl;
		return ;
		//----------- recode this cond - behaviour ?
	}
	struct pollfd new_fd;
	new_fd.fd = accept_fd;
	new_fd.events = POLLIN;
	_fds.push_back(new_fd);
}

void Server::recv_request(int i, Clients *c){

	char _buffer[1024];
	std::cout << "Receiving request" << std::endl;
	int r = recv(_fds[i].fd, _buffer, sizeof(_buffer), 0);
	if (r < 0){
		std::cout << " map size = "  << c->connections.size() << std::endl;
		std::cout << "HERE\n";
		c->remove_clients(_fds[i].fd);
		close(_fds[i].fd);
		_fds.erase(_fds.begin() + i); //?
		return;
	}
	try{
		c->connections[_fds[i].fd].first.parse(_buffer, r);
	}
	catch(request::RequestNotValid &e){
		c->connections[_fds[i].fd].first.forceStopParsing();
		// send error response - bad request 400 -
	}
	// catch => forceStopParsing => isComplete
	memset(_buffer, 0, sizeof(_buffer));
	// when the request is complete switch the type of event to POLLOUT
	if (c->connections[_fds[i].fd].first.isComplete()){
		std::cout << "Request is complete " << std::endl;
		_fds[i].events = POLLOUT;
	}
}

void Server::close_fd(void){

	for (size_t i = 0; i < _fds.size(); i++){
		// shutdown(_fds[i].fd, 2);
		close(_fds[i].fd);
	}
}

void Server::brokenPipe(Clients *c, int i){

	close(_fds[i].fd);
	c->remove_clients(_fds[i].fd);
	_fds.erase(_fds.begin() + i);
}

void Server::send_response(int i, Clients *c){

	// std::cout << "Sending response" << std::endl;
	std::pair<std::string, std::string> rep = c->connections[_fds[i].fd].second.get_response();
	std::string headers = rep.first;
	std::string filename = rep.second;
	int headers_size = headers.size();

	// std::cout << "*****************\n" << headers << filename << "\n*****************\n" << std::endl;

	char buff[2048*1000];
	int total_size, o, x;
	int s = 0, len = c->connections[_fds[i].fd].second.get_cursor();

	if (len < headers_size){

		std::string str = headers.substr(len);
		s = send(_fds[i].fd, str.c_str(), str.length(), 0);
		//- check for sigpipe - if global bool = true -> close accept_fd and remove client from map
		// if (broken_pipe == true){
		// 	brokenPipe(c, i);
		// 	return;
		// }
	}
	else if (filename.length() != 0){

		// std::cout << "sending body ...\n";
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
			if (r > 0){
				s = send(_fds[i].fd, buff, BUFF_SIZE, 0);
				// if (broken_pipe == true){
				// 	close(o);
				// 	brokenPipe(c, i);
				// 	return;
				// }
			}
			memset(buff, 0, BUFF_SIZE);
		}
		close(o);
	}
	if (s < 0){
		close(_fds[i].fd);
		// _fds.erase(_fds.begin() + i); //? why
		std::cout << "send failure s <= 0\n";
		return;
	}
	if (c->connections[_fds[i].fd].second.is_complete(s, filename)){
		std::cout << "response is complete\n";
		int file_descriptor = _fds[i].fd;
		if (c->connections[_fds[i].fd].second.IsKeepAlive() == false){
			// std::cout << "Closing socket - keepAlive = false" << std::endl;
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i);
		}
		else
			_fds[i].events = POLLIN;
    	//- remove node client from the map
		c->remove_clients(file_descriptor);
	}
}

void Server::socketio(std::vector<serverInfo> server_conf){

	Clients c;

    while (1){

		for (size_t i = 0; i < _fds.size(); i++){

			std::cout << "Polling ... "  << c.connections.size() << std::endl;
			int p = poll(&_fds.front(), _fds.size(), -1);
			if (p <= 0){
				// throw SocketException("Poll failed: Unexpected event occured"); // !! do not exit on error
				// std::cout << "Poll failed: Unexpected event occured" << std::endl;
				// if p == 0 -> no new connection
				break;
			}
			if (!_fds[i].revents){
				// std::cout << "No r events - _fds[" << i << "] = " << _fds[i].fd << std::endl; //!!!
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

				if (c.connections[_fds[i].fd].second.get_cursor() == 0){

					serverInfo s;
					std::string serv_name = c.connections[_fds[i].fd].first.getHost();
					int port = c.connections[_fds[i].fd].first.getPort();
					// std::cout << "HOST >>> " << serv_name << " - PORT >>> " << port << std::endl;
					for (size_t i = 0; i < server_conf.size(); i++){	//!!!! optimize
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
				close(_fds[i].fd);
				_fds.erase(_fds.begin() + i);
				break;
			}
		}
	}
	//? Terminate the connection
	close_fd();
}
