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
#include "mcc_generated_files/mcc.h"
#include "SSD1306.h"
#include "I2C.h"
#include "SBUS.h"
#include <stdio.h>
#include <string.h>
#include <avr/delay.h>
#define PIN_1WIRE   4
/*
 * 
    Main application
*/
void OneWireWrite (uint8_t data) {
  PORTA_set_pin_dir(6,PORT_DIR_OUT);    
  for (int i = 0; i<8; i++) {
    if ((data & 1) == 1) {
        PORTA_set_pin_level(6,0);
        _delay_us(6);
        PORTA_set_pin_level(6,1); 
        _delay_us(64);
    }else{
      PORTA_set_pin_level(6,0);
      _delay_us(60);
      PORTA_set_pin_level(6,1); 
    _delay_us(10);
    }
        
   
    data = data >> 1;
  }
}
int main(void)
{

    /* Initializes MCU, drivers and middleware */
    SYSTEM_Initialize();
    SSD1306_int();
   // char s[]={0x7f,'F'};
  // SSD1306_PrintStr("Temp: 131",0,0,true);
    //SSD1306_PrintStr(s,0,72,true);
//_delay_ms(5000);


    char t[40];
    uint64_t yy = 0;
   
    //SBUS_Read_Bytes(5);   
    //ltoa (DataBytes[1],t,10); 
    //SSD1306_PrintStr(t,0,h,false);
    
    
    while(1){
        SBUS_Reset();
        memset(DataBytes, 0x00, 9);
        PORTB_set_pin_pull_mode(PIN_1WIRE,PORT_PULL_UP);
        PORTB_set_pin_dir(PIN_1WIRE,PORT_DIR_OUT);    
        PORTB_set_pin_level(PIN_1WIRE,0);
        _delay_ms(20);
        PORTB_set_pin_level(PIN_1WIRE,1);
        PORTB_set_pin_dir(PIN_1WIRE,PORT_DIR_IN);
        for (int j = 0; j<5; j++){
            for (int i = 0; i<8; i++) {
                DELAY_microseconds(15);
                yy=yy | PORTB_get_pin_level(PIN_1WIRE)<<(i * (j+1));
               DataBytes[j]= DataBytes[j] | PORTB_get_pin_level(5) << i;
                DELAY_microseconds(55);
            }
        }
   if(DataBytes[0]+DataBytes[1]+DataBytes[2]+DataBytes[3] != DataBytes[4])
      SSD1306_PrintStr("ERROR! 3",2,0,1);

   ltoa (DataBytes[0],t,10);
    SSD1306_PrintStr(t,0,0,1);
  
   ltoa (DataBytes[1],t,10);
  SSD1306_PrintStr(t,0,32,1);
  
    ltoa (DataBytes[2],t,10);
   SSD1306_PrintStr(t,0,64,1);
   
  ltoa (DataBytes[3],t,10);
 SSD1306_PrintStr(t,1,0,1);

ltoa (DataBytes[4],t,10);
SSD1306_PrintStr(t,1,32,1);
    _delay_ms(1000);
  
    };

}


/**
    End of File
*/