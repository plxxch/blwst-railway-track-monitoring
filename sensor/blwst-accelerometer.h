/*
 * blwst-accelerometer.h
 *
 * Project header for the BLWST accelerometer node (railway track monitoring).
 *
 * This replaces TI's RF_Toggle_LED_Demo.h, which only contained includes and
 * function declarations and is distributed under TI's restrictive MSP license.
 *
 * Copyright (c) 2021 Pengyu Li
 * SPDX-License-Identifier: MIT
 */

#ifndef BLWST_ACCELEROMETER_H
#define BLWST_ACCELEROMETER_H

#include "cc430x513x.h"
#include "RF1A.h"

void Transmit(unsigned char *buffer, unsigned char length);
void ReceiveOn(void);
void ReceiveOff(void);
void InitRadio(void);

#endif /* BLWST_ACCELEROMETER_H */
