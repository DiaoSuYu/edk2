#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "Include/Library/DemoLib.h"


void *__stack_chk_guard = 0;
void __attribute__((noreturn)) __stack_chk_fail(void) {
    // Handle stack smashing detection (e.g., reboot, halt, etc.)
    while (1);
}

/***
  Print a welcoming message.

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
  EFI_TIME curTime;

  Print(L"ShellAppMain: Hello!\n");

  DemoLibFunction();

  gBS->Stall(2000);
  gRT->GetTime(&curTime,NULL);

  Print(L"ShellAppMain: Current Time: %d-%d-%d %02d:%02d:%02d\n", \
  curTime.Year,curTime.Month,curTime.Day, \
  curTime.Hour,curTime.Minute,curTime.Second);

  gST->ConOut->OutputString(gST->ConOut,L"ShellAppMain: Bye!\n\r");

  return(0);
}
