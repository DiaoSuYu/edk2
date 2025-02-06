#include "CyclicDrawingDriver.h"

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>


// Global variable
UINTN DisplayTime = 0;
EFI_EVENT RefreshIntervalEvent = NULL;
EFI_HANDLE gDemoDriverImageHandle = NULL;
EFI_HANDLE gConsoleOutputDeviceHandle = NULL;
EFI_COMPONENT_NAME_PROTOCOL gCyclicDrawingDriverName;
EFI_DRIVER_BINDING_PROTOCOL gCyclicDrawingDriverBindingProtocol;


/**
  Draws a circle on the screen with the specified color.

  This function locates the Graphics Output Protocol, allocates a buffer, and draws
  a circle in the specified color on the screen.

  @param[in] Color    The color of the circle to be drawn.

  @retval EFI_SUCCESS           The circle was successfully drawn.
  @retval EFI_OUT_OF_RESOURCES  If memory allocation fails.
**/
EFI_STATUS
EFIAPI
DrawCircle (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color
)
{
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;

    // Locate the Graphics Output Protocol
    Status = gBS->LocateProtocol (
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        (VOID **)&GraphicsOutput
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> DrawCircle]: Graphics Output Protocol not available.\n");
        return Status;
    }

    // Allocate memory for the buffer
    BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)AllocateZeroPool (
        sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) *
        GraphicsOutput->Mode->Info->HorizontalResolution *
        GraphicsOutput->Mode->Info->VerticalResolution
    );
    if (BltBuffer == NULL) {
        Print(L"[CyclicDrawingDriver -> DrawCircle]: Allocate BltBuffer Failed.\n");
        return EFI_OUT_OF_RESOURCES;
    }

    UINTN Radius  = 50;
    UINTN CenterX = GraphicsOutput->Mode->Info->HorizontalResolution / 2;
    UINTN CenterY = GraphicsOutput->Mode->Info->VerticalResolution / 2;

    // Prepare the buffer of circle with the specified color
    for (UINTN Y = 0; Y < GraphicsOutput->Mode->Info->VerticalResolution; Y++) {
        for (UINTN X = 0; X < GraphicsOutput->Mode->Info->HorizontalResolution; X++) {
            if ((X - CenterX) * (X - CenterX) + (Y - CenterY) * (Y - CenterY) <= Radius * Radius) {
                BltBuffer[Y * GraphicsOutput->Mode->Info->HorizontalResolution + X] = Color;
            }
        }
    }

    // Draw the buffer to the screen
    Status = GraphicsOutput->Blt(
        GraphicsOutput,
        BltBuffer,
        EfiBltBufferToVideo,
        0,
        0,
        0,
        0,
        GraphicsOutput->Mode->Info->HorizontalResolution,
        GraphicsOutput->Mode->Info->VerticalResolution,
        0
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> DrawCircle]: Failed to Graphics Output = %d(%r).\n", Status, Status);
        return Status;
    } else {
        FreePool(BltBuffer);
    }

    return Status;
}


/**
  Timer event callback function to periodically refresh the display.

  This function is triggered at regular intervals by the timer to alternate between
  drawing an orange or black circle on the center screen.

  @param[in] Event        The event that triggered the callback.
  @param[in] Context      Context data for the event callback.
**/
VOID
EFIAPI
RefreshIntervalProcess(
  IN EFI_EVENT Event,
  IN VOID *Context
  )
{
    EFI_STATUS Status;
    EFI_TIME CurrentTime;
    static EFI_TIME LastExecutedTime;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL OrangeColor = {0x00, 0x99, 0xFF, 0x00};
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL BlackColor  = {0x00, 0x00, 0x00, 0x00};

    Status = gRT->GetTime(&CurrentTime, NULL);
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> RefreshIntervalProcess]: Failed to get time = %d(%r).\n", Status, Status);
        return;
    }

    if (CurrentTime.Second != LastExecutedTime.Second) {
        if ((DisplayTime++ % 2) == 0) {
            Status = DrawCircle(OrangeColor);
        } else {
            Status = DrawCircle(BlackColor);
        }
        if (EFI_ERROR(Status)) {
            Print(L"[CyclicDrawingDriver -> RefreshIntervalProcess]: Failed to Draw Circle = %d(%r).\n", Status, Status);
            return;
        }
        LastExecutedTime = CurrentTime;
    }
}


/**
  Retrieves the name of the Cyclic Drawing driver.

  @param[in]  This               Pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in]  Language           Pointer to the language code in ASCII.
  @param[out] DriverName         Pointer to a pointer to the driver's name.

  @retval EFI_SUCCESS            The driver name was successfully retrieved.
  @retval EFI_INVALID_PARAMETER  If either the Language or DriverName is NULL.
  @retval EFI_UNSUPPORTED        If the specified language is not supported.
**/
EFI_STATUS 
EFIAPI 
GetCyclicDrawingDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{
    if (!Language || !DriverName) {
        Print(L"[CyclicDrawingDriver -> GetCyclicDrawingDriverName]: Passed-in parameters are valid.\n");
        return EFI_INVALID_PARAMETER;
    }

    if (AsciiStriCmp(Language, LANGUAGE_CODE_ENGLISH)) {
        Print(L"[CyclicDrawingDriver -> GetCyclicDrawingDriverName]: This language is not supported, this efi driver only support in English.\n");
        return EFI_UNSUPPORTED;
    } else {
        *DriverName = CyclicDrawingDriverName;
    }

    return EFI_SUCCESS;
}


/**
  Retrieves the name of the controller associated with the Cyclic Drawing driver.

  @param[in]  This               Pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in]  Controller         Handle of the controller whose name is being retrieved.
  @param[in]  ChildHandle        Optional handle of a child device.
  @param[in]  Language           Pointer to the language code in ASCII (e.g., "en").
  @param[out] ControllerName     Pointer to a pointer to the controller's name.

  @retval EFI_SUCCESS            The controller name was successfully retrieved.
  @retval EFI_INVALID_PARAMETER  If either the Language or ControllerName is NULL.
**/
EFI_STATUS 
EFIAPI 
GetCyclicDrawingControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **ControllerName
  )
{
    if (!Language || !ControllerName) {
        Print(L"[CyclicDrawingDriver -> GetCyclicDrawingControllerName]: Passed-in parameters are valid.\n");
        return EFI_INVALID_PARAMETER;
    }

    if (AsciiStriCmp(Language, LANGUAGE_CODE_ENGLISH)) {
        Print(L"[CyclicDrawingDriver -> GetCyclicDrawingControllerName]: This language is not supported, this efi driver only support in English.\n");
        return EFI_UNSUPPORTED;
    } else {
        *ControllerName = CyclicDrawingDriverName;
    }

    return EFI_SUCCESS;
}


/**
  This function checks if the driver can be bound to the specified controller.

  The function determines whether the driver can be bound to the controller by checking if the controller
  has the necessary protocol (Simple Text Output Protocol) installed, and whether the controller is the console output device.

  @param[in] This                The driver binding protocol instance.
  @param[in] ControllerHandle    The handle of the controller to check. This is the controller that the driver is attempting to bind to.
  @param[in] RemainingDevicePath The device path for remaining devices to be checked. This is not used in this function.

  @retval EFI_SUCCESS            The driver can be bound to the controller, meaning the controller has the required protocol (Simple Text Output Protocol).
  @retval EFI_UNSUPPORTED        The driver cannot be bound to the controller, meaning the controller either doesn't have
                                 the necessary protocol or is not the correct device (e.g., not the console output device).
**/
EFI_STATUS
EFIAPI
CyclicDrawingDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                    ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
)
{
    EFI_STATUS Status;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *TextOut;
    
    // Check if the controller is the Console Output Device
    if (ControllerHandle != gST->ConsoleOutHandle) {
        // Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverSupported]: ControllerHandle = %p, it is not Console Output Device, this efi driver only support Console Output Device.\n", ControllerHandle);
        return EFI_UNSUPPORTED;
    }

    // Check if the Console Output Device supports the Simple Text Output Protocol
    Status = gBS->HandleProtocol (
        ControllerHandle,
        &gEfiSimpleTextOutProtocolGuid,
        (VOID **)&TextOut
    );
    if (EFI_ERROR(Status)) {
        // Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverSupported]: ControllerHandle = %p, it does not support [Simple Text Output Protocol].\n", ControllerHandle);
        return EFI_UNSUPPORTED;
    }

    Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverSupported]: ControllerHandle = %p, it is Console Output Device and it supports Simple Text Output Protocol.\n", ControllerHandle);

    return EFI_SUCCESS;
}


/**
  This function starts the driver and binds it to the specified controller.

  @param[in] This                The driver binding protocol instance.
  @param[in] ControllerHandle    The handle of the controller to bind the driver to.
  @param[in] RemainingDevicePath The device path for remaining devices to be bound. This parameter is not used in this function.

  @retval EFI_SUCCESS            The driver was started and bound to the controller successfully.
  @retval Other values           An error occurred while starting the driver or binding it to the controller.
**/
EFI_STATUS
EFIAPI
CyclicDrawingDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
)
{
    EFI_STATUS Status;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *TextOut;

    // Retrieve the Simple Text Output Protocol from the controller
    // In this case, the ControllerHandle only refers specifically to Console Output Device
    Status = gBS->HandleProtocol (
        ControllerHandle,
        &gEfiSimpleTextOutProtocolGuid,
        (VOID **)&TextOut
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStart]: Failed to handle protocol for EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL = %d(%r).\n", Status, Status);
        return Status;
    }

    // Backup the handle(Console Output Device) which will need to recover the attribute after this driver stopped
    gConsoleOutputDeviceHandle = ControllerHandle;

    // Set the text output color to yellow
    Status = TextOut->SetAttribute (
        TextOut,
        EFI_YELLOW
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStart]: Failed to set text color = %d(%r).\n", Status, Status);
        return Status;
    }

    // Create a timer event to periodically refresh the display
    Status = gBS->CreateEvent (
        EVT_TIMER | EVT_NOTIFY_SIGNAL,
        TPL_CALLBACK,
        RefreshIntervalProcess,
        NULL,
        &RefreshIntervalEvent
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStart]: Failed to create event = %d(%r).\n", Status, Status);
        return Status;
    }

    // Set the timer to trigger the refresh event periodically
    Status = gBS->SetTimer (
        RefreshIntervalEvent,
        TimerPeriodic,
        DELAY_5_SECOND
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStart]: Failed to set timer = %d(%r).\n", Status, Status);
        return Status;
    }

    Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStart]: Driver loaded and text color changed to yellow.\n");

    return EFI_SUCCESS;
}


/**
  This function stops the driver and unbinds it from the specified controller.

  @param[in] This               The driver binding protocol instance. This parameter is not used in this function.
  @param[in] ControllerHandle   The handle of the controller to unbind. This handle identifies the controller to which the driver is bound.
  @param[in] NumberOfChildren   The number of children handles. This parameter is not used in this function.
  @param[in] ChildHandleBuffer  The buffer containing the child handles. This parameter is not used in this function.

  @retval EFI_SUCCESS           The driver was stopped successfully, and all associated resources were cleaned up.
  @retval Other values          An error occurred while stopping the driver. The specific error is returned in the status variable.
**/
EFI_STATUS
EFIAPI
CyclicDrawingDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer
)
{
    EFI_STATUS Status;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *TextOut;

    Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStop]: ControllerHandle = %p.\n", ControllerHandle);

    // Retrieve the Simple Text Output Protocol from the controller
    // In this case, the ControllerHandle only refers specifically to Console Output Device
    Status = gBS->HandleProtocol (
        gConsoleOutputDeviceHandle,
        &gEfiSimpleTextOutProtocolGuid,
        (VOID **)&TextOut
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStop]: Failed to handle protocol for EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL = %d(%r).\n", Status, Status);
        return Status;
    }

    // Reset the text color to white (default) by setting the attribute
    Status = TextOut->SetAttribute (
        TextOut,
        EFI_LIGHTGRAY
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStop]: Failed to recover text color = %d(%r).\n", Status, Status);
        return Status;
    }

    // If the refresh interval event is active, cancel the timer
    if (RefreshIntervalEvent != NULL) {
        // Cancel the periodic timer that was set to refresh the display
        Status = gBS->SetTimer (
            RefreshIntervalEvent,
            TimerCancel,
            0x00
        );
        if (EFI_ERROR(Status)) {
            Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStop]: Failed to cancel timer = %d(%r).\n", Status, Status);
            return Status;
        }

        // Close the event to clean up resources
        Status = gBS->CloseEvent(RefreshIntervalEvent);
        if (EFI_ERROR(Status)) {
            Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStop]: Failed to close timer event = %d(%r).\n", Status, Status);
            return Status;
        }

        // Set the event handle to NULL after it is closed to indicate that it is no longer valid
        RefreshIntervalEvent = NULL;
    }

    // Uninstall protocol which has installed to the handle
    Status = gBS->UninstallMultipleProtocolInterfaces (
        ControllerHandle,
        &gEfiDriverBindingProtocolGuid, This,
        &gEfiComponentNameProtocolGuid, &gCyclicDrawingDriverName,
        &gEfiComponentName2ProtocolGuid, &gCyclicDrawingDriverName,
        NULL
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStop]: Failed to uninstall protocol: %d(%r).\n", Status, Status);
        return Status;
    }

    Print(L"[CyclicDrawingDriver -> CyclicDrawingDriverStop]: Driver successfully Stopped.\n");

    return EFI_SUCCESS;
}


/**
  This function is called to unload the Cyclic Drawing driver.

  @param[in] ImageHandle       The handle of the driver's image. This handle uniquely identifies the driver image in the UEFI system.

  @retval EFI_SUCCESS          The driver was successfully unloaded, and all protocols were uninstalled.
  @retval EFI_ERROR            An error occurred during the unload process. The error status is returned.
**/
EFI_STATUS
EFIAPI
CyclicDrawingDriverUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
    EFI_STATUS Status;

    Print(L"[CyclicDrawingDriverUnload]: DriverUnload() called.\n");

    // Try to stop the driver
    Status = gCyclicDrawingDriverBindingProtocol.Stop (
        &gCyclicDrawingDriverBindingProtocol,
        gCyclicDrawingDriverBindingProtocol.DriverBindingHandle,
        0,
        NULL
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriverUnload]: Failed to stop driver: %d(%r).\n", Status, Status);
        return Status;
    }
    
    Print(L"[CyclicDrawingDriverUnload]: Driver successfully unloaded.\n");
    
    return EFI_SUCCESS;
}


/**
  This is the entry point for the Cyclic Drawing Driver.

  It installs the driver binding protocol, registers the component name protocols, and registers
  an unload callback for driver unloading.

  @param[in] ImageHandle       The handle of the driver's image.
  @param[in] SystemTable       A pointer to the system table.

  @retval EFI_SUCCESS          The driver was initialized successfully.
  @retval Other values         The driver failed to initialize.
**/
EFI_STATUS
EFIAPI
CyclicDrawingDriverMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS Status;

    Print(L"[CyclicDrawingDriverMain]: CyclicDrawingDriverMain() called.\n");

    // Save the driver's image handle for future reference
    gDemoDriverImageHandle = ImageHandle;

    // Initialize the component name protocol for the driver
    // This allows the driver to report its name in a given language (English in this case)
    gCyclicDrawingDriverName.GetDriverName      = GetCyclicDrawingDriverName;
    gCyclicDrawingDriverName.GetControllerName  = GetCyclicDrawingControllerName;
    gCyclicDrawingDriverName.SupportedLanguages = LANGUAGE_CODE_ENGLISH;

    // Set up the driver binding protocol for the driver
    // This allows the system to bind this driver to controllers that support it
    gCyclicDrawingDriverBindingProtocol.Supported           = CyclicDrawingDriverSupported;
    gCyclicDrawingDriverBindingProtocol.Start               = CyclicDrawingDriverStart;
    gCyclicDrawingDriverBindingProtocol.Stop                = CyclicDrawingDriverStop;
    gCyclicDrawingDriverBindingProtocol.Version             = CYCLIC_DRAWING_DRIVER_VERSION;
    gCyclicDrawingDriverBindingProtocol.ImageHandle         = ImageHandle;
    gCyclicDrawingDriverBindingProtocol.DriverBindingHandle = ImageHandle;

    // Install the driver binding protocol and component name protocols for this driver
    // These protocols make the driver available for binding to controllers and allow the system to query its name
    Status = gBS->InstallMultipleProtocolInterfaces(
        &ImageHandle,
        &gEfiDriverBindingProtocolGuid, &gCyclicDrawingDriverBindingProtocol,
        &gEfiComponentNameProtocolGuid, &gCyclicDrawingDriverName,
        &gEfiComponentName2ProtocolGuid, &gCyclicDrawingDriverName,
        NULL
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriverMain]: Failed to install driver binding protocol: %d(%r).\n", Status, Status);
        return Status;
    }

    // Register the unload callback function for the 'unload' operation in the UEFI Shell
    // This callback will be called when user want to unload this driver in the UEFI Shell
    Status = gBS->HandleProtocol (
        ImageHandle,
        &gEfiLoadedImageProtocolGuid,
        (VOID**)&gBS->UnloadImage
    );
    if (EFI_ERROR(Status)) {
        Print(L"[CyclicDrawingDriverMain]: Failed to handle loaded image protocol: %d(%r).\n", Status, Status);
        return Status;
    } else {
        gBS->UnloadImage = CyclicDrawingDriverUnload;
    }

    Print(L"[CyclicDrawingDriverMain]: CyclicDrawingDriverMain() successfully executed.\n");

    return EFI_SUCCESS;
}
