/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/28 11:12:07 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/23 00:36:34 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Socket.hpp"
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
			std::string path = argv[1];
			configurationReader cfg_reader(path);
			cfg_reader.parser();
			//std::cout<<cfg_reader<<std::endl;
			//! pass struct cfg_reader to the socket class
    		std::vector<serverInfo> virtualServer = cfg_reader.getVirtualServer();
			//std::cout<<cfg_reader<<std::endl;
    		for (size_t i = 0; i < virtualServer.size(); i++)
			 	Socket b(virtualServer[i].port);
			socket_io(virtualServer);
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