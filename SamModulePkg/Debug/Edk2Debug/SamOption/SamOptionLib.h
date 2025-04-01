/** @file
  This section is used to add a Sam Option under SETUP for learning and debugging.
**/

#ifndef _SAM_OPTION_H_
#define _SAM_OPTION_H_

#include <Uefi.h>
#include <Protocol/HiiConfigAccess.h>
#include "SamOptionLibHii.h"

#pragma pack(1)

/**
  HII specific Vendor Device Path definition.
  
  This structure defines a vendor-specific device path used for HII configuration.
*/
typedef struct {
  VENDOR_DEVICE_PATH          VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    End;
} HII_VENDOR_DEVICE_PATH;

#pragma pack()


/**
  Signature for SAM Option Callback Data structure.
  
  This signature helps identify valid instances of the structure.
*/
#define SAM_OPTION_CALLBACK_DATA_SIGNATURE  SIGNATURE_32 ('S', 'O', 'C', 'B')

/**
  Get a pointer to the SAM_OPTION_CALLBACK_DATA structure from the given pointer.

  Here, 'a' is a pointer to an EFI_HII_CONFIG_ACCESS_PROTOCOL instance, and it is type-casted and verified for the signature through the CR macro.
*/
#define SAM_OPTION_CALLBACK_DATA_FROM_THIS(a)  CR(a, SAM_OPTION_CALLBACK_DATA, SamOptionLibConfigAccess, SAM_OPTION_CALLBACK_DATA_SIGNATURE)

/**
  Define the maximum allowed counter value.
*/
#define MAX_COUNTER_VALUE (UINTN)(-1)

/**
  Time definitions
*/
#define ONE_SECOND  10000000


/**
  Structure for handling HII-related data and protocols.
  
  This structure holds HII handles, driver handles, and protocol instances
  required for HII configuration.
*/
typedef struct {
  UINTN                            Signature;                  ///< Signature for validation.
  EFI_HII_HANDLE                   HiiHandle;                  ///< HII handle for formset.
  EFI_HANDLE                       DriverHandle;               ///< UEFI driver handle.
  EFI_HII_CONFIG_ACCESS_PROTOCOL   SamOptionLibConfigAccess;   ///< HII configuration access protocol.
  SAM_OPTION_LIB_NV_DATA           SamOptionLibNvData;
  SAM_OPTION_LIB_NV_DATA           SamOptionLibOldNvData;
  EFI_EVENT                        MainPageRefreshEvent;       ///< Event to refresh the main page.
} SAM_OPTION_CALLBACK_DATA;


EFI_STATUS
EFIAPI
SamOptionLibExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Request,
  OUT EFI_STRING                            *Progress,
  OUT EFI_STRING                            *Results
  );


EFI_STATUS
EFIAPI
SamOptionLibRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Configuration,
  OUT EFI_STRING                            *Progress
  );


EFI_STATUS
EFIAPI
SamOptionLibCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  );

#endif // _SAM_OPTION_H_