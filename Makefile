NAME = scop

CC = c++

INCLUDES_DIR = includes

CFLAGS = -Wall -Werror -Wextra -g3 -fsanitize=address -fsanitize=undefined -I./$(INCLUDES_DIR)

GLFLAGS = -lGLEW -lglfw -lGL -lX11 -lpthread -ldl

SRCS_DIR = srcs

SRCS =	$(SRCS_DIR)/main.cpp \
		$(SRCS_DIR)/parser/pars.cpp
		$(SRCS_DIR)/engine.cpp

OBJ_DIR = obj

OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

RM = rm -rf

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(CFLAGS) $(OBJS) $(GLFLAGS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re