#include "../includes/configurationReader.hpp"

configurationReader::configurationReader(){}

configurationReader::configurationReader(std::string path):_path(path), _state(CLOSED) {}

configurationReader::configurationReader(const configurationReader& obj):_path(obj._path), _virtualServer(obj._virtualServer), _state(obj._state)
{}

configurationReader& configurationReader::operator=(const configurationReader &obj)
{
    _path = obj._path;
    _virtualServer = obj._virtualServer;
    _state = obj._state;
    return *this;
}
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
    //!I should accept only 127.0.0.1
    if (words.size() != 3 || _state != INSIDESERVER || words[2].empty())
        throw configurationReader::invalidSyntax();
    //std::cout<<"|"<<server.host<<"|"<<std::endl;
    if (server.host != 0) //duplication
        throw configurationReader::invalidSyntax();
    server.host=convertStrIPv4toUnsinedInt(words[1]);
    //check if words[2] isNumber
    for (size_t i = 0; i< words[2].size(); i++)
    {
        if (!std::isdigit(words[2][i]))
            throw configurationReader::invalidSyntax();
    }
    try{
        server.port = stoi(words[2]);
    }catch(std::exception &e){throw configurationReader::invalidSyntax();}//! catch exception invalid argument
    if (server.port < 0)
        throw configurationReader::invalidSyntax();
}

void configurationReader::setServerName(std::vector<std::string> words, serverInfo& server)
{
    if (words.size() != 2 || words[1].empty() ||  _state != INSIDESERVER)
        throw configurationReader::invalidSyntax();
    // for (size_t i = 1; i < words.size(); i++)
    // {
    //     if (words[i].empty())
    //         continue;
    //     server.serverName.push_back(words[i]);
    // }
     if (!server.serverName.empty())
         throw configurationReader::invalidSyntax();
    server.serverName = words[1];
}

void configurationReader::setIndex(std::vector<std::string> words, serverInfo& server, locationInfos& location)
{
    // if (words.size() != 2 || _state != INLOCATION)
    //     throw configurationReader::invalidSyntax();
    // location.index = words[1];
    if (words.size() < 2 || _state == CLOSED)
        throw configurationReader::invalidSyntax();
    if (_state == INLOCATION)
    {
        for (size_t i = 1; i < words.size(); i++)
        {
            if (words[i].empty())
                continue;
            location.index.push_back(words[i]);
        }
        if (location.index.empty())
            throw configurationReader::invalidSyntax();
    }
    else if (_state == INSIDESERVER)
    {
        for (size_t i = 1; i < words.size(); i++)
        {
            if (words[i].empty())
                continue;
            server.index.push_back(words[i]);
        }
        if (server.index.empty())
            throw configurationReader::invalidSyntax();
    }

}

void configurationReader::setAllowedMethods(std::vector<std::string> words, locationInfos& location)
{
    if (words.size() <= 1 || _state != INLOCATION)
        throw configurationReader::invalidSyntax();
    for (size_t i = 1; i < words.size(); i++)
    {
        if (words[i].empty())
            continue;
        location.allow_methods.push_back(words[i]);
    }
    if (location.allow_methods.empty())
        throw configurationReader::invalidSyntax();
}

void configurationReader::setRoot(std::vector<std::string> words, serverInfo& server, locationInfos &location)
{
    if (words.size() != 2 || _state == CLOSED || words[1].empty())
        throw configurationReader::invalidSyntax();
    if (_state == INSIDESERVER && !server.root.size())
        server.root = words[1];
    else if (_state == INLOCATION && !location.root.size())
        location.root = words[1];
    else
         throw configurationReader::invalidSyntax();
}

void configurationReader::setSize(std::vector<std::string> words,serverInfo &server)
{
    if (words.size() != 2 || words[1].empty() || server.size != 0 || _state != INSIDESERVER)
        throw configurationReader::invalidSyntax();
     for (size_t i = 0; i< words[1].size(); i++)
    {
        if (!std::isdigit(words[1][i]))
            throw configurationReader::invalidSyntax();
    }
    server.size = stoi(words[1]);
}

void configurationReader::setErrorPage(std::vector<std::string> words, serverInfo &server, locationInfos &location)
{
    if (words.size() != 3 || _state == CLOSED || words[2].empty())
        throw configurationReader::invalidSyntax();
    int statusCode;
    try{statusCode = stoi(words[1]);}catch(std::exception &e){throw invalidSyntax();}
    if (_state == INSIDESERVER)
        server.errorPage.insert(std::make_pair(statusCode, words[2]));
    else if (_state == INLOCATION)
        location.errorPage.insert(std::make_pair(statusCode, words[2]));
}

void configurationReader::setautoIndex(std::vector<std::string> words, serverInfo &server, locationInfos &location)
{
    if (words.size() != 2 || _state == CLOSED || words[1] != "on")
        throw configurationReader::invalidSyntax();
    if (_state == INLOCATION && location.autoindex == OFF)
        location.autoindex = ON;
    else if (_state == INSIDESERVER && server.autoindex == OFF)
         server.autoindex =  ON;
    else
        throw configurationReader::invalidSyntax();
}

void configurationReader::setRedirection(std::vector<std::string> words, serverInfo &server, locationInfos &location)
{
    //! if duplicated it takes the last value into consideration
    if (words.size() != 3 || _state == CLOSED || words[1].empty() || words[2].empty())
        throw configurationReader::invalidSyntax();
    if (_state == INSIDESERVER)
    {
        server.redirect.first = words[1];
        server.redirect.second = words[2];
    }
    else if (_state == INLOCATION)
    {
        location.redirect.first = words[1];
        location.redirect.second = words[2];
    }
}
void   configurationReader::setCGI(std::vector<std::string> words, locationInfos &location)
{
    if (words.size() != 3 || _state != INLOCATION || words[1].empty() || words[2].empty())
        throw configurationReader::invalidSyntax();
    if (!location.cgi.first.empty())
        throw configurationReader::invalidSyntax();
    location.cgi.first=words[1];
    location.cgi.second=words[2];
}

void clearLocation(locationInfos & location)
{
    location.index.clear();
    location.root = "";
    location.uri = "";
    location.autoindex = OFF;
    location.cgi.first.clear();
    location.cgi.second.clear();
    location.redirect.first.clear();
    location.redirect.second.clear();
    location.allow_methods.clear();
    location.errorPage.clear();
}

void resetServer(serverInfo & server)
{
    server.port = -1;
    server.host = 0;
    server.root = "";
    server.size = 0;
    server.autoindex = OFF;
    server.redirect.first.clear();
    server.redirect.second.clear();
    server.errorPage.clear();
    server.serverName.clear();
    server.index.clear();
    server.location.clear();
    
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

void configurationReader::defaultForMissingValues(serverInfo &server)
{
    if (server.port == -1 && server.host == 0)
    {
        server.port = 8080;
        server.host = convertStrIPv4toUnsinedInt("127.0.0.1");
    }
    if (server.size == 0)
        server.size = 1000000;
}

void configurationReader::defaultForMissingValues(locationInfos &location)
{
    if (location.allow_methods.empty())
        location.allow_methods.push_back("GET");
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
                }
                else if (words[0] == "}" && words.size() == 1)
                {
                    if (_state == INSIDESERVER)
                    {
                        //! check if there is any missing mandatory directive => listen => default values !!
                        defaultForMissingValues(server);
                        _virtualServer.push_back(server);
                        resetServer(server);
                        _state = CLOSED;
                    }
                    else if (_state == INLOCATION)
                    {
                        _state = INSIDESERVER;
                        //defalut values for allow_methods
                        defaultForMissingValues(location);
                        server.location.push_back(location);
                        clearLocation(location);
                    }
                    else if (_state == CLOSED)
                        throw configurationReader::invalidSyntax();
                }
                else if (words[0] == "listen")
                    setPortHost(words, server);
                else if (words[0] == "server_name")
                    setServerName(words, server);
                else if (words[0] == "index")
                    setIndex(words, server, location);
                else if (words[0] == "root")
                    setRoot(words, server, location);
                else if (words[0] == "client_max_body_size")
                    setSize(words, server);
                else if (words[0] == "location")
                {
                    if (words.size() != 3 || words[2] != "{" || _state != INSIDESERVER)
                        throw configurationReader::invalidSyntax();
                    location.uri = words[1];
                    _state = INLOCATION;
                }  
                else if (words[0] == "error_page")
                    setErrorPage(words, server, location);
                else if (words[0] == "autoindex")
                    setautoIndex(words, server, location);
                else if (words[0] == "allow_methods")
                    setAllowedMethods(words, location);
                else if (words[0] == "rewrite")
                    setRedirection(words, server, location);
                else if (words[0] == "cgi")
                    setCGI(words, location);
                else
                    throw configurationReader::invalidSyntax();
            }
            if (_state == INSIDESERVER || _state == INLOCATION || communPortSameName()) //! check for duplicate port
                throw configurationReader::invalidSyntax();
        _infile.close();
}

const char * configurationReader::invalidSyntax::what()const throw()
{
    return "Syntax Error :: config file corrupted !";
}

bool configurationReader::communPortSameName()
{
    for (size_t i = 0; i < _virtualServer.size(); i++)
    {
        for (size_t j = i + 1; j < _virtualServer.size(); j++)
        {
            if (_virtualServer[i].port == _virtualServer[j].port && _virtualServer[i].serverName == _virtualServer[j].serverName)
                return true; //check server name
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
        o << "root          |"<<virtualServer[i].root<<"|"<<std::endl;
        o <<"size           "<<virtualServer[i].size<<std::endl;
        o <<"redirection    "<<virtualServer[i].redirect.first<<" "<<virtualServer[i].redirect.second<<std::endl;
        o << "Autoindex     ";
        if (virtualServer[i].autoindex)
            o<<"ON"<<std::endl;
        else
            o<<"OFF"<<std::endl;
        o << "Server Name   "<<virtualServer[i].serverName<<std::endl;
        // for (size_t j = 0; j < virtualServer[i].serverName.size(); j++)
        //     o << virtualServer[i].serverName[j] <<" ";
        // std::cout<<std::endl;
        o << "index    ";
        for (size_t j = 0; j < virtualServer[i].index.size(); j++)
            o<<virtualServer[i].index[j]<<" ";
        o <<std::endl;
        std::map<int, std::string>::iterator itb = virtualServer[i].errorPage.begin();
        std::map<int, std::string>::iterator ite = virtualServer[i].errorPage.end();
        o << "Error pages ";
        while (itb != ite)
        {
            o<<itb->first<<" "<<itb->second <<" | ";
            itb++;
        }
        o << std::endl;
        o << "[Location]    "<<std::endl;
         
        for (size_t k = 0; k < virtualServer[i].location.size(); k++)
        {
            o << "URI "<<virtualServer[i].location[k].uri <<std::endl;
            o <<"redirection    "<<virtualServer[i].location[k].redirect.first<<" "<<virtualServer[i].location[k].redirect.second<<std::endl;
            o <<"CGI    "<<virtualServer[i].location[k].cgi.first<<" "<<virtualServer[i].location[k].cgi.second<<std::endl;
            o << "root      "<<virtualServer[i].location[k].root<<std::endl;
            o << "Autoindex     ";
            if (virtualServer[i].location[k].autoindex)
                o<<"ON"<<std::endl;
             else
                o<<"OFF"<<std::endl;
            o << "Allowed methods ";
            for (size_t l = 0; l < virtualServer[i].location[k].allow_methods.size(); l++)
            {
                o << virtualServer[i].location[k].allow_methods[l] << " | ";
            }
            o<< std::endl;
            o <<"index ";
            for (size_t l = 0; l < virtualServer[i].location[k].index.size(); l++)
            {
                o << virtualServer[i].location[k].index[l] << " | ";
            }
            o<<std::endl;
            std::map<int, std::string>::iterator itb = virtualServer[i].location[k].errorPage.begin();
            std::map<int, std::string>::iterator ite = virtualServer[i].location[k].errorPage.end();
            o << "Error pages ";
            while (itb != ite)
            {
                o<<itb->first<<" "<<itb->second <<" | ";
                itb++;
            }
            }

        }
    return o;
}
//TODO
//remove duplication
//missing dirctive
//Default values


