# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: viet <viet@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/28 14:17:05 by iidzim            #+#    #+#              #
#    Updated: 2022/05/12 00:16:56 by viet             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = web_server
C = c++
FLAGS =  -std=c++98 #-Ofast
SRC = main.cpp srcs/*.cpp

all: $(NAME)

$(NAME): $(SRC)
	@ $(C) $(FLAGS) $(SRC) -o $(NAME)
clean:

fclean: clean
	@ rm -f $(NAME)
	@ cd var/www/html/bodies && rm -f body*
re: fclean all
	@ ./web_server ./configFile/valid_confg/default.conf

.PHONY: all clean fclean re