#include "DelayTool.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/ShellParameters.h>


/**
  The entry point for the DelayTool application. This function handles the processing
  of command line arguments and implements a delay mechanism based on user input.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DelayToolEFIMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status;
    EFI_SHELL_PARAMETERS_PROTOCOL  *EfiShellParameters = NULL;

    // Open protocol to get the Shell Parameter
    // This allows the application to access the shell parameters provided to it.
    Status = gBS->OpenProtocol (
        ImageHandle,
        &gEfiShellParametersProtocolGuid,
        (VOID **)&EfiShellParameters,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR(Status)) {
        // If there is an error opening the protocol, print the error code and return it.
        Print(L"[DelayToolEFIMain]: OpenProtocol(gEfiShellParametersProtocolGuid) - Status = 0x%x(%r)\n", Status, Status);
        return Status;
    }

    // Retrieve the command line arguments passed to the application
    // ArgcTotal is the total number of arguments
    // EfiName is the name of the application
    // DelayTime is the argument for the delay time (in seconds)
    UINTN   ArgcTotal  = EfiShellParameters->Argc;
    CHAR16  *EfiName   = EfiShellParameters->Argv[0];
    CHAR16  *DelayTime = EfiShellParameters->Argv[1];

    // Handle different argument counts
    if(ArgcTotal == 1 || ArgcTotal > 2) {
        // If there are no arguments or too many arguments, print the usage help message
        Print(L"[DelayToolEFIMain]: Usage Guild:\n");
        Print(L"Rule: %s + Seconds\n", EfiName);
        Print(L"Example: %s 10\n", EfiName);
    } else if(ArgcTotal == 2) {
        // If there are exactly two arguments, convert the delay time (second argument) from string to UINTN
        UINTN DelaySecond = StrHexToUintn(DelayTime);

        // Call the Stall function to implement the delay (in microseconds)
        // DelayTime is provided in seconds, but the Stall function expects microseconds
        gBS->Stall(DelaySecond * 1000000);
    }

    return EFI_SUCCESS;
}
