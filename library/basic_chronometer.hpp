//
// Created by Simon Cros on 12/31/24.
//

#ifndef BASIC_CHRONOMETER_HPP
#define BASIC_CHRONOMETER_HPP

#include "chronometer_base.hpp"

class BasicChronometer : public ChronometerBase {
public:
    using ChronometerBase::reset;
    using ChronometerBase::startOrResume;
    using ChronometerBase::pause;
};

#endif //BASIC_CHRONOMETER_HPP
