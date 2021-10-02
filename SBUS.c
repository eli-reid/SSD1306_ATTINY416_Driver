#include "SBUS.h"
 uint8_t DataBytes[10];

void SBUS_Reset(){
        PORTB_set_pin_dir(5,PORT_DIR_OUT);
        PORTB_set_pin_level(5,0);
        _delay_us(500);
        PORTB_set_pin_level(5,1);
     
}
uint8_t SBUS_Read_Byte(){
    PORTB_set_pin_dir(6,PORT_DIR_OUT);    
    PORTB_set_pin_level(5,0);
    _delay_ms(20);
    PORTB_set_pin_level(65,1);
    //_delay_ms(15);
    PORTB_set_pin_dir(5,PORT_DIR_IN);
    uint8_t Tbyte;
       // _delay_us(80);
        for (int i = 0; i<8; i++) {
            
            Tbyte = Tbyte | PORTB_get_pin_level(5) << i;
            _delay_us(80);
       }

    
  return Tbyte;
}

void SBUS_Read_Bytes (int bytes) {
  for (int i=0; i<bytes; i++) {
    DataBytes[i] = SBUS_Read_Byte();
  }
}