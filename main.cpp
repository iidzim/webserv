/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/28 11:12:07 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/20 06:37:19 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Socket.hpp"
#include "includes/configurationReader.hpp"

// int main(int argc, char** argv){

// 	if (argc > 2){
// 		std::cerr << "usage:\t./webserv [configuration file]" << std::endl;
// 		return (-1);
// 	}
// 	try{
// 		if (argc == 1)
// 			fSocket a;
// 		else
// 		{
// 			std::string path = argv[1];
// 			configurationReader cfg_reader(path);
// 			cfg_reader.parser();
// 			std::cout<<cfg_reader<<std::endl;
// 			//! pass struct cfg_reader to the socket class 
// 			Socket b(AF_INET, SOCK_STREAM, 0, PORT, BACKLOG);
// 		}
// 	}
// 	catch (std::exception &e){
// 		std::cerr << "excption ..." << std::endl;
// 	}

// 	return (0);
// }

int main(int argc, char** argv){

	if (argc != 2){
		std::cerr << "usage:\t./webserv [configuration file]" << std::endl;
		return (-1);
	}
	(void)argv;
	try{
		Socket a;
		// Socket b(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 5);
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