#include <stdio.h>
#include <string>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "TCA9548.hpp"
#include "MS5837.hpp"

// TCA9548A addresses.
#define TCA_ADDR_0 0x70
#define TCA_ADDR_1 0x71

// I2C constants.
#define I2C_PORT_0 i2c0
#define I2C_PORT_1 i2c1
#define I2C_FREQ 200000

void read_sensor_data(MS5837& sensor, std::string name) {
    if (sensor.read_values()) {
        float temp = sensor.get_temperature();
        float comp_press = sensor.get_compensated_pressure();
        float uncomp_press = sensor.get_uncompensated_pressure();
        printf("Name: %s ; T: %f ; C: %f ; U: %f\n", name.c_str(), temp, comp_press, uncomp_press);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // Initialise i2c lines.
    i2c_init(I2C_PORT_0, I2C_FREQ);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_set_pulls(0, false, false);
    gpio_set_pulls(1, false, false);

    // Initialise i2c lines.
    i2c_init(I2C_PORT_1, I2C_FREQ);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_set_pulls(2, false, false);
    gpio_set_pulls(3, false, false);
    sleep_ms(50);
    
    // Create mux objects.
    TCA9548 mux_0_7(I2C_PORT_0, TCA_ADDR_0);
    TCA9548 mux_8_15(I2C_PORT_1, TCA_ADDR_1);
    
    // Creatse sensor objects.
    printf("Finding sensors...\n");
    MS5837 sensor0(I2C_PORT_0, &mux_0_7, 0);
    MS5837 sensor1(I2C_PORT_0, &mux_0_7, 1);
    MS5837 sensor2(I2C_PORT_0, &mux_0_7, 2);
    MS5837 sensor3(I2C_PORT_0, &mux_0_7, 3);
    MS5837 sensor4(I2C_PORT_0, &mux_0_7, 4);
    MS5837 sensor5(I2C_PORT_0, &mux_0_7, 5);
    MS5837 sensor6(I2C_PORT_0, &mux_0_7, 6);
    MS5837 sensor7(I2C_PORT_0, &mux_0_7, 7);
    MS5837 sensor8(I2C_PORT_1, &mux_8_15, 0);
    MS5837 sensor9(I2C_PORT_1, &mux_8_15, 1);
    MS5837 sensor10(I2C_PORT_1, &mux_8_15, 2);
    MS5837 sensor11(I2C_PORT_1, &mux_8_15, 3);
    MS5837 sensor12(I2C_PORT_1, &mux_8_15, 4);
    MS5837 sensor13(I2C_PORT_1, &mux_8_15, 5);
    MS5837 sensor14(I2C_PORT_1, &mux_8_15, 6);
    MS5837 sensor15(I2C_PORT_1, &mux_8_15, 7);

    printf("Reading sensor data...\n");
    while (true) {
        // Read data from sensors connected to each channel.
        read_sensor_data(sensor0, "sensor0");
        read_sensor_data(sensor1, "sensor1");
        read_sensor_data(sensor2, "sensor2");
        read_sensor_data(sensor3, "sensor3");
        read_sensor_data(sensor4, "sensor4");
        read_sensor_data(sensor5, "sensor5");
        read_sensor_data(sensor6, "sensor6");
        read_sensor_data(sensor7, "sensor7");
        read_sensor_data(sensor8, "sensor8");
        read_sensor_data(sensor9, "sensor9");
        read_sensor_data(sensor10, "sensor10");
        read_sensor_data(sensor11, "sensor11");
        read_sensor_data(sensor12, "sensor12");
        read_sensor_data(sensor13, "sensor13");
        read_sensor_data(sensor14, "sensor14");
        read_sensor_data(sensor15, "sensor15");
        printf("\n");
    }   

    return 0;
}