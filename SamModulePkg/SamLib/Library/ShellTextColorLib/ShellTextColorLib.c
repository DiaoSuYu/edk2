#include "Library/ShellTextColorLib.h"

#include <Library/UefiLib.h>


/**

  Print text with specified foreground and background colors.

  @param[in] SystemTable    Pointer to the EFI system table.
  @param[in] Foreground     Foreground color to use.
  @param[in] Background     Background color to use.
  @param[in] Number         Optional pointer to a number to print in hexadecimal format.
  @param[in] Character      Optional pointer to a string to print.

**/
VOID
PrintColorText(
  IN EFI_SYSTEM_TABLE  *SystemTable,
  IN UINTN             Foreground,
  IN UINTN             Background,
  IN UINTN             *Number OPTIONAL,
  IN CHAR16            *Character OPTIONAL
  )
{
    // Get the console output protocol
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut = SystemTable->ConOut;

    // Save the original console attribute
    UINTN OriginalAttribute = ConOut->Mode->Attribute;

    // Set the new attribute for foreground and background colors
    ConOut->SetAttribute(ConOut, Foreground | (Background << 4));

    // Print the character string if provided
    if(Character != NULL){
        Print(L"%s", Character);
    }
    
    // Print the number in hexadecimal format if provided
    if(Number != NULL){
        Print(L"%x", *Number);
    }

    // Restore the original console attribute
    ConOut->SetAttribute(ConOut, OriginalAttribute);
}
