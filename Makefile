# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/28 14:17:05 by iidzim            #+#    #+#              #
#    Updated: 2022/04/28 02:35:35 by iidzim           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = web_server
C = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g
SRC = main.cpp srcs/*.cpp

all: $(NAME)

$(NAME): $(SRC)
	@ $(C) $(FLAGS) $(SRC) -o $(NAME)
clean:

fclean: clean
	@ rm -f $(NAME)
	@rm -f body*
re: fclean all
	@ ./web_server ./configFile/valid_confg/default.conf

.PHONY: all clean fclean re