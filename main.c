#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "tpl1401.h"

#define ADDR 0x48
#define LED_PIN 14
#define COMP_PIN 22
#define STATUS_PIN 16

bool repeating_timer_callback(struct repeating_timer *t) {
    gpio_put(LED_PIN, !gpio_get(LED_PIN));
    return true;
}

void gpio_callback(uint gpio, uint32_t events) {
    if(events == GPIO_IRQ_EDGE_RISE){
        gpio_put(STATUS_PIN, true);
        detected_timestamp = to_us_since_boot(get_absolute_time());
    }
    if(events == GPIO_IRQ_EDGE_FALL){
        gpio_put(STATUS_PIN, false);
    }
}

int main() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(STATUS_PIN);
    gpio_set_dir(STATUS_PIN, GPIO_OUT);
    gpio_init(COMP_PIN);
    gpio_set_dir(COMP_PIN, GPIO_IN);

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

    gpio_set_irq_enabled_with_callback(COMP_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    tpl1401_init(i2c_default, ADDR);
    tpl1401_set_threshold(i2c_default, ADDR, 127);
    printf("%#x\n", tpl1401_reg_read(i2c_default, ADDR, 0x21));

    struct repeating_timer timer;
    add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);

    while (true) {
        tight_loop_contents();
    }
    return 0;
}
