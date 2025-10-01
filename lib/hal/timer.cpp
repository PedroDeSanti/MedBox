/**
 * @file
 */

#include "timer.hpp"

namespace hal {
Timer::Timer(const Config& config) : enable_microseconds{config.enable_microseconds} { }

uint32_t Timer::get_counter_ms() {
    return millis();
}

uint32_t Timer::get_counter_us() const {
    if (this->enable_microseconds) {
        return micros();
    }

    return 1000UL * millis();
}
}  // namespace hal
