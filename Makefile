NAME := ft_ping
CC := clang

LIBFT_BIN := libft.a
LIBFT_DIR := libft_extended
LIBFT_INC := libft.h

OBJ_DIR := objs
SRC_DIR := srcs
INC_DIR := inc

CFLAGS := -Wall -Wextra -Werror -I$(LIBFT_DIR) -I$(INC_DIR) -fsanitize=address #-g3   
LDFLAGS := -L$(LIBFT_DIR) -fsanitize=address 
LDLIBS := -lft


SRCS := ft_ping.c \
	error_handling.c \
	interface.c \
	options.c \
	options_check.c \
	options_error.c \
	session.c \
	host.c \
	socket.c \
	icmp_datagram.c
	# verify_header.c \
	# receive.c \
	# print.c \
	# data.c \
	# signal_handler.c \
	# get_time.c \
	# host.c \
	# socket.c \
	# loop.c
OBJS := $(SRCS:%.c=%.o)

SRC_PATH := $(addprefix $(SRC_DIR)/,$(SRCS))
OBJ_PATH := $(addprefix $(OBJ_DIR)/,$(OBJS))
INC := $(LIBFT_DIR)/$(LIBFT_INC) \
$(INC_DIR)/ft_ping.h \
$(INC_DIR)/ft_opt.h

.PHONY: all clean fclean re 

########################################################
## To check leaks and bugs :
#valgrind --leak-check=full --show-leak-kinds=all -s

all : $(NAME)

$(NAME) : $(LIBFT_DIR)/$(LIBFT_BIN) $(OBJ_PATH)
	$(CC)  $(OBJ_PATH) $(LDFLAGS) $(LDLIBS)  -o $@
	chmod +x $(NAME)

$(LIBFT_DIR)/$(LIBFT_BIN) :
	$(MAKE) -C $(LIBFT_DIR)
	
# Rebuild if the header changed
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<
	
$(OBJ_DIR):
	mkdir -p $@

########################################################


clean:
	$(RM) $(OBJ_PATH)
	$(MAKE) -C $(LIBFT_DIR) clean
fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
re: fclean all