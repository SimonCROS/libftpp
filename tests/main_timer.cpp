//
// Created by Simon Cros on 29/12/2024.
//

#include <thread>
#include <timer.hpp>

#include <cassert>
#include <iostream>

int main()
{
    using namespace std::literals::chrono_literals;

    Timer t{1s};

    std::this_thread::sleep_for(0.5s);
    std::cout << (!t.timed_out() ? "Ok" : "Err") << std::endl;

    std::this_thread::sleep_for(0.5s);
    std::cout << (t.timed_out() ? "Ok" : "Err") << std::endl;

    t.reset();
    std::cout << (!t.timed_out() ? "Ok" : "Err") << std::endl;

    std::this_thread::sleep_for(0.5s);
    t.set_duration(400ms);
    std::cout << (t.timed_out() ? "Ok" : "Err") << std::endl;

    t.set_duration(1min);
    std::cout << (!t.timed_out() ? "Ok" : "Err") << std::endl;

    return 0;
}

