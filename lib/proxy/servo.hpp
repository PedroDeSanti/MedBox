/**
 * @file
 */

#ifndef PROXY_SERVO_HPP
#define PROXY_SERVO_HPP

#include <cstdint>

#include "pwm.hpp"

namespace proxy {
/**
 * @brief Class for controlling servo motors using PWM.
 */
class Servo {
public:
    /**
     * @brief Configuration struct for the servo.
     */
    struct Config {
        hal::Pwm::Config pwm_config;
        float            max_angle{180.0f};        // Maximum servo angle in degrees
        float            initial_position{90.0f};  // Initial servo position in degrees
    };

    /**
     * @brief Construct a new Servo object.
     *
     * @param config Servo configuration.
     */
    explicit Servo(const Config& config);

    /**
     * @brief Set the servo position.
     *
     * @param angle Desired angle in degrees (0 to max_angle).
     */
    void set_angle(float angle);

    /**
     * @brief Get the current servo position.
     *
     * @return Current servo angle in degrees.
     */
    float get_angle() const;

    /**
     * @brief Get the maximum servo angle.
     *
     * @return Maximum servo angle in degrees.
     */
    float get_max_angle() const;

    /**
     * @brief Move servo to center position.
     */
    void center();

    /**
     * @brief Move servo to minimum position (0 degrees).
     */
    void move_to_min();

    /**
     * @brief Move servo to maximum position.
     */
    void move_to_max();

private:
    /**
     * @brief Convert angle to PWM duty cycle percentage.
     *
     * @param angle Servo angle in degrees.
     * @return PWM duty cycle percentage (5.0% to 10.0%).
     */
    float angle_to_duty_cycle(float angle) const;

    /**
     * @brief PWM object for servo control.
     */
    hal::Pwm pwm;

    /**
     * @brief Maximum servo angle in degrees.
     */
    float max_angle;

    /**
     * @brief Current servo position in degrees.
     */
    float current_angle;

    /**
     * @brief Servo PWM frequency (50Hz for 20ms period).
     */
    static constexpr uint32_t SERVO_FREQUENCY = 50;

    /**
     * @brief Minimum duty cycle percentage (at 0 degrees).
     */
    static constexpr float MIN_DUTY_CYCLE = 5.0f;

    /**
     * @brief Maximum duty cycle percentage (at max angle).
     */
    static constexpr float MAX_DUTY_CYCLE = 10.0f;
};
}  // namespace proxy

#endif  // PROXY_SERVO_HPP
