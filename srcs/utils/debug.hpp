#ifndef UTILS_DEBUG_HPP_
#define UTILS_DEBUG_HPP_

#include "color.hpp"
#include <iostream>
#include <string>

namespace utils {

template <typename T>
void Debug(const T &x) {
	std::cerr << COLOR_GLAY << x << COLOR_RESET << std::endl;
}

template <typename T>
void Debug(const std::string &title, const T &x) {
	std::cerr << COLOR_GLAY << "[" << title << "] " << x << COLOR_RESET << std::endl;
}

template <typename T, typename U>
void Debug(const std::string &title, const T &x, const U &y) {
	std::cerr << COLOR_GLAY << "[" << title << "] " << x << "(" << y << ")" << COLOR_RESET
			  << std::endl;
}

} // namespace utils

#endif /* UTILS_DEBUG_HPP_ */
