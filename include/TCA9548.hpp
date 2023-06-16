#ifndef TCA9548_H
#define TCA9548_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define TCA_RESET 0x00
#define TCA_CHANNEL_0 0x01
#define TCA_CHANNEL_1 0x02
#define TCA_CHANNEL_2 0x04
#define TCA_CHANNEL_3 0x08
#define TCA_CHANNEL_4 0x10
#define TCA_CHANNEL_5 0x20
#define TCA_CHANNEL_6 0x40
#define TCA_CHANNEL_7 0x80

class TCA9548 {
    public:
        TCA9548(i2c_inst_t* i2c_port, uint8_t i2c_address);
        void enable_channel(uint8_t channel);
        uint8_t enabled_channel();
        void reset();
    private:
        i2c_inst_t* i2c_port;
        uint8_t i2c_address;
        uint8_t channel;
};

#endif