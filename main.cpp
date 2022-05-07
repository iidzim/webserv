/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/28 11:12:07 by iidzim            #+#    #+#             */
/*   Updated: 2022/05/07 13:16:38 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
#include "includes/configurationReader.hpp"

int main(int argc, char** argv){

	if (argc > 2){
		std::cerr << "usage:\t./webserv [configuration file]" << std::endl;
		return (-1);
	}
	try{
		if (argc == 1)
			//* add default configuration file
			Socket a;
		else
		{
			std::vector<int> ports;
			std::vector<Socket> sockets;
			std::vector<struct sockaddr_in> add;
			std::string path = argv[1];
			configurationReader cfg_reader(path);
			cfg_reader.parser();
			// std::cout<<cfg_reader<<std::endl;
			// ! pass struct cfg_reader to the socket class
    		std::vector<serverInfo> virtualServer = cfg_reader.getVirtualServer();
			// std::cout<<cfg_reader<<std::endl;
    		for (size_t i = 0; i < virtualServer.size(); i++){
				if (std::find(ports.begin(), ports.end(), virtualServer[i].port) == ports.end()){
					ports.push_back(virtualServer[i].port);
					sockets.push_back(Socket(virtualServer[i].port));
				}
			}
			Server s(sockets, virtualServer);
		}
	}
	catch (std::exception &e){
		std::cerr << e.what() << std::endl;
	}

	return (0);
}

//td:
	//? parse config file
	//? create sockets based on server configuration
	//? sockets listening
	//? establish connection
	//? receive request
	//? parse request
	//? generate response
	//? send response
	//? close connection
	//? repeat - handle multiple server & sockets at the same time