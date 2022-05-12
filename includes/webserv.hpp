#ifndef WEBSERV_HPP
#define WEBSERV_HPP


#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <time.h>
#include <algorithm>
#include <stdlib.h>


#include "mimeTypes.hpp"
#include "Request.hpp"
#include "configurationReader.hpp"
#include "Response.hpp"
#include "autoIndex.hpp"
#include "cgi.hpp"
extern char **environ;
// #include "Socket.hpp"
// #include "Client.hpp"
// #include "Server.hpp"

static std::string pwd;

#endif