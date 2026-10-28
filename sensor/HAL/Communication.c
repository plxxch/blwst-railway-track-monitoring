/***************************************************************************//**
 *   @file   Communication.c
 *   @brief  Implementation of Communication Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: 797
*******************************************************************************/

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "Communication.h"

#include "msp430.h"
//#include "cc430f5137.h"
/******************************************************************************/
/* Variables Declarations                                                     */
/******************************************************************************/


/***************************************************************************//**
 * @brief Initializes the SPI communication peripheral.
 *
 * @param lsbFirst - Transfer format (0 or 1).
 *                   Example: 0x0 - MSB first.
 *                            0x1 - LSB first.
 * @param clockFreq - SPI clock frequency (Hz).
 *                    Example: 1000 - SPI clock frequency is 1 kHz.
 * @param clockPol - SPI clock polarity (0 or 1).
 *                   Example: 0x0 - Idle state for clock is a low level; active
 *                                  state is a high level;
 *                        0x1 - Idle state for clock is a high level; active
 *                                  state is a low level.
 * @param clockEdg - SPI clock edge (0 or 1).
 *                   Example: 0x0 - Serial output data changes on transition
 *                                  from idle clock state to active clock state;
 *                            0x1 - Serial output data changes on transition
 *                                  from active clock state to idle clock state.
 *
 * @return status - Result of the initialization procedure.
 *                  Example: 1 - if initialization was successful;
 *                           0 - if initialization was unsuccessful.
*******************************************************************************/
unsigned char SPI_Init()
{

         PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs
         P2MAP0 = PM_UCA0SIMO;                     // Map UCA0SIMO output to P2.0
         P2MAP2 = PM_UCA0SOMI;                     // Map UCA0SOMI output to P2.2
         P2MAP4 = PM_UCA0CLK;                      // Map UCA0CLK output to P2.4
         PMAPPWD = 0;                              // Lock port mapping registers

         //P2OUT |= BIT5;                            // Set P1.0 for LED
            P1OUT |= BIT2;                                      // Set P1.2 for slave reset
                            // Set P1.0, P1.2 to output direction
         P2DIR |= BIT0 + + BIT2 + BIT4;              // ACLK, MCLK, SMCLK set out to pins
         P2SEL |= BIT0 +BIT2+ BIT4;              // P2.0,2,4 for debugging purposes.
         P1DIR |= BIT2;
         UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
           UCA0CTL0 &= ~UCCKPL;
           UCA0CTL0 |= UCCKPH;
           UCA0CTL0 |= UCMST+UCSYNC+UCMSB;    // 3-pin, 8-bit SPI master
                                                   // Clock polarity high, MSB
         UCA0CTL1 |= UCSSEL_2;                     // SMCLK
         UCA0BR0 = 0x02;                           // /2-1MHz 1-2MHz
         UCA0BR1 = 0;                              //
         UCA0MCTL = 0;                             // No modulation
         UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
         UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
         __bis_SR_register(GIE);
       return 1;
}

/***************************************************************************//**
 * @brief Reads data from SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data - Data represents the write buffer as an input parameter and the
 *               read buffer as an output parameter.
 * @param bytesNumber - Number of bytes to read.
 *
 * @return Number of read bytes.
*******************************************************************************/
unsigned char SPI_Read(unsigned char slaveDeviceId,
                       unsigned char* data,
                       unsigned char bytesNumber)
{

    unsigned char byte = 0;

    P1OUT &= ~0X04;

        for(byte = 0; byte < bytesNumber; byte++)
           {
            UCA0TXBUF=data[byte];
               //while (!(UCA0IFG&UCTXIFG));
            __bis_SR_register(LPM3_bits + GIE);
                       __delay_cycles(10);
               data[byte]= UCA0RXBUF;
           }
      //__delay_cycles(20);
       P1OUT |= 0X04;
      // __delay_cycles(10);


    return bytesNumber;
}


/***************************************************************************//**
 * @brief Writes data to SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data - Data represents the write buffer.
 * @param bytesNumber - Number of bytes to write.
 *
 * @return Number of written bytes.
*******************************************************************************/
unsigned char SPI_Write(unsigned char slaveDeviceId,
                        unsigned char* data,
                        unsigned char bytesNumber)
{
    unsigned char byte = 0;
    //unsigned char read = 0;

    P1OUT &= ~0X04;

    for(byte = 0; byte < bytesNumber; byte++)
    {
        UCA0TXBUF=data[byte];
       // while (!(UCA0IFG&UCTXIFG));
        __bis_SR_register(LPM3_bits + GIE);

      //  __delay_cycles(10);
    }

    P1OUT |= 0X04;
    return bytesNumber;
}


#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)

{
  switch(__even_in_range(UCA0IV,4))
  {
    case 0: break;                          // Vector 0 - no interrupt
    case 2:                                 // Vector 2 - RXIFG
        __bic_SR_register_on_exit(LPM3_bits);
                                            // make sure slave can process information
      break;
    case 4:
        __bic_SR_register_on_exit(LPM3_bits);
        break;                          // Vector 4 - TXIFG
    default: break;
  }
}
