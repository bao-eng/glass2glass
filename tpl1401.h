#include "hardware/i2c.h"
#define TPL1401_DELAY 100

typedef struct {
  uint8_t regaddr;
  uint16_t regdata;
  size_t delay;
} i2cparams;

i2cparams tpl1401_opamp_init[] = {{0xD1, 0x01E0, TPL1401_DELAY},
                                  {0x21, 0x0000, TPL1401_DELAY},
                                  {0xD3, 0x0010, TPL1401_DELAY}};

void tpl1401_reg_write(i2c_inst_t *i2c, uint8_t addr, i2cparams data) {
  uint8_t buf[2];
  buf[2] = (data.regdata >> 0) & 0xFF;
  buf[1] = (data.regdata >> 8) & 0xFF;
  buf[0] = data.regaddr;
  i2c_write_blocking(i2c, addr, buf, 3, false);
  if (data.delay) {
    sleep_ms(data.delay);
  }
}

uint16_t tpl1401_reg_read(i2c_inst_t *i2c, uint8_t addr, uint8_t regaddr) {
  uint8_t buf[2];
  i2c_write_blocking(i2c, addr, &regaddr, 1, true);
  i2c_read_blocking(i2c, addr, buf, 2, false);
  return (buf[0] << 8) | buf[1];
}

void tpl1401_init(i2c_inst_t *i2c, uint8_t addr) {
  for (size_t i = 0; i < 3; i++) {
    tpl1401_reg_write(i2c, addr, tpl1401_opamp_init[i]);
  }
}

void tpl1401_set_threshold(i2c_inst_t *i2c, uint8_t addr, uint8_t threshold) {
  i2cparams data = {0x21, ((uint16_t)threshold) << 4, 0};
  tpl1401_reg_write(i2c, addr, data);
}
