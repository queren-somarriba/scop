#pragma once

#include <glad/glad.h>
#include "scop.hpp"

constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;


GLFWwindow* initWindow();

void		renderOBJ(GLFWwindow* window, scopData& data);