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


#include "../include/twi0_master.h"
#include <stdbool.h>
#include <stdlib.h>

/***************************************************************************/
// I2C STATES
typedef enum {
    I2C0_IDLE = 0,
    I2C0_SEND_ADR_READ,
    I2C0_SEND_ADR_WRITE,
    I2C0_TX,
    I2C0_RX,
    I2C0_TX_EMPTY,
    I2C0_SEND_RESTART_READ,
    I2C0_SEND_RESTART_WRITE,
    I2C0_SEND_RESTART,
    I2C0_SEND_STOP,
    I2C0_RX_DO_ACK,
    I2C0_TX_DO_ACK,
    I2C0_RX_DO_RX_NACK_STOP,
    I2C0_RX_DO_NACK_RESTART,
    I2C0_RESET,
    I2C0_ADDRESS_NACK,
    I2C0_BUS_COLLISION,
    I2C0_BUS_ERROR
} twi0_fsm_states_t;

// I2C Event Callback List
typedef enum {
    I2C0_DATA_COMPLETE = 0,
    I2C0_WRITE_COLLISION,
    I2C0_ADDRESSNACK,
    I2C0_DATA_NACK,
    I2C0_TIMEOUT,
    I2C0_NULL
} IC_callbackIndex_t;

// I2C Status Structure
typedef struct {
    twi0_callback_t callbackTable[6];
    void *       callbackPayload[6];
    uint16_t         timeout;
    uint16_t         timeout_value;
    twi0_address_t    address;
    uint8_t *        data_ptr;
    size_t           data_length;
    twi0_fsm_states_t state;
    twi0_error_t      error;
    unsigned         addressNACKCheck : 1;
    unsigned         busy : 1;
    unsigned         inUse : 1;
    unsigned         bufferFree : 1;
    /*if timeoutDriverEnabled
    timerStruct_t timeout;
    */
} IC_status_t;
IC_status_t IC_status = {0};
typedef twi0_fsm_states_t(stateHandlerFunction)(void);

/* I2C Interfaces */
void IC_Poller(void);

/* I2C Internal API's */
/* Master */
void IC_MasterOpen(void);
void IC_MasterClose(void);
char IC_MasterGetRxData(void);
void IC_MasterTxData(char d);
void IC_MasterTxAddr(char d);
void IC_MasterResetBus(void);
void IC_MasterIsRxOrTx(void);
void IC_MasterStop(void);
bool IC_MasterIsNack(void);
void IC_MasterSendAck(void);
void IC_MasterSendNack(void);
void IC_MasterClearBusCollision(void);
bool IC_MasterBusErrorOverride();
bool IC_MasterArbitrationlostOverride(void);
void IC_MasterEnableIrq(void);
bool IC_MasterIsIrqEnabled(void);
void IC_MasterDisableIrq(void);
void IC_MasterClearIrq(void);
void IC_MasterWaitForEvent(void);
static void IC_set_callback(IC_callbackIndex_t idx, twi0_callback_t cb, void *funPtr);
static twi0_operations_t IC_RETURN_STOP(void *funPtr);
static twi0_operations_t IC_RETURN_RESET(void *funPtr);
static void IC_MasterIsr(void);

/* Helper Functions */
static twi0_fsm_states_t IC_DO_IDLE(void);
static twi0_fsm_states_t IC_DO_SEND_ADR_READ(void);
static twi0_fsm_states_t IC_DO_SEND_ADR_WRITE(void);
static twi0_fsm_states_t IC_DO_TX(void);
static twi0_fsm_states_t IC_DO_RX(void);
static twi0_fsm_states_t IC_DO_TX_EMPTY(void);
static twi0_fsm_states_t IC_DO_SEND_RESTART_READ(void);
static twi0_fsm_states_t IC_DO_SEND_RESTART_WRITE(void);
static twi0_fsm_states_t IC_DO_SEND_RESTART(void);
static twi0_fsm_states_t IC_DO_SEND_STOP(void);
static twi0_fsm_states_t IC_DO_RX_ACK(void);
static twi0_fsm_states_t IC_DO_TX_ACK(void);
static twi0_fsm_states_t IC_DO_RX_NACK_STOP(void);
static twi0_fsm_states_t IC_DO_RX_NACK_RESTART(void);
static twi0_fsm_states_t IC_DO_RESET(void);
static twi0_fsm_states_t IC_DO_ADDRESS_NACK(void);
static twi0_fsm_states_t IC_DO_BUS_COLLISION(void);
static twi0_fsm_states_t IC_DO_BUS_ERROR(void);

typedef twi0_fsm_states_t(stateHandlerFunction)(void);
stateHandlerFunction *IC_fsmStateTable[] = {
    IC_DO_IDLE,               // I2C_IDLE
    IC_DO_SEND_ADR_READ,      // I2C_SEND_ADR_READ
    IC_DO_SEND_ADR_WRITE,     // I2C_SEND_ADR_WRITE
    IC_DO_TX,                 // I2C_TX
    IC_DO_RX,                 // I2C_RX
    IC_DO_TX_EMPTY,           // I2C_TX_EMPTY
    IC_DO_SEND_RESTART_READ,  // I2C_SEND_RESTART_READ
    IC_DO_SEND_RESTART_WRITE, // I2C_SEND_RESTART_WRITE
    IC_DO_SEND_RESTART,       // I2C_SEND_RESTART
    IC_DO_SEND_STOP,          // I2C_SEND_STOP
    IC_DO_RX_ACK,          // I2C_RX_DO_ACK
    IC_DO_TX_ACK,          // I2C_TX_DO_ACK
    IC_DO_RX_NACK_STOP,       // I2C_RX_DO_NACK_STOP
    IC_DO_RX_NACK_RESTART,    // I2C_RX_DO_NACK_RESTART
    IC_DO_RESET,              // I2C_RESET
    IC_DO_ADDRESS_NACK,    // I2C_ADDRESS_NACK
    IC_DO_BUS_COLLISION,      // I2C_BUS_COLLISION
    IC_DO_BUS_ERROR           // I2C_BUS_ERROR
};

void IC_SetDataCompleteCallback(twi0_callback_t cb, void *funPtr)
{
    IC_set_callback(I2C0_DATA_COMPLETE, cb, funPtr);
}

void IC_SetWriteCollisionCallback(twi0_callback_t cb, void *funPtr)
{
    IC_set_callback(I2C0_WRITE_COLLISION, cb, funPtr);
}

void IC_SetAddressNackCallback(twi0_callback_t cb, void *funPtr)
{
    IC_set_callback(I2C0_ADDRESSNACK, cb, funPtr);
}

void IC_SetDataNackCallback(twi0_callback_t cb, void *funPtr)
{
    IC_set_callback(I2C0_DATA_NACK, cb, funPtr);
}

void IC_SetTimeoutCallback(twi0_callback_t cb, void *funPtr)
{
    IC_set_callback(I2C0_TIMEOUT, cb, funPtr);
}

uint8_t IC_Initialize()
{
    //SDASETUP 4CYC; SDAHOLD OFF; FMPEN disabled; 
    TWI0.CTRLA = 0x00;
    
    //Debug Run
    TWI0.DBGCTRL = 0x00;
    
    //Master Baud Rate Control
    TWI0.MBAUD = (uint8_t)TWI0_BAUD(100000, 0);
    
    //RIEN disabled; WIEN disabled; QCEN disabled; TIMEOUT DISABLED; SMEN disabled; ENABLE enabled; 
    TWI0.MCTRLA = 0x01;
    
    //RIF disabled; WIF disabled; CLKHOLD disabled; ARBLOST disabled; BUSERR disabled; BUSSTATE UNKNOWN; 
    TWI0.MSTATUS = 0x00;
    
    //Master Address
    TWI0.MADDR = 0x00;
    
    //FLUSH disabled; ACKACT ACK; MCMD NOACT; 
    TWI0.MCTRLB = 0x00;
    
    //Master Data
    TWI0.MDATA = 0x00;
    
    return 0;
}

// when you call open, you supply a device address.
// if you get the bus, the function returns true
twi0_error_t IC_Open(twi0_address_t address)
{
    twi0_error_t ret = I2C0_BUSY;

    if (!IC_status.inUse) {
        IC_status.address          = address;
        IC_status.busy             = 0;
        IC_status.inUse            = 1;
        IC_status.addressNACKCheck = 0;
        IC_status.state            = I2C0_RESET;
        IC_status.timeout_value    = 500; // MCC should determine a reasonable starting value here.
        IC_status.bufferFree       = 1;
        
        // set all the call backs to a default of sending stop
        IC_status.callbackTable[I2C0_DATA_COMPLETE]     = IC_RETURN_STOP;
        IC_status.callbackPayload[I2C0_DATA_COMPLETE]   = NULL;
        IC_status.callbackTable[I2C0_WRITE_COLLISION]   = IC_RETURN_STOP;
        IC_status.callbackPayload[I2C0_WRITE_COLLISION] = NULL;
        IC_status.callbackTable[I2C0_ADDRESSNACK]      = IC_RETURN_STOP;
        IC_status.callbackPayload[I2C0_ADDRESSNACK]    = NULL;
        IC_status.callbackTable[I2C0_DATA_NACK]         = IC_RETURN_STOP;
        IC_status.callbackPayload[I2C0_DATA_NACK]       = NULL;
        IC_status.callbackTable[I2C0_TIMEOUT]          = IC_RETURN_RESET;
        IC_status.callbackPayload[I2C0_TIMEOUT]        = NULL;

        IC_MasterResetBus();
        // Reset module
        IC_MasterClearIrq();

        ret = I2C0_NOERR;
    }
    return ret;
}

void IC_SetAddress(twi0_address_t address)
{
    IC_status.address = address;
}

// close the bus if it is not busy
twi0_error_t IC_Close(void)
{
    twi0_error_t ret = I2C0_BUSY;
    // Bus is in error state, reset I2C hardware and report error
    if (IC_MasterBusErrorOverride()) {
        IC_status.busy  = false;
        IC_status.error = I2C0_FAIL;
    }
    if (!IC_status.busy) {
        IC_status.inUse = 0;
        // close it down
        IC_status.address = 0xff; // 8-bit address is invalid so this is FREE
        IC_MasterClearIrq();
        IC_MasterDisableIrq();
        ret = IC_status.error;
    }
    return ret;
}

void IC_SetTimeout(uint8_t to)
{
    IC_MasterDisableIrq();
    IC_status.timeout_value = to;
    IC_MasterEnableIrq();
}

void IC_SetBuffer(void *buffer, size_t bufferSize)
{
    if (IC_status.bufferFree) {
        IC_status.data_ptr    = buffer;
        IC_status.data_length = bufferSize;
        IC_status.bufferFree  = false;
    }
}
twi0_error_t IC_MasterOperation(bool read)
{
    twi0_error_t ret = I2C0_BUSY;
    if (!IC_status.busy) {
        IC_status.busy = true;
        ret                       = I2C0_NOERR;

        if (read) {
            IC_status.state = I2C0_SEND_ADR_READ;
        } else {
            IC_status.state = I2C0_SEND_ADR_WRITE;
        }
        IC_MasterIsr();

        IC_Poller();
    }
    return ret;
}

twi0_error_t IC_MasterRead(void)

{
    return IC_MasterOperation(true);
}



twi0_error_t IC_MasterWrite(void)
{
    return IC_MasterOperation(false);
}

/************************************************************************/
/* Helper Functions                                                     */
/************************************************************************/

void IC_Poller(void)
{
    while (IC_status.busy)
    {
        IC_MasterWaitForEvent();
        IC_MasterIsr();
    }
}

static twi0_fsm_states_t IC_DO_RESET(void)
{
    IC_MasterResetBus();
    IC_status.busy  = false; // Bus Free
    IC_status.error = I2C0_NOERR;
    return I2C0_RESET; // park the FSM on reset
}

static twi0_fsm_states_t IC_DO_IDLE(void)
{
    IC_status.busy  = false; // Bus Free
    IC_status.error = I2C0_NOERR;
    return I2C0_IDLE; // park the FSM on IDLE
}

static twi0_fsm_states_t IC_DO_SEND_RESTART_READ(void)
{
    return IC_DO_SEND_ADR_READ();
}

static twi0_fsm_states_t IC_DO_SEND_RESTART_WRITE(void)
{
    return IC_DO_SEND_ADR_WRITE();
}

static twi0_fsm_states_t IC_DO_SEND_RESTART(void)
{
    return IC_DO_SEND_ADR_READ();
}

static twi0_fsm_states_t IC_DO_SEND_STOP(void)
{
    IC_MasterStop();
    return IC_DO_IDLE();
}

// TODO: probably need 2 addressNACK's one from read and one from write.
//       the do NACK before RESTART or STOP is a special case that a new state simplifies.
static twi0_fsm_states_t IC_DO_ADDRESS_NACK(void)
{
    IC_status.addressNACKCheck = 0;
    IC_status.error            = I2C0_FAIL;
    switch (IC_status.callbackTable[I2C0_ADDRESSNACK](IC_status.callbackPayload[I2C0_ADDRESSNACK])) {
    case I2C0_RESTART_READ:
        return IC_DO_SEND_RESTART_READ();
    case I2C0_RESTART_WRITE:
        return IC_DO_SEND_RESTART_WRITE();
    default:
        return IC_DO_SEND_STOP();
    }
}

static twi0_fsm_states_t IC_DO_SEND_ADR_READ(void)
{

    IC_status.addressNACKCheck = 1;
    IC_MasterTxAddr(IC_status.address << 1 | 1);
    return I2C0_RX;
}

static twi0_fsm_states_t IC_DO_SEND_ADR_WRITE(void)
{

    IC_status.addressNACKCheck = 1;
    IC_MasterTxAddr(IC_status.address << 1);
    return I2C0_TX;
}

static twi0_fsm_states_t IC_DO_RX_ACK(void)
{
    IC_MasterSendAck();
    return I2C0_RX;
}

static twi0_fsm_states_t IC_DO_TX_ACK(void)
{
    IC_MasterSendAck();
    return I2C0_TX;
}

static twi0_fsm_states_t IC_DO_RX_NACK_STOP(void)
{
    IC_MasterSendNack();
    IC_MasterStop();
    return IC_DO_IDLE();
}

static twi0_fsm_states_t IC_DO_RX_NACK_RESTART(void)
{
    IC_MasterSendNack();
    return I2C0_SEND_RESTART;
}

static twi0_fsm_states_t IC_DO_TX(void)
{
    if (IC_MasterIsNack()) // Slave replied with NACK
    {
        switch (IC_status.callbackTable[I2C0_DATA_NACK](IC_status.callbackPayload[I2C0_DATA_NACK])) {
        case I2C0_RESTART_READ:
            return IC_DO_SEND_RESTART_READ();
        case I2C0_RESTART_WRITE:
            return IC_DO_SEND_RESTART_WRITE();
        default:
        case I2C0_CONTINUE:
        case I2C0_STOP:
            return IC_DO_SEND_STOP();
        }
    } else {
        IC_status.addressNACKCheck = 0;
        IC_MasterTxData(*IC_status.data_ptr++);
        return (--IC_status.data_length) ? I2C0_TX : I2C0_TX_EMPTY;
    }
}

static twi0_fsm_states_t IC_DO_RX(void)
{
    IC_status.addressNACKCheck = 0;

    if (IC_status.data_length == 1)
        IC_MasterSendNack(); // Next byte will be last to be received, setup NACK
    else
        IC_MasterSendAck(); // More bytes to receive, setup ACK

    if (--IC_status.data_length) {
        *IC_status.data_ptr = IC_MasterGetRxData();
        IC_status.data_ptr++;
        IC_MasterIsRxOrTx();
        return I2C0_RX;
    } else {
        *IC_status.data_ptr = IC_MasterGetRxData();
        IC_status.data_ptr++;
        IC_status.bufferFree = true;
        switch (IC_status.callbackTable[I2C0_DATA_COMPLETE](IC_status.callbackPayload[I2C0_DATA_COMPLETE])) {
        case I2C0_RESTART_WRITE:
        case I2C0_RESTART_READ:
            return IC_DO_RX_NACK_RESTART();
        default:
        case I2C0_CONTINUE:
        case I2C0_STOP:
            return IC_DO_RX_NACK_STOP();
        }
    }
}

static twi0_fsm_states_t IC_DO_TX_EMPTY(void)
{
    if (IC_MasterIsNack()) // Slave replied with NACK
    {
        switch (IC_status.callbackTable[I2C0_DATA_NACK](IC_status.callbackPayload[I2C0_DATA_NACK])) {
        case I2C0_RESTART_READ:
            return IC_DO_SEND_RESTART_READ();
        case I2C0_RESTART_WRITE:
            return IC_DO_SEND_RESTART_WRITE();
        default:
        case I2C0_CONTINUE:
        case I2C0_STOP:
            return IC_DO_SEND_STOP();
        }
    } else {
        IC_status.bufferFree = true;
        switch (IC_status.callbackTable[I2C0_DATA_COMPLETE](IC_status.callbackPayload[I2C0_DATA_COMPLETE])) {
        case I2C0_RESTART_READ:
            return IC_DO_SEND_RESTART_READ();
        case I2C0_RESTART_WRITE:
            return IC_DO_SEND_RESTART_WRITE();
        case I2C0_CONTINUE:
            return IC_DO_TX();
        default:
        case I2C0_STOP:
            return IC_DO_SEND_STOP();
        }
    }
}

static twi0_fsm_states_t IC_DO_BUS_COLLISION(void)
{
    // Clear bus collision status flag
    IC_MasterClearBusCollision();

    IC_status.error = I2C0_FAIL;
    switch (IC_status.callbackTable[I2C0_WRITE_COLLISION](IC_status.callbackPayload[I2C0_WRITE_COLLISION])) {
    case I2C0_RESTART_READ:
        return IC_DO_SEND_RESTART_READ();
    case I2C0_RESTART_WRITE:
        return IC_DO_SEND_RESTART_WRITE();
    default:
        return IC_DO_RESET();
    }
}

static twi0_fsm_states_t IC_DO_BUS_ERROR(void)
{
    IC_MasterResetBus();
    IC_status.busy  = false;
    IC_status.error = I2C0_FAIL;
    return I2C0_RESET; // park the FSM on reset
}



void IC_MasterIsr(void)
{
    IC_MasterClearIrq();

    // NOTE: We are ignoring the Write Collision flag.

    // Address phase received NACK from slave, override next state
    if (IC_status.addressNACKCheck && IC_MasterIsNack()) {
        IC_status.state = I2C0_ADDRESS_NACK; // State Override
    }

    // Bus arbitration lost to another master, override next state
    if (IC_MasterArbitrationlostOverride()) {
        IC_status.state = I2C0_BUS_COLLISION; // State Override
    }

    // Bus error, override next state
    if (IC_MasterBusErrorOverride()) {
        IC_status.state = I2C0_BUS_ERROR; // State Override
    }

    IC_status.state = IC_fsmStateTable[IC_status.state]();
}

/************************************************************************/
/* Helper Functions                                                     */
/************************************************************************/
static twi0_operations_t IC_RETURN_STOP(void *p)
{
    return I2C0_STOP;
}

static twi0_operations_t IC_RETURN_RESET(void *p)
{
    return I2C0_RESET_LINK;
}

static void IC_set_callback(IC_callbackIndex_t idx, twi0_callback_t cb, void *funPtr)
{
    if (cb) {
        IC_status.callbackTable[idx]   = cb;
        IC_status.callbackPayload[idx] = funPtr;
    } else {
        IC_status.callbackTable[idx]   = IC_RETURN_STOP;
        IC_status.callbackPayload[idx] = NULL;
    }
}

/* Master Definitions */

void IC_MasterOpen(void)
{
    TWI0.MCTRLA = 1 << TWI_ENABLE_bp;
}

void IC_MasterClose(void)
{
    TWI0.MCTRLA = 0 << TWI_ENABLE_bp;
}

/* Interrupt Control */
void IC_MasterEnableIrq(void)
{
    TWI0.MCTRLA |= (TWI_RIEN_bm | TWI_WIEN_bm);
}

bool IC_MasterIsIrqEnabled(void)
{
    return ((TWI0.MCTRLA & TWI_WIEN_bm) && (TWI0.MCTRLA & TWI_RIEN_bm));
}

void IC_MasterDisableIrq(void)
{
    TWI0.MCTRLA &= ~(TWI_RIEN_bm | TWI_WIEN_bm);
}

void IC_MasterClearIrq(void)
{
    TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
}

bool IC_MasterBusErrorOverride(void)
{
    return TWI0.MSTATUS & TWI_BUSERR_bm;
}

bool IC_MasterArbitrationlostOverride(void)
{
    return TWI0.MSTATUS & TWI_ARBLOST_bm;
}

void IC_MasterResetBus(void)
{
    TWI0.MCTRLB |= TWI_FLUSH_bm;
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

void IC_MasterIsRxOrTx(void)
{
    TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;
}

void IC_MasterClearBusCollision(void)
{
    TWI0.MSTATUS |= TWI_ARBLOST_bm;
}

void IC_MasterWaitForEvent(void)
{
   while (!(TWI0.MSTATUS & TWI_RIF_bm) && !(TWI0.MSTATUS & TWI_WIF_bm)) 
   {
   };
}

void IC_MasterStop(void)
{
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

bool IC_MasterIsNack(void)
{
    return TWI0.MSTATUS & TWI_RXACK_bm;
}

char IC_MasterGetRxData(void)
{
    return TWI0.MDATA;
}

void IC_MasterTxData(char d)
{
    TWI0.MDATA = d;
}

void IC_MasterTxAddr(char d)
{
    TWI0.MADDR = d;
}

void IC_MasterSendAck(void)
{
    TWI0.MCTRLB &= ~(1 << TWI_ACKACT_bp);
}

void IC_MasterSendNack(void)
{
    TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
}

twi0_operations_t IC_SetReturnStopCallback(void *funPtr)
{
    return I2C0_STOP;
}

twi0_operations_t IC_SetReturnResetCallback(void *funPtr)
{
    return I2C0_RESET_LINK;
}

twi0_operations_t IC_SetRestartWriteCallback(void *funPtr)
{
    return I2C0_RESTART_WRITE;
}

twi0_operations_t IC_SetRestartReadCallback(void *funPtr)
{
    return I2C0_RESTART_READ;
}