/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: framdani <framdani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/28 11:12:07 by iidzim            #+#    #+#             */
/*   Updated: 2022/05/26 17:18:32 by framdani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"
#include "includes/configurationReader.hpp"


void signalhandler(int signum){

	broken_pipe = true;
	(void)signum;
	// std::cout << "Interrupt signal (" << signum << ") received.\n";
}
 std::string getCurrentDirectory(){
    char cwd[256];
    std::string currPath(getcwd(cwd, sizeof(cwd)));
    return currPath;
}

int main(int argc, char** argv){

	std::string pwd;
	broken_pipe = false;
	signal(SIGPIPE, signalhandler);
	if (argc > 2){
		std::cerr << "usage:\t./webserv [configuration file]" << std::endl;
		return (-1);
	}
	pwd = getCurrentDirectory();
	try{
		std::string path;
		if (argc == 1)
			path = pwd+"configFile/valid_confg/default";
		else
			path = argv[1];	
		std::vector<int> ports;
		std::vector<Socket> sockets;
		configurationReader cfg_reader(path);
		cfg_reader.parser();
    	std::vector<serverInfo> virtualServer = cfg_reader.getVirtualServer();
		// std::cout<<cfg_reader<<std::endl;
    	for (size_t i = 0; i < virtualServer.size(); i++){
			if (std::find(ports.begin(), ports.end(), virtualServer[i].port) == ports.end()){
				ports.push_back(virtualServer[i].port);
				sockets.push_back(Socket(virtualServer[i].port));
			}
		}
		Server s(sockets, virtualServer, pwd);
		ports.clear();
		sockets.clear();
	}
	catch (std::exception &e){
		std::cerr << e.what() << std::endl;
	}
	return (0);
}



//+ curl --resolve ok.ma:8081:127.0.0.1 http://ok.ma:8081
