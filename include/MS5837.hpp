#ifndef MS5837_H
#define MS5837_H

#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include <math.h>
#include <assert.h>
#include "TCA9548.hpp"


// MS5837 commands.
#define MS5837_ADDR 0x76

#define MS5837_RESET 0x1E
#define MS5837_PROM_C0 0xA0
#define MS5837_PROM_C1 0xA2
#define MS5837_PROM_C2 0xA4
#define MS5837_PROM_C3 0xA6
#define MS5837_PROM_C4 0xA8
#define MS5837_PROM_C5 0xAA
#define MS5837_PROM_C6 0xAC

#define MS5837_PRESS_OSR_256 0x40
#define MS5837_PRESS_OSR_512 0x42
#define MS5837_PRESS_OSR_1024 0x44
#define MS5837_PRESS_OSR_2048 0x46
#define MS5837_PRESS_OSR_4096 0x48
#define MS5837_PRESS_OSR_8192 0x4A

#define MS5837_TEMP_OSR_256 0x50
#define MS5837_TEMP_OSR_512 0x52
#define MS5837_TEMP_OSR_1024 0x54
#define MS5837_TEMP_OSR_2048 0x56
#define MS5837_TEMP_OSR_4096 0x58
#define MS5837_TEMP_OSR_8192 0x5A

#define MS5837_ADC_READ 0x00

class MS5837 {
    public:
        MS5837(i2c_inst_t* i2c_port);
        MS5837(i2c_inst_t* i2c_port, TCA9548* mux, uint8_t mux_channel);
        bool read_temperature();
        bool read_pressure();
        bool read_values();
        float get_temperature();
        float get_uncompensated_pressure();
        float get_compensated_pressure();
        void set_pressure_osr(int level);
        void set_temperature_osr(int level);
    private:
        void select_mux_channel();
        void reset();
        void read_prom_values(int timeout_ms=1000);
        uint16_t i2c_read16(uint8_t address, uint8_t reg);
        i2c_inst_t* i2c_port;
        TCA9548* mux = NULL;
        uint8_t mux_channel;
        uint8_t temp_osr = MS5837_TEMP_OSR_256;
        uint8_t press_osr = MS5837_PRESS_OSR_256;
        bool calibrated = false;
        uint16_t _prom_C0 = 0;
        uint16_t _prom_C1 = 0;
        uint16_t _prom_C2 = 0;
        uint16_t _prom_C3 = 0;
        uint16_t _prom_C4 = 0;
        uint16_t _prom_C5 = 0;
        uint16_t _prom_C6 = 0;
        int32_t D1, D2;
        int32_t d_T;
        float uncompensated_pressure, compensated_pressure, temperature;
};

#endif // MS5837_H
