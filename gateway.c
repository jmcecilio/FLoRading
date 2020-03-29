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


#define SAMPLE_GATHER_PERIOD 1000
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

#define N_NODES  3
//byte nodeAddresses[3] = {0xCC, 0xBB, 0xDD};
byte nodeAddresses[N_NODES] = {0x1A, 0x1B, 0xCC};

byte hello[32] = "HELLO: ";
char gpsTextToOutput [128];


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
        
        if (msgTo == (byte)0xCE){  //endreço dado à gateway
            for(int i = 0; i < msgLength-2; i++)
            {
                payload[i] = (char)readReg(REG_FIFO);
            }
            
            //printf("%s\n", payload);
            
            rcvCrc[0] = (byte)readReg(REG_FIFO);
            rcvCrc[1] = (byte)readReg(REG_FIFO);
            
            //printf("crc: %x%x\n", rcvCrc[0],rcvCrc[1]);
            unsigned char calCRC [2];
            solve_CRC (payload, &calCRC[0]);
            //printf("CRC: %x%x\n", calCRC[0],calCRC[1]);

            
            if ((rcvCrc[0] == calCRC[0]) && (rcvCrc[1] == calCRC[1]) )
                return true;
            
            
            
            
            
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
    
    sprintf(textToOutput, "%lu;%s;%s",current_ts, gpsTextToOutput, data);

    printf("%s", textToOutput);  // Print data to stdout
        /* Write data to file */
    fputs(textToOutput, fPtr);
    
                /* Close file to save file data */
        fclose(fPtr);
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
            
            char text [256];
            sprintf(text, "0x%x;0x%x;%d;%d;%li;%d;%d;%s;\n", msgFrom, msgTo, readReg(0x1A)-rssicorr, readReg(0x1B)-rssicorr, SNR, msgCountFromNode, msgLength, message);
            writeToFile(text);
            
            return msgFrom;

        } // received a message

    } // dio0=1
    return 0;
}



int main (int argc, char *argv[]) {
    
    long int last_sending_node_ts;
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

    // Get current time
    settime_s(&current_ts);
    ts = *localtime(&current_ts);
    strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", &ts);
    
    uint8_t ncount = 0;
    
    int type_of_execution = 0; //RTT
    int first_time = 1;
    
    if (argc < 2) {
        printf ("Usage: argv[0] rtt|loss \n");
        exit(1);
    }
    
    if (!strcmp("rtt", argv[1])){
        type_of_execution = 0;
        char msg_str[] = "RRTTT: ";
        string2ByteArray(msg_str, hello);
    }
    else if (!strcmp("loss", argv[1])){
        type_of_execution = 1;
        char msg_str[] = "LOSSS: ";
        string2ByteArray(msg_str, hello);
    }
    else if (!strcmp("exit", argv[1])){
        type_of_execution = 0;
        char msg_str[] = "OOFFF: ";
        string2ByteArray(msg_str, hello);
    }
    else
        type_of_execution = -1;
        
    sprintf(full_filename,"%sGW_%s-%s.txt",filename, argv[1], buf);
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
    sprintf(text,"satellites_used;fix.time;fix.latitude;fix.longitude;fix.altitude;fix.speed;msgFrom;msgTo;readReg(0x1A)-rssicorr;readReg(0x1B)-rssicorr;SNR;msgCountFromNode;msgLength;Node:sender;Node:receiver;Node:inCommingMsgID;Node:sendingTS;Node:RSSI;Node:SNR;Node:nSatelites;Node:lat;Node:lon;Node:Date;Node:altitude;Node:speed_kmph\n");
    writeToFile(text);
    
    
    
    while (1) {
        
        status = read_gps_data(&gps_data, rc);
        get_gps_info(&gps_data, gpsTextToOutput, status);
        
        if (type_of_execution == 0 || (type_of_execution == 1 && first_time == 1) ) {
            
            //printf("Sending a request...\n");
            //printf("%s\n",(char *)hello);
            
            first_time = 0;
            
            byte frame[256];
            
            frame[1] = 0xCE;    //Origin
            frame[2] = ncount++;       //Msg Count

            settime_ms(&last_sending_ts);

                
            for (int j=0; j<N_NODES; j++) {
                /* TRANSMIT MODE*/
                
                frame[0] = nodeAddresses[j];    //destination
                int size = 4;
                // enter standby mode (required for FIFO loading))
                SetupLoRa();
                opmodeLora();
                opmode(OPMODE_STANDBY);

                writeReg(RegPaRamp, (readReg(RegPaRamp) & 0xF0) | 0x08); // set PA ramp-up time 50 uSec

                configPower(27);

                for(unsigned int i = 0; i < strlen((char *)hello); i++)
                {
                    frame[size++] = hello[i];
                }
                
                settime_ms(&current_ts);

                char ts [32];
                sprintf(ts,"%lu",current_ts);
                
                //sprintf(ts, "123456789");
                
                for(unsigned int i = 0; i < strlen((char *)ts); i++)
                {
                    frame[size++] = ts[i];
                }
                
                char payload[256];
                int size_payload = 0;
                for(unsigned int i = 4; i < size; i++)
                {
                    payload[size_payload++] = frame[i];
                }
                payload[size_payload]='\n';
                
                unsigned char calCRC [2];
                solve_CRC (&payload[0], &calCRC[0]);
                
                //printf("CRC: %x%x\n", calCRC[0],calCRC[1]);
                
                frame[size++] = calCRC[0];
                frame[size++] = calCRC[1];
                
                frame[3] = strlen((char *)hello) + strlen((char *)ts) + 2;       //Size
                //frame[4] = 0;
                
                txlora(frame, size);
                settime_ms(&last_sending_node_ts);
                delay(1);
                
                
                /* RECEIVE MODE*/
                SetupLoRa();
                opmodeLora();
                opmode(OPMODE_STANDBY);
                opmode(OPMODE_RX);
                
                
                settime_ms(&current_ts);

                
                while((last_sending_node_ts + 1000) >= current_ts) {
                    byte node = receivepacket(); 
                    if (node == nodeAddresses[j]){
                        delay(10);
                        break;
                    }
                    delay(1);
                    settime_ms(&current_ts);
                }
            }
        } else {
            settime_ms(&last_sending_ts);
        }
        
        settime_ms(&current_ts);
        while((last_sending_ts + SAMPLE_GATHER_PERIOD) >= current_ts) {
            receivepacket(); 
            delay(1);
            settime_ms(&current_ts);
        }
	}
    

    close_gps(&gps_data);
    return EXIT_SUCCESS;
}
