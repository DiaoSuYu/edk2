#ifndef __CYCLIC_DRAWING_DRIVER_H__
#define __CYCLIC_DRAWING_DRIVER_H__

#include <Uefi.h>

// Define the language code for English (United States)
#define LANGUAGE_CODE_ENGLISH   "en-US"

// Define the version number
#define CYCLIC_DRAWING_DRIVER_VERSION   0x10

// Define the delay time
#define DELAY_5_SECOND   5 * 10 * 1000 * 1000   // 5 second (in 100ns units)

// Define the driver name (used in UEFI protocols)
CHAR16 *CyclicDrawingDriverName = L"Cyclic Drawing Driver";

#endif // __CYCLIC_DRAWING_DRIVER_H__
