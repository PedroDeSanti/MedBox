/**
 * @file
 */

#include <cmath>
#include <algorithm>

#include "servo.hpp"

namespace proxy {
Servo::Servo(const Config& config) :
    pwm{config.pwm_config}, max_angle{config.max_angle}, current_angle{config.initial_position} {
    this->set_angle(config.initial_position);
}

void Servo::set_angle(float angle) {
    // Clamp angle to valid range
    angle = std::clamp(angle, 0.0f, this->max_angle);
    this->current_angle = angle;

    // Convert angle to duty cycle and set PWM
    const float duty_cycle = this->angle_to_duty_cycle(angle);
    this->pwm.set_duty_cycle(duty_cycle);
}

float Servo::get_angle() const {
    return this->current_angle;
}

float Servo::get_max_angle() const {
    return this->max_angle;
}

void Servo::center() {
    this->set_angle(this->max_angle / 2.0f);
}

void Servo::move_to_min() {
    this->set_angle(0.0f);
}

void Servo::move_to_max() {
    this->set_angle(this->max_angle);
}

float Servo::angle_to_duty_cycle(float angle) const {
    // Linear mapping from angle to duty cycle
    // 0 degrees -> 5% duty cycle
    // max_angle degrees -> 10% duty cycle

    const float normalized_angle = angle / this->max_angle;  // 0.0 to 1.0
    const float duty_cycle = MIN_DUTY_CYCLE + (normalized_angle * (MAX_DUTY_CYCLE - MIN_DUTY_CYCLE));

    return duty_cycle;
}
}  // namespace proxy
