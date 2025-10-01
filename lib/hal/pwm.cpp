/**
 * @file
 */

#include <cmath>

#include "pwm.hpp"

namespace hal {
Pwm::Pwm(const Config& config) : pin{config.pin}, current_frequency{config.frequency}, current_duty_cycle{0.0f} {
    // Call initialization function if provided
    if (config.init_function != nullptr) {
        config.init_function();
    }

    // Set pin as output for PWM
    pinMode(this->pin, OUTPUT);

    // Initialize PWM with 0% duty cycle
    analogWrite(this->pin, 0);

    // Set initial frequency if supported by platform
    this->set_frequency(config.frequency);
}

void Pwm::set_duty_cycle(float duty_cycle) {
    // Clamp duty cycle to valid range
    duty_cycle = constrain(duty_cycle, 0.0f, 100.0f);
    this->current_duty_cycle = duty_cycle;

    // Convert percentage to 8-bit PWM value (0-255)
    const uint8_t pwm_value = static_cast<uint8_t>(std::lround(duty_cycle * 255.0f / 100.0f));

    analogWrite(this->pin, pwm_value);
}

void Pwm::set_frequency(uint32_t frequency) {
    this->current_frequency = frequency;

    // Note: Arduino's analogWrite() uses a fixed frequency (usually ~490Hz or ~980Hz)
    // For true frequency control, platform-specific code would be needed
    // This is a placeholder that stores the frequency for future use

    // On some platforms like ESP32, you could use:
    // ledcSetup(channel, frequency, resolution);
    // ledcAttachPin(pin, channel);

    // For now, we just update the duty cycle to maintain the current setting
    this->set_duty_cycle(this->current_duty_cycle);
}
}  // namespace hal
