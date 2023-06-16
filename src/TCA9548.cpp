#include "TCA9548.hpp"

TCA9548::TCA9548(i2c_inst_t* i2c_port, uint8_t i2c_address) {
    this->i2c_port = i2c_port;
    this->i2c_address = i2c_address;
    reset();
}

void TCA9548::enable_channel(uint8_t channel) {
    this->channel = channel;
    uint8_t mask;
    if (channel == 0) {
        mask = TCA_CHANNEL_0;
    } else if (channel == 1) {
        mask = TCA_CHANNEL_1;
    } else if (channel == 2) {
        mask = TCA_CHANNEL_2;
    } else if (channel == 3) {
        mask = TCA_CHANNEL_3;
    } else if (channel == 4) {
        mask = TCA_CHANNEL_4;
    } else if (channel == 5) {
        mask = TCA_CHANNEL_5;
    } else if (channel == 6) {
        mask = TCA_CHANNEL_6;
    } else if (channel == 7) {
        mask = TCA_CHANNEL_7;
    }
    i2c_write_blocking(i2c_port, i2c_address, &mask, 1, false);
}

uint8_t TCA9548::enabled_channel() {
    return this->channel;
}
void TCA9548::reset() {
    uint8_t reset = TCA_RESET;
    i2c_write_blocking(i2c_port, i2c_address, &reset, 1, false);
    sleep_ms(50);
}