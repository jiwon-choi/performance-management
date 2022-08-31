TARGET			= agent

CC					= gcc -fsanitize=address
CFLAGS			= -Wall -Werror -Wextra
RM					= rm -rf

INCS_DIR		= ./incs/
SRCS_DIR		= ./srcs/
OBJS_DIR		= ./objs/

SRC			= agent.c \
					parse.c \
					queue.c

SRCS		= $(addprefix $(SRCS_DIR), $(SRC))
OBJS		= $(addprefix $(OBJS_DIR), $(SRC:.c=.o))

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) -I $(INCS_DIR) -c $< -o $@

clean :
	$(RM) $(OBJS_DIR)

fclean : clean
	$(RM) $(TARGET)

re : clean all


.PHONY : all clean fclean re
