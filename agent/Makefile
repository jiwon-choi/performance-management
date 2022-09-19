TARGET			= agent

CC					= gcc
CFLAGS			= -Wall -Werror -Wextra
RM					= rm -rf

INCS_DIR		= ./incs/
SRCS_DIR		= ./srcs/
OBJS_DIR		= ./objs/

INCLUDES		= -I./incs/ -I../library/utils/incs/
LIBRARYS		= -L ../library/utils/ -lutils

SRC			= agent.c \
					socket.c \
					parse.c

SRCS		= $(addprefix $(SRCS_DIR), $(SRC))
OBJS		= $(addprefix $(OBJS_DIR), $(SRC:.c=.o))

all : $(TARGET)

$(TARGET) : $(OBJS)
	make -C ../library/utils
	make -C ../library/hooking
	$(CC) -o $(TARGET) $(OBJS) $(LIBRARYS)

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

clean :
	make -C ../library/utils clean
	$(RM) $(OBJS_DIR)

fclean : clean
	make -C ../library/utils fclean
	make -C ../library/hooking fclean
	$(RM) $(TARGET)

re : clean all


.PHONY : all clean fclean re
