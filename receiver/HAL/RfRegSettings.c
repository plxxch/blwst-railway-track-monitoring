/*
 * RfRegSettings.c
 *
 * CC430 RF1A register settings used by the BLWST receiver.
 *
 * The values below are the radio configuration values used in the original
 * project (fixed packet length, 8-byte payload). They are plain register
 * values and are re-expressed here with the BSD-licensed RF_SETTINGS struct
 * from RF1A.h.
 *
 * Copyright (c) 2021 Pengyu Li
 * SPDX-License-Identifier: MIT
 */

#include "RF1A.h"

RF_SETTINGS rfSettings = {
    0x0E,   /* FSCTRL1  Frequency synthesizer control */
    0x10,   /* FREQ2    Frequency control word, high byte */
    0xA7,   /* FREQ1    Frequency control word, middle byte */
    0x62,   /* FREQ0    Frequency control word, low byte */
    0x0E,   /* MDMCFG4  Modem configuration */
    0x3B,   /* MDMCFG3  Modem configuration */
    0x73,   /* MDMCFG2  Modem configuration */
    0x42,   /* MDMCFG1  Modem configuration */
    0xF8,   /* MDMCFG0  Modem configuration */
    0x00,   /* DEVIATN  Modem deviation setting */
    0xB6,   /* FREND1   Front end RX configuration */
    0x10,   /* FREND0   Front end TX configuration */
    0x18,   /* MCSM0    Main Radio Control State Machine configuration */
    0x1D,   /* FOCCFG   Frequency offset compensation */
    0x1C,   /* BSCFG    Bit synchronization */
    0xC7,   /* AGCCTRL2 AGC control */
    0x00,   /* AGCCTRL1 AGC control */
    0xB0,   /* AGCCTRL0 AGC control */
    0xEA,   /* FSCAL3   Frequency synthesizer calibration */
    0x2A,   /* FSCAL2   Frequency synthesizer calibration */
    0x00,   /* FSCAL1   Frequency synthesizer calibration */
    0x1F,   /* FSCAL0   Frequency synthesizer calibration */
    0x09,   /* TEST0    Various test settings */
    0x06,   /* IOCFG0   GDO0 output pin configuration */
    0x04,   /* PKTCTRL0 Fixed packet length, CRC enabled */
    0x08    /* PKTLEN   8-byte payload */
};
