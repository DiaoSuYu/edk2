/**
  DO NOT EDIT
  FILE auto-generated
  Module name:
    AutoGen.h
  Abstract:       Auto-generated AutoGen.h for building module or library.
**/

#ifndef _AUTOGENH_24978f74_4b03_4269_abdd_570358183b8e
#define _AUTOGENH_24978f74_4b03_4269_abdd_570358183b8e

#ifdef __cplusplus
extern "C" {
#endif

#include <Base.h>

extern GUID  gEfiCallerIdGuid;
extern GUID  gEdkiiDscPlatformGuid;
extern CHAR8 *gEfiCallerBaseName;

#define STACK_COOKIE_VALUE 0xFB64A80EEBBF304DULL

// Definition of SkuId Array
extern UINT64 _gPcd_SkuId_Array[];

RETURN_STATUS
EFIAPI
DemoCapsulationLibConstructor (
  VOID
  );

RETURN_STATUS
EFIAPI
DemoCapsulationLibDestructor (
  VOID
  );


#ifdef __cplusplus
}
#endif

#endif
