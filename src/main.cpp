// Варіант 2: схема 2 (C++23)

#include <array>
#include <iostream>
#include <latch>
#include <format>
#include <syncstream>
#include <thread>
#include <memory>
#include <functional>

#include "Task.hpp"
#include "say.hpp"
#include "f.hpp"

using namespace std;
using namespace std::chrono_literals;

constexpr int nt = 4; // nt=4
constexpr int sets = 13;
constexpr int per_thread = 17;

// a=4, b=8, c=6, d=6, e=7, f=7, g=6, h=7, i=5, j=9, k=3, m=3, n=2
constexpr array<int, sets> counts{4, 8, 6, 6, 7, 7, 6, 7, 5, 9, 3, 3, 2};
constexpr array<array<Task, per_thread>, nt> schedule{{
    {{{'a', 1}, {'b', 1}, {'b', 5}, {'c', 1}, {'c', 5}, {'g', 3}, {'d', 1}, {'d', 5}, {'e', 3}, {'e', 7}, {'h', 4}, {'i', 1}, {'i', 5}, {'j', 4}, {'j', 8}, {'k', 3}, {'f', 4}}},
    {{{'a', 2}, {'b', 2}, {'b', 6}, {'c', 2}, {'c', 6}, {'g', 4}, {'d', 2}, {'d', 6}, {'e', 4}, {'h', 1}, {'h', 5}, {'i', 2}, {'j', 1}, {'j', 5}, {'j', 9}, {'f', 1}, {'f', 5}}},
    {{{'a', 3}, {'b', 3}, {'b', 7}, {'c', 3}, {'g', 1}, {'g', 5}, {'d', 3}, {'e', 1}, {'e', 5}, {'h', 2}, {'h', 6}, {'i', 3}, {'j', 2}, {'j', 6}, {'k', 1}, {'f', 2}, {'f', 6}}},
    {{{'a', 4}, {'b', 4}, {'b', 8}, {'c', 4}, {'g', 2}, {'g', 6}, {'d', 4}, {'e', 2}, {'e', 6}, {'h', 3}, {'h', 7}, {'i', 4}, {'j', 3}, {'j', 7}, {'k', 2}, {'f', 3}, {'f', 7}}},
}};

constexpr int idx(char x) { return x - 'a'; }

constexpr bool schedule_ok() {
    auto countsTimer = counts;
    string symbols = "";

    for (const auto& row : schedule) {
        for (const Task& t : row) {
            auto id = idx(t.set);
            
            if (id > sets) return false;
            countsTimer[id]--;
        }
    }

    for (int i = 0; i < countsTimer.size(); i++) {
        if (countsTimer[i] != 0 && countsTimer[i] != counts[i]) {
            return false;
        }
    }

    return true;
}

static_assert(schedule_ok(), "Shedule isn't ok");

const string deps(char x) {
    switch (x) {
        case 'd': return "bc";
        case 'e': return "a";
        case 'f': return "hj";
        case 'g': return "a";
        case 'h': return "dg";
        case 'i': return "e";
        case 'j': return "e";
        case 'k': return "i";
        default:  return "";  // a, b, c
    }
}

void worker(int id, array<unique_ptr<latch>, sets>& done) {
    for (const auto& [x, i] : schedule[id]) {
        for (char d : deps(x)) done[idx(d)]->wait();
        f(x, i);
        done[idx(x)]->count_down();
    }
}

int main() {
    say("Обчислення розпочато.");

    const auto start = chrono::steady_clock::now();

    array<unique_ptr<latch>, sets> done;
    for (int i = 0; i < sets; i++) done[i] = make_unique<latch>(counts[i]);

    {
        array<jthread, nt> threads;
        for (int t = 0; t < nt; ++t) threads[t] = jthread{worker, t, ref(done)};
    }
    
    const chrono::duration<double> d = chrono::steady_clock::now() - start;
    say("Обчислення завершено. Тривалість обчислення {:.3f} секунд.", d.count());
}