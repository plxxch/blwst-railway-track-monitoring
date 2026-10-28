/*
 * blwst-receiver.h
 *
 * Project header for the BLWST receiver (railway track monitoring).
 *
 * This replaces TI's RF_Toggle_LED_Demo.h, which only contained includes and
 * function declarations and is distributed under TI's restrictive MSP license.
 *
 * Copyright (c) 2021 Pengyu Li
 * SPDX-License-Identifier: MIT
 */

#ifndef BLWST_RECEIVER_H
#define BLWST_RECEIVER_H

#include "cc430x513x.h"
#include "RF1A.h"
#include "HAL_PMM.h"

void Transmit(unsigned char *buffer, unsigned char length);
void ReceiveOn(void);
void ReceiveOff(void);
void InitRadio(void);

#endif /* BLWST_RECEIVER_H */
