#ifndef __SHELL_TEXT_COLOR_LIB_H__
#define __SHELL_TEXT_COLOR_LIB_H__

#include <Uefi.h>


VOID
PrintColorText (
  IN EFI_SYSTEM_TABLE  *SystemTable,
  IN UINTN             Foreground,
  IN UINTN             Background,
  IN UINTN             *Number OPTIONAL,
  IN CHAR16            *Character OPTIONAL
  );

#endif // __SHELL_TEXT_COLOR_LIB_H__
