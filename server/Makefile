TARGET			= server

CC					= gcc
CFLAGS			= -Wall -Werror -Wextra
RM					= rm -rf

INCS_DIR		= ./incs/
SRCS_DIR		= ./srcs/
OBJS_DIR		= ./objs/

INCLUDES		= -I./incs/ -I../library/incs/ -I/usr/include/mysql
LIBRARYS		= -L../library/ -lutils -L/usr/lib/mysql -lmysqlclient

SRC			= save.c \
					server.c \
					socket.c

SRCS		= $(addprefix $(SRCS_DIR), $(SRC))
OBJS		= $(addprefix $(OBJS_DIR), $(SRC:.c=.o))

all : $(TARGET)

$(TARGET) : $(OBJS)
	make -C ../library
	$(CC) -o $(TARGET) $(OBJS) $(LIBRARYS)

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

clean :
	make -C ../library clean
	$(RM) $(OBJS_DIR)

fclean : clean
	make -C ../library fclean
	$(RM) $(TARGET)

re : clean all


.PHONY : all clean fclean re
