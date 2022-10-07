NAME := ft_ping
CC := clang

LIBFT_BIN := libft.a
LIBFT_DIR := libft_extended
LIBFT_INC := libft.h

CFLAGS := -Wall -Wextra -Werror -I$(LIBFT_DIR)
LDFLAGS := -L$(LIBFT_DIR)  
#LDLIBS := -lft

SRCS := ft_ping.c
OBJS := $(SRCS:%.c=%.o)
INC := $(LIBFT_DIR)/$(LIBFT_INC) ft_ping.h

.PHONY: all clean fclean re

########################################################

all : $(NAME)

$(NAME) : $(LIBFT_DIR)/$(LIBFT_BIN) $(OBJS)

$(LIBFT_DIR)/$(LIBFT_BIN) :
	$(MAKE) -C $(LIBFT_DIR)
	
########################################################

clean:
	$(RM) $(OBJS)
	$(MAKE) -C $(LIBFT_DIR) clean
fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
re: fclean all