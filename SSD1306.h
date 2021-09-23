/* 
 * File:   ICFUNC.h
 * Author: edog0049a
 *
 * Created on September 22, 2021, 9:45 PM
 */

#ifndef SSD1306_H
#define	SSD1306_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "mcc_generated_files/mcc.h"
#include "fonts.h"


#define SSD1306_ADDR      0x3C
#define SSD1306_WIDTH     128
#define SSD1306_HEIGHT    64

#define SSD1306_BUFFER_MAX   33

// Display Commands

//#define SSD1306_CONTROL_CMD_SINGLE      0x80
#define SSD1306_CONTROL_CMD_STREAM      0x00
////#define SSD1306_CONTROL_DATA_SINGLE     0xC0
#define SSD1306_CONTROL_DATA_STREAM     0x40

#define SSD1306_ALL_ON_RAM              0xA4 
#define SSD1306_ALL_ON                  0xA5
#define SSD1306_DISPLAY_NORMAL          0xA6
//#define SSD1306_DISPLAY_INVERSE         0xA7
#define SSD1306_SET_MPLX                0xA8
//#define SSD1306_DISPLAY_OFF             0xAE
#define SSD1306_DISPLAY_ON              0xAF 
#define SSD1306_SET_CONTRAST            0x81 // 0 - 255
#define SSD1306_OSC                     0xD5 
#define SSD1306_SET_OFFSET              0xD3
#define SSD1306_SET_START               0x40 // range 0x40~0x7F
#define SSD1306_CHARGE_PUMP             0x8D
#define SSD1306_CHARGE_PUMP_ON          0x14
//#define SSD1306_CHARGE_PUMP_OFF         0x10
#define SSD1306_CHARGE_PERIOD           0xD9
#define SSD1306_MEM_ADDR_MODE           0x20
#define SSD1306_COL_NORM                0xA0
//#define SSD1306_COL_INVERSE             0xA1
#define SSD1306_ROW_NORM                0xC0
//#define SSD1306_ROW_INVERSE             0xC8

#define SSD1306_SCROLL_DISABLE          0x2E
//#define SSD1306_SET_VERT_SCROLL_AREA    0xA3

typedef struct{
    int Current_col;
    int Current_row;
    int Width;
    int Hieght;
}SSD1306_Cursor_t;

typedef struct {
  uint8_t Index;
  uint8_t Buffer[SSD1306_BUFFER_MAX];
} SSD1306_Buffer_t;

void SSD1306_int();
void SSD1306_clr_screen(uint8_t color);
void SSD1306_Buffer_Init(bool isData);
void SSD1306_BufferAdd1(uint8_t data);
void SSD1306_BufferAdd2(uint8_t data0, uint8_t data);
void SSD1306_BufferAdd3(uint8_t data0, uint8_t data1,  uint8_t data2);
void SSD1306_PrintChar(char chr);
void SSD1306_PrintStr(char* str);
void SSD1306_Move_Cursor(int spaces);
void SSD1306_Reset_Cursor();
void SSD1306_Screen_Test();
void SSD1306_Increment_Cursor();
void SDD1306_Send();
void IC_writeNBytes(twi0_address_t address, void* data, size_t len);
extern  SSD1306_Buffer_t SSD1306_buffer;

#ifdef	__cplusplus
}
#endif

#endif	/* ICFUNC_H */