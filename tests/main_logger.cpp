//
// Created by Simon Cros on 1/4/25.
//

#include <thread>

#include "logger.hpp"

#ifdef LIBFTPP_LOGGER

int main()
{
    LOG_DEBUG("I wonder how");
    LOG_INFO("I wonder why");
    LOG_WARNING("Yesterday you told me 'bout the blue blue sky");
    LOG_ERROR("And all that I can see is just another lemon tree");

    // The output may be mixed between stdout and stderr on CLion "Run" panel, but I want the lyrics to be in order
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    tscout() << std::endl;

    LOG_LEVEL(Debug);
    LOG_DEBUG("I'm a beautiful debug line!");
    LOG_LEVEL(Error);
    LOG_INFO("Can you hear me?");
    LOG_WARNING("Can you hear me?");
    LOG_ERROR("Can you hear me?");

    // The output may be mixed between stdout and stderr on CLion "Run" panel, but I want a perfect output
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    tscout() << std::endl;

    const Logger otherLogger(Info);
    otherLogger.info("I'm another logger allowing for info logs");

    return 0;
}

#else
#error LIBFTPP_LOGGER is not available
#endif
