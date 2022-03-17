/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 10:15:33 by iidzim            #+#    #+#             */
/*   Updated: 2022/03/17 18:28:09 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void){

    int socket_fd, r, port = 8080;
    char buffer[1024] = {0};
    struct sockaddr_in address;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("creat socket failed");
        return (0);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    //! convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(address.sin_family, "127.0.0.1", &address.sin_addr) < 0){
        perror("invalid/not supported address");
        return (0);
    }
    if (connect(socket_fd, (struct sockaddr *) &address, sizeof(address)) < 0){
        perror("connect failed");
        return (0);
    }
    // char *hi = "hello from client";
    // send(socket_fd, hi, strlen(hi), 0);
    // send(socket_fd, "000", 3, 0);
    r = recv(socket_fd, buffer, sizeof(buffer), 0);
    std::cout << buffer << std::endl;

    return (0);
}