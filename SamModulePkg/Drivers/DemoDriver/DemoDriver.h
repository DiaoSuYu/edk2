#ifndef __DEMO_UEFI_DRIVER_H__
#define __DEMO_UEFI_DRIVER_H__

#include <Uefi.h>

// Define the language code for English (United States)
#define LANGUAGE_CODE_ENGLISH   "en-US"

// Define the version number
#define DEMO_DRIVER_VERSION   0x10

// Define the driver name (used in UEFI protocols)
CHAR16 *DemoDriverName = L"UEFI Demo Driver";

#endif // __DEMO_UEFI_DRIVER_H__
