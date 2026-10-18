#pragma once

#include <format>
#include <iostream>
#include <syncstream>
#include <utility>

template <class... Args>
void say(std::format_string<Args...> fmt, Args&&... args) {
    std::osyncstream out{std::cout};
    out << std::format(fmt, std::forward<Args>(args)...) << '\n';
};
