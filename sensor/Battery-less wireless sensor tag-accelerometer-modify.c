
/******************************************************************************

 @file  Battery-less wireless sensor tag-accelerometer-modify.c

 @brief BLWST accelerometer node for railway track condition monitoring.
        Samples an ADXL362 accelerometer over SPI and transmits the result
        over the CC430F5137 sub-1 GHz radio. Based on example code from
        Texas Instruments.

 @author Pengyu Li (STVLAb, City University of Hong Kong)

 Copyright (c) 2021 Pengyu Li
 SPDX-License-Identifier: MIT

 Target Device: CC430F5137

 ******************************************************************************/

#include "blwst-accelerometer.h"
#include "ADXL362.h"
#include "Communication.h"
#include "CC430F5137.h"


unsigned char TxBuffer[8]={};
unsigned char stop_timer=1;
unsigned char awake=0; // timer1_stop for exiting low power mode
unsigned char XL1 = 0; //Low bits of x axis
unsigned char XH1 = 0;
unsigned char YL1 = 0;
unsigned char YH1 = 0;
unsigned char ZL1 = 0;
unsigned char ZH1 = 0;
#define  PATABLE_VAL  (0xc6); //Tx power = 0 dBm:0x50  10dBm c6

void maincode(void);

void Trans_Data(void);

void timer(unsigned int t);


void main( void )
{  

    //P1OUT |=BIT5;
    InitRadio();            //Initialize the radio core

    __bis_SR_register(GIE); //Enable interrupt

//        ADXL362_Init();     //Initialize the accelerometer
//
//        ADXL362_SetPowerMode(1); //Set accelerometer to active mode
//        ADXL362_SetRange(2);

        TxBuffer[6]=0x02;  //id code

        TxBuffer[7]=0x03;  //verification code
//P1DIR |=BIT0;
        timer(10);
 // __bis_SR_register(LPM3_bits + GIE);  //Enter low power mode 3



while(1)
{

         maincode();    //Execute measurement and transmission
         //P1OUT ^=BIT0;
         timer(10000);
       //  timer(32760);


}
}




void InitRadio(void)
{
  // Set the High-Power Mode Request Enable bit so LPM3 can be entered
  // with active radio enabled 
  PMMCTL0_H = 0xA5;
  PMMCTL0_L |= PMMHPMRE_L; 
  PMMCTL0_H = 0x00; 
  
  WriteRfSettings();
  
  WriteSinglePATable(0x26);  //c3 >10dbm  26 <-6dbm
}



void Transmit(unsigned char *buffer, unsigned char length)
{

  RF1AIES |= BIT9;                          
  RF1AIFG &= ~BIT9;                         // Clear pending interrupts
  RF1AIE |= BIT9;                           // Enable TX end-of-packet interrupt
  
  WriteBurstReg(RF_TXFIFOWR, buffer, length);     
  
  Strobe( RF_STX );                         // Strobe STX
 // Strobe( RF_STX );
}

void ReceiveOn(void)
{  
  RF1AIES |= BIT9;                          // Falling edge of RFIFG9
  RF1AIFG &= ~BIT9;                         // Clear a pending interrupt
  RF1AIE  |= BIT9;                          // Enable the interrupt 
  
  // Radio is in IDLE following a TX, so strobe SRX to enter Receive Mode
  Strobe( RF_SRX );                      
}

void ReceiveOff(void)
{
  RF1AIE &= ~BIT9;                          // Disable RX interrupts
  RF1AIFG &= ~BIT9;                         // Clear pending IFG

  // It is possible that ReceiveOff is called while radio is receiving a packet.
  // Therefore, it is necessary to flush the RX FIFO after issuing IDLE strobe 
  // such that the RXFIFO is empty prior to receiving a packet.
  Strobe( RF_SIDLE );
  Strobe( RF_SFRX  );                       
}

#pragma vector=CC1101_VECTOR
__interrupt void CC1101_ISR(void)
{
  switch(__even_in_range(RF1AIV,32))        // Prioritizing Radio Core Interrupt 
  {
    case  0: break;                         // No RF core interrupt pending
    case  2: break;                         // RFIFG0
    case  4: break;                         // RFIFG1
    case  6: break;                         // RFIFG2
    case  8: break;                         // RFIFG3
    case 10: break;                         // RFIFG4
    case 12: break;                         // RFIFG5
    case 14: break;                         // RFIFG6
    case 16: break;                         // RFIFG7
    case 18: break;                         // RFIFG8
    case 20:                                // RFIFG9
               RF1AIE &= ~BIT9;                    // Disable TX end-of-packet interrupt
               __bis_SR_register(LPM3_bits + GIE);
             break;
    case 22: break;                         // RFIFG10
    case 24: break;                         // RFIFG11
    case 26: break;                         // RFIFG12
    case 28: break;                         // RFIFG13
    case 30: break;                         // RFIFG14
    case 32: break;                         // RFIFG15
  }  
  __bic_SR_register_on_exit(LPM3_bits);
}




void timer(unsigned int t)    //For delay
{
    TA0CCTL0=CCIE;
    TA0CCR0=t;
    TA0CTL=TASSEL_1 +MC_1 + TACLR;
    __bis_SR_register(LPM3_bits | GIE);
}

void timer_stop()
{
    TA0CTL=MC_0;
}






#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)

{

    __bic_SR_register_on_exit(LPM3_bits);
}


void maincode()
{
    P1OUT &=~BIT5;
    P1OUT |=BIT4;
    timer(100);
    ADXL362_Init();     //Initialize the accelerometer

    ADXL362_SetPowerMode(1); //Set accelerometer to active mode
    ADXL362_SetRange(2);
    ReceiveOff();
    timer(1000);
        ADXL362_GetXyz(&XL1,&XH1,&YL1,&YH1,&ZL1,&ZH1);  //Measurement
        ADXL362_GetXyz(&XL1,&XH1,&YL1,&YH1,&ZL1,&ZH1);  //Measurement
        timer(100);
        P1OUT &=~BIT4;
Trans_Data();     //Package the data
P1OUT |=BIT5;


Transmit(TxBuffer, sizeof TxBuffer);  //Transmit data

}


void Trans_Data(void)
{

    TxBuffer[0]=XL1;
    TxBuffer[1]=XH1;
    TxBuffer[2]=YL1;
    TxBuffer[3]=YH1;
    TxBuffer[4]=ZL1;
    TxBuffer[5]=ZH1;
}



