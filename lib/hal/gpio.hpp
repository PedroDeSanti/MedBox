/**
 * @file
 */

#ifndef HAL_GPIO_HPP
#define HAL_GPIO_HPP

#include <Arduino.h>

namespace hal {
/**
 * @brief Class to handle GPIO pins.
 */
class Gpio {
public:
    /**
     * @brief Configuration struct for GPIO pin.
     */
    struct Config {
        uint8_t pin;
        uint8_t mode;
    };

    /**
     * @brief Construct a new Gpio object.
     *
     * @param config Configuration for the GPIO pin.
     */
    explicit Gpio(const Config& config);

    /**
     * @brief Read the current state of the GPIO pin.
     *
     * @return True if the current state of the GPIO pin is high, false otherwise.
     */
    bool read() const;

    /**
     * @brief Write a new state to the GPIO pin.
     *
     * @param pin_state The state to be written (true for high, false for low).
     */
    void write(bool state);

    /**
     * @brief Toggle the state of the GPIO pin.
     */
    void toggle();

private:
    /**
     * @brief The pin number of the GPIO.
     */
    uint8_t pin;

    /**
     * @brief The mode of the GPIO.
     */
    uint8_t mode;
};
}  // namespace hal

#endif  // HAL_GPIO_HPP
