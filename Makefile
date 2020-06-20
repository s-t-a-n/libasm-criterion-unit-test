# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: sverschu </var/mail/sverschu>                +#+                      #
#                                                    +#+                       #
#    Created: 2019/10/30 16:10:10 by sverschu      #+#    #+#                  #
#    Updated: 2019/11/24 16:00:30 by sverschu      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

# C sourcefiles
SRC = criterion_libasm.c $(LIBASM_FOLDER)/libasm.a

# compiler and linker
CC = clang
CC_FLAGS = -Werror -Wextra -Wall -lcriterion

# commands
all: libasm $(NAME)

$(NAME): $(SRC)
	@$(CC) $(CC_FLAGS) $(EXT_FLAGS) -I$(LIBASM_INC) $(SRC) -o $(NAME)

clean:
	@$(RM) $(OBJ)

fclean: clean
	@$(RM) $(NAME)

libasm: 
	@$(MAKE) -C $(LIBASM_FOLDER) re

love: all
	@echo "Functions like split and trim are open to interpretation; your idea of these functions might be different than mine."
	@./$(NAME)
re: fclean all
