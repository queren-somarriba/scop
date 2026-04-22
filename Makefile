NAME         = scop
CC           = c++

DEBUG        = 1

RED     = \033[1;31m
GREEN   = \033[1;32m
BLUE    = \033[1;34m
ORANGE  = \033[1;33m
RESET   = \033[0m

ifeq ($(DEBUG), 1)
    CXXFLAGS    = -Wall -Wextra -Werror -g -march=native -fsanitize=address -fno-omit-frame-pointer
    LDFLAGS     = -fsanitize=address
else
    CXXFLAGS    = -Wall -Wextra -Werror -O3 -march=native
    LDFLAGS     = 
endif

INCLUDES_DIR = includes
OBJ_DIR      = obj
SRCS_DIR     = srcs

GLFLAGS      = -lglfw -lGL -lX11 -lpthread -ldl -lXrandr -lXinerama -lXcursor -lXi

SRCS         = $(SRCS_DIR)/main.cpp \
               $(SRCS_DIR)/glad.cpp \
               $(SRCS_DIR)/VBO.cpp \
               $(SRCS_DIR)/EBO.cpp \
               $(SRCS_DIR)/VAO.cpp \
               $(SRCS_DIR)/pars.cpp \
               $(SRCS_DIR)/engine.cpp \
               $(SRCS_DIR)/shader.cpp \
               $(SRCS_DIR)/texture.cpp \
               $(SRCS_DIR)/camera.cpp \
               $(SRCS_DIR)/utils.cpp

OBJS         = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(BLUE)Compiling $<...$(RESET)"
	$(CC) $(CXXFLAGS) -I$(INCLUDES_DIR) -c $< -o $@

$(NAME): $(OBJS)
	@echo "$(BLUE)Linking $(NAME)...$(RESET)"
	@$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) $(GLFLAGS)
	@echo "$(GREEN)Done! Binary $(NAME) is ready.$(RESET)"

clean:
	@echo "$(ORANGE)Cleaning objects...$(RESET)"
	rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Full cleaning $(NAME)...$(RESET)"
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re