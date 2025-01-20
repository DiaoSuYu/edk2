#ifndef __COMMON_LIB_H__
#define __COMMON_LIB_H__

#include <Uefi.h>


#define UEFI_32_BIT_MODE   0x04
#define UEFI_64_BIT_MODE   0x08


BOOLEAN
IsPrint (
  IN CHAR8  c
  );

VOID
PrintBytes (
  IN UINTN  BufferCb,
  IN UINT8  Buffer[]
  );

EFI_STATUS
HexToAscii (
  IN  UINT8   *HexData,
  IN  UINTN   HexDataLength,
  OUT CHAR16  *AsciiStr
  );

EFI_STATUS
AsciiToHex (
  IN UINT8  c,
  IN UINT8  *h
  );

EFI_STATUS
StringToHex (
  IN UINT16  *string,
  IN UINT32  *Data
  );

VOID
UpdateProgressBar (
  IN UINT8  Progress
  );

VOID
MultiCharacterPrint (
  IN CHAR16 *character,
  IN UINTN  count
  );

EFI_STATUS
CheckUEFIBitMode (
  IN OUT UINT8 *bitMode
  );

VOID*
ScanEfiConfigurationTable (
  IN EFI_SYSTEM_TABLE *SystemTable,
  IN EFI_GUID         *Guid
  );

VOID
CalculateElapsedTime (
  IN EFI_TIME  StartTime,
  IN EFI_TIME  EndTime
  );

#endif // __COMMON_LIB_H__
