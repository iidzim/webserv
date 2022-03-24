/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 18:23:18 by iidzim            #+#    #+#             */
/*   Updated: 2022/03/24 20:46:02 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

int main(void){

    // ft::Socket a;
    ft::Socket b(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 5);
    std::cout << "end" << std::endl;
    return (0);
}