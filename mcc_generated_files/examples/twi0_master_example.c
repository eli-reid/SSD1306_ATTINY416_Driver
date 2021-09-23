/**
  @Company
    Microchip Technology Inc.

  @Description
    This Source file provides APIs.
    Generation Information :
    Driver Version    :   1.0.0
*/
/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/


/*
 This file provides some basic blocking helper functions for common operations on the i2c API
 */

#include "../include/twi0_master.h"
#include "twi0_master_example.h"

/****************************************************************/
static twi0_operations_t wr1RegCompleteHandler_example(void *ptr)
{
    IC_SetBuffer(ptr,1);
    IC_SetDataCompleteCallback(NULL,NULL);
    return I2C0_CONTINUE;
}

void IC_example_write1ByteRegister(twi0_address_t address, uint8_t reg, uint8_t data)
{
    while(!IC_Open(address)); // sit here until we get the bus..
    IC_SetDataCompleteCallback(wr1RegCompleteHandler_example,&data);
    IC_SetBuffer(&reg,1);
    IC_SetAddressNackCallback(IC_SetRestartWriteCallback,NULL); //NACK polling?
    IC_MasterWrite();
    while(I2C0_BUSY == IC_Close()); // sit here until finished.
}

void IC_example_writeNBytes(twi0_address_t address, void* data, size_t len)
{
    while(!IC_Open(address)); // sit here until we get the bus..
    IC_SetBuffer(data,len);
    IC_SetAddressNackCallback(IC_SetRestartWriteCallback,NULL); //NACK polling?
    IC_MasterWrite();
    while(I2C0_BUSY == IC_Close()); // sit here until finished.
}

/****************************************************************/
static twi0_operations_t rd1RegCompleteHandler_example(void *ptr)
{
    IC_SetBuffer(ptr,1);
    IC_SetDataCompleteCallback(NULL,NULL);
    return I2C0_RESTART_READ;
}

uint8_t IC_example_read1ByteRegister(twi0_address_t address, uint8_t reg)
{
    uint8_t    d2=42;
    twi0_error_t e;
    int x;

    for(x = 2; x != 0; x--)
    {
        while(!IC_Open(address)); // sit here until we get the bus..
        IC_SetDataCompleteCallback(rd1RegCompleteHandler_example,&d2);
        IC_SetBuffer(&reg,1);
        IC_SetAddressNackCallback(IC_SetRestartWriteCallback,NULL); //NACK polling?
        IC_MasterWrite();
        while(I2C0_BUSY == (e = IC_Close())); // sit here until finished.
        if(e==I2C0_NOERR) break;
    }
    

    return d2;
}

/****************************************************************/
static twi0_operations_t rd2RegCompleteHandler_example(void *ptr)
{
    IC_SetBuffer(ptr,2);
    IC_SetDataCompleteCallback(NULL,NULL);
    return I2C0_RESTART_READ;
}

uint16_t IC_example_read2ByteRegister(twi0_address_t address, uint8_t reg)
{
    // result is little endian
    uint16_t    result;

    while(!IC_Open(address)); // sit here until we get the bus..
    IC_SetDataCompleteCallback(rd2RegCompleteHandler_example,&result);
    IC_SetBuffer(&reg,1);
    IC_SetAddressNackCallback(IC_SetRestartWriteCallback,NULL); //NACK polling?
    IC_MasterWrite();
    while(I2C0_BUSY == IC_Close()); // sit here until finished.
    
    return (result << 8 | result >> 8);
}

/****************************************************************/
static twi0_operations_t wr2RegCompleteHandler_example(void *ptr)
{
    IC_SetBuffer(ptr,2);
    IC_SetDataCompleteCallback(NULL,NULL);
    return I2C0_CONTINUE;
}

void IC_example_write2ByteRegister(twi0_address_t address, uint8_t reg, uint16_t data)
{
    while(!IC_Open(address)); // sit here until we get the bus..
    IC_SetDataCompleteCallback(wr2RegCompleteHandler_example,&data);
    IC_SetBuffer(&reg,1);
    IC_SetAddressNackCallback(IC_SetRestartWriteCallback,NULL); //NACK polling?
    IC_MasterWrite();
    while(I2C0_BUSY == IC_Close()); // sit here until finished.
}

/****************************************************************/
typedef struct
{
    size_t len;
    char *data;
}buf_t;

static twi0_operations_t rdBlkRegCompleteHandler_example(void *ptr)
{
    IC_SetBuffer(((buf_t *)ptr)->data,((buf_t*)ptr)->len);
    IC_SetDataCompleteCallback(NULL,NULL);
    return I2C0_RESTART_READ;
}

void IC_example_readDataBlock(twi0_address_t address, uint8_t reg, void *data, size_t len)
{
    // result is little endian
    buf_t    d;
    d.data = data;
    d.len = len;

    while(!IC_Open(address)); // sit here until we get the bus..
    IC_SetDataCompleteCallback(rdBlkRegCompleteHandler_example,&d);
    IC_SetBuffer(&reg,1);
    IC_SetAddressNackCallback(IC_SetRestartWriteCallback,NULL); //NACK polling?
    IC_MasterWrite();
    while(I2C0_BUSY == IC_Close()); // sit here until finished.
}

void IC_example_readNBytes(twi0_address_t address, void *data, size_t len)
{
    while(!IC_Open(address)); // sit here until we get the bus..
    IC_SetBuffer(data,len);
    IC_MasterRead();
    while(I2C0_BUSY == IC_Close()); // sit here until finished.
}