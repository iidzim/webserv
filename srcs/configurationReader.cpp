#include "../includes/configurationReader.hpp"

configurationReader::configurationReader(){}

configurationReader::configurationReader(std::string path):_path(path), _state(CLOSED) {}

//* listen describes all addresses and ports that should accept connections for the server
//! if it's missing port by default is 80

unsigned int    configurationReader::convertStrIPv4toUnsinedInt(const std::string& IPV4)
{
    //! inet_pton() //convert IPV4 from text to binary form
    //! inet_aton() //convert HOST FROM ipv4 NUMBERS IN DOTS NOTATION INTO BINARY FORM

    unsigned int addr;
    if (IPV4 != "127.0.0.1")
        throw configurationReader::invalidSyntax();
    inet_pton(AF_INET, IPV4.c_str(), &addr);

    return htonl(addr);
}

void configurationReader::setPortHost(std::vector<std::string> words, serverInfo &server)
{
    //! Maybe I should accept only 127.0.0.1
    if (words.size() != 3 || _state != INSIDESERVER)
        throw configurationReader::invalidSyntax();
    server.host= convertStrIPv4toUnsinedInt(words[1]);
    //check if words[2] isNumber
    for (size_t i = 0; i< words[2].size(); i++)
    {
        if (!std::isdigit(words[2][i]))
            throw configurationReader::invalidSyntax();
    }
    server.port = stoi(words[2]); //! catch exception invalid argumenet
}

void configurationReader::setServerName(std::vector<std::string> words, serverInfo& server)
{
    if (_state != INSIDESERVER)
        throw configurationReader::invalidSyntax();
    for (size_t i = 1; i < words.size(); i++)
    {
        if (words[i].empty())
            continue;
        server.serverName.push_back(words[i]);
    }
}

void configurationReader::setIndex(std::vector<std::string> words, locationInfos& location)
{
    if (words.size() != 2 || _state != INLOCATION)
        throw configurationReader::invalidSyntax();
    location.index = words[1];
}

void configurationReader::setRoot(std::vector<std::string> words, serverInfo& server, locationInfos &location)
{
    if (words.size() != 2 || _state == CLOSED)
        throw configurationReader::invalidSyntax();
    if (_state == INSIDESERVER)
        server.root = words[1];
    else if (_state == INLOCATION)
        location.root = words[1];
}

void configurationReader::setSize(std::vector<std::string> words,serverInfo &server)
{
    if (words.size() != 2)
        throw configurationReader::invalidSyntax();
     for (size_t i = 0; i< words[1].size(); i++)
    {
        if (!std::isdigit(words[1][i]))
            throw configurationReader::invalidSyntax();
    }
    server.size = stoi(words[1]);
}

void configurationReader::setErrorPage(std::vector<std::string> words, serverInfo &server)
{
    if (words.size() != 3 || _state != INSIDESERVER)
        throw configurationReader::invalidSyntax();
    server.errorPage.first = words[1];
    server.errorPage.second = words[2]; 
}

void configurationReader::setautoIndex(std::vector<std::string> words, serverInfo &server)
{
    if (words.size() != 2 || _state != INSIDESERVER)
        throw configurationReader::invalidSyntax();
    if (words[1] == "on")
        server.autoindex = ON;
    else if (words[1] == "off")
        server.autoindex = OFF;
    else
        throw configurationReader::invalidSyntax();
}

void clearLocation(locationInfos & location)
{
    location.index = "";
    location.root = "";
}

void resetServer(serverInfo & server)
{
    server.port = 0;
    server.host = 0;
    server.root = "";
    server.size = 0;
    server.errorPage.first = "";
    server.errorPage.second = "";
    server.autoindex = OFF;
    server.serverName.clear();
    for (size_t i = 0; i < server.location.size(); i++)
        clearLocation(server.location[i]);
}

bool configurationReader::isCommentOrEmptyLine(std::string & line)
{
    while (isspace(line[0]))
        line.erase(0, 1);
    if (line.empty() || line[0] == '#')
        return true;
    return false;  
}

bool configurationReader::isValidPath()
{
    _infile.open(_path);
    if (!_infile)
        throw configurationReader::invalidSyntax();
    return true;
}

std::vector<std::string> configurationReader::splitbySpace(std::string& line)
{
    std::vector<std::string> words;
    size_t pos = 0;

    while ((pos=line.find(" ")) != std::string::npos)
    {
        words.push_back(line.substr(0, pos));
        line.erase(0, pos+1);
    }
    words.push_back(line);

    return words;
}

void configurationReader::parser()
{
        std::string     line;

        isValidPath();
        serverInfo      server;
        resetServer(server);
        locationInfos   location;
        while (std::getline(_infile, line))
        {    
            if (isCommentOrEmptyLine(line))
                 continue;
            std::vector<std::string> words = splitbySpace(line);
            if (words[0] == "server")
            {
                if (words[1] != "{" || words.size() != 2 || _state == INSIDESERVER)
                        throw configurationReader::invalidSyntax();
                    _state = INSIDESERVER;
                    
                 //   server.port = "80"; //default port
                  //  server.host = "0.0.0.0"; //default host
                }
                else if (words[0] == "}" && words.size() == 1)
                {
                    if (_state == INSIDESERVER)
                    {
                        //! check if there is any missing mandatory directive
                        _virtualServer.push_back(server);
                        resetServer(server);
                        _state = CLOSED;
                    }
                    else if (_state == INLOCATION)
                    {
                        _state = INSIDESERVER;
                        
                        //! check if there is any missing mandatory directive
                        server.location.push_back(location);
                        clearLocation(location);
                    }
                }
                else if (words[0] == "listen")
                    setPortHost(words, server);
                else if (words[0] == "server_name")
                    setServerName(words, server);
                else if (words[0] == "index")
                    setIndex(words, location);
                else if (words[0] == "root")
                    setRoot(words, server, location);
                else if (words[0] == "client_max_body_size")
                    setSize(words, server);
                else if (words[0] == "location")
                {
                    if (words.size() != 3 || words[2] != "{" || _state != INSIDESERVER)
                        throw configurationReader::invalidSyntax();
                    _state = INLOCATION;
                }  
                else if (words[0] == "error_page")
                    setErrorPage(words, server);
                else if (words[0] == "autoindex")
                    setautoIndex(words, server);
                else
                    throw configurationReader::invalidSyntax();
            }
            if (_state == INSIDESERVER || _state == INLOCATION || hasDuplicatePort()) //! check for duplicate port
                throw configurationReader::invalidSyntax();
        _infile.close();
}

const char * configurationReader::invalidSyntax::what()const throw()
{
    return "Syntax Error :: config file corrupted !";
}

bool configurationReader::hasDuplicatePort()
{
    for (size_t i = 0; i < _virtualServer.size(); i++)
    {
        for (size_t j = i + 1; j < _virtualServer.size(); j++)
        {
            if (_virtualServer[i].port == _virtualServer[j].port)
                return true;
        }
    }
    return false;
}

std::vector<serverInfo>  configurationReader::getVirtualServer() const
{
    return _virtualServer;
}

std::ostream& operator<<(std::ostream& o, configurationReader const & rhs)
{
    std::vector<serverInfo> virtualServer = rhs.getVirtualServer();

    for (size_t i = 0; i < virtualServer.size(); i++)
    {
        o << "-------server " << i << " -------"<<std::endl;
        o << "Port          "<<virtualServer[i].port<<std::endl;
        o <<"Host           "<<virtualServer[i].host<<std::endl;
        o << "root          "<<virtualServer[i].root<<std::endl;
        o <<"size           "<<virtualServer[i].size<<std::endl;
        o <<"Error page     "<<virtualServer[i].errorPage.first<<" "<<virtualServer[i].errorPage.second<<std::endl;
        o << "Autoindex     ";
        if (virtualServer[i].autoindex)
            o<<"ON"<<std::endl;
        else
            o<<"OFF"<<std::endl;
        o << "Server Name   ";
        for (size_t j = 0; j < virtualServer[i].serverName.size(); j++)
            o << virtualServer[i].serverName[j] <<" ";
        std::cout<<std::endl;
        o << "[Location]    "<<std::endl;
        for (size_t k = 0; k < virtualServer[i].location.size(); k++)
        {      
            o << "index     "<<virtualServer[i].location[k].index<<std::endl;
            o << "root      "<<virtualServer[i].location[k].root<<std::endl;
        }
    }
    return o;
}
//TODO
//remove duplication
//missing dirctive
//Default values


