/*******************************************************************************
 *
 * Copyright (c) 2019 
 *
 * Rafael Cruz & José Cecílio
 *
 *******************************************************************************/

#ifndef DRAGINOLORA_H_   /* Include guard */
#define DRAGINOLORA_H_

#include <stdio.h>
#include "lora.h"

/*******************************************************************************
 *
 * Configure these values!
 *
 *******************************************************************************/

// SX1272 - Raspberry connections
static int ssPin = 6;
static int dio0  = 7;
static int RST   = 0;

// Set spreading factor (SF7 - SF12)
static sf_t sf = SF12;

// Set center frequency
static uint32_t  freq = 868100000; // in Mhz! (868.1)

static const int CHANNEL = 0;

static bool sx1272 = true;



/*******************************************************************************
 *
 * end of Configure these values!
 *
 *******************************************************************************/
 

typedef bool boolean;
typedef unsigned char byte;

void die(const char *s);
void selectreceiver();
void unselectreceiver();
byte readReg(byte addr);
void writeReg(byte addr, byte value);
void opmode (uint8_t mode);
void opmodeLora();
void SetupLoRa();
void clean(char *var);
void configPower (int8_t pw);
void writeBuf(byte addr, byte *value, byte len);
void txlora(byte *frame, byte datalen);
void string2ByteArray(char* input, byte* output);


#endif
