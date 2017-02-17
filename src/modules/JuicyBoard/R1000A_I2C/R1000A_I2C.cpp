/*
 * R1000A_I2C.cpp
 *
 *  Created on: Sep 17, 2016
 *      Author: sherifeid
 */

#include "R1000A_I2C.h"
#include "libs/Kernel.h"


R1000A_I2C::R1000A_I2C(){
    // Default Constructor
    this->i2c = new mbed::I2C(P0_27, P0_28);    // define master i2c comm class
    this->i2c->frequency(200000);               // set I2C bus freq in Hz
}

R1000A_I2C::~R1000A_I2C(){
    // Destructor
}

int R1000A_I2C::I2C_ReadREG(int slotnum, char REGAddr, char * data, int length){
    // perform burst register read
    int i;                                      // for loop variable
    char I2CAddr = getSlotI2CAdd(slotnum);      // get slot I2C address
    // set the register to access
    this->i2c->start();
    if (this->i2c->write(I2CAddr) != 1){        // check for slave ack
        // slave I2C is not acknowledging, exit function
        this->i2c->stop();
        return -1;
    }
    this->i2c->write(REGAddr);                  // register address
    this->i2c->stop();

    // read part
    this->i2c->start();
    this->i2c->write(I2CAddr | 0x01);           // slave I2C address, with read command
    for (i=0; i<length; i++){                   // loop over every byte
        data[i] = this->i2c->read(1);
    }
    this->i2c->read(0);                         // extra dummy read for mbed I2C to stop properly
    this->i2c->stop();
    return 0;
}

int R1000A_I2C::I2C_WriteREG(int slotnum, char REGAddr, char * data, int length){
    // perform burst register read
    int i;                                      // for loop variable
    char I2CAddr = getSlotI2CAdd(slotnum);     // get slot I2C address
    // set the register to access
    this->i2c->start();
    if (this->i2c->write(I2CAddr) != 1){        // check for slave ack
        // slave I2C is not acknowledging, exit function
        this->i2c->stop();
        return -1;
    }
    this->i2c->write(REGAddr);                  // register address
    for (i=0; i<length; i++){
        this->i2c->write(data[i]);              // write data one by one
    }
    this->i2c->stop();
    return 0;
}

int R1000A_I2C::I2C_CheckAddr(char I2CAddr){
    // check if I2C address acknowledges
    // set the register to access
    this->i2c->start();
    if (this->i2c->write((I2CAddr << 1) | 0x01) != 1){        // check for slave ack
        // slave I2C is not acknowledging, exit function
        this->i2c->stop();
        return -1;
    }
    else
    {
        this->i2c->stop();
        return 0;
    }
}

char R1000A_I2C::getSlotI2CAdd(int slotnum){
    // returns I2C address of the specific slot
    // This slot numbers are ordered as follows
    // Slots 1~15 are pluggable modules
    // Slot 100 is reserved for the power monitor chip
    // Slot 200 is reserved for the EEPROM
    if (slotnum == PWRMON_SLOT){
        return (PWRMON_BASE << 1);
    }
    else if ((slotnum >= EEPROM_SLOT_BASE) && (slotnum < (EEPROM_SLOT_BASE + EEPROM_NUM_SLOTS))){
        // this returns the address of EEPROM section, limit is 4 sections x 256 bytes
        return ((EEPROM_I2C_BASE + slotnum - EEPROM_SLOT_BASE) << 1);
    }
    else{
        return ((R1000_I2C_BASE + slotnum - 1) << 1);
    }
}

char R1000A_I2C::getBLSlotI2CAdd(int slotnum){
    // returns I2C address of the specific slot
    // when in bootloader mode
    return ((R1000_I2C_BLBASE + slotnum - 1) << 1);
}

