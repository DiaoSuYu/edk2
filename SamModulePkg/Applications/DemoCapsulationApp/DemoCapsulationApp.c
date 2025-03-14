#include "DemoCapsulationApp.h"
// #include <Library/DemoCapsulationLib.h>
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
DemoCapsulationApp (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  DemoCapsulationLibFunction();
  Print(L"DemoCapsulationApp: Hello!\n");
  return EFI_SUCCESS;
}
