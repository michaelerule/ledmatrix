/*
FHT for arduino - hartley transform
guest openmusiclabs.com 9.1.12
this is a speed optimized program
for calculating an N point FHT on a block of data
please read the read_me file for more info
*/

#ifndef _safht_h // include guard
#define _safht_h

#define STRINGIFY_(a) #a
#define STRINGIFY(a) STRINGIFY_(a)

#define FHT_N  256
#define SCALE    1
#define WINDOW   1
#define OCT_NORM 1
#define REORDER  1
#define LOG_OUT  1
#define LIN_OUT  0
#define LIN_OUT8 0
#define OCTAVE   0
#define LOG_N    8
#define _R_V     8 // reorder value - used for reorder list

#include <avr/pgmspace.h>

extern PROGMEM  prog_int16_t _cas_constants[];
extern PROGMEM  prog_uint8_t _reorder_table[];
extern PROGMEM  prog_uint8_t _log_table[];
extern uint8_t  fht_log_out[(FHT_N/2)]; // FHT log output magintude buffer
extern PROGMEM  prog_int16_t _window_func[FHT_N];

extern int  fht_input[(FHT_N)]; // FHT input data buffer
extern void fht_run(void);
extern void fht_reorder(void);
extern void fht_mag_log(void);
extern void fht_mag_lin(void);
extern void fht_mag_lin8(void);
extern void fht_window(void);
extern void fht_mag_octave(void);

#endif // end include guard

