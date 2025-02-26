#include "SamOptionLib.h"

#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
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
  Extracts the current configuration for the requested elements.

  @param This       Pointer to EFI_HII_CONFIG_ACCESS_PROTOCOL instance.
  @param Request    Configuration request string.
  @param Progress   Pointer to the character in Request where parsing stopped.
  @param Results    Pointer to the filled configuration response string.

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

  @param This          Pointer to EFI_HII_CONFIG_ACCESS_PROTOCOL instance.
  @param Configuration Configuration response string.
  @param Progress      Pointer to the last successfully processed character.

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

  @param This          Pointer to EFI_HII_CONFIG_ACCESS_PROTOCOL instance.
  @param Action        Browser action triggering the callback.
  @param QuestionId    Unique identifier for the question.
  @param Type          Data type of the question.
  @param Value         Pointer to the data associated with the question.
  @param ActionRequest Output parameter indicating requested action.

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

  @param ImageHandle   The image handle.
  @param SystemTable   The system table.

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

    return EFI_SUCCESS;
}


/**
  Unloads the boot manager library and removes installed protocols.

  @param ImageHandle   The image handle.
  @param SystemTable   The system table.

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
