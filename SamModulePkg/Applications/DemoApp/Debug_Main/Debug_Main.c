#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>


/**
  Application Entry Point for Debugging.

  This is the main entry point for the debugging application. It demonstrates how to print
  debug information and the entry point address of the application.

  @param[in] ImageHandle    The firmware-allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point executed successfully.
  @retval Other             An error occurred during execution.
**/
EFI_STATUS
EFIAPI
Debug_Main (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{

  // Insert a CPU breakpoint for debugging. This will cause the debugger to break execution at this point.
  CpuBreakpoint ();

  // Define debug variable
  UINTN  Number  = 65536;
  CHAR16 *String = L"Debug";

  // Print debug variable
  Print (L"Debug_Main(Number): %u\n",Number);
  Print (L"Debug_Main(String): %s\n",String);

  // Log the entry point address to the debug output.
  DEBUG((EFI_D_INFO, "Debug_Main(DEBUG-EFI_D_INFO): This application entry point is 0x%08x\n", (CHAR16*)Debug_Main));

  // Print the entry point address to the console.
  Print(L"Debug_Main(Print): This application entry point is 0x%08x\n", (CHAR16*)Debug_Main);

  return EFI_SUCCESS;
}