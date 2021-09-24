/* 
 * File:   I2C.h
 * Author: edog0049a
 *
 * Created on September 22, 2021, 9:45 PM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif
#define I2C_BUFFER_SIZE 33    
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "mcc_generated_files/mcc.h"

    
typedef struct {
  uint8_t Index;
  uint8_t Buffer[I2C_BUFFER_SIZE];
} I2C_Buffer_t;  
    
void I2C_Write(twi0_address_t address, uint8_t command);
void I2C_Reset_Buffer();
void I2C_Set_Buffer_Index(uint8_t Index);
void I2C_Fill_Buffer(uint8_t fill);
void I2C_Buffer_Add1(uint8_t data);
void I2C_Buffer_Add2(uint8_t data0, uint8_t data);
void I2C_Buffer_Add3(uint8_t data0, uint8_t data1,  uint8_t data2);
extern I2C_Buffer_t I2C_buffer;

#ifdef	__cplusplus
}
#endif

#endif	/* ICFUNC_H */

