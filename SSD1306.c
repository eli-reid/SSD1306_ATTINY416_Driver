#include "SSD1306.h"


SSD1306_Buffer_t SSD1306_buffer = {
  .Index = 0
};
SSD1306_Cursor_t SSD1306_Cursor = {
    .Current_col = 1,
    .Current_row = 1,
    .Width = 128,
    .Hieght = 8   
};

void IC_writeNBytes(twi0_address_t address, void* data, size_t len)
{
    while(!IC_Open(address)); // sit here until we get the bus..
    IC_SetBuffer(data,len);
    IC_SetAddressNackCallback(IC_SetRestartWriteCallback,NULL); //NACK polling?
    IC_MasterWrite();
    while(I2C0_BUSY == IC_Close()); // sit here until finished.
}
void SSD1306_clr_screen(uint8_t color){
    // send buffer 4 times to = 128 width
    
    SSD1306_Buffer_Init(1);
    memset(SSD1306_buffer.Buffer + 1, color, 33);
    SSD1306_buffer.Index = 33;
    SSD1306_send();
    SSD1306_send();
    SSD1306_send();
    SSD1306_send();
}
void SSD1306_Buffer_Init(bool isData) {

    if(isData)
        SSD1306_buffer.Buffer[0] = SSD1306_CONTROL_DATA_STREAM;
    else
        SSD1306_buffer.Buffer[0] = SSD1306_CONTROL_CMD_STREAM;
  SSD1306_buffer.Index = 1;
}

void SSD1306_BufferAdd1(uint8_t data) {
  SSD1306_buffer.Buffer[SSD1306_buffer.Index++] = data;
}

void SSD1306_BufferAdd2(uint8_t data0, uint8_t data1) {
  SSD1306_buffer.Buffer[SSD1306_buffer.Index++] = data0;
  SSD1306_buffer.Buffer[SSD1306_buffer.Index++] = data1;
}

void SSD1306_BufferAdd3(uint8_t data0, uint8_t data1, uint8_t data2) {
  SSD1306_buffer.Buffer[SSD1306_buffer.Index++] = data0;
  SSD1306_buffer.Buffer[SSD1306_buffer.Index++] = data1;
  SSD1306_buffer.Buffer[SSD1306_buffer.Index++] = data2;
}

void SSD1306_PrintChar(char chr) {
  SSD1306_Buffer_Init(1);
  for(int i=0; i < 5; i++){
    SSD1306_Increment_Cursor();
    SSD1306_buffer.Buffer[SSD1306_buffer.Index++] = Font7x10[(chr - 32)][i] ;
  }
  SSD1306_send();
  return;
}

void SSD1306_PrintStr(char* str){
    for(int i=0; i< strlen(str);i++)
        SSD1306_PrintChar(str[i]);
}

void SSD1306_send(){
   IC_writeNBytes( SSD1306_ADDR, SSD1306_buffer.Buffer, SSD1306_buffer.Index);
}



void SSD1306_Increment_Cursor(){
        if(SSD1306_Cursor.Current_col++ == 128){
            SSD1306_Cursor.Current_col = 1;
            if(SSD1306_Cursor.Current_row++ == 8)
                SSD1306_Cursor.Current_row = 1;
        }
}

void SSD1306_Move_Cursor(int spaces){
    
    SSD1306_Buffer_Init(1);
    for(int i=0; i<spaces; i++){
        if(SSD1306_buffer.Index == 33) {
         
            SSD1306_send();  
            SSD1306_Buffer_Init(1);   
        }
        SSD1306_BufferAdd1(0x00);
        if(SSD1306_Cursor.Current_col++ < 129){
            SSD1306_Cursor.Current_col=1;
            if(SSD1306_Cursor.Current_row++ < 9)
                SSD1306_Cursor.Current_row = 1;
        }
    }
    SSD1306_send(); 
}

void SSD1306_Reset_Cursor(){
    int f,g;
    if(SSD1306_Cursor.Current_row == 0 && SSD1306_Cursor.Current_col == 0)
        return;
    f = SSD1306_Cursor.Hieght;
    g = f*128-SSD1306_Cursor.Current_col;
    SSD1306_Move_Cursor(g);
} 

void SSD1306_int(){
    SSD1306_Buffer_Init(0); 
    SSD1306_BufferAdd2(SSD1306_SET_MPLX,  SSD1306_HEIGHT - 1);
    SSD1306_BufferAdd2(SSD1306_SET_OFFSET,0);
    SSD1306_BufferAdd2(SSD1306_MEM_ADDR_MODE,0);
    SSD1306_BufferAdd1(SSD1306_SET_START);
    SSD1306_BufferAdd1(SSD1306_COL_NORM); 
    SSD1306_BufferAdd1(SSD1306_ROW_NORM); 
    SSD1306_BufferAdd2(0xDA,0x02);
    SSD1306_BufferAdd2(SSD1306_SET_CONTRAST,200);
    SSD1306_BufferAdd1(SSD1306_DISPLAY_NORMAL );
    SSD1306_BufferAdd2(SSD1306_OSC, 0x81 );
    SSD1306_BufferAdd2(SSD1306_CHARGE_PUMP ,SSD1306_CHARGE_PUMP_ON);
    SSD1306_BufferAdd2(SSD1306_CHARGE_PERIOD , 2);
    SSD1306_BufferAdd1(SSD1306_DISPLAY_ON);  
    SSD1306_send();
    SSD1306_Screen_Test();
}

void SSD1306_Screen_Test(){
    int i=0;
    while(i<8){
        i++;
        SSD1306_clr_screen(0xff);
    };
    i=0;
    while(i<8){
        i++;
        SSD1306_clr_screen(0x00);
    }; 
}