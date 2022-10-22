NAME := ft_ping
CC := clang

LIBFT_BIN := libft.a
LIBFT_DIR := libft_extended
LIBFT_INC := libft.h


CFLAGS := -fsanitize=address -g3 -Wall -Wextra -Werror -I$(LIBFT_DIR) 
LDFLAGS := -L$(LIBFT_DIR)  -fsanitize=address -g3
LDLIBS := -lft

SRCS := ft_ping.c \
	icmp_datagram.c \
	icmp_checksum.c \
	error_handling.c \
	verify_header.c \
	receive.c \
	print.c \
	data.c \
	signal_handler.c \
	get_time.c \
	host.c \
	options.c \
	socket.c
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