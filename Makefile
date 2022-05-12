# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/28 14:17:05 by iidzim            #+#    #+#              #
#    Updated: 2022/05/12 09:37:59 by iidzim           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = web_server
C = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 #-g -Ofast
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