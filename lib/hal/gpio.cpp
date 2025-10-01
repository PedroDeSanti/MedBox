/**
 * @file
 */

#include "gpio.hpp"

namespace hal {
Gpio::Gpio(const Config& config) : pin{config.pin}, mode{config.mode} {
    pinMode(this->pin, this->mode);
}

bool Gpio::read() const {
    return digitalRead(this->pin) == HIGH;
}

void Gpio::write(bool state) {
    digitalWrite(this->pin, state ? HIGH : LOW);
}

void Gpio::toggle() {
    digitalWrite(this->pin, !this->read() ? HIGH : LOW);
}
}  // namespace hal
