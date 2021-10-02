/* 
 * File:   SBUS.h
 * Author: edog0049a
 *
 * Created on September 28, 2021, 7:56 PM
 */

#ifndef SBUS_H
#define	SBUS_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "I2C.h"
#include <avr/delay.h>
#include <stdint.h>
#include "mcc_generated_files/mcc.h"

/* bus pull low timings
#define SBUS_RESET              500
#define SBUS_START_PULL_DOWN    60
#define SBUS_START_PULL_UP      15
#define SBUS_MASTER_ACK         1
/*/

void SBUS_Reset();
uint8_t SBUS_Read_Byte();
void SBUS_Read_Bytes (int bytes);
extern uint8_t DataBytes[10];
  
#ifdef	__cplusplus
}
#endif

#endif	/* SWS_H */

