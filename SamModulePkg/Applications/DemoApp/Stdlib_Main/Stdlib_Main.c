#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <stdio.h>
#include <stdlib.h>

#include "Include/Library/DemoLib.h"


void *__stack_chk_guard = 0;
void __attribute__((noreturn)) __stack_chk_fail(void) {
    // Handle stack smashing detection (e.g., reboot, halt, etc.)
    while (1);
}

/***
  Demonstrates basic workings of the main() function by displaying a
  welcoming message.

  Note that the UEFI command line is composed of 16-bit UCS2 wide characters.
  The easiest way to access the command line parameters is to cast Argv as:
      wchar_t **wArgv = (wchar_t **)Argv;

  @param[in]  Argc    Number of argument tokens pointed to by Argv.
  @param[in]  Argv    Array of Argc pointers to command line tokens.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
int
main (
  IN int Argc,
  IN char **Argv
  )
{
  EFI_TIME curTime;

  Print(L"Stdlib main: Hello!\n");

  DemoLibFunction();

  gBS->Stall(2000);
  gRT->GetTime(&curTime,NULL);

  printf("Stdlib main: Current Time: %d-%d-%d %02d:%02d:%02d\n", \
  curTime.Year,curTime.Month,curTime.Day, \
  curTime.Hour,curTime.Minute,curTime.Second);

  gST->ConOut->OutputString(gST->ConOut,L"Stdlib main: Bye!\n\r");

  return 0;
}
