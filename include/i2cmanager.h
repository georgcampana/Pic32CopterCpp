/* 
 * File:   i2cmanager.h
 * Author: georg
 *
 * Created on 22 ottobre 2012, 0.30
 */

#ifndef I2CMANAGER_H
#define	I2CMANAGER_H


class i2chandler {
    enum STATUS {

        NOT_INIT = 0,
        BUS_IDLE ,
        START_SENT,
        S_ADDRESS_SENT,
        S_ADDR_ACK_RECEIVED,
        DATA_SENT,
        DATA_ACK_RECEIVED,
        RESTART_SENT,
        RS_ADDRESS_SENT,
        RS_ADDR_ACK_RECEIVED,
        DATA_RECEIVED,
        DATA_ACK_SENT,

        DATA_NACK_SENT,

        STOP_SENT
    };

    enum ACCESS_SCHEMA {

    };

    STATUS currentstatus = NOT_INIT;
    ACCESS_SCHEMA currentschema;

public:
    //constructor
    void i2chandler();

};


extern i2chandler SPIHandler();

#ifdef	__cplusplus
extern "C" {
#endif

int i2cm_IsBusy();




#ifdef	__cplusplus
}
#endif

#endif	/* I2CMANAGER_H */

