// 47 available notes
// song spans 31 notes
// need 5 bits for notes
// lengthmax is 7
// combo set set is 25
#define NNOTES  264
#define NUNIQUE 25
#define NPITCH  24
uint16_t  pitches[NPITCH] = {134, 151, 25, 28, 31, 33, 35, 37, 40, 42, 45, 47, 50, 56, 60, 63, 67, 75, 80, 84, 90, 95, 101, 113};
uint16_t unique[NUNIQUE] = {0x0408, 0x0409, 0x0402, 0x0403, 0x0400, 0x0401, 0x0406, 0x0407, 0x0404, 0x0405, 0x030F, 0x040B, 0x040C, 0x040A, 0x040F, 0x040D, 0x040E, 0x0411, 0x0410, 0x0413, 0x0412, 0x0415, 0x0414, 0x0417, 0x0416};
const uint8_t  notes[NNOTES]   PROGMEM = {12, 15, 12, 14, 12, 18, 12, 17, 12, 20, 12, 17, 12, 18, 12, 14, 12, 24, 12, 22, 12, 20, 12, 17, 12, 20, 12, 17, 12, 18, 12, 14, 7, 1, 7, 11, 7, 12, 7, 15, 7, 16, 7, 15, 7, 12, 7, 11, 7, 17, 7, 18, 7, 16, 7, 15, 7, 16, 7, 15, 7, 12, 7, 23, 17, 7, 17, 23, 17, 7, 17, 6, 19, 15, 19, 6, 19, 15, 19, 19, 14, 12, 14, 19, 14, 12, 14, 21, 17, 14, 17, 21, 17, 14, 17, 21, 18, 15, 18, 21, 18, 15, 18, 24, 20, 18, 20, 24, 20, 18, 20, 5, 17, 14, 17, 5, 17, 14, 17, 4, 22, 20, 24, 4, 24, 20, 24, 5, 24, 17, 24, 5, 24, 17, 24, 4, 15, 21, 15, 4, 15, 21, 23, 14, 2, 18, 2, 17, 2, 15, 2, 14, 2, 18, 2, 14, 2, 0, 2, 7, 2, 12, 2, 17, 2, 12, 2, 14, 2, 18, 2, 17, 2, 0, 2, 12, 2, 9, 2, 14, 2, 1, 2, 15, 2, 1, 2, 7, 3, 13, 3, 19, 3, 7, 3, 18, 3, 11, 3, 14, 3, 11, 3, 1, 8, 12, 8, 21, 8, 1, 8, 5, 8, 12, 8, 18, 8, 12, 8, 11, 9, 15, 9, 24, 9, 11, 9, 12, 9, 15, 9, 5, 9, 15, 9, 12, 7, 24, 7, 21, 9, 21, 9, 24, 7, 24, 7, 18, 0, 11, 0, 12, 7, 14, 7, 21, 9, 21, 9, 24, 7, 24, 0, 10};
