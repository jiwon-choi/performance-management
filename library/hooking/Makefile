TARGET			= hooking.so

CC					= gcc
CFLAGS			= -Wall -Werror -Wextra

SRC			= hooking.c

SRCS		= $(SRC)
OBJS		= $(SRC:.c=.o)

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -fPIC -shared -o $(TARGET) $(OBJS) -ldl

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(TARGET)

re : clean all


.PHONY : all clean fclean re
