# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: sverschu </var/mail/sverschu>                +#+                      #
#                                                    +#+                       #
#    Created: 2019/10/30 16:10:10 by sverschu      #+#    #+#                  #
#    Updated: 2020/06/29 21:14:44 by sverschu      ########   odam.nl          #
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
ifndef NAME
	$(error NAME is not set mattie --->> run ./rolmops.sh <<---)
endif
	@$(MAKE) -C $(LIBASM_FOLDER) re

re: fclean all
