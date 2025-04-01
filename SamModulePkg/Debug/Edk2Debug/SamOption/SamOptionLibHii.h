/** @file
  This section is used to add a Sam Option under SETUP for learning and debugging.
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
  GUID for the SAM Option form refresh event.
  
  This GUID is used to identify the refresh event for the SAM Option form.
*/
#define SAM_OPTION_FORM_REFRESH_GUID \
{ \
  0xc23ce237, 0x4788, 0x4eab, {0xaf, 0xb2, 0x88, 0xc7, 0x91, 0xfe, 0x7a, 0x55} \
}


/**
  Form ID for SAM Option menu.
  
  This ID is used to identify the form inside the formset.
*/
#define SAM_OPTION_FORM_ID  0x1000
#define MAXIMUM_FORM_ID  0x10FF


/**
  Option ID for SAM Option menu.
  
  This ID is used to identify options in the form.
*/
#define SAM_KEY_VALUE_CHECKBOX   0x1100
#define SAM_KEY_VALUE_STRING     0x1101
#define SAM_KEY_VALUE_ONEOF      0x1102
#define SAM_KEY_VALUE_NUMERIC    0x1103
#define SAM_KEY_VALUE_PASSWORD   0x1104
#define SAM_KEY_VALUE_ACTION     0x1105


/**
  Structure to store non - volatile data for the SAM Option library.
  
  This structure holds the values of different configuration options in the SAM Option form.
  It is used to save and load the configuration data to and from non - volatile storage.
  This structure is designed to store the non-volatile data of the SAM Option library. It holds the values of various configuration options in the SAM Option form and is used for saving and loading configuration data to and from non-volatile storage.
*/
typedef struct {
  UINT8    CheckBox;  // Stores the state of the checkbox, typically 0 for unchecked and 1 for checked.
  UINT16   String[128];  // Stores the string value entered by the user, with a maximum capacity of 128 16-bit characters.
  UINT8    Oneof;  // Stores the selected value of the single-selection option, with the specific meaning defined by the form.
  UINT16   Numeric;  // Stores the numeric value entered by the user.
  CHAR16   Password[128];  // Stores the password entered by the user, with a maximum capacity of 128 16-bit characters.
} SAM_OPTION_LIB_NV_DATA;

#endif // _SAM_OPTION_HII_H_