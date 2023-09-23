#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "tpl1401.h"

#define ADDR 0x48

int main() {
    const uint led_pin = 14;
    const uint comp_pin = 22;
    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    gpio_init(comp_pin);
    gpio_set_dir(comp_pin, GPIO_IN);

    // Enable UART so we can print status output
    stdio_init_all();
    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(8, GPIO_FUNC_I2C);
    gpio_set_function(9, GPIO_FUNC_I2C);
    gpio_pull_up(8);
    gpio_pull_up(9);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(8, 9, GPIO_FUNC_I2C));

    tpl1401_init(i2c_default, ADDR);
    tpl1401_set_threshold(i2c_default, ADDR, 127);
    {
        uint8_t reg = 0x21;
        uint8_t buf[2];
        i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
        i2c_read_blocking(i2c_default, ADDR, buf, 2, false);  // false - finished with bus
        printf("%#X %#X\n",buf[0], buf[1]);
    }

    while (true) {
        // Blink LED
        printf("Blinking!\r\n");
        gpio_put(led_pin, true);
        sleep_ms(500);
        gpio_put(led_pin, false);
        sleep_ms(500);
    }
    return 0;
}
