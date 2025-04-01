#include "SamOptionLib.h"
#include "SamOptionLibHii.h"

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/DisplayProtocol.h>
#include <Protocol/HiiConfigRouting.h>
#include <Guid/MdeModuleHii.h>


// The binary representation of VFR data compiled by the VFR compiler
extern UINT8  SamOptionLibVfrBin[];

// GUID for this formset
EFI_GUID mSamOptionLibGuid = SAM_OPTION_FORMSET_GUID;
EFI_GUID mSamOptionLibFormRefreshGuid = SAM_OPTION_FORM_REFRESH_GUID;

// Nvram name
CHAR16 mSamOptionLibVarName[] = L"SamOptionLibNvVar";

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
      // {3662E39A-F80D-4A0E-95BF-E76E6B8BE83B}
      //
      {
        0x3662e39a, 0xf80d, 0x4a0e, { 0x95, 0xbf, 0xe7, 0x6e, 0x6b, 0x8b, 0xe8, 0x3b }
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


SAM_OPTION_CALLBACK_DATA gSamOptionLibPrivate = {
    SAM_OPTION_CALLBACK_DATA_SIGNATURE,
    NULL,
    NULL,
    {
      SamOptionLibExtractConfig,
      SamOptionLibRouteConfig,
      SamOptionLibCallback
    }
};


SAM_OPTION_CALLBACK_DATA  *mSamOptionCallbackInfo  = &gSamOptionLibPrivate;


/**
  Empty function for event process function.

  @param Event    The Event need to be process
  @param Context  The context of the event.

**/
VOID
EFIAPI
SamOptionLibInternalEmptyFunction (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
    Print(L"[SamOptionLibInternalEmptyFunction]: Entry\n");
    DEBUG ((DEBUG_INFO, "[SamOptionLibInternalEmptyFunction]: Entry\n"));
    return;
}


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

    UINTN *SecondCounter = (UINTN*)Context;

    // Reset counter if it exceeds the maximum value
    if (*SecondCounter >= MAX_COUNTER_VALUE) {
        *SecondCounter = 0;
    }

    UINTN SecondCounterValue = (*SecondCounter)++;

    // Calculate the number of digits needed for the counter value
    UINTN NumDigits = (SecondCounterValue == 0) ? 1 : Log10(SecondCounterValue) + 1;

    // The buffer size needs to include space for digits, 
    // 's' and null terminator, + 1 for 's', + 1 for '\0' (null terminator)
    UINTN BufferSize = (NumDigits + 2) * sizeof(CHAR16);
    CHAR16 *Buffer = AllocatePool(BufferSize);
    if (Buffer == NULL) {
        return;
    }

    // Format the counter value as a string, appending 's' at the end.
    UnicodeSPrint(Buffer, BufferSize, L"%ds", SecondCounterValue);

    // Update the HII string with the newly formatted value.
    HiiSetString (
        mSamOptionCallbackInfo->HiiHandle,
        STRING_TOKEN(STR_SAM_STATIC_SECOND_VALUE),
        Buffer,
        NULL
    );

    // Free the dynamically allocated memory to avoid memory leaks.
    FreePool(Buffer);
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
CounterEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
      DEBUG ((DEBUG_INFO, "[CounterEventNotify]: Entry\n"));
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
    UnicodeSPrint(Buffer, BufferSize, L"%d", CounterValue);

    // Update the HII string with the newly formatted value.
    HiiSetString (
        mSamOptionCallbackInfo->HiiHandle,
        STRING_TOKEN(STR_SAM_STATIC_COUNTER_VALUE),
        Buffer,
        NULL
    );

    // Free the dynamically allocated memory to avoid memory leaks.
    FreePool(Buffer);
}


/**
  Converts the unicode character of the string from uppercase to lowercase.
  This is a internal function.

  @param ConfigString  String to be converted

**/
VOID
SamTestHiiToLower (
  IN EFI_STRING  ConfigString
  )
{
    EFI_STRING  String;
    BOOLEAN     Lower;

    ASSERT (ConfigString != NULL);

    //
    // Convert all hex digits in range [A-F] in the configuration header to [a-f]
    //
    for (String = ConfigString, Lower = FALSE; *String != L'\0'; String++) {
        if (*String == L'=') {
            Lower = TRUE;
        } else if (*String == L'&') {
            Lower = FALSE;
        } else if (Lower && (*String >= L'A') && (*String <= L'F')) {
            *String = (CHAR16)(*String - L'A' + L'a');
        }
    }
}


/**
  Update the progress string through the offset value.

  @param Offset           The offset value
  @param Configuration    Point to the configuration string.

**/
EFI_STRING
SamTestUpdateProgress (
  IN  UINTN       Offset,
  IN  EFI_STRING  Configuration
  )
{
  UINTN       Length;
  EFI_STRING  StringPtr;
  EFI_STRING  ReturnString;

  StringPtr    = NULL;
  ReturnString = NULL;

  //
  // &OFFSET=XXXX followed by a Null-terminator.
  // Length = StrLen (L"&OFFSET=") + 4 + 1
  //
  Length = StrLen (L"&OFFSET=") + 4 + 1;

  StringPtr = AllocateZeroPool (Length * sizeof (CHAR16));

  if (StringPtr == NULL) {
      return NULL;
  }

  UnicodeSPrint (
      StringPtr,
      (8 + 4 + 1) * sizeof (CHAR16),
      L"&OFFSET=%04x",
      Offset
  );

  ReturnString = StrStr(Configuration, StringPtr);

  if (ReturnString == NULL) {
      //
      // If doesn't find the string in Configuration, convert the string to lower case then search again.
      //
      SamTestHiiToLower (StringPtr);
      ReturnString = StrStr(Configuration, StringPtr);
  }

  FreePool (StringPtr);

  return ReturnString;
}


/**
   Initialize all of Sam Option configuration data

  @param CallbackData    The context data.

**/
VOID
InitializeSamOptionConfig (
  IN  SAM_OPTION_CALLBACK_DATA  *CallbackData
  )
{
  ASSERT (CallbackData != NULL);

  //
  // Backup Initialize Sam Option configuartion data
  //
  CopyMem (&CallbackData->SamOptionLibOldNvData, &CallbackData->SamOptionLibNvData, sizeof (SAM_OPTION_LIB_NV_DATA));
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
    DEBUG ((DEBUG_INFO, "[SamOptionLibExtractConfig]: Entry\n"));

    EFI_STATUS                 Status;
    UINTN                      BufferSize;
    UINTN                      Size;
    BOOLEAN                    AllocatedRequest;
    EFI_STRING                 ConfigRequestHdr;
    EFI_STRING                 ConfigRequest;
    SAM_OPTION_CALLBACK_DATA   *Private;

    if ((Progress == NULL) || (Results == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    *Progress = Request;
    if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &mSamOptionLibGuid, mSamOptionLibVarName)) {
        return EFI_NOT_FOUND;
    }

    ConfigRequestHdr = NULL;
    ConfigRequest    = NULL;
    AllocatedRequest = FALSE;
    Size             = 0;

    Private = SAM_OPTION_CALLBACK_DATA_FROM_THIS (This);
    //
    // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
    //
    BufferSize    = sizeof (SAM_OPTION_LIB_NV_DATA);
    ConfigRequest = Request;
    if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
        //
        // Request has no request element, construct full request string.
        // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
        // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
        //
        ConfigRequestHdr = HiiConstructConfigHdr (&mSamOptionLibGuid, mSamOptionLibVarName, Private->DriverHandle);
        Size             = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
        ConfigRequest    = AllocateZeroPool (Size);
        ASSERT (ConfigRequest != NULL);
        AllocatedRequest = TRUE;
        UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
        FreePool (ConfigRequestHdr);
    }

    Status = gHiiConfigRouting->BlockToConfig (
        gHiiConfigRouting,
        ConfigRequest,
        (UINT8*)&Private->SamOptionLibNvData,
        BufferSize,
        Results,
        Progress
    );

    //
    // Free the allocated config request string.
    //
    if (AllocatedRequest) {
        FreePool (ConfigRequest);
        ConfigRequest = NULL;
    }

    //
    // Set Progress string to the original request string.
    //
    if (Request == NULL) {
        *Progress = NULL;
    } else if (StrStr (Request, L"OFFSET") == NULL) {
        *Progress = Request + StrLen (Request);
    }

    DEBUG ((DEBUG_INFO, "[SamOptionLibExtractConfig]: End\n"));

    return Status;
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
    DEBUG ((DEBUG_INFO, "[SamOptionLibRouteConfig]: Entry\n"));

    EFI_STATUS Status;
    SAM_OPTION_CALLBACK_DATA         *Private;
    EFI_HII_CONFIG_ROUTING_PROTOCOL  *ConfigRouting;
    UINTN                            Offset;
    UINTN                            BufferSize;
    SAM_OPTION_LIB_NV_DATA           *NewSamOptionLibNvData;
    SAM_OPTION_LIB_NV_DATA           *OldSamOptionLibNvData;

    if (Progress == NULL) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: Progress = NULL\n"));
        return EFI_INVALID_PARAMETER;
    }

    *Progress = Configuration;

    if (Configuration == NULL) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: Configuration = NULL\n"));
        return EFI_INVALID_PARAMETER;
    }

    //
    // Check routing data in <ConfigHdr>.
    // Note: there is no name for Name/Value storage, only GUID will be checked
    //
    if (HiiIsConfigHdrMatch(Configuration, &mSamOptionLibGuid, mSamOptionLibVarName) != TRUE) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: Routing information does not match\n"));
        return EFI_NOT_FOUND;
    }

    Status = gBS->LocateProtocol (
        &gEfiHiiConfigRoutingProtocolGuid,
        NULL,
        (VOID **)&ConfigRouting
    );
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: LocateProtocol(gEfiHiiConfigRoutingProtocolGuid) = %r(0x%x)\n", Status, Status));
        return Status;
    }

    Private = SAM_OPTION_CALLBACK_DATA_FROM_THIS(This);

    //
    // Get Buffer Storage data from EFI variable
    //
    BufferSize            = sizeof(SAM_OPTION_LIB_NV_DATA);
    NewSamOptionLibNvData = &Private->SamOptionLibNvData;
    OldSamOptionLibNvData = &Private->SamOptionLibOldNvData;
    Offset                = 0;

    //
    // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
    //
    Status = ConfigRouting->ConfigToBlock (
        ConfigRouting,
        Configuration,
        (UINT8*)NewSamOptionLibNvData,
        &BufferSize,
        Progress
    );
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: ConfigToBlock = %r(0x%x)\n", Status, Status));
        return Status;
    }
    ASSERT_EFI_ERROR (Status);

    DEBUG ((EFI_D_INFO, "[SamOptionLibRouteConfig]: Configuration = %s\n", Configuration));
    DEBUG ((EFI_D_INFO, "[SamOptionLibRouteConfig]: Progress = %s\n", Progress));

    if (CompareMem(&NewSamOptionLibNvData->CheckBox, &OldSamOptionLibNvData->CheckBox, sizeof(NewSamOptionLibNvData->CheckBox)) != 0) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: CheckBox CompareMem is true\n"));
        Status = gRT->SetVariable (
            mSamOptionLibVarName,
            &mSamOptionLibGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
            BufferSize,
            &(NewSamOptionLibNvData->CheckBox)
        );
        if(EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: SetVariable(CheckBox) = %r(0x%x)\n", Status, Status));
            Offset = OFFSET_OF (SAM_OPTION_LIB_NV_DATA, CheckBox);
            goto Exit;
        }
    }

    if (CompareMem(&NewSamOptionLibNvData->String, &OldSamOptionLibNvData->String, sizeof(NewSamOptionLibNvData->String)) != 0) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: Oneof CompareMem is true\n"));
        Status = gRT->SetVariable (
            mSamOptionLibVarName,
            &mSamOptionLibGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
            BufferSize,
            &(NewSamOptionLibNvData->String)
        );
        if(EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: SetVariable(String) = %r(0x%x)\n", Status, Status));
            Offset = OFFSET_OF (SAM_OPTION_LIB_NV_DATA, String);
            goto Exit;
        }
    }

    if (CompareMem(&NewSamOptionLibNvData->Oneof, &OldSamOptionLibNvData->Oneof, sizeof(NewSamOptionLibNvData->Oneof)) != 0) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: Oneof CompareMem is true\n"));
        Status = gRT->SetVariable (
            mSamOptionLibVarName,
            &mSamOptionLibGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
            BufferSize,
            &(NewSamOptionLibNvData->Oneof)
        );
        if(EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: SetVariable(Oneof) = %r(0x%x)\n", Status, Status));
            Offset = OFFSET_OF (SAM_OPTION_LIB_NV_DATA, Oneof);
            goto Exit;
        }
    }

    if (CompareMem(&NewSamOptionLibNvData->Numeric, &OldSamOptionLibNvData->Numeric, sizeof(NewSamOptionLibNvData->Numeric)) != 0) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: Numeric CompareMem is true\n"));
        Status = gRT->SetVariable (
            mSamOptionLibVarName,
            &mSamOptionLibGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
            BufferSize,
            &(NewSamOptionLibNvData->Numeric)
        );
        if(EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: SetVariable(Numeric) = %r(0x%x)\n", Status, Status));
            Offset = OFFSET_OF (SAM_OPTION_LIB_NV_DATA, Numeric);
            goto Exit;
        }
    }

    if (CompareMem(&NewSamOptionLibNvData->Password, &OldSamOptionLibNvData->Password, sizeof(NewSamOptionLibNvData->Password)) != 0) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: Password CompareMem is true\n"));
        Status = gRT->SetVariable (
            mSamOptionLibVarName,
            &mSamOptionLibGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
            BufferSize,
            &(NewSamOptionLibNvData->Password)
        );
        if(EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: SetVariable(Password) = %r(0x%x)\n", Status, Status));
            Offset = OFFSET_OF (SAM_OPTION_LIB_NV_DATA, Password);
            goto Exit;
        }
    }

    //
    // After user do the save action, need to update NewSamOptionLibNvData.
    //
    CopyMem(OldSamOptionLibNvData, NewSamOptionLibNvData, sizeof(SAM_OPTION_LIB_NV_DATA));

    DEBUG ((DEBUG_INFO, "[SamOptionLibExtractConfig]: End, Status = %r(0x%x)\n",EFI_SUCCESS, EFI_SUCCESS));

    return EFI_SUCCESS;

Exit:
    //
    // Fail to save the data, update the progress string.
    //
    *Progress = SamTestUpdateProgress(Offset, Configuration);

    DEBUG ((DEBUG_INFO, "[SamOptionLibExtractConfig]: End, Status = %r(0x%x)\n",Status, Status));

    return Status;
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
    DEBUG ((EFI_D_INFO, "[SamOptionLibCallback]: Entry, Action = 0x%x, QuestionId = 0x%x\n", Action, QuestionId));

    if ((Action != EFI_BROWSER_ACTION_CHANGING) && (Action != EFI_BROWSER_ACTION_CHANGED) && (Action != EFI_BROWSER_ACTION_FORM_OPEN)) {
        //
        // Do nothing for other UEFI Action. Only do call back when data is changed or the form is open.
        //
        return EFI_UNSUPPORTED;
    }

    BOOLEAN                    HiiGetStatus;
    BOOLEAN                    HiiSetStatus;
    SAM_OPTION_CALLBACK_DATA   *Private;
    SAM_OPTION_LIB_NV_DATA     *NewSamOptionLibNvData;
    SAM_OPTION_LIB_NV_DATA     *OldSamOptionLibNvData;

    Private = SAM_OPTION_CALLBACK_DATA_FROM_THIS(This);

    //
    // Retrieve uncommitted data from Form Browser
    //
    NewSamOptionLibNvData = &Private->SamOptionLibNvData;
    OldSamOptionLibNvData = &Private->SamOptionLibOldNvData;
    HiiGetStatus = HiiGetBrowserData (
        &mSamOptionLibGuid,
        mSamOptionLibVarName,
        sizeof(SAM_OPTION_LIB_NV_DATA),
        (UINT8*)NewSamOptionLibNvData
    );
    if(HiiGetStatus != TRUE) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: HiiGetStatus = 0x%x\n", HiiGetStatus));
        return EFI_DEVICE_ERROR;
    }

    if (Action == EFI_BROWSER_ACTION_CHANGED) {
        //
        // Pass changed uncommitted data back to Form Browser
        //
        HiiSetStatus = HiiSetBrowserData (
            &mSamOptionLibGuid,
            mSamOptionLibVarName,
            sizeof(SAM_OPTION_LIB_NV_DATA),
            (UINT8*)NewSamOptionLibNvData,
            NULL
        );
        if(HiiSetStatus != TRUE) {
            DEBUG((DEBUG_ERROR, "[SamOptionLibRouteConfig]: HiiSetStatus = 0x%x\n", HiiSetStatus));
            return EFI_DEVICE_ERROR;
        }
    }

    DEBUG ((EFI_D_INFO, "[SamOptionLibCallback]: End\n"));
  
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

    Status = gBS->InstallMultipleProtocolInterfaces (
        &mSamOptionCallbackInfo->DriverHandle,
        &gEfiDevicePathProtocolGuid,
        &mSamOptionLibHiiVendorDevicePath,
        &gEfiHiiConfigAccessProtocolGuid,
        &mSamOptionCallbackInfo->SamOptionLibConfigAccess,
        NULL
    );
    ASSERT_EFI_ERROR (Status);

    mSamOptionCallbackInfo->HiiHandle = HiiAddPackages (
        &mSamOptionLibGuid,
        mSamOptionCallbackInfo->DriverHandle,
        SamOptionLibVfrBin,
        SamOptionLibStrings,
        NULL
    );
    ASSERT (gSamOptionLibPrivate.HiiHandle != NULL);

    //
    // Update page
    //
    InitializeSamOptionConfig(mSamOptionCallbackInfo);

    EFI_EVENT  SecondEvent;
    UINTN *SecondCounter = AllocateZeroPool(sizeof(UINTN));
    if (SecondCounter == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = gBS->CreateEvent (
        EVT_TIMER | EVT_NOTIFY_SIGNAL,
        TPL_CALLBACK,
        SecondEventNotify,
        (VOID*)SecondCounter,
        &SecondEvent
    );
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibConstructor]: CreateEvent(Second) Failed = %r(0x%x)\n", Status, Status));
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

    UINTN *TestCounter = AllocateZeroPool(sizeof(UINTN));
    if (TestCounter == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = gBS->CreateEventEx (
        EVT_NOTIFY_SIGNAL,
        TPL_NOTIFY,
        CounterEventNotify,
        (VOID*)TestCounter,
        &mSamOptionLibFormRefreshGuid,
        &mSamOptionCallbackInfo->MainPageRefreshEvent
    );
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "[SamOptionLibConstructor]: CreateEventEx(TestCounter) Failed = %r(0x%x)\n", Status, Status));
        gBS->CloseEvent(mSamOptionCallbackInfo->MainPageRefreshEvent);
    }

    DEBUG ((DEBUG_INFO, "[SamOptionLibConstructor]: End\n"));

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
    DEBUG ((DEBUG_INFO, "[SamOptionLibDestructor]: Entry\n"));

    EFI_STATUS  Status;

    Status = gBS->UninstallMultipleProtocolInterfaces (
        gSamOptionLibPrivate.DriverHandle,
        &gEfiDevicePathProtocolGuid,
        &mSamOptionLibHiiVendorDevicePath,
        &gEfiHiiConfigAccessProtocolGuid,
        &gSamOptionLibPrivate.SamOptionLibConfigAccess,
        NULL
    );
    ASSERT_EFI_ERROR (Status);

    HiiRemovePackages (gSamOptionLibPrivate.HiiHandle);

    DEBUG ((DEBUG_INFO, "[SamOptionLibDestructor]: End\n"));

    return EFI_SUCCESS;
}
