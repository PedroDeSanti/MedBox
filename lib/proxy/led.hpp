/**
 * @file
 */

#ifndef LED_HPP
#define LED_HPP

#include "gpio.hpp"

namespace proxy {
/**
 * @brief Class for controlling an LED.
 */
class Led {
public:
    /**
     * @brief Configuration struct for LED.
     */
    struct Config {
        hal::Gpio::Config gpio;
    };

    /**
     * @brief Construct a new Led object.
     *
     * @param config Configuration for the LED.
     */
    explicit Led(const Config& config);

    /**
     * @brief Turn the LED on.
     */
    void turn_on();

    /**
     * @brief Turn the LED off.
     */
    void turn_off();

    /**
     * @brief Toggle the LED.
     */
    void toggle();

private:
    /**
     * @brief Gpio object for the LED.
     */
    hal::Gpio gpio;
};
}  // namespace proxy

#endif  // LED_HPP
