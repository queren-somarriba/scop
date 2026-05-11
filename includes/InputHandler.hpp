#pragma once

#include <glad/glad.h>
#include "scop.hpp"

void handleControls(GLFWwindow *window, AppState& state);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);