TARGET	= libutils.a

CC			= gcc
CFLAGS	= -Wall -Wextra -Werror

INCS_DIR    = ./incs/
SRCS_DIR    = ./srcs/
OBJS_DIR    = ./objs/

SRC			= log.c \
					queue.c \
					sig.c

SRCS    = $(addprefix $(SRCS_DIR), $(SRC))
OBJS    = $(addprefix $(OBJS_DIR), $(SRC:.c=.o))

all : $(TARGET) hooking

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) -I $(INCS_DIR) -c $< -o $@

$(TARGET) : $(OBJS)
	ar rcs $@ $?

hooking :
	$(CC) -fPIC -shared -o hooking.so hooking.c -ldl

clean :
	rm -rf $(OBJS_DIR)

fclean : clean
	rm -rf $(TARGET) hooking.so

re : clean all

.PHONY : all clean fclean re
