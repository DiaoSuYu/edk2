#include <Library/DemoLib.h>
#include <Library/UefiLib.h>


/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_TIME curTime;

  Print(L"UefiMain: Hello!\n");

  DemoLibFunction();

  SystemTable->BootServices->Stall(2000);
  SystemTable->RuntimeServices->GetTime(&curTime,NULL);

  Print(L"UefiMain: Current Time: %d-%d-%d %02d:%02d:%02d\n", \
  curTime.Year,curTime.Month,curTime.Day, \
  curTime.Hour,curTime.Minute,curTime.Second);

  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"UefiMain: Bye!\n\r");

  return EFI_SUCCESS;
}
