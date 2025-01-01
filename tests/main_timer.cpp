//
// Created by Simon Cros on 29/12/2024.
//

#include "timer.hpp"

#include <thread>
#include <cassert>
#include <iostream>

int main()
{
    using namespace std::literals::chrono_literals;

    Timer t{1s};

    assert(t.running() == false);
    assert(t.paused() == false);

    assert(t.pause() == false);

    std::cout << "Setup : ok" << std::endl;

    assert(t.startOrResume() == true);
    assert(t.startOrResume() == false);
    std::this_thread::sleep_for(0.5s);
    assert(t.timedOut() == false);
    assert(t.running() == true);
    assert(t.paused() == false);
    assert(t.elapsedTime() >= 0.5s && t.elapsedTime() <= 0.6s);
    assert(t.remainingTime() <= 0.5s && t.elapsedTime() >= 0.4s);

    std::cout << "Running after 0.5s : ok" << std::endl;

    std::this_thread::sleep_for(0.5s);
    assert(t.timedOut() == true);
    assert(t.elapsedTime() >= 1.0s && t.elapsedTime() <= 1.1s);
    assert(t.remainingTime() <= Timer::duration_type::zero());

    std::cout << "Running after 1s : ok" << std::endl;

    t.reset();
    assert(t.running() == false);
    assert(t.paused() == false);
    assert(t.pause() == false);

    std::cout << "Reset : ok" << std::endl;

    t.setDuration(500ms);
    assert(t.duration() == 500ms);

    std::cout << "Update duration : ok" << std::endl;

    assert(t.startOrResume() == true);
    assert(t.startOrResume() == false);
    std::this_thread::sleep_for(300ms);
    assert(t.timedOut() == false);
    assert(t.pause() == true);
    assert(t.pause() == false);
    const auto elapsedBeforeSleep = t.elapsedTime();
    const auto remainingBeforeSleep = t.remainingTime();
    assert(elapsedBeforeSleep + remainingBeforeSleep == t.duration());

    std::cout << "Paused after 300ms : ok" << std::endl;

    std::this_thread::sleep_for(300ms);
    assert(t.timedOut() == false);
    assert(t.pause() == false);
    assert(t.paused() == true);
    assert(t.running() == false);
    assert(elapsedBeforeSleep == t.elapsedTime());
    assert(remainingBeforeSleep == t.remainingTime());

    std::cout << "Paused after 300ms + 300ms sleep : ok" << std::endl;

    assert(t.startOrResume() == true);
    assert(t.startOrResume() == false);
    assert(t.running() == true);
    assert(t.paused() == false);
    std::this_thread::sleep_for(200ms);
    t.pause();
    assert(t.timedOut() == true);
    assert(t.elapsedTime() >= 500ms && t.elapsedTime() <= 600ms);
    assert(t.remainingTime() <= Timer::duration_type::zero());
    assert(t.elapsedTime() + t.remainingTime() == t.duration());

    std::cout << "Paused after 500ms : ok" << std::endl;

    return 0;
}

