#include "DemoDriver.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>


// Global variable to store the driver's image handle
EFI_HANDLE gDemoDriverImageHandle = NULL;


/**
  Get the name of the driver.

  This function returns the name of the driver according to the specified language.

  @param[in]  This        Pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in]  Language    Specified language code.
  @param[out] DriverName  Pointer to a pointer to store the driver name.

  @retval EFI_SUCCESS             The driver name was successfully obtained.
  @retval EFI_INVALID_PARAMETER   The passed-in parameters are invalid.
  @retval EFI_UNSUPPORTED         The specified language is not supported.
**/
EFI_STATUS
EFIAPI
GetDemoDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
)
{
    // Check if the passed-in parameters are valid
    if (!Language || !DriverName) {
        Print(L"[DemoDriverMain -> GetDemoDriverName]: Passed-in parameters are valid.\n");
        return EFI_INVALID_PARAMETER;
    }

    // Check if it is the supported language (English)
    if (AsciiStriCmp(Language, LANGUAGE_CODE_ENGLISH)) {
        Print(L"[DemoDriverMain -> GetDemoDriverName]: This language is not supported, this efi driver only support in English.\n");
        return EFI_UNSUPPORTED;
    } else {
        *DriverName = DemoDriverName;
    }

    return EFI_SUCCESS;
}


/**
  Get the name of the controller.

  This function returns the name of the controller.
  In this example, the controller name is the same as the driver name.

  @param[in]  This            Pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in]  Controller      Handle of the controller.
  @param[in]  ChildHandle     Handle of the child device (optional).
  @param[in]  Language        Specified language code.
  @param[out] ControllerName  Pointer to a pointer to store the controller name.

  @retval EFI_SUCCESS         The controller name was successfully obtained.
**/
EFI_STATUS
EFIAPI
GetDemoControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **ControllerName
  )
{
    // Check if the passed-in parameters are valid
    if (!Language || !ControllerName) {
        Print(L"[DemoDriverMain -> GetDemoControllerName]: Passed-in parameters are valid.\n");
        return EFI_INVALID_PARAMETER;
    }

    // Check if it is the supported language (English)
    if (AsciiStriCmp(Language, LANGUAGE_CODE_ENGLISH)) {
        Print(L"[DemoDriverMain -> GetDemoControllerName]: This language is not supported, this efi driver only support in English.\n");
        return EFI_UNSUPPORTED;
    } else {
        *ControllerName = DemoDriverName;
    }

    return EFI_SUCCESS;
}


// Driver component name protocol instance, defining the functions to get the driver and controller names
EFI_COMPONENT_NAME_PROTOCOL gDemoDriverName = {
    GetDemoDriverName,
    GetDemoControllerName,
    LANGUAGE_CODE_ENGLISH
};


/**
  Check if the driver supports the specified controller.

  This function is used to determine whether the driver supports the passed-in controller.
  In this example, the driver supports all devices.

  @param[in] This                 Pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in] ControllerHandle     Handle of the controller to be checked.
  @param[in] RemainingDevicePath  Pointer to the remaining device path protocol.

  @retval EFI_SUCCESS             The driver supports the controller.
  @retval Other values            The driver does not support the controller.
**/
EFI_STATUS
EFIAPI
DemoDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
    Print(L"[DemoDriverMain -> DemoDriverBindingSupported]: ControllerHandle = %p.\n", ControllerHandle);
    return EFI_SUCCESS; // Support all devices
}


/**
  Start the driver.

  When the driver is bound to a supported controller, this function is called to start the driver.

  @param[in] This                 Pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in] ControllerHandle     Handle of the controller to start the driver on.
  @param[in] RemainingDevicePath  Pointer to the remaining device path protocol.

  @retval EFI_SUCCESS             The driver started successfully.
  @retval Other values            The driver failed to start.
**/
EFI_STATUS
EFIAPI
DemoDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
    Print(L"[DemoDriverMain -> DemoDriverBindingStart]: ControllerHandle = %p.\n", ControllerHandle);
    return EFI_SUCCESS; // Driver successfully Started
}


/**
  Stop the driver.

  When the driver needs to be uninstalled or stopped from controlling the controller, this function is called.

  @param[in] This                 Pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in] ControllerHandle     Handle of the controller to stop the driver on.
  @param[in] NumberOfChildren     Number of child devices.
  @param[in] ChildHandleBuffer    Buffer for child device handles.

  @retval EFI_SUCCESS             The driver stopped successfully.
  @retval Other values            The driver failed to stop.
**/
EFI_STATUS
EFIAPI
DemoDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer
  )
{
    EFI_STATUS Status;

    // Print a message indicating that the Stop function is called and the controller handle
    Print(L"[DemoDriverMain -> DemoDriverBindingStop]: ControllerHandle = %p.\n", ControllerHandle);

    // Uninstall the driver binding protocol
    Status = gBS->UninstallMultipleProtocolInterfaces (
        ControllerHandle,
        &gEfiDriverBindingProtocolGuid, This,
        &gEfiComponentNameProtocolGuid, &gDemoDriverName,
        &gEfiComponentName2ProtocolGuid, &gDemoDriverName,
        NULL
    );
    if (EFI_ERROR(Status)) {
        // Print an error message if the protocol uninstallation fails
        Print(L"[DemoDriverMain -> DemoDriverBindingStop]: Failed to uninstall driver binding protocol: %d(%r).\n", Status, Status);
        return Status;
    }

    // Print a message indicating that the driver has been successfully stopped
    Print(L"[DemoDriverMain -> DemoDriverBindingStop]: Driver successfully Stopped.\n");

    return EFI_SUCCESS; // Driver successfully stopped
}


// Driver binding protocol instance, defining the driver's support, start, and stop behaviors
EFI_DRIVER_BINDING_PROTOCOL gDemoDriverBindingProtocol = {
    DemoDriverBindingSupported,
    DemoDriverBindingStart,
    DemoDriverBindingStop,
    DEMO_DRIVER_VERSION,
    NULL,
    NULL
};


/**
  Unload the driver.

  This function is used to unload the driver. 
  It tries to stop the driver or uninstall protocol.
  Then performs the unloading operation.

  @param[in] ImageHandle  Handle of the driver's image.

  @retval EFI_SUCCESS     The driver was successfully unloaded.
  @retval Other values    The driver failed to unload.
**/
EFI_STATUS
EFIAPI
DemoDriverUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
    EFI_STATUS Status;

    Print(L"[DemoDriverMain -> DemoDriverUnload]: DemoDriverUnload() called.\n");

    // Try to stop the driver
    Status = gDemoDriverBindingProtocol.Stop (
        &gDemoDriverBindingProtocol,
        gDemoDriverBindingProtocol.DriverBindingHandle,
        0,
        NULL
    );
    if (EFI_ERROR(Status)) {
        Print(L"[DemoDriverMain -> DemoDriverUnload]: Failed to stop driver: %d(%r).\n", Status, Status);
        return Status;
    }

    Print(L"[DemoDriverMain -> DemoDriverUnload]: Driver successfully unloaded.\n");
    return EFI_SUCCESS;
}


/**
  Driver entry point function.

  This function is the entry point of the driver, responsible for installing the driver binding protocol and registering the unload callback function.

  @param[in] ImageHandle    Handle of the driver's image.
  @param[in] SystemTable    Pointer to the system table.

  @retval EFI_SUCCESS       The driver was initialized successfully.
  @retval Other values      The driver failed to initialize.
**/
EFI_STATUS
EFIAPI
DemoDriverMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status;

    // Save the driver's image handle
    gDemoDriverImageHandle                         = ImageHandle;
    gDemoDriverBindingProtocol.ImageHandle         = ImageHandle;
    gDemoDriverBindingProtocol.DriverBindingHandle = ImageHandle;

    // Method 1: Install the driver binding protocol
    /*
        Status = gBS->InstallProtocolInterface(
            &ImageHandle,
            &gEfiDriverBindingProtocolGuid,
            EFI_NATIVE_INTERFACE,
            &gDemoDriverBindingProtocol
        );
    */

    // Method 2: Install the driver binding protocol, include ComponentName
    Status = gBS->InstallMultipleProtocolInterfaces (
        &ImageHandle,
        &gEfiDriverBindingProtocolGuid, &gDemoDriverBindingProtocol,
        &gEfiComponentNameProtocolGuid, &gDemoDriverName,
        &gEfiComponentName2ProtocolGuid, &gDemoDriverName,
        NULL
    );
    if (EFI_ERROR(Status)) {
        Print(L"[DemoDriverMain]: Failed to install driver binding protocol: %d(%r).\n", Status, Status);
        return Status;
    }

    // Register the unload callback for the 'unload' operation in the UEFI SHELL
    Status = gBS->HandleProtocol (
        ImageHandle,
        &gEfiLoadedImageProtocolGuid,
        // (VOID**)&SystemTable->BootServices->UnloadImage // Method 1
        (VOID**)&gBS->UnloadImage // Method 2
    );
    if (EFI_ERROR(Status)) {
        Print(L"[DemoDriverMain]: Failed to Handle loaded image protocol: %d(%r).\n", Status, Status);
        return Status;
    } else {
        // SystemTable->BootServices->UnloadImage = DemoDriverUnload; // Method 1
        gBS->UnloadImage = DemoDriverUnload; // Method 2
    }

    return EFI_SUCCESS;
}