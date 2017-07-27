/*
FHT for arduino - hartley transform
guest openmusiclabs.com 9.1.12
this is a speed optimized program
for calculating an N point FHT on a block of data
please read the read_me file for more info
*/

#include "SpectrumFHT.h"

PROGMEM  prog_int16_t _cas_constants[]  = {
#include <cas_lookup_256.inc>
};

PROGMEM  prog_uint8_t _reorder_table[]  = {
#include <256_reorder.inc>
};

PROGMEM  prog_uint8_t _log_table[]  = {
#include <decibel.inc>
};


PROGMEM  prog_int16_t _window_func[]  = {
#include <hann_256.inc>
};

uint8_t fht_log_out[(FHT_N/2)]; // FHT log output magintude buffer
int fht_input[(FHT_N)]; // FHT input data buffer

/*
FHT memory
256*2

*/
