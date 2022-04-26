/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 21:03:58 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/26 04:39:42 by mac              ###   ########.fr       */
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
		std::cout<<"******** Request exception ****** "<<e.what()<<std::endl;
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

//! test with this function - static reponse
// bool Server::send_response(int i, Clients *c, int *l){

// 	(void)l;
// 	(void)c;
// 	std::cout << "Sending response" << std::endl;
// 	std::string rep = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 37\r\n\r\n<html><body><h2>ok</h2></body></html>";
// 	int s = send(_fds[i].fd, rep.c_str(), rep.length(), 0);
// 	if (s <= 0){
// 		close(_fds[i].fd);
// 		_fds.erase(_fds.begin() + i);
// 		std::cout << "here\n";
// 		return false;
// 	}
// 	if (s == 0){
// 		std::cout << "- Connection closed" << std::endl;
// 		return true;
// 	}
// 	_fds[i].events = POLLIN;
// 	return true;
// }

void Server::close_fd(void){

	for (size_t i = 0; i < _fds.size(); i++){
		shutdown(_fds[i].fd, 2);
	}
}

void Server::socketio(std::vector<serverInfo> server_conf){

	Clients c;
	size_t j = 0;
	int l;

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
				send_response(i, &c, &l);
				// std::cout << "Done" << std::endl;
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
// bool Server::send_response(int i, Clients *c, int *l){

// 	(void)l;
// 	std::cout << "Sending response" << std::endl;
// 	std::pair<std::string, std::string> rep = c->connections[_fds[i].fd].second.get_response();
// 	std::cout << rep.first << " - " << rep.second << std::endl;

// 	std::string headers = rep.first;
// 	int hs = headers.size();
// 	std::string filename = rep.second;

// 	std::cout << "test test" << std::endl;
// 	std::fstream file;
// 	file.open(filename, std::ios::in | std::ios::binary);
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
// 	int file_size = end - begin;
// 	file.seekg(0, std::ios::beg);

// 	std::cout << "File size = " << file_size << std::endl;
// 	std::cout << "headers size = " << headers.size() << std::endl;
// 	int size = file_size + hs;
// 	std::cout << "total size = " << size << std::endl;

// 	while (1){ // ! blocking send
// 		if (file.eof()){
// 			std::cout << "End Of File" << std::endl;
// 			break;
// 		}
// 		int lps = size - len;
// 		std::cout << "lps >>>>>>>>>> " << lps << " - len = " << len << std::endl;

// 		if ((size_t)lps >= sizeof(buff)){ //? if total_size > 1024
// 			std::cout << "here111111\n";
// 			if (len < hs && sizeof(buff) > (size_t)hs){ //? headers not send yet

// 				std::cout << "send headers" << std::endl;
// 				char bf[sizeof(buff) - (headers.size() - len)];
// 				file.read(bf, sizeof(bf));
// 				(headers.substr(len)).append(bf);
// 				s = send(_fds[i].fd, headers.c_str(), sizeof(headers), 0);
// 				std::cout << "**********************************************" << s << std::endl;
// 				memset(bf, 0, sizeof(bf));
// 			}
// 			else{
// 				std::cout << "send body" << std::endl;
// 				file.read(buff, sizeof(buff));
// 				s = send(_fds[i].fd, buff, sizeof(buff), 0);
// 				memset(buff, 0, sizeof(buff));
// 			}
// 		}
// 		else{ //? if total_size(lps) < 1024
// 			std::cout << "Last packet size = " << lps << std::endl;
// 			if (len < hs){

// 				std::cout << "send headers" << std::endl;
// 				const int x = lps - (headers.size() - len);
// 				char bf[x];
// 				std::cout << "x = " << sizeof(bf) << std::endl;
// 				file.read(bf, sizeof(bf));
// 				if (!file)
// 					std::cout << "read failure" << std::endl;
// 				std::cout << "bf -> " << bf << std::endl;  //& bf
// 				headers.substr(len).append(bf); //! dynamic-stack-buffer-overflow
// 				s = send(_fds[i].fd, headers.c_str(), sizeof(headers), 0);
// 				std::cout << "**********************************************" << s << std::endl;
// 				memset(bf, 0, sizeof(bf));
// 			}
// 			else{
// 				std::cout << "send body" << std::endl;
// 				char bu[lps - len];
// 				file.read(bu, sizeof(bu));
// 				s = send(_fds[i].fd, bu, sizeof(bu), 0);
// 				memset(bu, 0, sizeof(bu));
// 			}
// 		}

// 		if (s < 0){
// 			close(_fds[i].fd);
// 			_fds.erase(_fds.begin() + i);
// 			std::cout << "hereeee s < 0\n";
// 			break;
// 		}
// 		if (s == 0){
// 			std::cout << "- Connection closed" << std::endl;
// 			return true;
// 		}

//         len += s;
//         //+ move cursor to pos[len] and continue reading from that position //? if we sent all headers
// 		if ((size_t)len > headers.size()){
// 			std::cout << "move cursor" << std::endl;
//         	file.seekg((len - headers.size()), std::ios::beg);
// 		}
// 		else{
// 			std::cout << "move to beg" << std::endl;
// 			file.seekg(0, std::ios::beg);
// 		}
//         if (len >= size){
// 			std::cout << "here3333333\n";
// 			break;
// 		}
// 	}
// 	std::cout << "------------------- len = " << len << std::endl;
// 	file.close();
// 	std::cout << "File closed" << std::endl;
// 	// close the file if keep_alive is false
// 	int file_descriptor = _fds[i].fd;
// 	if (c->connections[_fds[i].fd].second.IsKeepAlive() == false){
// 		std::cout << "Closing socket - keepAlive = false" << std::endl;
// 		close(_fds[i].fd);
// 		_fds.erase(_fds.begin() + i);
// 	}
// 	else // when the request is complete switch the type of event to POLLIN
// 		_fds[i].events = POLLIN;
//     //- remove node client from the map
// 	c->remove_clients(file_descriptor);
// 	std::cout << "client removed" << std::endl;
// 	return true;
// }


// bool Server::send_response(int i, Clients *c, int *len){

// 	std::pair<std::string, std::string> rep = c->connections[_fds[i].fd].second.get_response();
	
// 	// std::cout << "***** rep.sec ===== "<< rep.first << std::endl;
// 	// std::cout << "***** rep.sec ===== "<< rep.second << std::endl;
// 	std::cout << "Sending response" << std::endl;
// 	std::string headers = rep.first;
// 	int header_size = rep.first.size();
// 	std::cout << "headers size = " << header_size << std::endl;
// 	while (1)
// 	{
// 		int s = send(_fds[i].fd, headers.c_str(), sizeof(headers.c_str()), 0);
// 		std::cout << "s = " << s << std::endl;
// 		if (s <= 0){
// 			close(_fds[i].fd);
// 			std::cout << "send failed" << std::endl;
// 			return false;
// 		}
// 		*len += s;
// 		std::cout << "len = " << *len << std::endl;
// 		// std::cout << headers << std::endl;
// 		if (*len < header_size){
// 			headers.substr(*len);
// 		}
// 		// std::cout << headers << std::endl;
		
// 		if (*len >= header_size){
// 			std::cout << "headers sent √" << std::endl;
// 			_fds[i].events = POLLIN;
// 			break;
// 		}
// 	}
// 	return true;
// }

bool Server::send_response(int i, Clients *c, int *l){

	(void)l;
	std::cout << "Sending response" << std::endl;
	std::pair<std::string, std::string> rep = c->connections[_fds[i].fd].second.get_response();
	// std::cout << rep.first << " - " << rep.second << std::endl;
	std::string filename = rep.second;
	std::string headers = rep.first;
	int hs = headers.size();

	std::fstream file;
	file.open(filename, std::ios::in | std::ios::binary);
	if (!file.is_open()){
		std::cout << "Failed to open file - no such file" << std::endl;
		return false;
	}
	std::cout << "File opened" << std::endl;


	std::streampos begin, end;
	begin = file.tellg();
	file.seekg(0, std::ios::end);
	end = file.tellg();
	int file_size = end - begin;
	file.seekg(0, std::ios::beg);

	int total_size = file_size + hs;
	std::cout << "File size = " << file_size;
	std::cout << "  - headers size = " << headers.size();
	std::cout << "  - total size = " << total_size << std::endl;


	// int o = open(filename.c_str(), O_RDONLY);
	// if (o < 0)
	// 	std::cout << "open failed\n";

	int s, len = 0;
	char buff[1024];
	
	while (1){ // + blocking send
		if (file.eof()){
			std::cout << "End Of File" << std::endl;
			break;
		}
		std::cout << "total_size >>>>>>>>>> " << total_size << " - cursor = " << len << std::endl;
		
		//&&&&&&&&&&&&&&&&&&&&&

		if (len < hs){

			std::cout << "send headers" << std::endl;
			// file.seekg(0, std::ios::beg);
			int x;
			if ((size_t)total_size > sizeof(buff))// && (size_t)hs < sizeof(buff))
				x = sizeof(buff) - (headers.size() - len);
			else
				x = total_size - (headers.size() - len);
			std::cout << "x = " << x << std::endl;
			file.read(buff, x);
			if (!file)
				std::cout << "read failure !!!!!!!!!!!!!!!" << std::endl;
			if (file.eof()){
				std::cout << "End Of File" << std::endl;
				break;
			}
			headers.substr(len).append(buff);
			s = send(_fds[i].fd, headers.c_str(), sizeof(headers), 0);
			std::cout << "*****************************************" << s << std::endl;
		}
		else{

			std::cout << "send body" << std::endl;
			int x;
			if ((size_t)total_size > sizeof(buff))
				x = sizeof(buff);
			else
				x = file_size - (len - hs);
			std::cout << "x = " << x << std::endl;
			// char bf[x];
			file.read(buff, x);
			if (!file)
				std::cout << "read failure" << std::endl;
			// int r = read(o, buff, x);
			// std::cout << "...................r = " << r << std::endl;
			s = send(_fds[i].fd, buff, sizeof(buff), 0);
			std::cout << "**********************************************" << s << std::endl;
		}
		memset(buff, 0, sizeof(buff));
		std::cout << "cursor = " << len << std::endl;
		//&&&&&&&&&&&&&&
		if (s < 0){
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i);
			std::cout << "hereeee s < 0\n";
			break;
		}
		if (s == 0){
			std::cout << "- Connection closed" << std::endl;
			return true;
		}
        len += s;
        if (len >= total_size){
			std::cout << "here3333333\n";
			break;
		}
        //+ move cursor to pos[len] and continue reading from that position //? if we sent all headers
		if (len > hs){
			std::cout << "move cursor" << std::endl;
        	file.seekg((len - hs), std::ios::beg);
			if (!file)
				std::cout << "seekg failed" << std::endl;
		}
		else{
			std::cout << "move to beg" << std::endl; //!!!!!!!!!!!!!!!!!!!!!!!!!
			file.seekg(0, std::ios::beg);
			if (!file)
				std::cout << "seekg failed" << std::endl;
		}
	}
	std::cout << "------------------- len = " << len << std::endl;
	// close(o);
	file.close();
	std::cout << "File closed" << std::endl;
	// close the file if keep_alive is false
	int file_descriptor = _fds[i].fd;
	if (c->connections[_fds[i].fd].second.IsKeepAlive() == false){
		std::cout << "Closing socket - keepAlive = false" << std::endl;
		close(_fds[i].fd);
		_fds.erase(_fds.begin() + i);
	}
	else // when the request is complete switch the type of event to POLLIN
		_fds[i].events = POLLIN;
    //- remove node client from the map
	c->remove_clients(file_descriptor);
	std::cout << "client removed" << std::endl;
	return true;
}
