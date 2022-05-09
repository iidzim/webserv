/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:45:25 by iidzim            #+#    #+#             */
/*   Updated: 2022/05/09 18:35:08 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Response.hpp"
#include "Request.hpp"
// #include "webserv.hpp"
#include <utility>
#include <map>

class Clients{

    public:
        std::map<int, std::pair<request, Response> > connections;

        Clients(void);
        Clients(int fd, request req, Response res);
        ~Clients(void);
        void remove_clients(int fd);
};

#endif