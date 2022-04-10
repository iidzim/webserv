/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:50:38 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/10 17:40:44 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

class Request{

	private:
		int _is_complete;

	public:
		Request(): _is_complete(1) {}
		void parse(char *buffer){
			(void)buffer;
		}
		int is_complete(){
			return _is_complete;
		}
		~Request(){}
};

#endif