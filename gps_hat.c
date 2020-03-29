/*******************************************************************************
 *
 * Copyright (c) 2019 
 *
 * Rafael Cruz & José Cecílio
 * 
 * 
 * 
 * para compilar em separado usar: gcc -o gps gps.c -lm -lgps
 *
 *******************************************************************************/

#include <gps.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "gps_hat.h"

int open_gps(struct gps_data_t * gps_data){
    int rc;
    if ((rc = gps_open("localhost", "2947", gps_data)) == -1) {
        printf("code: %d, reason: %s\n", rc, gps_errstr(rc));
        return EXIT_FAILURE;
    }
    return rc;
}

void create_gps_stream(struct gps_data_t * gps_data){
    gps_stream(gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
}

void close_gps(struct gps_data_t * gps_data){
    /* When you are done... */
    gps_stream(gps_data, WATCH_DISABLE, NULL);
    gps_close (gps_data);
}


int read_gps_data(struct gps_data_t * gps_data, int rc){
    /* wait for 2 seconds to receive data */
    if (gps_waiting (gps_data, 2)) {
        /* read data */
        if ((rc = gps_read(gps_data)) == -1) {
            //printf("error occured reading gps data. code: %d, reason: %s\n", rc, gps_errstr(rc));
            return 1;
        } else {
            /* Display data from the GPS receiver. */
            if (gps_data->set && (gps_data->status == STATUS_FIX) && 
                (gps_data->fix.mode == MODE_2D || gps_data->fix.mode == MODE_3D) &&
                !isnan(gps_data->fix.latitude) && 
                !isnan(gps_data->fix.longitude)) {
                   return 0; 
            } else {
                //printf("no GPS data available\n");
                return 1;
            }
        }
    }
    
}

void print_gps_data(struct gps_data_t * gps_data, char *buf2){
    //gettimeofday(&tv, NULL); EDIT: tv.tv_sec isn't actually the timestamp!
    //printf("latitude: %f, longitude: %f, speed: %f, timestamp: %lf\n", gps_data.fix.latitude, gps_data.fix.longitude, gps_data.fix.speed, gps_data.fix.time); //EDIT: Replaced tv.tv_sec with gps_data.fix.time      
    printf("\n--- GPS ---\n");
    printf("gps_data.online:           %10.0f\n", gps_data->online);
    printf("gps_data.status:           %d\n", gps_data->status);
    printf("gps_data.satellites_used:  %d\n", gps_data->satellites_used);
    printf("gps_data.fix.mode:         %d\n", gps_data->fix.mode);
    printf("gps_data.fix.time:         %10.0f\n", gps_data->fix.time);
    printf("gps_data.fix.latitude:     %f\n", gps_data->fix.latitude);
    printf("gps_data.fix.longitude:    %f\n", gps_data->fix.longitude);
    printf("gps_data.fix.altitude:     %f\n", gps_data->fix.altitude);
    printf("gps_data.fix.speed:        %f\n", gps_data->fix.speed);
    printf("gps_data.fix.track:        %f\n", gps_data->fix.track);
    printf("gps_data.dop.pdop:         %f\n", gps_data->dop.pdop);
            
}

void get_gps_info(struct gps_data_t * gps_data, char *buf, int status){ 
    if (status == 0)  
        sprintf (buf, "%d;%10.0f;%f;%f;%f;%f", gps_data->satellites_used,
            gps_data->fix.time, gps_data->fix.latitude, gps_data->fix.longitude,
            gps_data->fix.altitude, gps_data->fix.speed);
    else
        sprintf (buf, "xx;xx;xx;xx;xx;xx");
            
}



/*
int main() {
    int rc;
    int status;
    struct gps_data_t gps_data;
    
    rc = open_gps(&gps_data);
    create_gps_stream(&gps_data);
    
    while (1) {
        
        status = read_gps_data(&gps_data, rc);
        
        if (status == 0) {
            print_gps_data(&gps_data);
        }
        
        sleep(1);
    }

    close_gps(&gps_data);
    return EXIT_SUCCESS;
}
*/
