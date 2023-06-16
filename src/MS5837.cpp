#include "MS5837.hpp"

MS5837::MS5837(i2c_inst_t* i2c_port) {
    this->i2c_port = i2c_port;
    reset();
    read_prom_values();
    sleep_ms(50);
}

MS5837::MS5837(i2c_inst_t* i2c_port, TCA9548* mux, uint8_t mux_channel) {
    this->i2c_port = i2c_port;
    this->mux = mux;
    this->mux_channel = mux_channel;
    reset();
    read_prom_values();
}

void MS5837::select_mux_channel() {
    if (mux != NULL && mux->enabled_channel() != mux_channel) {
        mux->enable_channel(mux_channel);
    }
}

void MS5837::reset() {
    select_mux_channel();
    uint8_t reset_cmd = MS5837_RESET;
    i2c_write_blocking(i2c_port, MS5837_ADDR, &reset_cmd, 1, false);
    sleep_ms(50);
}

void MS5837::read_prom_values(int timeout_ms) {
    select_mux_channel();
    absolute_time_t timeout_time = make_timeout_time_ms(timeout_ms);
    bool timeout = false;
    while (!calibrated && !timeout) {
        // Load calibration data
        _prom_C0 = i2c_read16(MS5837_ADDR, MS5837_PROM_C0);
        _prom_C1 = i2c_read16(MS5837_ADDR, MS5837_PROM_C1);
        _prom_C2 = i2c_read16(MS5837_ADDR, MS5837_PROM_C2);
        _prom_C3 = i2c_read16(MS5837_ADDR, MS5837_PROM_C3);
        _prom_C4 = i2c_read16(MS5837_ADDR, MS5837_PROM_C4);
        _prom_C5 = i2c_read16(MS5837_ADDR, MS5837_PROM_C5);
        _prom_C6 = i2c_read16(MS5837_ADDR, MS5837_PROM_C6);
        if (_prom_C0 != 0 && _prom_C1 != 0 && _prom_C2 != 0 && _prom_C3 != 0 && _prom_C4 != 0 && _prom_C5 != 0 && _prom_C6 != 0) {
            calibrated = true;
        }
        // Check timeout.
        if (absolute_time_min(get_absolute_time(), timeout_time)) {
            timeout = true;
        }
    }
}

bool MS5837::read_temperature() {
    if (!calibrated) {
        return false;
    } else {
        // Select mux channel.
        select_mux_channel();

        // Read ADC.
        uint8_t cmd_D2 = temp_osr;
        i2c_write_blocking(i2c_port, MS5837_ADDR, &cmd_D2, 1, true);
        sleep_ms(50);
        uint8_t cmd_ADC = MS5837_ADC_READ;
        i2c_write_blocking(i2c_port, MS5837_ADDR, &cmd_ADC, 1, true);
        sleep_ms(50);
        uint8_t buffer_D2[3];
        i2c_read_blocking(i2c_port, MS5837_ADDR, buffer_D2, 3, false);
        int32_t D2 = (buffer_D2[0] << 16) | (buffer_D2[1] << 8) | buffer_D2[2];

        // Calculate temperature.
        this->d_T = D2 - (_prom_C5 * pow(2, 8));
        temperature = (2000 + (d_T * _prom_C6 / pow(2, 23))) / 100.0;
        return true;
    }
}

bool MS5837::read_pressure() {
    if (!calibrated) {
        return false;
    } else {
        // Select mux channel.
        select_mux_channel();

        // Read ADC.
        uint8_t cmd_D1 = press_osr;
        i2c_write_blocking(i2c_port, MS5837_ADDR, &cmd_D1, 1, true);
        sleep_ms(50);
        uint8_t cmd_ADC = MS5837_ADC_READ;
        i2c_write_blocking(i2c_port, MS5837_ADDR, &cmd_ADC, 1, true);
        sleep_ms(50);
        uint8_t buffer_D1[3];
        i2c_read_blocking(i2c_port, MS5837_ADDR, buffer_D1, 3, false);
        int32_t D1 = (buffer_D1[0] << 16) | (buffer_D1[1] << 8) | buffer_D1[2];

        // Calculate temperature compensated pressure.
        int64_t OFF_COM = _prom_C2 * pow(2, 17) + (_prom_C4 * d_T) / pow(2, 6);
        int64_t SENS_COM = _prom_C1 * pow(2, 16) + (_prom_C3 * d_T) / pow(2, 7);
        compensated_pressure = ((D1 * SENS_COM / pow(2, 21) - OFF_COM) / pow(2, 15)) / 1000.0;
        int32_t d_T_0 = (_prom_C5 * pow(2, 8));
        
        // Calculate uncompensated pressure.
        int64_t OFF_UN = _prom_C2 * pow(2, 17) + (_prom_C4 * d_T_0) / pow(2, 6);
        int64_t SENS_UN = _prom_C1 * pow(2, 16) + (_prom_C3 * d_T_0) / pow(2, 7);
        uncompensated_pressure = ((D1 * SENS_UN / pow(2, 21) - OFF_UN) / pow(2, 15)) / 1000.0;
        return true;
    }
}

bool MS5837::read_values() {
    if (read_temperature() && read_pressure()) {
        return true;
    } else {
        return false;
    }
}

float MS5837::get_temperature() {
    return temperature;
}

float MS5837::get_uncompensated_pressure() {
    return uncompensated_pressure;
}

float MS5837::get_compensated_pressure() {
    return compensated_pressure;
}

uint16_t MS5837::i2c_read16(uint8_t address, uint8_t reg) {
    select_mux_channel();
    uint8_t buffer[2];
    i2c_write_blocking(i2c_port, address, &reg, 1, true);
    sleep_ms(50);
    int ret;
    ret = i2c_read_blocking(i2c_port, address, buffer, 2, false);
    if (ret != 2) {
        // Hasn't returned the corect number of bytes. Device not found.
        return 0;
    }
    sleep_ms(50);
    uint16_t value = (buffer[0] << 8) | buffer[1];
    return value;
}

void MS5837::set_temperature_osr(int level) {
    switch(level) {
        case 1:
            temp_osr = MS5837_TEMP_OSR_256;
            break;
        case 2:
            temp_osr = MS5837_TEMP_OSR_512;
            break;
        case 3:
            temp_osr = MS5837_TEMP_OSR_1024;
            break;
        case 4:
            temp_osr = MS5837_TEMP_OSR_2048;
            break;
        case 5:
            temp_osr = MS5837_TEMP_OSR_4096;
            break;
        case 6:
            temp_osr = MS5837_TEMP_OSR_8192;
            break;
        default:
            temp_osr = MS5837_TEMP_OSR_256;
    }
}

void MS5837::set_pressure_osr(int level) {
    switch(level) {
        case 1:
            press_osr = MS5837_PRESS_OSR_256;
            break;
        case 2:
            press_osr = MS5837_PRESS_OSR_512;
            break;
        case 3:
            press_osr = MS5837_PRESS_OSR_1024;
            break;
        case 4:
            press_osr = MS5837_PRESS_OSR_2048;
            break;
        case 5:
            press_osr = MS5837_PRESS_OSR_4096;
            break;
        case 6:
            press_osr = MS5837_PRESS_OSR_8192;
            break;
        default:
            press_osr = MS5837_PRESS_OSR_256;
    }
}