#pragma once

#include <thread>
#include <chrono>
#include <thread>

#include "say.hpp"

using namespace std::chrono_literals;

void f(char x, int i) {
    std::this_thread::sleep_for(1s);
    say("З набору {} виконано дію {}.", x, i);
}
