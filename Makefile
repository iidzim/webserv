# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/28 14:17:05 by iidzim            #+#    #+#              #
#    Updated: 2022/04/19 01:26:39 by iidzim           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = web_server
C = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address
SRC = main.cpp srcs/*.cpp

all: $(NAME)

$(NAME): $(SRC)
	@ $(C) $(FLAGS) $(SRC) -o $(NAME)
clean:

fclean: clean
	@ rm -f $(NAME)
	@rm -f body*
re: fclean all
	@ ./web_server server.conf

.PHONY: all clean fclean re