#ifndef __ACPI_LIB_H__
#define __ACPI_LIB_H__

#include <Uefi.h>


//----------------------------------------------------------------------------
// Base on ACPI 6.5
//----------------------------------------------------------------------------

//
// Root System Description Pointer (RSDP)
//
#define ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE_ASCII       {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '}
#define ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE SIGNATURE_64('R', 'S', 'D', ' ', 'P', 'T', 'R', ' ')
#define RSDP_SIGNATURE_BYTE_OFFEST           0
#define RSDP_CHECKSUM_BYTE_OFFEST            8
#define RSDP_OEMID_BYTE_OFFEST               9
#define RSDP_REVISION_BYTE_OFFEST            15
#define RSDP_RSDT_ADDRESS_BYTE_OFFEST        16
#define RSDP_LENGTH_BYTE_OFFEST              20
#define RSDP_XSDT_ADDRESS_BYTE_OFFEST        24
#define RSDP_EXTENDED_CHECKSUM_BYTE_OFFEST   32
#define RSDP_RESERVED_BYTE_OFFEST            33
#define RSDP_SIGNATURE_BYTE_LENGTH           8
#define RSDP_CHECKSUM_BYTE_LENGTH            1
#define RSDP_OEMID_BYTE_LENGTH               6
#define RSDP_REVISION_BYTE_LENGTH            1
#define RSDP_RSDT_ADDRESS_BYTE_LENGTH        4
#define RSDP_LENGTH_BYTE_LENGTH              4
#define RSDP_XSDT_ADDRESS_BYTE_LENGTH        8
#define RSDP_EXTENDED_CHECKSUM_BYTE_LENGTH   1
#define RSDP_RESERVED_BYTE_LENGTH            3

//
// Root System Description Table Fields (RSDT)
//
#define ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE_ASCII       {'R', 'S', 'D', 'T'}
#define ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE SIGNATURE_32('R', 'S', 'D', 'T')
#define RSDT_SIGNATURE_BYTE_OFFEST          0
#define RSDT_LENGTH_BYTE_OFFEST             4
#define RSDT_REVISION_BYTE_OFFEST           8
#define RSDT_CHECKSUM_BYTE_OFFEST           9
#define RSDT_OEM_ID_BYTE_OFFEST             10
#define RSDT_OEM_TABLE_ID_BYTE_OFFEST       16
#define RSDT_OEM_REVISION_BYTE_OFFEST       24
#define RSDT_CREATOR_ID_BYTE_OFFEST         28
#define RSDT_CREATOR_REVISION_BYTE_OFFEST   32
#define RSDT_DEFINITION_BLOCK_BYTE_OFFEST   36
#define RSDT_SIGNATURE_BYTE_LENGTH          4
#define RSDT_LENGTH_BYTE_LENGTH             4
#define RSDT_REVISION_BYTE_LENGTH           1
#define RSDT_CHECKSUM_BYTE_LENGTH           1
#define RSDT_OEM_ID_BYTE_LENGTH             6
#define RSDT_OEM_TABLE_ID_BYTE_LENGTH       8
#define RSDT_OEM_REVISION_BYTE_LENGTH       4
#define RSDT_CREATOR_ID_BYTE_LENGTH         4
#define RSDT_CREATOR_REVISION_BYTE_LENGTH   4
// #define RSDT_DEFINITION_BLOCK_BYTE_LENGTH  4*n

//
// Extended System Description Table Fields (XSDT)
//
#define EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE_ASCII       {'X', 'S', 'D', 'T'}
#define EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE SIGNATURE_32('X', 'S', 'D', 'T')
#define XSDT_SIGNATURE_BYTE_OFFEST          0
#define XSDT_LENGTH_BYTE_OFFEST             4
#define XSDT_REVISION_BYTE_OFFEST           8
#define XSDT_CHECKSUM_BYTE_OFFEST           9
#define XSDT_OEM_ID_BYTE_OFFEST             10
#define XSDT_OEM_TABLE_ID_BYTE_OFFEST       16
#define XSDT_OEM_REVISION_BYTE_OFFEST       24
#define XSDT_CREATOR_ID_BYTE_OFFEST         28
#define XSDT_CREATOR_REVISION_BYTE_OFFEST   32
#define XSDT_DEFINITION_BLOCK_BYTE_OFFEST   36
#define XSDT_SIGNATURE_BYTE_LENGTH          4
#define XSDT_LENGTH_BYTE_LENGTH             4
#define XSDT_REVISION_BYTE_LENGTH           1
#define XSDT_CHECKSUM_BYTE_LENGTH           1
#define XSDT_OEM_ID_BYTE_LENGTH             6
#define XSDT_OEM_TABLE_ID_BYTE_LENGTH       8
#define XSDT_OEM_REVISION_BYTE_LENGTH       4
#define XSDT_CREATOR_ID_BYTE_LENGTH         4
#define XSDT_CREATOR_REVISION_BYTE_LENGTH   4
// #define RSDT_DEFINITION_BLOCK_BYTE_LENGTH  8*n

//
// Differentiated System Description Table Fields (DSDT)
//
#define DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE_ASCII       {'D', 'S', 'D', 'T'}
#define DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE SIGNATURE_32('D', 'S', 'D', 'T')
#define DSDT_SIGNATURE_BYTE_OFFEST          0
#define DSDT_LENGTH_BYTE_OFFEST             4
#define DSDT_REVISION_BYTE_OFFEST           8
#define DSDP_CHECKSUM_BYTE_OFFEST           9
#define DSDT_OEM_ID_BYTE_OFFEST             10
#define DSDT_OEM_TABLE_ID_BYTE_OFFEST       16
#define DSDT_OEM_REVISION_BYTE_OFFEST       24
#define DSDT_CREATOR_ID_BYTE_OFFEST         28
#define DSDT_CREATOR_REVISION_BYTE_OFFEST   32
#define DSDT_DEFINITION_BLOCK_BYTE_OFFEST   36
#define DSDT_SIGNATURE_BYTE_LENGTH          4
#define DSDT_LENGTH_BYTE_LENGTH             4
#define DSDT_REVISION_BYTE_LENGTH           1
#define DSDP_CHECKSUM_BYTE_LENGTH           1
#define DSDT_OEM_ID_BYTE_LENGTH             6
#define DSDT_OEM_TABLE_ID_BYTE_LENGTH       8
#define DSDT_OEM_REVISION_BYTE_LENGTH       4
#define DSDT_CREATOR_ID_BYTE_LENGTH         4
#define DSDT_CREATOR_REVISION_BYTE_LENGTH   4
// #define DSDT_DEFINITION_BLOCK_BYTE_LENGTH  n

//
// Secondary System Description Table Fields (SSDT)
//
#define SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE_ASCII       {'S', 'S', 'D', 'T'}
#define SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE SIGNATURE_32('S', 'S', 'D', 'T')
#define SSDT_SIGNATURE_BYTE_OFFEST          0
#define SSDT_LENGTH_BYTE_OFFEST             4
#define SSDT_REVISION_BYTE_OFFEST           8
#define SSDP_CHECKSUM_BYTE_OFFEST           9
#define SSDT_OEM_ID_BYTE_OFFEST             10
#define SSDT_OEM_TABLE_ID_BYTE_OFFEST       16
#define SSDT_OEM_REVISION_BYTE_OFFEST       24
#define SSDT_CREATOR_ID_BYTE_OFFEST         28
#define SSDT_CREATOR_REVISION_BYTE_OFFEST   32
#define SSDT_DEFINITION_BLOCK_BYTE_OFFEST   36
#define SSDT_SIGNATURE_BYTE_LENGTH          4
#define SSDT_LENGTH_BYTE_LENGTH             4
#define SSDT_REVISION_BYTE_LENGTH           1
#define SSDP_CHECKSUM_BYTE_LENGTH           1
#define SSDT_OEM_ID_BYTE_LENGTH             6
#define SSDT_OEM_TABLE_ID_BYTE_LENGTH       8
#define SSDT_OEM_REVISION_BYTE_LENGTH       4
#define SSDT_CREATOR_ID_BYTE_LENGTH         4
#define SSDT_CREATOR_REVISION_BYTE_LENGTH   4
// #define DSDT_DEFINITION_BLOCK_BYTE_LENGTH  n

//
// AML Decode
//
#define BytePrefix   0x0A
#define WordPrefix   0x0B
#define DWordPrefix  0x0C
#define StringPrefix 0x0D
#define QWordPrefix  0x0E
#define AML_NAME_OPERATION_CODE 0x08

//
// ASL Object Information
//
typedef struct {
	UINTN ObjectAddress;
	UINT16 ObjectType;
	UINT32 ObjectName;
	UINT8 ObjectDataType;
	UINT8 ObjectDataLength;
	UINTN ObjectDataStartAddress;
} ASL_OBJECT_INFO;

//
// Root System Description Pointer Structure
//
typedef struct {
  UINT8     Signature[8];
  UINT8     Checksum;
  UINT8     OemId[6];
  UINT8     Revision;
  UINT32    RsdtAddress;
  UINT32    Length;
  UINT64    XsdtAddress;
  UINT8     ExtendedChecksum;
  UINT8     Reserved[3];
} ROOT_SYSTEM_DESCRIPTION_POINTER;

//
// Root System Description Table Structure
//
typedef struct {
  UINT8     Signature[4];
  UINT32    Length;
  UINT8     Revision;
  UINT8     Checksum;
  UINT8     OemId[6];
  UINT8     OemTableId[8];
  UINT32    OemRevision;
  UINT8     CreatorID[4];
  UINT32    CreatorRevision;
  UINT32    Entry;
} ROOT_SYSTEM_DESCRIPTION_TABLE;

//
// Extended System Description Table Structure
//
typedef struct {
  UINT8     Signature[4];
  UINT32    Length;
  UINT8     Revision;
  UINT8     Checksum;
  UINT8     OemId[6];
  UINT8     OemTableId[8];
  UINT32    OemRevision;
  UINT8     CreatorID[4];
  UINT32    CreatorRevision;
  UINT64    Entry;
} EXTENDED_SYSTEM_DESCRIPTION_TABLE;

//
// Differentiated System Description Table Structure
//
typedef struct {
  UINT8     Signature[4];
  UINT32    Length;
  UINT8     Revision;
  UINT8     Checksum;
  UINT8     OemId[6];
  UINT8     OemTableId[8];
  UINT32    OemRevision;
  UINT8     CreatorID[4];
  UINT32    CreatorRevision;
  UINTN     DefinitionBlock;
} DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE;

//
// Secondary System Description Table Structure
//
typedef struct {
  UINT8     Signature[4];
  UINT32    Length;
  UINT8     Revision;
  UINT8     Checksum;
  UINT8     OemId[6];
  UINT8     OemTableId[8];
  UINT32    OemRevision;
  UINT8     CreatorID[4];
  UINT32    CreatorRevision;
  UINTN     DefinitionBlock;
} SECONDARY_SYSTEM_DESCRIPTION_TABLE;


//
//  ACPI Tables header structure
//
typedef struct {
    UINT32      Signature;
    UINT32      Length;
    UINT8       Revision;
    UINT8       Checksum;
    UINT8       OemId[6]; 
    UINT8       OemTblId[8];
    UINT32      OemRev;
    UINT32      CreatorId;
    UINT32      CreatorRev;
} ACPI_TABLE_HEADER;


//
// ACPI RSDT Data
//
typedef struct {
    UINT8  Signature[4];
    UINTN Address;
} ACPI_RSDT_DATA;

//
// ACPI XSDT Data
//
typedef struct {
    UINT8  Signature[4];
    UINTN Address;
} ACPI_XSDT_DATA;


UINT8
CalculateTableChecksum (
  IN UINT8   *tableStart,
  IN UINT32  bytesCount
  );

VOID
PrintXSDTInfo (
  IN EXTENDED_SYSTEM_DESCRIPTION_TABLE  *XSDT
  );

VOID
PrintRSDTInfo (
  IN ROOT_SYSTEM_DESCRIPTION_TABLE  *RSDT
  );

VOID
PrintRSDPInfo (
  IN OUT ROOT_SYSTEM_DESCRIPTION_POINTER  *RSDP
  );

EFI_STATUS
GetSSDT (
  IN  EFI_SYSTEM_TABLE  *SystemTable,
  IN  UINT8             *SsdtSignature,
  OUT UINTN             *SsdtAddress
  );

EFI_STATUS
GetXSDT (
  IN     EFI_SYSTEM_TABLE                   *SystemTable,
  IN OUT EXTENDED_SYSTEM_DESCRIPTION_TABLE  *XSDT
  );

EFI_STATUS
GetRSDT (
  IN     EFI_SYSTEM_TABLE               *SystemTable,
  IN OUT ROOT_SYSTEM_DESCRIPTION_TABLE  *RSDT
  );

EFI_STATUS
GetRSDP (
  IN     EFI_SYSTEM_TABLE                 *SystemTable,
  IN OUT ROOT_SYSTEM_DESCRIPTION_POINTER  *RSDP
  );

EFI_STATUS
UpdateNameObjectInAsl (
  IN UINTN  TableAddress,
  IN UINT8  *ObjectName,
  IN UINTN  ObjectNameLength,
  IN UINTN  Value
  );

#endif // __ACPI_LIB_H__
