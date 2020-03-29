#include <stdlib.h>
#include <stdio.h>
#include "checksum.h"


static bool             crc_tab16_init          = false;
static uint16_t         crc_tab16[256];




void solve_CRC (char *payload, unsigned char *calCRC){
  
  uint16_t crc_16_val         = 0x0000;
    unsigned char *ptr;
    unsigned char prev_byte;
    
  ptr = (unsigned char *) payload;
                while ( *ptr  &&  *ptr != '\r'  &&  *ptr != '\n' ) ptr++;
                *ptr = 0;



                prev_byte = 0;
                  ptr       = (unsigned char *) payload;

                  while ( *ptr ) {

                    crc_16_val         = update_crc_16(     crc_16_val,         *ptr            );
                    prev_byte = *ptr;
                    ptr++;
                  }
                  
                  
                  calCRC[0] = crc_16_val>>8;
                  calCRC[1] = crc_16_val;
  
}



/***********************************************************************************
 * 
 * 
 * Calculo do CRC
 * 
 * 
 * *********************************************************************************/


/*
 * uint8_t crc_8( const unsigned char *input_str, size_t num_bytes );
 *
 * The function crc_8() calculates the 8 bit wide CRC of an input string of a
 * given length.
 */

uint8_t crc_8( const unsigned char *input_str, size_t num_bytes ) {

  size_t a;
  uint8_t crc;
  const unsigned char *ptr;

  crc = CRC_START_8;
  ptr = input_str;

  if ( ptr != NULL ) for (a=0; a<num_bytes; a++) {

    crc = sht75_crc_table[(*ptr++) ^ crc];
  }

  return crc;

}  /* crc_8 */

/*
 * uint8_t update_crc_8( unsigned char crc, unsigned char val );
 *
 * Given a databyte and the previous value of the CRC value, the function
 * update_crc_8() calculates and returns the new actual CRC value of the data
 * comming in.
 */

uint8_t update_crc_8( unsigned char crc, unsigned char val ) {

  return sht75_crc_table[val ^ crc];

}  /* update_crc_8 */


/*
 * uint16_t crc_16( const unsigned char *input_str, size_t num_bytes );
 *
 * The function crc_16() calculates the 16 bits CRC16 in one pass for a byte
 * string of which the beginning has been passed to the function. The number of
 * bytes to check is also a parameter. The number of the bytes in the string is
 * limited by the constant SIZE_MAX.
 */

uint16_t crc_16( const unsigned char *input_str, size_t num_bytes ) {

  uint16_t crc;
  const unsigned char *ptr;
  size_t a;

  if ( ! crc_tab16_init ) init_crc16_tab();

  crc = CRC_START_16;
  ptr = input_str;

  if ( ptr != NULL ) for (a=0; a<num_bytes; a++) {

    crc = (crc >> 8) ^ crc_tab16[ (crc ^ (uint16_t) *ptr++) & 0x00FF ];
  }

  return crc;

}  /* crc_16 */

/*
 * uint16_t crc_modbus( const unsigned char *input_str, size_t num_bytes );
 *
 * The function crc_modbus() calculates the 16 bits Modbus CRC in one pass for
 * a byte string of which the beginning has been passed to the function. The
 * number of bytes to check is also a parameter.
 */

uint16_t crc_modbus( const unsigned char *input_str, size_t num_bytes ) {

  uint16_t crc;
  const unsigned char *ptr;
  size_t a;

  if ( ! crc_tab16_init ) init_crc16_tab();

  crc = CRC_START_MODBUS;
  ptr = input_str;

  if ( ptr != NULL ) for (a=0; a<num_bytes; a++) {

    crc = (crc >> 8) ^ crc_tab16[ (crc ^ (uint16_t) *ptr++) & 0x00FF ];
  }

  return crc;

}  /* crc_modbus */

/*
 * uint16_t update_crc_16( uint16_t crc, unsigned char c );
 *
 * The function update_crc_16() calculates a new CRC-16 value based on the
 * previous value of the CRC and the next byte of data to be checked.
 */

uint16_t update_crc_16( uint16_t crc, unsigned char c ) {

  if ( ! crc_tab16_init ) init_crc16_tab();

  return (crc >> 8) ^ crc_tab16[ (crc ^ (uint16_t) c) & 0x00FF ];

}  /* update_crc_16 */

/*
 * static void init_crc16_tab( void );
 *
 * For optimal performance uses the CRC16 routine a lookup table with values
 * that can be used directly in the XOR arithmetic in the algorithm. This
 * lookup table is calculated by the init_crc16_tab() routine, the first time
 * the CRC function is called.
 */

void init_crc16_tab( void ) {

  uint16_t i;
  uint16_t j;
  uint16_t crc;
  uint16_t c;

  for (i=0; i<256; i++) {

    crc = 0;
    c   = i;

    for (j=0; j<8; j++) {

      if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ CRC_POLY_16;
      else                      crc =   crc >> 1;

      c = c >> 1;
    }

    crc_tab16[i] = crc;
  }

  crc_tab16_init = true;

}  /* init_crc16_tab */


/*
 * uint32_t crc_32( const unsigned char *input_str, size_t num_bytes );
 *
 * The function crc_32() calculates in one pass the common 32 bit CRC value for
 * a byte string that is passed to the function together with a parameter
 * indicating the length.
 */



uint32_t crc_32( const unsigned char *input_str, size_t num_bytes ) {

  uint32_t crc;
  const unsigned char *ptr;
  size_t a;

  crc = CRC_START_32;
  ptr = input_str;

  if ( ptr != NULL ) for (a=0; a<num_bytes; a++) {

    crc = (crc >> 8) ^ crc_tab32[ (crc ^ (uint32_t) *ptr++) & 0x000000FFul ];
  }

  return (crc ^ 0xFFFFFFFFul);

}  /* crc_32 */

/*
 * uint32_t update_crc_32( uint32_t crc, unsigned char c );
 *
 * The function update_crc_32() calculates a new CRC-32 value based on the
 * previous value of the CRC and the next byte of the data to be checked.
 */

uint32_t update_crc_32( uint32_t crc, unsigned char c ) {

  return (crc >> 8) ^ crc_tab32[ (crc ^ (uint32_t) c) & 0x000000FFul ];

}  /* update_crc_32 */


  
