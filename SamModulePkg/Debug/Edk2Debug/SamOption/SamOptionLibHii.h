/** @file
  This section is used to add a Sam Option under SETUP for learning and debugging.

Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SAM_OPTION_HII_H_
#define _SAM_OPTION_HII_H_

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
  Form ID for SAM Option menu.
  
  This ID is used to identify the form inside the formset.
*/
#define SAM_OPTION_FORM_ID  0x1000

#define MAXIMUM_FORM_ID  0x10FF

// Option ID
#define SAM_KEY_VALUE_CHECKBOX   0x1100
#define SAM_KEY_VALUE_STRING     0x1101
#define SAM_KEY_VALUE_ONEOF      0x1102
#define SAM_KEY_VALUE_NUMERIC    0x1103
#define SAM_KEY_VALUE_ACTION     0x1104

/**
  Varstore ID for SAM Option menu.
  
  This ID is used to identify the varstores inside the formset.
*/
#define SAM_OPTION_VARSTORE_ID  0x2000


typedef struct {
  BOOLEAN   CheckBox;
  UINT8     String;
  UINT8     Oneof;
  UINT16    Numeric;
} SAM_OPTION_LIB_DATA;

#endif // _SAM_OPTION_HII_H_