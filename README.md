# BLWST — Battery-Less Wireless Sensor Tag for Railway Track Monitoring

Firmware for the battery-less wireless sensor tag (BLWST) and its receiver
described in:

> P. Li, Z. Long, and Z. Yang, “RF Energy Harvesting for Batteryless and
> Maintenance-Free Condition Monitoring of Railway Tracks,” *IEEE Internet of
> Things Journal*, vol. 8, no. 5, pp. 3512–3523, 2021.
> DOI: [10.1109/JIOT.2020.3023475](https://doi.org/10.1109/JIOT.2020.3023475)

## Overview

The sensor node is powered only by RF energy harvested from a reader installed
on the train. It measures track vibration with an ADXL362 accelerometer and
transmits the reading over a 433 MHz link.

| Part | Role |
|---|---|
| `sensor/` | CC430F5137 + ADXL362 accelerometer (SPI); RF energy harvesting front end and LTC3588-1 power management are external to this firmware. |
| `receiver/` | CC430F5137 / CC430F6137 receiver; forwards the received samples to a laptop over UART. |

## Data packet

Each transmission is 8 bytes (fixed packet length, `PKTLEN = 0x08`):

| Byte | Content |
|---|---|
| 0 | X low byte |
| 1 | X high byte |
| 2 | Y low byte |
| 3 | Y high byte |
| 4 | Z low byte |
| 5 | Z high byte |
| 6 | ID code `0x02` |
| 7 | Verification code `0x03` |

The receiver accepts a packet only when byte 7 is `0x03`, reconstructs the
three axes, and prints:

```
Data:<x>,<y>,<z>
```

over UART at **9600 8N1** (P1.5/P1.6, ACLK).

The accelerometer is configured for **±2 g** (`ADXL362_SetRange(2)`) and active
power mode 1. The working interval is set by the Timer_A delay in the main
loop (`timer(10000)`).

## Repository layout

```
.
├── sensor/                  # CCS project: BLWST accelerometer node
│   ├── Battery-less wireless sensor tag-accelerometer-modify.c
│   ├── blwst-accelerometer.h
│   ├── HAL/                 # ADXL362, SPI communication, RF1A (TI/ADI)
│   └── targetConfigs/
├── receiver/                # CCS project: receiver node
│   ├── main.c
│   ├── blwst-receiver.h
│   ├── HAL/                 # RF1A, PMM, port mapper, RF settings
│   └── targetConfigs/
├── docs/
│   └── README-Supplementary Code.pdf
├── THIRD_PARTY_NOTICES.md
├── CITATION.cff
└── LICENSE
```

## Building

1. Install **Code Composer Studio 8.3.0** with MSP430 support (the version used
   for the original work).
2. `File → Import → CCS Projects`, then import both `sensor/` and `receiver/`.
3. Select the **CC430F5137** target (the receiver project also contains a
   `lnk_cc430f6137.cmd` linker script for the CC430F6137 variant).
4. Build and flash with an MSP-FET / eZ430 probe.

No external files need to be downloaded: all third-party source code in this
repository is BSD-licensed or covered by the Analog Devices driver license.

> **Hardware note:** the receiver's `HAL/RfRegSettings.c` reproduces the radio
> register values used by the original firmware with the BSD-licensed
> `RF_SETTINGS` structure. Verify the link on hardware before relying on it.

## License

The MIT license in [LICENSE](LICENSE) covers the original code authored by
Pengyu Li. Third-party files remain under their own licenses — see
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

## Citation

```bibtex
@article{li2021railway,
  author  = {Li, Pengyu and Long, Zhihe and Yang, Zhengbao},
  title   = {RF Energy Harvesting for Batteryless and Maintenance-Free Condition Monitoring of Railway Tracks},
  journal = {IEEE Internet of Things Journal},
  volume  = {8},
  number  = {5},
  pages   = {3512--3523},
  year    = {2021},
  doi     = {10.1109/JIOT.2020.3023475}
}
```
