/*******************************************************************************
 *
 * Copyright (c) 2019 
 *
 * Rafael Cruz & José Cecílio
 *
 *******************************************************************************/

#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/ioctl.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>


// #############################################
// #############################################

#include "lora.h"
#include "draginoLora.h"

// #############################################
// #############################################
//


void die(const char *s)
{
    perror(s);
    exit(1);
}

void selectreceiver()
{
    digitalWrite(ssPin, LOW);
}

void unselectreceiver()
{
    digitalWrite(ssPin, HIGH);
}

byte readReg(byte addr)
{
    unsigned char spibuf[2];

    selectreceiver();
    spibuf[0] = addr & 0x7F;
    spibuf[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);
    unselectreceiver();

    return spibuf[1];
}

void writeReg(byte addr, byte value)
{
    unsigned char spibuf[2];

    spibuf[0] = addr | 0x80;
    spibuf[1] = value;
    selectreceiver();
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);

    unselectreceiver();
}

void opmode (uint8_t mode) {
    writeReg(REG_OPMODE, (readReg(REG_OPMODE) & ~OPMODE_MASK) | mode);
}

void opmodeLora() {
    uint8_t u = OPMODE_LORA;
    if (sx1272 == false)
        u |= 0x8;   // TBD: sx1276 high freq
    writeReg(REG_OPMODE, u);
}


void SetupLoRa()
{
    
    digitalWrite(RST, HIGH);
    delay(100);
    digitalWrite(RST, LOW);
    delay(100);

    byte version = readReg(REG_VERSION);

    if (version == 0x22) {
        // sx1272
        printf("SX1272 detected, starting.\n");
        sx1272 = true;
    } else {
        // sx1276?
        digitalWrite(RST, LOW);
        delay(100);
        digitalWrite(RST, HIGH);
        delay(100);
        version = readReg(REG_VERSION);
        if (version == 0x12) {
            // sx1276
            //printf("SX1276 detected, starting.\n");
            sx1272 = false;
        } else {
            printf("Unrecognized transceiver.\n");
            //printf("Version: 0x%x\n",version);
            exit(1);
        }
    }

    opmode(OPMODE_SLEEP);

    // set frequency
    uint64_t frf = ((uint64_t)freq << 19) / 32000000;
    writeReg(REG_FRF_MSB, (uint8_t)(frf>>16) );
    writeReg(REG_FRF_MID, (uint8_t)(frf>> 8) );
    writeReg(REG_FRF_LSB, (uint8_t)(frf>> 0) );

    writeReg(REG_SYNC_WORD, 0x34); // LoRaWAN public sync word

    if (sx1272) {
        if (sf == SF11 || sf == SF12) {
            writeReg(REG_MODEM_CONFIG,0x0B);
        } else {
            writeReg(REG_MODEM_CONFIG,0x0A);
        }
        writeReg(REG_MODEM_CONFIG2,(sf<<4) | 0x04);
    } else {
        if (sf == SF11 || sf == SF12) {
            writeReg(REG_MODEM_CONFIG3,0x0C);
        } else {
            writeReg(REG_MODEM_CONFIG3,0x04);
        }
        writeReg(REG_MODEM_CONFIG,0x72);
        writeReg(REG_MODEM_CONFIG2,(sf<<4) | 0x04);
    }

    if (sf == SF10 || sf == SF11 || sf == SF12) {
        writeReg(REG_SYMB_TIMEOUT_LSB,0x05);
    } else {
        writeReg(REG_SYMB_TIMEOUT_LSB,0x08);
    }
    writeReg(REG_MAX_PAYLOAD_LENGTH,0x80);
    writeReg(REG_PAYLOAD_LENGTH,PAYLOAD_LENGTH);
    writeReg(REG_HOP_PERIOD,0xFF);
    writeReg(REG_FIFO_ADDR_PTR, readReg(REG_FIFO_RX_BASE_AD));

    writeReg(REG_LNA, LNA_MAX_GAIN);

}

void configPower (int8_t pw) {
    if (sx1272 == false) {
        // no boost used for now
        if(pw >= 17) {
            pw = 15;
        } else if(pw < 2) {
            pw = 2;
        }
        // check board type for BOOST pin
        writeReg(RegPaConfig, (uint8_t)(0x80|(pw&0xf)));
        writeReg(RegPaDac, readReg(RegPaDac)|0x4);

    } else {
        // set PA config (2-17 dBm using PA_BOOST)
        if(pw > 17) {
            pw = 17;
        } else if(pw < 2) {
            pw = 2;
        }
        writeReg(RegPaConfig, (uint8_t)(0x80|(pw-2)));
    }
}


void writeBuf(byte addr, byte *value, byte len) {                                                       
    unsigned char spibuf[256];                                                                          
    spibuf[0] = addr | 0x80;                                                                            
    for (int i = 0; i < len; i++) {                                                                         
        spibuf[i + 1] = value[i];                                                                       
    }                                                                                                   
    selectreceiver();                                                                                   
    wiringPiSPIDataRW(CHANNEL, spibuf, len + 1);                                                        
    unselectreceiver();                                                                                 
}

void txlora(byte *frame, byte datalen) {

    // set the IRQ mapping DIO0=TxDone DIO1=NOP DIO2=NOP
    writeReg(RegDioMapping1, MAP_DIO0_LORA_TXDONE|MAP_DIO1_LORA_NOP|MAP_DIO2_LORA_NOP);
    // clear all radio IRQ flags
    writeReg(REG_IRQ_FLAGS, 0xFF);
    // mask all IRQs but TxDone
    writeReg(REG_IRQ_FLAGS_MASK, ~IRQ_LORA_TXDONE_MASK);

    // initialize the payload size and address pointers
    writeReg(REG_FIFO_TX_BASE_AD, 0x00);
    writeReg(REG_FIFO_ADDR_PTR, 0x00);
    writeReg(REG_PAYLOAD_LENGTH, datalen);

    // download buffer to the radio FIFO
    writeBuf(REG_FIFO, frame, datalen);
    // now we actually start the transmission
    opmode(OPMODE_TX);

}


void clean(char *var) {
    int i = 0;
    while(var[i] != '\0') {
        var[i] = '\0';
        i++;
    }
}

//function to convert string to byte array
void string2ByteArray(char* input, byte* output)
{
    int loop;
    int i;
    
    loop = 0;
    i = 0;
    
    while(input[loop] != '\0')
    {
        output[i++] = input[loop++];
    }
}
