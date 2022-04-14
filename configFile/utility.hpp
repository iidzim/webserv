#ifndef UTILITY_HPP
# define UTILITY_HPP
# define GET 0
# define POST 1
# define DELETE 2



typedef struct t_location
{
    //acceptedMethods
    //bool autoindex
}               Location;
typedef struct t_server
{
    //std::string port
    //std::string host
    //std::vector<std::string> serverNames
    //errorPages code(int) URI(std::string)
    //std::string clientBodySize => digits && m
    //index std::string => only one default page
    //return keyword for redirection 
    //bool autoindex => by default off
    //std::string root => to make available to all locations 

}               server;

//simple directive || block directive
//simple directive => name => parametrs separated by space ;
//consider comments that starts with #
//accepted methods => inside location block
//location block define how requests are proccessed for different URI and ressources
//more than one space between directive_name and parameters are not tolerated
#endif


//rules
