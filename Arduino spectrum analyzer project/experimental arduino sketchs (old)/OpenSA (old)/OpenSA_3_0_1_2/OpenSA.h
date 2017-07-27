#ifndef OpenSA_h
#define OpenSA_h
/** 
 * Open Spectrum Analizer Project
 * File created 2013-7-13, Michael Rule
 * 
 * There is a difficulty with incorporating the FHT library into this 
 * project using the Arduino IDE and C++ class conventions. The FHT library
 * contains global static variables, functions, and constants, all defined
 * in a single header file. There are multiple classes that need to access
 * the same shared FHT instance in spectrum Analyzer. FHT is not
 * encapuslated in an object so this is accomplished simply by
 * referencing the global variable state. The usual way to implement such
 * a shared global state would be to define the shared variables as
 * "extern" here ( meaning that they will be initialized and stored in a
 * different compiles .o object ). Then, one would define these shared
 * variables within the FHT.cpp definitions. However, since there is only
 * one FHT.h header which also defines the class, we run into difficulty. 
 * To inform objects about global variables, this header must be included. 
 * This results in each .o file having a separate copy of the global
 * variables associated with the FHT -- which results in redefinition
 * errors. This is a known issue with how Arduino handles libraries: it is
 * hard for libraries to cleanly include other libraries. One solution is
 * to have the user always import the FHT library manually in the sketch
 * before loading hte OpenSA library. This is undesirable. Another solution
 * is to avoid using any .cpp files. This will result in the entire library
 * being included by text substitution in one big ugly mess. However, this 
 * also means that only one compiled .o object is created, avoiding our
 * multiple definitions problem. A more principled solution, which we may
 * revisit later, is to repair the FHT library so that its state is stored\
 * in a separate object defines in FHT.cpp. The current workaround is a 
 * stopgap and causes the OpenSA library to inherent the same single-
 * include only problem that FHT.h exhibits.
 */
#include "Arduino.h"
#include <avr/io.h>
#include <stdint.h>

// Include the FHT library
#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht
#include <FHT.h>  // include the FHT library

#define NOP __asm__("nop\n\t");
#define delayop(ops) {for (int __delay__ops=ops; __delay__ops!=0; __delay__ops--) NOP;}

#endif //OpenSA_h

