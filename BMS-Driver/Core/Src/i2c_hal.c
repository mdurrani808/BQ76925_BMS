/*
 * Wrapper for I2C communication on STM32
 * Copyright (c) 2011 BroLab. All rights reserved.
 * Author: Sergii Kriachko
 * Description: Source file of BQ76952 10-series multicell.
 */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "main.h"
#include "i2c_hal.h"

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
    I2C_HandleTypeDef *hi2c;
} I2C_HAL;

static int Timeout = 1000;

void I2C_HAL_begin(I2C_HAL *i2c_hal) {
    i2c_hal->rxBufferIndex = 0;
    i2c_hal->rxBufferLength = 0;

    i2c_hal->txBufferIndex = 0;
    i2c_hal->txBufferLength = 0;
}

void I2C_HAL_begin_with_address(I2C_HAL *i2c_hal, uint8_t address) {
    i2c_hal->txAddress = address;
}

uint8_t I2C_HAL_requestFrom(I2C_HAL *i2c_hal, uint8_t address, uint8_t quantity, uint8_t sendStop) {
    // Clamp to buffer length
    if (quantity > BUFFER_LENGTH) {
        quantity = BUFFER_LENGTH;
    }

    // Perform blocking read into buffer
    HAL_I2C_Master_Receive(i2c_hal->hi2c, address, i2c_hal->rxBuffer, quantity, Timeout);

    // Set rx buffer iterator vars
    i2c_hal->rxBufferIndex = 0;
    i2c_hal->rxBufferLength = quantity;

    return quantity;
}

void I2C_HAL_beginTransmission(I2C_HAL *i2c_hal, uint8_t address) {
    // Indicate that we are transmitting
    i2c_hal->transmitting = 1;

    // Set address of targeted slave
    i2c_hal->txAddress = address;

    // Reset tx buffer iterator vars
    i2c_hal->txBufferIndex = 0;
    i2c_hal->txBufferLength = 0;
}

uint8_t I2C_HAL_endTransmission(I2C_HAL *i2c_hal, uint8_t sendStop) {
    // Transmit buffer (blocking)
    auto status = HAL_I2C_Master_Transmit(i2c_hal->hi2c, i2c_hal->txAddress, i2c_hal->txBuffer, i2c_hal->txBufferLength, Timeout);

    i2c_hal->txBufferIndex = 0;
    i2c_hal->txBufferLength = 0;

    // Indicate that we are done transmitting
    i2c_hal->transmitting = 0;

    return status == HAL_OK;
}

size_t I2C_HAL_write(I2C_HAL *i2c_hal, uint8_t data) {
    if (i2c_hal->transmitting) {
        // Don't care if buffer is full
        if (i2c_hal->txBufferLength >= BUFFER_LENGTH) {
            // Handle buffer overflow
            return 0;
        }

        // Add byte in tx buffer
        i2c_hal->txBuffer[i2c_hal->txBufferIndex] = data;
        ++i2c_hal->txBufferIndex;
        i2c_hal->txBufferLength = i2c_hal->txBufferIndex;
    } else {
        // Send response to master
        HAL_I2C_Slave_Transmit_IT(i2c_hal->hi2c, &data, 1);
    }
    return 1;
}

size_t I2C_HAL_write_array(I2C_HAL *i2c_hal, const uint8_t *data, size_t quantity) {
    if (i2c_hal->transmitting) {
        // In master transmitter mode
        for (size_t i = 0; i < quantity; ++i) {
            I2C_HAL_write(i2c_hal, data[i]);
        }
    } else {
        // Reply to master
        HAL_I2C_Slave_Transmit_IT(i2c_hal->hi2c, (uint8_t *)data, quantity);
    }
    return quantity;
}

int I2C_HAL_available(I2C_HAL *i2c_hal) {
    return i2c_hal->rxBufferLength - i2c_hal->rxBufferIndex;
}

int I2C_HAL_read(I2C_HAL *i2c_hal) {
    int value = -1;

    // Get each successive byte on each call
    if (i2c_hal->rxBufferIndex < i2c_hal->rxBufferLength) {
        value = i2c_hal->rxBuffer[i2c_hal->rxBufferIndex];
        ++i2c_hal->rxBufferIndex;
    }

    return value;
}
