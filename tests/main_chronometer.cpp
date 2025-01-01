//
// Created by Simon Cros on 29/12/2024.
//

#include "chronometer.hpp"

#include <thread>
#include <cassert>
#include <iostream>

int main()
{
    using namespace std::literals::chrono_literals;

    auto toSec = [](const Chronometer::duration_type &dur)
    {
        return std::chrono::duration_cast<std::chrono::duration<float>>(dur);
    };

    Chronometer c;

    assert(c.laps().empty());
    assert(!c.currentLap().has_value());
    assert(!c.lap().has_value());

    assert(c.running() == false);
    assert(c.paused() == false);

    assert(c.pause() == false);

    assert(c.startOrResume() == true);
    assert(c.startOrResume() == false);
    std::this_thread::sleep_for(0.5s);
    assert(c.currentLap().has_value());

    {
        const auto e_lap = c.lap();
        assert(e_lap.has_value());
        assert(e_lap->lapDuration == e_lap->totalDuration);
        assert(e_lap->totalDuration >= 0.5s && e_lap->totalDuration < 0.6s);
        std::cout << "Lap in " << toSec(e_lap->lapDuration) << " after " << toSec(e_lap->totalDuration) << std::endl;
        assert(c.laps().size() == 1);
        assert(c.laps().back() == *e_lap);
    }

    assert(c.startOrResume() == false);
    std::this_thread::sleep_for(0.2s);

    {
        const auto e_lap = c.lap();
        assert(e_lap.has_value());
        assert(e_lap->lapDuration >= 0.2s && e_lap->lapDuration < 0.3s);
        assert(e_lap->totalDuration >= 0.7s && e_lap->totalDuration < 0.8s);
        std::cout << "Lap in " << toSec(e_lap->lapDuration) << " after " << toSec(e_lap->totalDuration) << std::endl;
        assert(c.laps().size() == 2);
        assert(c.laps().back() == *e_lap);
    }

    std::this_thread::sleep_for(0.3s);
    assert(c.pause() == true);
    assert(c.paused() == true);
    assert(c.running() == false);
    assert(c.pause() == false);
    std::this_thread::sleep_for(0.2s);

    {
        const auto e_lap = c.lap();
        assert(e_lap.has_value());
        assert(e_lap->lapDuration >= 0.3s && e_lap->lapDuration < 0.4s);
        assert(e_lap->totalDuration >= 1s && e_lap->totalDuration < 1.1s);
        std::cout << "Lap in " << toSec(e_lap->lapDuration) << " after " << toSec(e_lap->totalDuration) << std::endl;
        assert(c.laps().size() == 3);
        assert(c.laps().back() == *e_lap);
    }

    c.reset();
    assert(c.laps().empty());
    assert(!c.currentLap().has_value());
    assert(!c.lap().has_value());

    assert(c.running() == false);
    assert(c.paused() == false);

    assert(c.pause() == false);

    return 0;
}

