/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:50:38 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/19 01:04:41 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

class Request{

	private:
		int _is_complete;
		// std::string _headers; //= start_line + headers
        // int _body_file; //= file descriptor of the body file
		// int _status_code;
		// int fd;

	public:
		Request(): _is_complete(1) {}
		Request(int fd): _is_complete(1) {(void)fd;}
		void parse(char *buffer, int r){
			(void)buffer;
			(void)r;
		}
		int is_complete(){
			return _is_complete;
		}
		// int get_fd(){
		// 	return fd;
		// }
		~Request(){}
};

#endif