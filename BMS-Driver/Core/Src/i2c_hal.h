/*
 * i2c_hal.h
 *
 * Created on: Oct 7, 2023
 * Author: mdurr
 */

#ifndef SRC_I2C_HAL_H_
#define SRC_I2C_HAL_H_

#include <inttypes.h>
#include "stm32h7xx_hal.h"

#define BUFFER_LENGTH 32

typedef struct {
    uint8_t rxBuffer[BUFFER_LENGTH];
    uint8_t rxBufferIndex;
    uint8_t rxBufferLength;

    uint8_t txAddress;
    uint8_t txBuffer[BUFFER_LENGTH];
    uint8_t txBufferIndex;
    uint8_t txBufferLength;

    uint8_t transmitting;
    void (*setWriteError)();
    I2C_HandleTypeDef *hi2c;
} I2C_HAL;

void I2C_HAL_begin(I2C_HAL *i2c_hal, I2C_HandleTypeDef *hi2c);
void I2C_HAL_begin_with_address(I2C_HAL *i2c_hal, uint8_t address);
void I2C_HAL_begin_with_frequency(I2C_HAL *i2c_hal, int frequency);
void I2C_HAL_setClock(I2C_HAL *i2c_hal, uint32_t clock);
void I2C_HAL_beginTransmission(I2C_HAL *i2c_hal, uint8_t address);
void I2C_HAL_beginTransmission_with_address(I2C_HAL *i2c_hal, int address);
uint8_t I2C_HAL_endTransmission(I2C_HAL *i2c_hal);
uint8_t I2C_HAL_endTransmission_with_stop(I2C_HAL *i2c_hal, uint8_t stop);
uint8_t I2C_HAL_requestFrom(I2C_HAL *i2c_hal, uint8_t address, uint8_t quantity);
uint8_t I2C_HAL_requestFrom_with_stop(I2C_HAL *i2c_hal, uint8_t address, uint8_t quantity, uint8_t stop);
uint8_t I2C_HAL_requestFrom_with_address(I2C_HAL *i2c_hal, int address, int quantity);
uint8_t I2C_HAL_requestFrom_with_address_and_stop(I2C_HAL *i2c_hal, int address, int quantity, int stop);
size_t I2C_HAL_write(I2C_HAL *i2c_hal, uint8_t data);
size_t I2C_HAL_write_array(I2C_HAL *i2c_hal, const uint8_t *data, size_t quantity);
int I2C_HAL_available(I2C_HAL *i2c_hal);
int I2C_HAL_read(I2C_HAL *i2c_hal);

#endif /* SRC_I2C_HAL_H_ */
