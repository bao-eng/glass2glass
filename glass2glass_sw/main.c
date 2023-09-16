#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

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

    {
        uint8_t reg = 0xD0;
        uint8_t buf[2];
        i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
        i2c_read_blocking(i2c_default, ADDR, buf, 2, false);  // false - finished with bus
        printf("%#X %#X\n",buf[0], buf[1]);
    }
    sleep_ms(100);
    {
        uint8_t reg = 0xD1;
        uint8_t buf[3];
        buf[2] = 0xE0;
        buf[1] = 0x01;
        buf[0] = reg;
        i2c_write_blocking(i2c_default, ADDR, buf, 3, false);
    }
    {
        uint8_t reg = 0xD1;
        uint8_t buf[2];
        i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
        i2c_read_blocking(i2c_default, ADDR, buf, 2, false);  // false - finished with bus
        printf("%#X %#X\n",buf[0], buf[1]);
    }
    sleep_ms(100);
    {
        uint8_t reg = 0x21;
        uint8_t buf[2];
        buf[2] = 0x00;
        buf[1] = 0xFF;
        buf[0] = reg;
        i2c_write_blocking(i2c_default, ADDR, buf, 3, false);
    }
    sleep_ms(100);
    {
        uint8_t reg = 0x21;
        uint8_t buf[2];
        i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
        i2c_read_blocking(i2c_default, ADDR, buf, 2, false);  // false - finished with bus
        printf("%#X %#X\n",buf[0], buf[1]);
    }
    {
        uint8_t reg = 0xD3;
        uint8_t buf[2];
        buf[2] = 0x10;
        buf[1] = 0x00;
        buf[0] = reg;
        i2c_write_blocking(i2c_default, ADDR, buf, 3, false);
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
