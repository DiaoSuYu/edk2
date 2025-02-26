/** @file
  This section is used to add a Sam Option under SETUP for learning and debugging.

Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SAM_OPTION_H_
#define _SAM_OPTION_H_

#include <Uefi.h>
#include <Protocol/HiiConfigAccess.h>

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
  GUID for the SAM Option Formset.
  
  This GUID is used to identify the formset in the HII database and must match
  the definition in the corresponding VFR file.
*/
#define SAM_OPTION_FORMSET_GUID \
{ \
  0x6086f8c4, 0x3f16, 0x47a4, {0x92, 0xfe, 0x98, 0x2c, 0x8f, 0x78, 0xfc, 0x92} \
}

/**
  Signature for SAM Option Callback Data structure.
  
  This signature helps identify valid instances of the structure.
*/
#define SAM_OPTION_CALLBACK_DATA_SIGNATURE  SIGNATURE_32 ('S', 'O', 'C', 'B')

/**
  Form ID for SAM Option menu.
  
  This ID is used to identify the form inside the formset.
*/
#define SAM_OPTION_FORM_ID  0x1000

/**
  Structure for handling HII-related data and protocols.
  
  This structure holds HII handles, driver handles, and protocol instances
  required for HII configuration.
*/
typedef struct {
  UINTN                             Signature;     ///< Signature for validation.
  EFI_HII_HANDLE                    HiiHandle;    ///< HII handle for formset.
  EFI_HANDLE                        DriverHandle; ///< UEFI driver handle.
  EFI_HII_CONFIG_ACCESS_PROTOCOL    ConfigAccess; ///< HII configuration access protocol.
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