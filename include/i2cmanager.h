/* 
 * File:   i2cmanager.h
 * Author: georg
 *
 * Created on 22 ottobre 2012, 0.30
 */

#ifndef I2CMANAGER_H
#define	I2CMANAGER_H

#include "system.h"

class I2c {
    enum STATUS {

        NOT_INIT = 0,
        BUS_IDLE ,
        START_SENT,
        S_ADDRESS_SENT,
        //S_ADDR_ACK_RECEIVED,
        REG_ADDR_SENT,
        DATA_SENT,
        //DATA_ACK_RECEIVED,
        RESTART_SENT,
        RS_ADDRESS_SENT,
        DATA_RECEIVE_ENABLED,
        //RS_ADDR_ACK_RECEIVED,
        DATA_RECEIVED,
        DATA_ACK_SENT,

        DATA_NACK_SENT,

        STOP_SENT
    };

    enum ACCESS_SCHEMA {
        NO_SCHEMA,

        WRITE_REG,
        READ_REG,

        REG_BASED_SCHEMA
    };
public:
    enum BusError {
        NO_ERROR = 0,
        START_COLLISION,
        SENDADDRESS_COLLISION,
        SENDADDRESS_NOTACKED,
        SENDREGADDR_COLLISION,
        RESTART_COLLISION,
        WRITEBYTE_COLLISION,
        REGADDR_NOTACKED,
        RESENDADDR_COLLISION,
        RESENDADDR_NOTACKED,
        RECEIVE_OVERFLOW,
        RCV_NODATA,
        WRITE_NOTACKED,
        WRITEXBYTE_COLLISION // differs from WRITEBYTE_COLLISION because raised for n bytes after the first

    };

private:
//    class TransferHandler {
//    public:
//        bool handleInterrupt();
//    };
//
//    class ReadByteHandler : TransferHandler {
//    public:
//        bool handleInterrupt();
//        ReadByteHandler();
//    };

    STATUS currentstatus;
    ACCESS_SCHEMA currentschema;
    
    I2C_MODULE module;

    UINT8 deviceaddr;
    UINT8 regaddress;
    const UINT8* datatxptr;
    UINT8* datarxptr;
    UINT16 datalen;
    bool   buserror;
    BusError errortype;

    void setupInterrupt();

    void setBusError(BusError type);
    void resetAnyBusError();

  public:
    void handleInterrupt();

    //constructor
    I2c(I2C_MODULE mod, UINT32 perif_freq, UINT32 i2c_freq);

    BusError getErrorType();

    bool isBusy();
    bool StartReadByteFromReg(UINT8 devaddress, UINT8 regaddress, UINT8* valuedest);
    bool StartWriteByteToReg(UINT8 devaddress, UINT8 regaddress, UINT8 value);

    bool StartReadFromReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, UINT8* valuesdest);
    bool StartWriteToReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, const UINT8* values);

    bool ReadByteFromReg(UINT8 devaddress, UINT8 regaddress, UINT8* valuedest);
    bool WriteByteToReg(UINT8 devaddress, UINT8 regaddress, UINT8 value);

    bool ReadFromReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, UINT8* valuesdest);
    bool WriteToReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, const UINT8* values);
};

inline bool I2c::isBusy() {  return (currentstatus > BUS_IDLE)? true:false; }
inline void I2c::setBusError(BusError type){
    buserror = true;
    errortype = type;
}

inline void I2c::resetAnyBusError(){
    buserror = false;
    errortype = NO_ERROR;
}

inline I2c::BusError I2c::getErrorType(){
    return errortype;
}

#ifdef	__cplusplus
extern "C" {
#endif

//void __ISR(_I2C_1_VECTOR, ipl5) I2cInterruptServiceRoutine(void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2CMANAGER_H */

