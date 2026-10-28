# Third-party notices

The MIT license in [LICENSE](LICENSE) applies only to the original code
authored by Pengyu Li. Third-party files remain under their own licenses; their
copyright notices and license texts are kept in the file headers.

## Included

| File | Copyright | License |
|---|---|---|
| `sensor/HAL/RF1A.c`, `sensor/HAL/RF1A.h` | © 2010 Texas Instruments Incorporated | BSD 3-Clause |
| `receiver/HAL/RF1A.c`, `receiver/HAL/RF1A.h` | © 2010 Texas Instruments Incorporated | BSD 3-Clause |
| `receiver/HAL/HAL_PMAP.c`, `receiver/HAL/HAL_PMAP.h` | © 2010 Texas Instruments Incorporated | BSD 3-Clause |
| `receiver/HAL/HAL_MACROS.h` | © 2010 Texas Instruments Incorporated | BSD 3-Clause |
| `receiver/HAL/HAL_PMM.c`, `receiver/HAL/HAL_PMM.h` | © 2010 Texas Instruments Incorporated | BSD 3-Clause |
| `sensor/pre_init.cpp` | © 2015 Texas Instruments Incorporated | BSD 3-Clause |
| `sensor/lnk_cc430f5137.cmd` | © 2018 Texas Instruments Incorporated | BSD 3-Clause |
| `receiver/lnk_cc430f5137.cmd` | © 2018 Texas Instruments Incorporated | BSD 3-Clause |
| `sensor/HAL/ADXL362.c`, `sensor/HAL/ADXL362.h` | © 2012 Analog Devices, Inc. | ADI driver license |
| `sensor/HAL/Communication.c`, `sensor/HAL/Communication.h` | © 2012 Analog Devices, Inc. | ADI driver license |

The Analog Devices driver license permits redistribution in source and binary
forms provided the copyright notice and conditions are retained. It also states
that the software must be used with, or directly connected to, an Analog
Devices component; this firmware uses the ADXL362, so that condition is met.

The BSD 3-Clause text is reproduced in each TI file above: redistribution is
permitted provided the copyright notice, conditions and disclaimer are
retained, and TI's name is not used to endorse derived products without written
permission. The software is provided “AS IS”, without warranty.

`receiver/HAL/RfRegSettings.c` is an original file written for this repository.
It reproduces the radio register values used by the original firmware (plain
register values) using the BSD-licensed `RF_SETTINGS` structure from `RF1A.h`.
It is covered by the MIT license.

## Not included

TI's CC430 RF examples package (SLAC525) is distributed under the restrictive
*MSP Source and Object Code Software License Agreement*. Its copies of
`RF_Toggle_LED_Demo.h`, `HAL_PMM.c/.h` and `HAL_MACROS.h` are not redistributed
here. The project-local headers `blwst-accelerometer.h` and `blwst-receiver.h`
replace the include-only `RF_Toggle_LED_Demo.h`; the BSD F5xx/6xx core-library
PMM files replace the restrictive PMM implementation.

## Trademarks

MSP430, CC430 and Code Composer Studio are trademarks or registered trademarks
of Texas Instruments Incorporated. ADXL362 is a trademark of Analog Devices,
Inc. All other trademarks are the property of their respective owners.
