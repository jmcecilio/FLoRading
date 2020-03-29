/*******************************************************************************
 *
 * Copyright (c) 2019 
 *
 * Rafael Cruz & José Cecílio
 *
 *******************************************************************************/
#include <gps.h>
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
#include "gps_hat.h"
#include "checksum.h"


#define SENDING_PERIOD 2500   //Tem de ser superior a 1307 :-)
// #############################################
// #############################################
//

char filename[] = "/home/pi/Desktop/embIoT/EWSN_app/RPI/logs/";
char full_filename [128];

char message[256];
byte msgFrom;
byte msgTo;
byte msgCountFromNode;
int msgLength;
byte rcvCrc [2];

byte receivedbytes;

byte myNodeAddress = 0x1A;

byte hello[32] = "HELLO: ";
char gpsTextToOutput [128];

uint8_t ncount = 0;

uint8_t type_of_exp = 0;


boolean receive(char *payload) {
    
    
    
    // clear rxDone
    writeReg(REG_IRQ_FLAGS, 0x40);

    int irqflags = readReg(REG_IRQ_FLAGS);

    //  payload crc: 0x20
    if((irqflags & 0x20) == 0x20)
    {
        printf("CRC error\n");
        writeReg(REG_IRQ_FLAGS, 0x20);
        return false;
    } else {

        byte currentAddr = readReg(REG_FIFO_RX_CURRENT_ADDR);
        byte receivedCount = readReg(REG_RX_NB_BYTES);
        receivedbytes = receivedCount;

        writeReg(REG_FIFO_ADDR_PTR, currentAddr);
        
        msgTo = (byte)readReg(REG_FIFO);
        msgFrom = (byte)readReg(REG_FIFO);
        msgCountFromNode = (byte)readReg(REG_FIFO);
        msgLength = (int)readReg(REG_FIFO);
        
        //printf("Aqui...%x, %x, %d\n",msgTo, msgFrom, msgLength);
        
        if (msgTo == myNodeAddress){  //endreço do nó
            for(int i = 0; i < msgLength-2; i++)
            {
                payload[i] = (char)readReg(REG_FIFO);
            }
            
            rcvCrc[0] = (byte)readReg(REG_FIFO);
            rcvCrc[1] = (byte)readReg(REG_FIFO);
            
            //printf("crc: %x%x\n", rcvCrc[0],rcvCrc[1]);
            unsigned char calCRC [2];
            solve_CRC (payload, &calCRC[0]);
            //printf("CRC: %x%x\n", calCRC[0],calCRC[1]);

            
            if ((rcvCrc[0] == calCRC[0]) && (rcvCrc[1] == calCRC[1]) ){
                //printf("CRC ok\n");
                return true;
            }
            
            
            
            
            
            return false;
        }
    }
    return false;
}


void settime_ms(long int * var){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    *var = tp.tv_sec * 1000 + tp.tv_usec / 1000;
}
void settime_s(long int * var){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    *var = tp.tv_sec;
}

void writeToFile(char data[]){
    
    
    
    struct timeval tp;
    
    FILE * fPtr;
    
    /* 
     * Open file in w (write) mode. 
     * "data/file1.txt" is complete path to create file
     */
 
        
    fPtr = fopen(full_filename, "a");


    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }
    
    
    long int     current_ts;
    
    // Get current time
    settime_ms(&current_ts);
    char textToOutput [1024];
    
    sprintf(textToOutput, "%lu;%s;%s\n",current_ts, gpsTextToOutput, data);

    printf("%s", textToOutput);  // Print data to stdout
        /* Write data to file */
    fputs(textToOutput, fPtr);
    
                /* Close file to save file data */
        fclose(fPtr);
}

void send_message(long int SNR, int rssicorr){
    ncount++;
            
            byte frame[256];
                
            frame[1] = myNodeAddress;    //Origin
            frame[2] = ncount;       //Msg Count


            frame[0] = msgFrom;    //destination
            int size = 4;
            // enter standby mode (required for FIFO loading))
            SetupLoRa();
            opmodeLora();
            opmode(OPMODE_STANDBY);

            writeReg(RegPaRamp, (readReg(RegPaRamp) & 0xF0) | 0x08); // set PA ramp-up time 50 uSec

            configPower(27);
            
            char ts[32];
            if (type_of_exp == 1){
                long int current_ts;
                settime_ms(&current_ts);
                sprintf(ts, "%lu", current_ts);
            } else {
                memcpy( ts, &message[7], 9 );
                ts[9] = '\0';
            }
            
            char textToSend [1024];
            sprintf(textToSend, "0x%x;0x%x;%d;%s;%d;%d;%s", 
            msgFrom, 
            msgTo,
            msgCountFromNode,
            ts,
            readReg(0x1A)-rssicorr, 
            SNR, 
            gpsTextToOutput);

            writeToFile(textToSend);

            for(unsigned int i = 0; i < strlen((char *)textToSend); i++)
            {
                frame[size++] = textToSend[i];
            }
                    
                    
            char payload[256];
            clean (payload);
            int size_payload = 0;
            for(unsigned int i = 4; i < size; i++)
            {
                payload[size_payload++] = frame[i];
            }
            payload[size_payload]='\n';
            payload[size_payload+1]='\0';

            printf("%s\n", payload);

            unsigned char calCRC [2];
            solve_CRC (&payload[0], &calCRC[0]);
                    
            //printf("CRC: %x%x\n", calCRC[0],calCRC[1]);
                    
            frame[size++] = calCRC[0];
            frame[size++] = calCRC[1];
                    
            frame[3] = strlen((char *)textToSend) + 2;       //Size
            //frame[4] = 0;
                    
            txlora(frame, size);
            delay(500);
                    
                    
            // RECEIVE MODE
            SetupLoRa();
            opmodeLora();
            opmode(OPMODE_STANDBY);
            opmode(OPMODE_RX);
}

byte receivepacket() {

    long int SNR;
    int rssicorr;

    if(digitalRead(dio0) == 1)
    {
        clean (message);
        if(receive(message)) {
            byte value = readReg(REG_PKT_SNR_VALUE);
            if( value & 0x80 ) // The SNR sign bit is 1
            {
                // Invert and divide by 4
                value = ( ( ~value + 1 ) & 0xFF ) >> 2;
                SNR = -value;
            }
            else
            {
                // Divide by 4
                SNR = ( value & 0xFF ) >> 2;
            }
            
            if (sx1272) {
                rssicorr = 139;
            } else {
                rssicorr = 157;
            }

            /*
            printf("\nDestination address: %x\n",msgFrom);
            printf("\nLocal address: %x\n",msgTo);
            printf("\nmsgcount: %i\n",msgCountFromNode);
            printf("\nmsglength: %i\n",msgLength);

            printf("Packet RSSI: %d, ", readReg(0x1A)-rssicorr);
            printf("RSSI: %d, ", readReg(0x1B)-rssicorr);
            printf("SNR: %li, ", SNR);
            printf("Payload: %s\n", message);
            */
            
            char inst[32];
            memcpy( inst, &message[0], 5 );
            inst[5] = '\0';
            
            
            if (!strcmp(inst, "RRTTT")){
                send_message(SNR, rssicorr);
                type_of_exp = 0;
                writeToFile((char *)"RRTTT");
            }
                
            if (!strcmp(inst, "LOSSS")){
                send_message(SNR, rssicorr);
                type_of_exp = 1;
                writeToFile((char *)"LOSSS");
            }
            
            if (!strcmp(inst, "OOFFF")){
                send_message(1000, 1000);
                type_of_exp = -1;
                writeToFile((char *)"OOFFF");
                system("poweroff");
            }
            
            return msgFrom;

        } // received a message

    } // dio0=1
    return 0;
}




int main (int argc, char *argv[]) {
    
    long int last_sending_ts;
    long int current_ts;
    char text [512];
    
    struct tm  ts;
    char       buf[80];

    int rc;
    int status;
    struct gps_data_t gps_data;
    
    rc = open_gps(&gps_data);
    create_gps_stream(&gps_data);
    
    settime_s(&current_ts);
    ts = *localtime(&current_ts);
    strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", &ts);
    sprintf(full_filename,"%sNODE_-%s.txt",filename, buf);
    printf("File: %s\n", full_filename);

    
    wiringPiSetup () ;
    pinMode(ssPin, OUTPUT);
    pinMode(dio0, INPUT);
    pinMode(RST, OUTPUT);

    wiringPiSPISetup(CHANNEL, 500000);
    sprintf(text, "Send and receive packets at SF%i on %.6lf Mhz.\n", sf,(double)freq/1000000);
    writeToFile(text);
    sprintf(text, "------------------\n");
    writeToFile(text);
    
    /* RECEIVE MODE*/
    SetupLoRa();
    opmodeLora();
    opmode(OPMODE_STANDBY);
    opmode(OPMODE_RX);
    
    // Get current time
    settime_ms(&last_sending_ts);
    
    while (1) {
        
        status = read_gps_data(&gps_data, rc);
        get_gps_info(&gps_data, gpsTextToOutput, status);
        settime_ms(&current_ts);
        
        if ( ( (last_sending_ts + SENDING_PERIOD -1307) <= current_ts) && type_of_exp == 1) {
            printf("\n\ntime between sendings: %d\n\n",current_ts - last_sending_ts);
            send_message(0, 0);
            settime_ms(&last_sending_ts);
        }

        receivepacket(); 
        delay(1);

	}
    

    close_gps(&gps_data);
    return EXIT_SUCCESS;
}
