#include "SamOptionLib.h"
#include "SamOptionLibHii.h"

#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/MdeModuleHii.h>


// The binary representation of VFR data compiled by the VFR compiler
extern UINT8  SamOptionLibVfrBin[];

// GUID for this formset
EFI_GUID  mSamOptionLibGuid = SAM_OPTION_FORMSET_GUID;

// Vendor device path structure for HII
HII_VENDOR_DEVICE_PATH  mSamOptionLibHiiVendorDevicePath = {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8)(sizeof (VENDOR_DEVICE_PATH)),
          (UINT8)((sizeof (VENDOR_DEVICE_PATH)) >> 8)
        }
      },
      //
      // Unique GUID for this device path
      //
      { 
        0x6086f8c4, 0x3f16, 0x47a4, {0x92, 0xfe, 0x98, 0x2c, 0x8f, 0x78, 0xfc, 0x92}
      }
    },
    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
        (UINT8)(END_DEVICE_PATH_LENGTH),
        (UINT8)((END_DEVICE_PATH_LENGTH) >> 8)
      }
    }
};


SAM_OPTION_CALLBACK_DATA  gSamOptionLibPrivate = {
    SAM_OPTION_CALLBACK_DATA_SIGNATURE,
    NULL,
    NULL,
    {
      SamOptionLibExtractConfig,
      SamOptionLibRouteConfig,
      SamOptionLibCallback
    }
};


/**
 * Computes the base - 10 logarithm (integer) of a given number.
 * Returns the number of digits in the number.
 *
 * @param[in] Value The unsigned integer number for which to calculate the number of digits.
 * @return The number of digits in the given number. For the number 0, it returns 1.
 */
UINTN
Log10 (
  IN UINTN  Value
  )
{
    UINTN Digits = 0;
    while (Value > 0) {
        Value /= 10;
        Digits++;
    }
    return Digits;
}


/**
 * Event notification function for the second event.
 * This function is called when the specified EFI event occurs.
 * It updates a counter, formats the counter value as a string,
 * and sets the corresponding HII (Human Interface Infrastructure) string.
 *
 * @param[in] Event   The EFI event that triggered this notification.
 * @param[in] Context A pointer to the context data associated with the event.
 *                    This context should point to a UINTN variable that serves as a counter.
 *
 * @retval None This function has a VOID return type. If the context is NULL or memory allocation fails, 
 *              the function exits early without performing the intended operations.
 */
VOID
EFIAPI
SecondEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
    // Check if the context is NULL to prevent dereferencing a NULL pointer.
    if (Context == NULL) {
        return;
    }

    UINTN *Counter = (UINTN*)Context;

    // Reset counter if it exceeds the maximum value
    if (*Counter >= MAX_COUNTER_VALUE) {
        *Counter = 0;
    }

    UINTN CounterValue = (*Counter)++;

    // Calculate the number of digits needed for the counter value
    UINTN NumDigits = (CounterValue == 0) ? 1 : Log10(CounterValue) + 1;

    // The buffer size needs to include space for digits, 
    // 's' and null terminator, + 1 for 's', + 1 for '\0' (null terminator)
    UINTN BufferSize = (NumDigits + 2) * sizeof(CHAR16);
    CHAR16 *Buffer = AllocatePool(BufferSize);
    if (Buffer == NULL) {
        return;
    }

    // Format the counter value as a string, appending 's' at the end.
    UnicodeSPrint(Buffer, BufferSize, L"%ds", CounterValue);

    // Update the HII string with the newly formatted value.
    HiiSetString (
        gSamOptionLibPrivate.HiiHandle,
        STRING_TOKEN(STR_SAM_STATIC_INFROMATION_SECOND_VALUE),
        Buffer,
        NULL
    );

    // Free the dynamically allocated memory to avoid memory leaks.
    FreePool(Buffer);
}


/**
  Extracts the current configuration for the requested elements.

  @param [in]  This       Pointer to EFI_HII_CONFIG_ACCESS_PROTOCOL instance.
  @param [in]  Request    Configuration request string.
  @param [out] Progress   Pointer to the character in Request where parsing stopped.
  @param [out] Results    Pointer to the filled configuration response string.

  @retval EFI_SUCCESS            Successfully retrieved the requested values.
  @retval EFI_OUT_OF_RESOURCES   Insufficient memory to store results.
  @retval EFI_INVALID_PARAMETER  Invalid request syntax.
  @retval EFI_NOT_FOUND          No matching storage found.
**/
EFI_STATUS
EFIAPI
SamOptionLibExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Request,
  OUT EFI_STRING                            *Progress,
  OUT EFI_STRING                            *Results
  )
{
    if ((Progress == NULL) || (Results == NULL)) {
      return EFI_INVALID_PARAMETER;
    }
    *Progress = Request;
    return EFI_NOT_FOUND;
}


/**
  Processes configuration changes.

  @param [in]  This          Pointer to EFI_HII_CONFIG_ACCESS_PROTOCOL instance.
  @param [in]  Configuration Configuration response string.
  @param [out] Progress      Pointer to the last successfully processed character.

  @retval EFI_SUCCESS            Configuration processed successfully.
  @retval EFI_INVALID_PARAMETER  Configuration string is NULL.
  @retval EFI_NOT_FOUND          No matching storage found.
**/
EFI_STATUS
EFIAPI
SamOptionLibRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Configuration,
  OUT EFI_STRING                            *Progress
  )
{
    if ((Configuration == NULL) || (Progress == NULL)) {
        return EFI_INVALID_PARAMETER;
    }
    *Progress = Configuration;
    return EFI_NOT_FOUND;
}


/**
  Callback function for boot manager menu actions.

  @param [in]  This          Pointer to EFI_HII_CONFIG_ACCESS_PROTOCOL instance.
  @param [in]  Action        Browser action triggering the callback.
  @param [in]  QuestionId    Unique identifier for the question.
  @param [in]  Type          Data type of the question.
  @param [in]  Value         Pointer to the data associated with the question.
  @param [out] ActionRequest Output parameter indicating requested action.

  @retval EFI_SUCCESS  Callback handled successfully.
**/
EFI_STATUS
EFIAPI
SamOptionLibCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  )
{
    DEBUG ((EFI_D_INFO, "[SamOptionLibCallback] Action = 0x%x, QuestionId = 0x%x\n", Action, QuestionId));
    return EFI_SUCCESS;
}


/**
  Initializes the boot manager library and installs required protocols.

  @param [in] ImageHandle   The image handle.
  @param [in] SystemTable   The system table.

  @retval EFI_SUCCESS  Initialization successful.
  @retval Other        Error occurred during initialization.
**/
EFI_STATUS
EFIAPI
SamOptionLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    DEBUG ((DEBUG_INFO, "[SamOptionLibConstructor]: Entry\n"));
    
    EFI_STATUS  Status;

    gSamOptionLibPrivate.DriverHandle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
        &gSamOptionLibPrivate.DriverHandle,
        &gEfiDevicePathProtocolGuid,
        &mSamOptionLibHiiVendorDevicePath,
        &gEfiHiiConfigAccessProtocolGuid,
        &gSamOptionLibPrivate.ConfigAccess,
        NULL
    );
    ASSERT_EFI_ERROR (Status);

    gSamOptionLibPrivate.HiiHandle = HiiAddPackages (
        &mSamOptionLibGuid,
        gSamOptionLibPrivate.DriverHandle,
        SamOptionLibVfrBin,
        SamOptionLibStrings,
        NULL
    );
    ASSERT (gSamOptionLibPrivate.HiiHandle != NULL);

#if 0
    EFI_EVENT  SecondEvent;
    UINTN *Counter = AllocateZeroPool(sizeof(UINTN));
    if (Counter == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = gBS->CreateEvent (
        EVT_TIMER | EVT_NOTIFY_SIGNAL,
        TPL_CALLBACK,
        SecondEventNotify,
        (VOID*)Counter,
        &SecondEvent
    );
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibConstructor]: Create Event(Second) Failed = %r(0x%x)\n", Status, Status));
        return Status;
    }

    Status = gBS->SetTimer (
        SecondEvent,
        TimerPeriodic,
        ONE_SECOND
    );
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibConstructor]: Set Timer Failed = %r(0x%x)\n", Status, Status));
        return Status;
    }
#endif

    return EFI_SUCCESS;
}


/**
  Unloads the boot manager library and removes installed protocols.

  @param [in] ImageHandle   The image handle.
  @param [in] SystemTable   The system table.

  @retval EFI_SUCCESS  Successfully unloaded.
**/
EFI_STATUS
EFIAPI
SamOptionLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS  Status;

    Status = gBS->UninstallMultipleProtocolInterfaces (
        gSamOptionLibPrivate.DriverHandle,
        &gEfiDevicePathProtocolGuid,
        &mSamOptionLibHiiVendorDevicePath,
        &gEfiHiiConfigAccessProtocolGuid,
        &gSamOptionLibPrivate.ConfigAccess,
        NULL
    );
    ASSERT_EFI_ERROR (Status);

    HiiRemovePackages (gSamOptionLibPrivate.HiiHandle);

    return EFI_SUCCESS;
}
