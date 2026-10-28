/******************************************************************************

 @file  main.c

 @brief BLWST receiver for railway track condition monitoring. Receives the
        accelerometer packets over the CC430F5137 sub-1 GHz radio and streams
        the three-axis data over UART. Based on example code from Texas
        Instruments.

 @author Pengyu Li (STVLAb, City University of Hong Kong)

 Copyright (c) 2021 Pengyu Li
 SPDX-License-Identifier: MIT

 Target Device: CC430F5137

 ******************************************************************************/

#include "blwst-receiver.h"
#include "MSP430.h"
#include "CC430F5137.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>



#define  PACKET_LEN         (8)            // PACKET_LEN = 8
#define  RSSI_IDX           (PACKET_LEN)    // Index of appended RSSI 
#define  CRC_LQI_IDX        (PACKET_LEN+1)  // Index of appended LQI, checksum
#define  CRC_OK             (BIT7)          // CRC_OK bit 
#define  PATABLE_VAL        (0x51)          // 0 dBm output 

extern RF_SETTINGS rfSettings;
unsigned char string[6] = "xxxxxx";
char data[35];


unsigned char nodetime=0;
unsigned char packetReceived;
unsigned char packetTransmit; 

unsigned char RxBuffer[PACKET_LEN+2];
unsigned char RxBufferLength = 0;
const unsigned char TxBuffer[PACKET_LEN];
unsigned char buttonPressed = 0;
unsigned int i = 0; 

unsigned char ready[]={"Ready"};
void Uart_ready(unsigned char* string2, unsigned int length);

unsigned char transmitting = 0; 
unsigned char receiving = 0; 
unsigned int a,b,c;
unsigned char recieved=0;
void SetupRtc(void);
void Uart_init(void);
char test[35];
char Tx_empty[21];
void Uart_Tx(char* string, unsigned int length);
char Tx_data[24];
unsigned char txtype=0;  // 0: empty packet 1: received data

unsigned char time=0;
float xais=0;
float yais=0;
float zais=0;
float range=0.001; //measurement range = 1g

unsigned char XL,XH,YL,YH,ZL,ZH;


unsigned char status = 0;
short         xAxis ;
short         yAxis ;
short         zAxis ;

void transport_xyz(void);
void timer(void);

void Tran_ACC(void);


void main()
{  
  // Stop watchdog timer to prevent time out reset 
  WDTCTL = WDTPW + WDTHOLD; 

  SetVCore(3);

  UCSCTL3 |= SELREF_2;                      // Set DCO FLL reference = REFO
               __bis_SR_register(SCG0);                  // Disable the FLL control loop
              // UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
               UCSCTL1 = DCORSEL_5;                      // Select DCO range 16MHz operation
               UCSCTL2 = FLLD_1 + 599;                   // Set DCO Multiplier for 8MHz
               __bic_SR_register(SCG0);
  Uart_init();
   ResetRadioCore();
  InitRadio();
   InitButtonLeds();
__bis_SR_register(GIE);
sprintf(Tx_empty,"Empty:%.2f,%.2f,%.2f\n",0.0,0.0,0.0);

timer();

  ReceiveOn();
  receiving = 1;

  while (1)
  {
      ReceiveOn();
      receiving = 1;
    }
}


void InitButtonLeds(void)
{
  // Set up the button as interruptible 
  P2DIR &= ~BIT1;
  P2REN |= BIT1;
  P2IES &= BIT1;
  P2IFG = 0;
  P2OUT |= BIT1;
  P2IE  |= BIT1;

  // Initialize Port J
  PJOUT = 0x00;
  PJDIR = 0xFF; 

  // Set up LEDs 
  PJOUT &= ~BIT0;
  PJDIR |= BIT0;
  PJOUT &= ~BIT2;
  PJDIR |= BIT2;
  PJDIR |= BIT3;
  P1DIR=0xFF;
}

void InitRadio(void)
{
  // Set the High-Power Mode Request Enable bit so LPM3 can be entered
  // with active radio enabled 
  PMMCTL0_H = 0xA5;
  PMMCTL0_L |= PMMHPMRE_L; 
  PMMCTL0_H = 0x00; 
  
  WriteRfSettings(&rfSettings);
  
  WriteSinglePATable(PATABLE_VAL);
}



void Transmit(unsigned char *buffer, unsigned char length)
{
  RF1AIES |= BIT9;                          
  RF1AIFG &= ~BIT9;                         // Clear pending interrupts
  RF1AIE |= BIT9;                           // Enable TX end-of-packet interrupt
  
  WriteBurstReg(RF_TXFIFOWR, buffer, length);     
  Strobe( RF_STX );                         // Strobe STX   
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
      if(receiving)             // RX end of packet
      {
        // Read the length byte from the FIFO       
        RxBufferLength = ReadSingleReg( RXBYTES );               
        ReadBurstReg(RF_RXFIFORD, RxBuffer, RxBufferLength); 


if(RxBuffer[7]==0x03)
{
XL=(short) RxBuffer[0];
XH=(short) RxBuffer[1];
YL=(short) RxBuffer[2];
YH=(short) RxBuffer[3];
ZL=(short) RxBuffer[4];
ZH=(short) RxBuffer[5];
Tran_ACC();       //Transform data
txtype=1;
sprintf(Tx_data,"Data:%.2f,%.2f,%.2f\n",xais,yais,zais);  //Package data
Uart_Tx(Tx_data, sizeof Tx_data);
P1OUT ^= BIT0;    // Toggle LED
txtype=0;
}

      ReceiveOff();

      }
      else if(transmitting)         // TX end of packet
      {
        RF1AIE &= ~BIT9;                    // Disable TX end-of-packet interrupt
        PJOUT &= ~BIT2;                     // Turn off LED after Transmit
        transmitting = 0; 
      }
      else while(1);                // trap
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


void timer(void)
{
    RTCCTL01 = RTCTEVIE + RTCSSEL_2 + RTCTEV_0;
      RTCPS0CTL = RT0PSDIV_2;                   // ACLK, /8, start timer
      RTCPS1CTL = RT1SSEL_2 + RT1PSDIV_3;
}



void Uart_init(void)
{              // Stop WDT

  PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs
  P1MAP5 = PM_UCA0RXD;                      // Map UCA0RXD output to P1.5
  P1MAP6 = PM_UCA0TXD;                      // Map UCA0TXD output to P1.6
  PMAPPWD = 0;                              // Lock port mapping registers

  P1DIR |= BIT6;                            // Set P1.6 as TX output
  P1SEL |= BIT5 + BIT6;                     // Select P1.5 & P1.6 to UART function

  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
  UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
  UCA0BR1 = 0x00;                           //
  UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void Uart_Tx(char* string, unsigned int length)
{
   unsigned int i;
for(i=0;i<length;i++)
   {
   UCA0TXBUF=string[i];
   while (!(UCA0IFG & UCTXIFG));
   }
}



#pragma vector=USCI_A0_VECTOR                     //Interrupt for uart
__interrupt void USCI_A0_ISR(void)
{
  switch(__even_in_range(UCA0IV,4))
  {
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG
    //while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
                      // TX -> RXed character
    break;
  case 4:break;                             // Vector 4 - TXIFG
  default: break;
  }
}



#pragma vector=RTC_VECTOR          //Interrupt for timer
__interrupt void RTC_ISR(void)

{
  switch(__even_in_range(RTCIV,16))
  {
    case 0: break;                          // No interrupts
    case 2: break;                          // RTCRDYIFG
    case 4:                                 // RTCEVIFG

      if(txtype==0)
       {

         Uart_Tx(Tx_empty, sizeof Tx_empty);
       }

      break;
    case 6: break;                          // RTCAIFG
    case 8: break;                          // RT0PSIFG
    case 10: break;                         // RT1PSIFG
    case 12: break;                         // Reserved
    case 14: break;                         // Reserved
    case 16: break;                         // Reserved
    default: break;
  }
}



void Tran_ACC(void)    //Transform data
{
    xAxis =((short)XH << 8) + XL;//-47;
    yAxis = ((short)YH << 8) + YL;//-24;
    zAxis = ((short)ZH << 8) + ZL;//-57;
    xais=range*xAxis;
    yais=range*yAxis;
    zais=range*zAxis;
    if(xais>2)
    {
        xais=0;
    }
    if(xais<-1)
    {
        xais=0;
    }

    if(yais>1)
    {
        yais=0;
    }
    if(yais<-1)
    {
        yais=0;
    }
    if(zais>1)
    {
       zais=0;
    }
    if(zais<-1)
    {
        zais=0;
    }

}
