/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/28 11:12:07 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/14 23:52:14 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"

int main(int argc, char** argv){

	if (argc != 2){
		std::cerr << "usage:\t./webserv [configuration file]" << std::endl;
		return (-1);
	}
	(void)argv;
	try{
		ft::Socket a;
		// ft::Socket b(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 5);
	}
	catch (std::exception &e){
		std::cerr << "excption ..." << std::endl;
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
