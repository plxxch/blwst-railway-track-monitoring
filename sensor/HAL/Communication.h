
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/

/******************************************************************************/
/* Functions Prototypes                                                       */
/******************************************************************************/



/*! Initializes the SPI communication peripheral. */
unsigned char SPI_Init();


/*! Reads data from SPI. */
unsigned char SPI_Read(unsigned char slaveDeviceId,
                       unsigned char* data,
                       unsigned char bytesNumber);

unsigned char SPI_Write(unsigned char slaveDeviceId,
                        unsigned char* data,
                        unsigned char bytesNumber);
#endif // _COMMUNICATION_H_
