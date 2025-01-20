#include "Library/CommonLib.h"

#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>


/**

  Check if a character is printable ASCII.

  @param[in] c   The character to check.

  @retval TRUE   The character is printable ASCII.
  @retval FALSE  The character is not printable ASCII.

**/
BOOLEAN
IsPrint (
  IN CHAR8  c
  )
{
    return (c >= 0x20 && c <= 0x7E);
}

/**

  Print bytes in hexadecimal and ASCII format.

  @param[in] BufferCb  The size of the buffer.
  @param[in] Buffer    The buffer containing bytes to print.

  ================================= Example ==================================
  [Address]  [                 Data Region                 ]  [ Ascii Region ]
             00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  0123456789ABCDEF
  00000000:  4E 56 41 52 B7 4D FF FF FF 82 00 53 74 64 44 65  NVAR.M.....StdDe
  00000010:  66 61 75 6C 74 73 00 4E 56 41 52 C1 0C FF FF FF  faults.NVAR.....
  00000020:  82 00 53 65 74 75 70 00 00 00 00 00 01 01 01 01  ..Setup.........
  00000030:  00 00 01 01 00 00 01 00 01 01 00 01 00 00 00 00  ................
  00000040:  00 00 00 02 01 01 01 00 02 01 00 00 00 00 00 00  ................
  00000050:  02 00 00 00 00 01 00 00 02 01 02 01 00 00 00 00  ................
  00000060:  00 00 00 00 00 01 01 03 01 01 01 01 01 01 01 01  ................
  00000070:  01 01 01 01 01 01 01 01 01 01 00 01 00 00 00 00  ................
  00000080:  00 00 94 11 00 00 03 00 00 00 00 00 00 00 00 00  ................
  00000090:  00 00 00 00 00 00 00 01 01 00 00 00 00 01 01 01  ................
  000000A0:  00 00 00 00 00 00 02 50 10 00 00 00 00 00 03 4D  .......P.......M
  000000B0:  00 29 00 00 00 00 00 01 03 16 0B 0B 0B 0B 0B 08  .)..............
  000000C0:  08 08 08 08 08 00 00 0C 00 00 00 01 01 01 01 01  ................
  000000D0:  01 00 00 00 00 00 00 01 20 10 00 00 00 02 02 03  ........ .......
  000000E0:  49 00 29 00 00 00 00 00 00 00 0D 0F 0B 0B 0B 0B  I.).............
  000000F0:  07 07 08 08 08 08 00 0B 00 00 00 00 01 01 01 01  ................

**/
VOID
PrintBytes (
  IN UINTN  BufferCb,
  IN UINT8  Buffer[]
  )
{

    Print(L"[Address]  [                 Data Region                 ]  [ Ascii Region ]\n");

    // Print Column Number of Data Region
    Print(L"           ");
    for (UINTN column = 0; column < 16; column++) {
        Print(L"%02X ", column);
    }
    // Print Column Number of Data Region

    // Print Column Number of Annotation Region
    Print(L" ");
    for (UINTN column = 0; column < 16; column++) {
        CHAR16 hexChar = (column < 10) ? (CHAR16)(L'0' + column) : (CHAR16)(L'A' + (column - 10));
        Print(L"%c", hexChar);
    }
    // Print Column Number of Annotation Region

    Print(L"\n");

    for (UINTN index = 0; index < BufferCb; index += 16) {

        // Print Row Number
        Print(L"%08X:  ", index);
        // Print Row Number

        for (UINTN i = index; i < (index + 16); i += 1) {
            if (i < BufferCb) {
                Print(L"%02x ", Buffer[i]);
            } else {
                Print(L"   ");
            }
        }

        Print(L" ");

        for (UINTN i = index; i < (index + 16); i += 1) {
            if (i < BufferCb) {
                if (IsPrint(Buffer[i])) {
                    Print(L"%c", Buffer[i]);
                } else {
                    Print(L".");
                }
            }
        }
        Print(L"\n");
    }
}


/**

  Convert hexadecimal data to ASCII string.

  @param[in]  HexData         The hexadecimal data.
  @param[in]  HexDataLength   The length of the hexadecimal data.
  @param[out] AsciiStr        The ASCII string representation of the hexadecimal data.

  @retval EFI_SUCCESS   The conversion was successful.

**/
EFI_STATUS
HexToAscii (
  IN  UINT8   *HexData,
  IN  UINTN   HexDataLength,
  OUT CHAR16  *AsciiStr
  )
{

    for (UINTN i = 0; i < HexDataLength; i++){
        // Printable ASCII range
        if (HexData[i] >= 0x20 && HexData[i] <= 0x7E) {
            AsciiStr[i] = (CHAR16)HexData[i];
        } else {
            AsciiStr[i] = L'.'; // Non-printable characters are replaced with '.'
        }
    }
    AsciiStr[HexDataLength] = L'\0'; // Null-terminate the string

    return EFI_SUCCESS;
}


/**

  Convert ASCII character to hexadecimal.

  @param[in]  c   The ASCII character.
  @param[out] h   The hexadecimal representation of the ASCII character.

  @retval EFI_SUCCESS       The conversion was successful.
  @retval EFI_UNSUPPORTED   The character is not supported.

**/
EFI_STATUS
AsciiToHex (
  IN UINT8  c,
  IN UINT8  *h
  )
{
    if ( (c >= 'a') && (c <= 'f') ) {
       *h = c - 0x61 + 0x0a;
       return EFI_SUCCESS;
    }
    if ( (c >= 'A') && (c <= 'F') ) {
       *h = c - 0x41 + 0x0a;
       return EFI_SUCCESS;
    }
    if ( (c >= '0') && ( c <= '9') ) {
       *h = c - 0x30;
       return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}


/**

  Convert string to hexadecimal.

  @param[in]  string  The string to convert.
  @param[out] Data    The converted hexadecimal data.

  @retval EFI_SUCCESS       The conversion was successful.
  @retval EFI_UNSUPPORTED   The conversion is unsupported.

**/
EFI_STATUS
StringToHex (
  IN UINT16  *string,
  IN UINT32  *Data
  )
{
    UINT8               s;
    UINT8               i,j;
    EFI_STATUS          Status = EFI_SUCCESS;
    UINT32              Result = 0;

    for (i = 0; i < 8 && *string != 0; i++) {
        s = (UINT8)*string;
        if (EFI_ERROR(AsciiToHex(s, &j))) {
            Status = EFI_UNSUPPORTED;
            break;
        }
        Result = Result << 4;
        Result = Result + j;
        string++;
    }
    *Data = Result;

    return Status;
}


/**

  Update the progress bar.

  @param[in] Progress   The progress value.

**/
VOID
UpdateProgressBar (
  IN UINT8  Progress
  )
{
    // 0: '['
    // 1~50: '#'
    // 51: ']'
    // 52: Null-terminate the string
    CHAR16 ProgressBar[53];

    ProgressBar[0] = L'[';
    for (UINT8 i = 1; i <= 50; i++) {
        if (i <= Progress / 2) {
            ProgressBar[i] = L'#';
        } else {
            ProgressBar[i] = L'-';
        }
    }
    ProgressBar[51] = L']';
    ProgressBar[52] = L'\0';

    // Print progress bar
    Print(L"\rProgress: %s %d%%", ProgressBar, Progress);
}


/**

  Print a specified character multiple times.

  This function prints the given character string a number of times as specified by the `count` parameter.

  @param[in] character   A pointer to the character string to print.
  @param[in] count       The number of times to print the character string.

**/
VOID
MultiCharacterPrint (
  IN CHAR16  *character,
  IN UINTN   count
  )
{
    for (UINT8 index = 0; index < count; index++) {
        Print(character);
    }
}


/**
  Determines the current UEFI bit mode (32-bit or 64-bit).

  This function checks the size of `UINTN` to determine whether the UEFI firmware 
  is running in 32-bit or 64-bit mode. It updates the `bitMode` parameter with the 
  appropriate constant (`UEFI_32_BIT_MODE` or `UEFI_64_BIT_MODE`) and returns 
  `EFI_SUCCESS` if successful. If the bit mode cannot be determined, `bitMode` 
  is set to zero, and the function returns `EFI_UNSUPPORTED`.

  @param[in, out] bitMode    Pointer to a UINT8 that receives the bit mode of the UEFI 
                             environment (32-bit or 64-bit).

  @retval EFI_SUCCESS        The bit mode was determined successfully.
  @retval EFI_UNSUPPORTED    The bit mode could not be determined (unknown mode).

**/
EFI_STATUS
CheckUEFIBitMode (
  IN OUT UINT8  *bitMode
  )
{
    EFI_STATUS Status;
    if (sizeof(UINTN) == UEFI_32_BIT_MODE) {
        // 32-bit UEFI mode
        *bitMode = UEFI_32_BIT_MODE;
        Status  = EFI_SUCCESS;
    } else if (sizeof(UINTN) == UEFI_64_BIT_MODE) {
        // 64-bit UEFI mode
        *bitMode = UEFI_64_BIT_MODE;
        Status  = EFI_SUCCESS;
    } else {
        // Unknown
        *bitMode = 0;
        Status  = EFI_UNSUPPORTED;
    }
    return Status;
}


/**
  Retrieve a pointer to a specific EFI configuration table.

  This function searches the EFI system table for a configuration table 
  that matches the specified GUID and returns a pointer to that table if found. 
  EFI configuration tables provide access to various firmware-related information.

  @param[in]  SystemTable  Pointer to the EFI system table.
  @param[in]  Guid         Pointer to the GUID that identifies the desired table.

  @retval     VOID*        A pointer to the requested configuration table if found.
  @retval     NULL         If the specified GUID is not found in the configuration table list.

**/
VOID*
ScanEfiConfigurationTable (
  IN EFI_SYSTEM_TABLE  *SystemTable,
  IN EFI_GUID          *Guid
  )
{
    EFI_CONFIGURATION_TABLE *Table = SystemTable->ConfigurationTable;
    for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; ++i, ++Table) {
        if (CompareMem(&Table->VendorGuid, Guid, sizeof(EFI_GUID))==0) {
            return Table->VendorTable;
        }
    }
    return NULL;
}


/**
  Calculate and display the elapsed time between two EFI_TIME values.

  This function calculates the difference in hours, minutes, and seconds 
  between a start time and an end time. It also ensures proper handling 
  of time overflow (e.g., when seconds or minutes are negative). 
  The calculated elapsed time is printed in both total seconds and 
  human-readable format (HH:MM:SS).

  @param[in]  StartTime   The starting EFI_TIME structure.
  @param[in]  EndTime     The ending EFI_TIME structure.

  @retval     VOID        The elapsed time is displayed using the Print function.
**/
VOID
CalculateElapsedTime (
  IN EFI_TIME  StartTime,
  IN EFI_TIME  EndTime
  )
{
    UINT8 StartHour   = StartTime.Hour;
    UINT8 StartMinute = StartTime.Minute;
    UINT8 StartSecond = StartTime.Second;
    UINT8 EndHour     = EndTime.Hour;
    UINT8 EndMinute   = EndTime.Minute;
    UINT8 EndSecond   = EndTime.Second;
    UINTN ElapsedHour   = 0;
    UINTN ElapsedMinute = 0;
    UINTN ElapsedSecond = 0;

    ElapsedSecond = EndSecond - StartSecond;
    if (ElapsedSecond < 0) {
        ElapsedSecond += 60;
        ElapsedMinute--;
    }

    ElapsedMinute = EndMinute - StartMinute;
    if (ElapsedMinute < 0) {
        ElapsedMinute += 60;
        ElapsedHour--;
    }

    ElapsedHour = EndHour - StartHour;

    if (ElapsedSecond >= 60) {
        ElapsedMinute += ElapsedSecond / 60;
        ElapsedSecond = ElapsedSecond % 60;
    }

    if (ElapsedMinute >= 60) {
        ElapsedHour += ElapsedMinute / 60;
        ElapsedMinute = ElapsedMinute % 60;
    }

    Print(
          L"Total Time: %ds (%02d:%02d:%02d - %02d:%02d:%02d)\n",
          ElapsedHour * 3600 + ElapsedMinute * 60 + ElapsedSecond,\
          StartHour, StartMinute, StartSecond,\
          EndHour, EndMinute, EndSecond
    );
}