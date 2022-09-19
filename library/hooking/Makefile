TARGET			= hooking.so

CC					= gcc

SRC			= hooking.c

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -fPIC -shared -o $(TARGET) $(SRCS) -ldl

fclean :
	rm -rf $(TARGET)

re : fclean all


.PHONY : all fclean re
