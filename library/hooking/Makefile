TARGET			= hooking.so

CC					= gcc
CFLAGS			= -Wall -Werror -Wextra

INCLUDES		= -I./incs/ -I../utils/incs/
LIBRARYS		= -L ../utils/ -lutils

SRC			= hooking.c

SRCS		= $(SRC)
OBJS		= $(SRC:.c=.o)

all : $(TARGET)

$(TARGET) : $(OBJS)
	make -C ../utils
	$(CC) -fPIC -shared -o $(TARGET) $(OBJS) $(LIBRARYS) -ldl

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

clean :
	make -C ../utils clean
	rm -rf $(OBJS)

fclean : clean
	make -C ../utils fclean
	rm -rf $(TARGET)

re : clean all


.PHONY : all clean fclean re
