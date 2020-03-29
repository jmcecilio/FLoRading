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
#ifndef GPS_HAT_H_   /* Include guard */
#define GPS_HAT_H_

#include <gps.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>


int open_gps(struct gps_data_t * gps_data);
void create_gps_stream(struct gps_data_t * gps_data);
void close_gps(struct gps_data_t * gps_data);
int read_gps_data(struct gps_data_t * gps_data, int rc);
void print_gps_data(struct gps_data_t * gps_data, char *buf);
void get_gps_info(struct gps_data_t * gps_data, char *buf, int status);


#endif
