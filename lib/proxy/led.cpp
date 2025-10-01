/**
 * @file
 */

#include "led.hpp"

namespace proxy {
Led::Led(const Config& config) : gpio{config.gpio} { }

void Led::turn_on() {
    this->gpio.write(true);
}

void Led::turn_off() {
    this->gpio.write(false);
}

void Led::toggle() {
    this->gpio.toggle();
}
}  // namespace proxy
