#include "utils.hpp"
#include <filesystem>

bool isValidFile(const std::string& filePath)
{
	std::filesystem::path p(filePath);
	return std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
}

std::string trim(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return {};
	size_t end = str.find_last_not_of(" \t\r\n");
	
	return str.substr(start, end - start + 1);
}

std::string parentDir(const std::string& path)
{
	auto p = std::filesystem::path(path).parent_path();

	return p.empty() ? "./" : (p.string() + "/");
}