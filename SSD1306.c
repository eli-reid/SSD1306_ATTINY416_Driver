#include "SSD1306.h"

void SSD1306_clr_screen(uint8_t color){
    // send buffer 4 times to = 128 width
    
    for(int i=0;i<8;i++){
        I2C_Reset_Buffer(); 
        I2C_Buffer_Add1(0xB0 + i);
        I2C_Buffer_Add1(0x00);
        I2C_Buffer_Add1(0x10);
        SSD1306_send_CMD();
        I2C_Fill_Buffer(color);
        for (int j=0;j<4;j++)
            SSD1306_send_data();
    }
}

void SSD1306_PrintChar(char chr, uint8_t row, uint8_t col) {
  I2C_Reset_Buffer();
  for(int i=0; i < 5; i++){
    I2C_Buffer_Add1(Font7x10[(chr - 32)][i]) ;
  }
 I2C_Buffer_Add3(0x00,0x00,0x00) ;
 SSD1306_send_data();
  
  return;
}

void SSD1306_PrintStr(char* str, uint8_t row, uint8_t col){
    I2C_Reset_Buffer();
    I2C_Buffer_Add1(0xB0 + row);
    I2C_Buffer_Add1((0x00) | (col & 0x0F));
    I2C_Buffer_Add1((0x10) | (col>>4));
    SSD1306_send_CMD();
    for(int i=0; i< strlen(str);i++)
        SSD1306_PrintChar(str[i],row,col);
}

void SSD1306_send_data(){
    I2C_Write(SSD1306_ADDR, SSD1306_CONTROL_DATA_STREAM);
}
void SSD1306_send_CMD(){
   I2C_Write( SSD1306_ADDR, SSD1306_CONTROL_CMD_STREAM);
}

void SSD1306_int(){
    I2C_Reset_Buffer();
    I2C_Buffer_Add2(SSD1306_SET_MPLX,  SSD1306_HEIGHT - 1);
    I2C_Buffer_Add2(SSD1306_SET_OFFSET,0);
    I2C_Buffer_Add2(SSD1306_MEM_ADDR_MODE,0);
    I2C_Buffer_Add1(SSD1306_SET_START);
    I2C_Buffer_Add1(SSD1306_COL_NORM); 
    I2C_Buffer_Add1(SSD1306_ROW_NORM); 
    I2C_Buffer_Add2(0x20,0x02);//set page address mode
    I2C_Buffer_Add3(0x22,0,7); //set page range
    I2C_Buffer_Add3(0x21,0,127);//set col range
    I2C_Buffer_Add2(0xDA,0x00);
    I2C_Buffer_Add2(SSD1306_SET_CONTRAST,200);
    I2C_Buffer_Add1(SSD1306_DISPLAY_NORMAL );
    I2C_Buffer_Add2(SSD1306_OSC, 0x81 );
    I2C_Buffer_Add2(SSD1306_CHARGE_PUMP ,SSD1306_CHARGE_PUMP_ON);
    I2C_Buffer_Add2(SSD1306_CHARGE_PERIOD , 2);
    I2C_Buffer_Add1(SSD1306_DISPLAY_ON);  
    SSD1306_send_CMD();
    SSD1306_Screen_Test();
}

void SSD1306_Screen_Test(){
        SSD1306_clr_screen(0xff);
        SSD1306_clr_screen(0x00);
}