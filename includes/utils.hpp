#pragma once

#include <string>
#include <filesystem>

bool isValidFile(const std::string& filePath);

std::string trim(const std::string& str);

std::string parentDir(const std::string& path);