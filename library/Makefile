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

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) -I $(INCS_DIR) -c $< -o $@

$(TARGET) : $(OBJS)
	ar rcs $@ $?

all : $(TARGET)

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(TARGET)

re : clean all

.PHONY : all clean fclean re
