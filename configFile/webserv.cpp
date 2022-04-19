/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: framdani <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/14 17:42:26 by framdani          #+#    #+#             */
/*   Updated: 2022/04/14 17:42:31 by framdani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include <fstream>
#include <string>
#include "request.hpp"

int main(int argc, char **argv)
{
    // if (argc == 2)
    // {
    //     std::string path = argv[1];

    //     configurationReader cfg_reader(path);
    //     cfg_reader.parser();
    //     std::cout<<cfg_reader<<std::endl;

    // }
    // //if argc == 1 => default
    // else
    //     std::cout<<"ERROR\n The program should take a config file as argument !"<<std::endl;
  
    char buffer[1024] = "GET URI HTTP/1.1";
   // char buf[1024] = ;
    
    request a;
    a.parse(buffer, 16);
    char buf [1024] = ("\r\nAccept: ok\r\nHost: 8080\r\nTest_header: pwd\r\n\r\nok ok");
    //!   a.setBuffer(buf);

    a.parse(buf, strlen(buf));

    char body[1024] = "tesetestest\r\n\r\n";
    a.parse(body, 15);
   // b.parse();
}
