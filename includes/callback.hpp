#pragma once

#include <glad/glad.h>
#include "scop.hpp"

void processInput(GLFWwindow *window, AppState& state);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);