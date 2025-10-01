/**
 * @file
 */

#ifndef HAL_PWM_HPP
#define HAL_PWM_HPP

#include <cstdint>
#include <Arduino.h>

namespace hal {
/**
 * @brief Class to handle PWM functionality on Arduino-compatible microcontrollers.
 */
class Pwm {
public:
    /**
     * @brief PWM configuration struct.
     */
    struct Config {
        void (*init_function)() = nullptr;
        uint8_t  pin;
        uint32_t frequency = 1000;  // Default 1kHz
    };

    /**
     * @brief Construct a new Pwm object.
     *
     * @param config Configuration for the PWM.
     */
    explicit Pwm(const Config& config);

    /**
     * @brief Set the PWM duty cycle.
     *
     * @param duty_cycle Duty cycle value in percentage (0.0 - 100.0).
     */
    void set_duty_cycle(float duty_cycle);

    /**
     * @brief Set the PWM frequency.
     *
     * @note Frequency control availability depends on the Arduino platform.
     * Some platforms may have limited frequency control or may require
     * platform-specific implementation.
     *
     * @param frequency Frequency value in Hz.
     */
    void set_frequency(uint32_t frequency);

private:
    /**
     * @brief PWM pin number.
     */
    uint8_t pin;

    /**
     * @brief Current PWM frequency.
     */
    uint32_t current_frequency;

    /**
     * @brief Current duty cycle (0.0 - 100.0).
     */
    float current_duty_cycle;
};
}  // namespace hal

#endif  // HAL_PWM_HPP
