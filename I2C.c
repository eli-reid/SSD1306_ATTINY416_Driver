#include "I2C.h"

I2C_Buffer_t I2C_buffer = {
  .Index = 1
};

void I2C_Write(twi0_address_t address, uint8_t command)
{
    I2C_buffer.Buffer[0] = command;
    while(!IC_Open(address)); // sit here until we get the bus..
    IC_SetBuffer(I2C_buffer.Buffer,I2C_buffer.Index);
    IC_SetAddressNackCallback(IC_SetRestartWriteCallback,NULL); //NACK polling?
    IC_MasterWrite();
    while(I2C0_BUSY == IC_Close()); // sit here until finished.
}
void I2C_Reset_Buffer(){
    I2C_buffer.Index = 1;
    
}
void I2C_Set_Buffer_Index(uint8_t Index){
    I2C_buffer.Index = Index;
}

void I2C_Fill_Buffer(uint8_t fill){
    memset(I2C_buffer.Buffer+1, fill, I2C_BUFFER_SIZE);
    I2C_buffer.Index = I2C_BUFFER_SIZE;
}
void I2C_Buffer_Add1(uint8_t data) {
  I2C_buffer.Buffer[I2C_buffer.Index++] = data;
}

void I2C_Buffer_Add2(uint8_t data0, uint8_t data1) {
  I2C_buffer.Buffer[I2C_buffer.Index++] = data0;
  I2C_buffer.Buffer[I2C_buffer.Index++] = data1;
}

void I2C_Buffer_Add3(uint8_t data0, uint8_t data1, uint8_t data2) {
  I2C_buffer.Buffer[I2C_buffer.Index++] = data0;
  I2C_buffer.Buffer[I2C_buffer.Index++] = data1;
  I2C_buffer.Buffer[I2C_buffer.Index++] = data2;
}