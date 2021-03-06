/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configurationReader.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/14 17:42:45 by framdani          #+#    #+#             */
/*   Updated: 2022/05/14 16:19:02 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATIONREADER_HPP
# define CONFIGURATIONREADER_HPP
#include <iostream>
#include <fstream>
#include<vector>
#include <map>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <exception>
#include <unistd.h>

# define INSIDESERVER   1
# define INLOCATION     2
# define CLOSED         0
# define ON true
# define OFF false

typedef struct s_location
{   
    std::string                         uri;
    std::string                         upload;
    //std::vector<std::string>            index;
    std::vector<std::string>            index;
    std::string                         root;
    std::vector<std::string>            allow_methods;
   // std::map<int, std::string>          errorPage;// statuscode | URL
    bool                                autoindex;
    std::pair<std::string, std::string> redirect; //first path | second=> replacement
    //std::pair<std::string, std::string> cgi;//first => extension | second => address
    std::string                         cgi;
}               locationInfos;

typedef struct s_server
{
    //! There are 65,535 ports available for communication between devices.
   // std::string                 port; // int 
   int                          port;
    //! an address can be a host and if only address is given the port is by default 80
  //  std::string                 host; // IPV4 unsigned int interface
    //std::vector<std::string> index;
    unsigned int                        host;
   // std::vector<std::string>            index;
    std::vector<std::string>            index;
    std::string                         root;
    unsigned long                       size;
    std::map<int, std::string>          errorPage;
    std::string                         serverName;
    bool                                autoindex;
    std::pair<std::string, std::string> redirect;
    std::vector<locationInfos>  location;
}               serverInfo;


class configurationReader
{
    private:
        std::string                 _path;
        std::vector<serverInfo>     _virtualServer;
        int                         _state;
        std::ifstream               _infile;
        
        bool                                    isValidPath();
        bool                                    isCommentOrEmptyLine(std::string & line);
        std::vector<std::string>                splitbySpace(std::string & line);
        
        void                                    setAllowedMethods(std::vector<std::string> words, locationInfos &location);
        void                                    setPortHost(std::vector<std::string> words, serverInfo &server);
        void                                    setServerName(std::vector<std::string> words, serverInfo &server);
        void                                    setRoot(std::vector<std::string> words, serverInfo &server, locationInfos &location);
        void                                    setIndex(std::vector<std::string> words,serverInfo &server, locationInfos &location);
        void                                    setSize(std::vector<std::string> words, serverInfo &server);
        void                                    setErrorPage(std::vector<std::string> words, serverInfo &server);
        void                                    setautoIndex(std::vector<std::string> words, serverInfo &server, locationInfos &location);
        void                                    setRedirection(std::vector<std::string> words, serverInfo &server, locationInfos &location);
        void                                    setCGI(std::vector<std::string> words, locationInfos &location);
        void                                    setUpload(std::vector<std::string> words, locationInfos &location);
        unsigned int                            convertStrIPv4toUnsinedInt(const std::string& IPV4);
        bool                                    communPortSameName();
        void                                    defaultForMissingValues(serverInfo &server);
        void                                    defaultForMissingValues(locationInfos &location);
    public:
        configurationReader();
        configurationReader(std::string path);
        configurationReader(const configurationReader &obj);
        configurationReader& operator=(const configurationReader &obj);
        std::vector<serverInfo>                 getVirtualServer() const;
        void                                    parser();
        class  invalidSyntax: public std::exception 
        {
            public:
                const char* what() const throw();
        };
};

std::ostream&                                   operator<<(std::ostream & o, configurationReader const & rhs);

//! The root directive tells Nginx to take the request url and append it behind the specified directory.
//! Make sure teh braces are closed 
//! onkly server && location are acceoopted as context
//! if there is no directive listen by default the port is 80
//! listen describes all addresses and ports that should accept connections for the server
//! error page is inherited from the previous  onfiguration level if and only if there are no error_page directives defined on the current level.
#endif
