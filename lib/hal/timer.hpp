/**
 * @file
 */

#ifndef HAL_TIMER_HPP
#define HAL_TIMER_HPP

#include <cstdint>
#include <Arduino.h>

namespace hal {
/**
 * @brief Class to handle timer functionality on Arduino-compatible microcontrollers.
 */
class Timer {
public:
    /**
     * @brief Timer configuration struct.
     */
    struct Config {
        bool enable_microseconds = true;
    };

    /**
     * @brief Construct a new Timer object.
     */
    Timer() = default;

    /**
     * @brief Construct a new Timer object.
     *
     * @param config Configuration for the timer.
     */
    explicit Timer(const Config& config);

    /**
     * @brief Get the current timer counter.
     *
     * @return Current timer counter in milliseconds.
     */
    static uint32_t get_counter_ms();

    /**
     * @brief Get the current timer counter.
     *
     * @return Current timer counter in microseconds.
     */
    uint32_t get_counter_us() const;

private:
    /**
     * @brief Flag to enable microseconds.
     */
    bool enable_microseconds{true};
};
}  // namespace hal

#endif  // HAL_TIMER_HPP
