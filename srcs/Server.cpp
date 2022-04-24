/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 21:03:58 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/24 00:13:56 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(std::vector<new_sock> s, std::vector<serverInfo> server_conf){

	//& initialize class attribute
	for (size_t i = 0; i < s.size(); i++){
		_socket_fd.push_back(s[i].get_fd());
		_address.push_back(s[i].get_address());
	}
	//& fill fds struct
	struct pollfd new_fd;
	for (size_t i = 0; i < _socket_fd.size(); i++){
		new_fd.fd = _socket_fd[i];
		new_fd.events = POLLIN;
		_fds.push_back(new_fd);
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
	if (accept_fd < 0){
		throw::new_sock::SocketException(_msg);
		std::cout << _msg << std::endl;
		return ;
		//&&&&&&&&&& recode this cond - behaviour ?
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
	if (r <= 0){
		c->remove_clients(_fds[i].fd);
		close(_fds[i].fd);
		_fds.erase(_fds.begin() + i);
		return;
	}
	// std::cout << ">>> Received " << r << " bytes" << "\n" << "|" << _buffer << "|" << std::endl;
	//& parse the buffer
	try{
		c->connections[_fds[i].fd].first.parse(_buffer, r);
	}
	catch(request::RequestNotValid &e){
		c->connections[_fds[i].fd].first.forceStopParsing();
		//! send error response - bad request 400 -
	}
	//catch => forceStopParsing => isComplete
	memset(_buffer, 0, sizeof(_buffer));
	//& when the request is complete switch the type of event to POLLOUT
	if (c->connections[_fds[i].fd].first.isComplete()){
		std::cout << "Request is complete " << std::endl;
		_fds[i].events = POLLOUT;
	}
}

bool Server::send_response(int i, Clients *c){

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
	return true;
}

void Server::close_fd(void){

	for (size_t i = 0; i < _fds.size(); i++){
		shutdown(_fds[i].fd, 2);
	}
}

void Server::socketio(std::vector<serverInfo> server_conf){

	Clients c;
	size_t j = 0;

    while (1){

		for (size_t i = 0; i < _fds.size(); i++){

			// std::cout << "fds size = " <<_fds.size() << std::endl;
			std::cout << "Polling ... " << std::endl;
			int p = poll(&_fds.front(), _fds.size(), -1);
			// std::cout << "p = " << p << std::endl;
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
				// std::cout << "No r events - _fds[" << i << "] = " << _fds[i].fd << std::endl; //!!!
				continue;
			}
			else if (_fds[i].revents & POLLIN){
			
				if (find(_socket_fd.begin() ,_socket_fd.end(), _fds[i].fd) != _socket_fd.end())
					accept_connection(i);
				else{
					// std::cout << "here\n";
					// std::cout << "fds.size = " << _fds.size() << " - _socket_fd size = " << _socket_fd.size() << std::endl;
					j = i - _fds.size() + _socket_fd.size();
					// std::cout << "j = " << j << std::endl;
					c.connections.insert(std::make_pair(_fds[i].fd, std::make_pair(request(server_conf[j]), Response())));
					recv_request(i, &c);
				}
			}
			else if (_fds[i].revents & POLLOUT){
				j = i - _fds.size() + _socket_fd.size();
				c.connections[_fds[i].fd].second = Response(c.connections[_fds[i].fd].first, server_conf[j]);
				send_response(i, &c);
				std::cout << "Done" << std::endl;
			}
			else if ((_fds[i].revents & POLLHUP) || (_fds[i].revents & POLLERR) || (_fds[i].revents & POLLNVAL)){
				close(_fds[i].fd);
				_fds.erase(_fds.begin() + i);
				break;
			}
			// std::cout << "********************\n" << std::endl;
		}
	}
	//? Terminate the connection
	close_fd();
}

// &&&&&&&&&&&&&&&&&
// bool Server::send_response(int i, Clients *c){

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

// int Server::file_size(std::fstream file){

// 	std::streampos begin, end;
// 	begin = file.tellg();
// 	file.seekg(0, std::ios::end);
// 	end = file.tellg();
// 	int size = end - begin;
// 	file.seekg(0, std::ios::beg);
// 	return size;
// }

// &&&&&&&&&&&&&&&&&
// bool Server::send_response(int i, Clients *c){

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
