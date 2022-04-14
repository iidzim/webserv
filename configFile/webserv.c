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
#include "configurationReader.hpp"

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        std::string path = argv[1];

        configurationReader cfg_reader(path);
        cfg_reader.parser();
        std::cout<<cfg_reader<<std::endl;

    }
    //if argc == 1 => default
    else
        std::cout<<"ERROR\n The program should take a config file as argument !"<<std::endl;
}
