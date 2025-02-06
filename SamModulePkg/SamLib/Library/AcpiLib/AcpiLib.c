#include "Library/AcpiLib.h"

#include <Library/BaseMemoryLib.h>
#include <Library/CommonLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>



/**
  Static variable to store the RSDT (Root System Description Table) address.

  This variable holds the address of the RSDT table when operating in 32-bit UEFI mode.
  It is set once retrieved from the RSDP structure and used across functions that 
  require access to ACPI tables through the RSDT.

**/
static UINTN rsdtAddress = 0;

/**
  Static variable to store the XSDT (Extended System Description Table) address.

  This variable holds the address of the XSDT table when operating in 64-bit UEFI mode.
  It is set once retrieved from the RSDP structure and is used across functions that 
  require access to ACPI tables through the XSDT.

**/
static UINTN xsdtAddress = 0;

/**
  Static variable to store the RSDP (Root System Description Pointer) address.

  This variable holds the address of the RSDP structure, which is used to retrieve
  either the RSDT or XSDT address based on the current UEFI bit mode. The RSDP address 
  is obtained through the system table configuration and remains accessible for various
  ACPI-related functions.

**/
static UINTN rsdpAddress = 0;


/**
  Calculate the checksum of a table.

  This function calculates an 8-bit checksum for a table by summing all the bytes 
  in the table and returning the two's complement of the sum. The checksum ensures 
  that the sum of all bytes in the table (including the checksum byte itself) equals zero.

  @param[in]  tableStart   Pointer to the start of the table to be checksummed.
  @param[in]  bytesCount   Number of bytes to include in the checksum calculation.

  @retval     UINT8        The calculated checksum (8-bit).

**/
UINT8
CalculateTableChecksum (
  IN UINT8   *tableStart,
  IN UINT32  bytesCount
  )
{
    UINT8 result = 0;
    for (UINTN i = 0; i < bytesCount; i++) {
        result += tableStart[i];
    }
    result = 0 - result;
    return result;
}


/**
  Print information about the Extended System Description Table (XSDT).

  This function displays details of the XSDT structure, including its 
  signature, length, revision, checksum, OEM ID, OEM Table ID, and creator information.
  It then lists the ACPI tables referenced in the XSDT.

  @param[in]  XSDT         Pointer to the XSDT structure containing ACPI information.

**/
VOID
PrintXSDTInfo (
  IN EXTENDED_SYSTEM_DESCRIPTION_TABLE  *XSDT
  )
{
    Print(L"=============== XSDT ================\n");

    Print(L"Signature: ");
    for (UINT8 i = 0; i < XSDT_SIGNATURE_BYTE_LENGTH; i++) {
        Print(L"%c", XSDT->Signature[i]);
    }
    Print(L"\n");

    Print(L"Length: 0x%08x\n", XSDT->Length);

    Print(L"Revision: 0x%02x\n", XSDT->Revision);

    Print(L"Checksum: 0x%02x\n", XSDT->Checksum);

    Print(L"OEMID: ");
    for (UINT8 i = 0; i < XSDT_OEM_ID_BYTE_LENGTH; i++) {
        Print(L"%c", XSDT->OemId[i]);
    }
    Print(L"\n");

    Print(L"OEM Table ID: ");
    for (UINT8 i = 0; i < XSDT_OEM_TABLE_ID_BYTE_LENGTH; i++) {
        Print(L"%c", XSDT->OemTableId[i]);
    }
    Print(L"\n");

    Print(L"OEM Revision: 0x%08x\n", XSDT->OemRevision);
    
    Print(L"Creator ID: ");
    for (UINT8 i = 0; i < XSDT_CREATOR_ID_BYTE_LENGTH; i++) {
        Print(L"%c", XSDT->CreatorID[i]);
    }
    Print(L"\n");

    Print(L"Creator Revision: 0x%08x\n", XSDT->CreatorRevision);

    Print(L"Entry: 0x%016x\n", XSDT->Entry);

    Print(L"========== ACPI XDST LIST ==========\n");
    Print(L"Index Signature      Address\n");
    UINTN tableNumber = (XSDT->Length - XSDT_DEFINITION_BLOCK_BYTE_OFFEST) / sizeof(XSDT->Entry);
    ACPI_XSDT_DATA *AcpiTables = AllocateZeroPool(tableNumber * sizeof(ACPI_XSDT_DATA));
    for (UINTN tableIndex = 0, XSDT_ACPI_Tables_Address = xsdtAddress + XSDT_DEFINITION_BLOCK_BYTE_OFFEST; \
        XSDT_ACPI_Tables_Address < xsdtAddress + XSDT->Length; \
        tableIndex++, XSDT_ACPI_Tables_Address += sizeof(XSDT->Entry)) {
        UINTN tableAddress = *(UINTN*)XSDT_ACPI_Tables_Address;
        for (UINT8 i = 0; i < sizeof(AcpiTables[0].Signature); i++) {
            AcpiTables[tableIndex].Signature[i] = *(UINT8*)(tableAddress + i);
        }
        AcpiTables[tableIndex].Address = tableAddress;
        Print(L"%04d    %a    0x%016x\n", tableIndex, AcpiTables[tableIndex].Signature, AcpiTables[tableIndex].Address);
    }
    Print(L"\n");
}


/**
  Print information about the Root System Description Table (RSDT).

  This function displays details of the RSDT structure, including its 
  signature, length, revision, checksum, OEM ID, OEM Table ID, and creator information.
  It then lists the ACPI tables referenced in the RSDT.

  @param[in]  RSDT         Pointer to the RSDT structure containing ACPI information.

**/
VOID
PrintRSDTInfo (
  IN ROOT_SYSTEM_DESCRIPTION_TABLE  *RSDT
  )
{
    Print(L"=============== RSDT ================\n");

    Print(L"Signature: ");
    for (UINT8 i = 0; i < RSDT_SIGNATURE_BYTE_LENGTH; i++) {
        Print(L"%c", RSDT->Signature[i]);
    }
    Print(L"\n");

    Print(L"Length: 0x%08x\n", RSDT->Length);

    Print(L"Revision: 0x%02x\n", RSDT->Revision);

    Print(L"Checksum: 0x%02x\n", RSDT->Checksum);

    Print(L"OEMID: ");
    for (UINT8 i = 0; i < RSDT_OEM_ID_BYTE_LENGTH; i++) {
        Print(L"%c", RSDT->OemId[i]);
    }
    Print(L"\n");

    Print(L"OEM Table ID: ");
    for (UINT8 i = 0; i < RSDT_OEM_TABLE_ID_BYTE_LENGTH; i++) {
        Print(L"%c", RSDT->OemTableId[i]);
    }
    Print(L"\n");

    Print(L"OEM Revision: 0x%08x\n", RSDT->OemRevision);

    Print(L"Creator ID: ");
    for (UINT8 i = 0; i < RSDT_CREATOR_ID_BYTE_LENGTH; i++) {
        Print(L"%c", RSDT->CreatorID[i]);
    }
    Print(L"\n");

    Print(L"Creator Revision: 0x%08x\n", RSDT->CreatorRevision);

    Print(L"Entry: 0x%08x\n", RSDT->Entry);

    Print(L"========== ACPI RDST LIST ==========\n");
    Print(L"Index Signature   Address \n");

    UINTN tableNumber = (RSDT->Length - RSDT_DEFINITION_BLOCK_BYTE_OFFEST) / sizeof(RSDT->Entry);
    ACPI_RSDT_DATA *AcpiTables = AllocateZeroPool(tableNumber * sizeof(ACPI_RSDT_DATA));
    for (UINTN tableIndex = 0, RSDT_ACPI_Tables_Address = rsdtAddress + RSDT_DEFINITION_BLOCK_BYTE_OFFEST; \
        RSDT_ACPI_Tables_Address < rsdtAddress + RSDT->Length; \
        tableIndex++, RSDT_ACPI_Tables_Address += sizeof(RSDT->Entry)) {
        UINTN tableAddress = *(UINTN*)RSDT_ACPI_Tables_Address;
        for (UINT8 i = 0; i < sizeof(AcpiTables[0].Signature); i++) {
            AcpiTables[tableIndex].Signature[i] = *(UINT8*)(tableAddress + i);
        }
        AcpiTables[tableIndex].Address = tableAddress;
        Print(L"%04d    %a    0x%08x\n", tableIndex, AcpiTables[tableIndex].Signature, AcpiTables[tableIndex].Address);
    }
    Print(L"\n");
}


/**
  Print information about the Root System Description Pointer (RSDP).

  This function displays the contents of the RSDP, including its 
  signature, checksum, OEM ID, version, RSDT and XSDT addresses, 
  extended checksum, and reserved fields.

  @param[in,out]  RSDP     Pointer to the RSDP structure containing ACPI information.

**/
VOID
PrintRSDPInfo (
  IN OUT ROOT_SYSTEM_DESCRIPTION_POINTER  *RSDP
  )
{

    Print(L"=============== RSDP ================\n");

    Print(L"Signature: ");
    for (UINT8 i = 0; i < RSDP_SIGNATURE_BYTE_LENGTH; i++) {
        Print(L"%c", RSDP->Signature[i]);
    }
    Print(L"\n");

    Print(L"Checksum: 0x%02x\n", RSDP->Checksum);

    Print(L"OEMID: ");
    for (UINT8 i = 0; i < RSDP_OEMID_BYTE_LENGTH; i++) {
        Print(L"%c", RSDP->OemId[i]);
    }
    Print(L"\n");

    Print(L"Revision: 0x%02x\n", RSDP->Revision);

    Print(L"rsdtAddress: 0x%08x\n", RSDP->RsdtAddress);

    Print(L"Length: 0x%08x\n", RSDP->Length);

    Print(L"xsdtAddress: 0x%016x\n", RSDP->XsdtAddress);

    Print(L"Extended Checksum: 0x%02x\n", RSDP->ExtendedChecksum);

    Print(L"Reserved: ");
    for (UINT8 i = 0; i < RSDP_RESERVED_BYTE_LENGTH; i++) {
        Print(L"0x%x ", RSDP->Reserved[i]);
    }
    Print(L"\n\n");
}


/**
  Retrieve the address of a specific SSDT table based on a provided signature.

  This function searches for a specified Secondary System Description Table (SSDT) in the
  ACPI table list, either through the RSDT (for 32-bit mode) or XSDT (for 64-bit mode), based 
  on the current UEFI bit mode. It compares table signatures to locate and return the address
  of the target SSDT table.

  @param[in]  SystemTable     Pointer to the EFI system table.
  @param[in]  TableSignature  Signature of the SSDT table to be located.
  @param[out] TableAddress    Address of the located SSDT table.

  @retval     EFI_SUCCESS     The specified SSDT table was found and its address is returned.
  @retval     EFI_NOT_FOUND   The specified SSDT table was not found.
  @retval     EFI_UNSUPPORTED The bit mode is unsupported.
  @retval     EFI_ERROR       An error occurred while retrieving the table.

**/
EFI_STATUS
GetSSDT (
  IN  EFI_SYSTEM_TABLE  *SystemTable,
  IN  UINT8             *TableSignature,
  OUT UINTN             *TableAddress
  )
{

    UINT8       bitMode     = 0;
    EFI_STATUS  Status      = EFI_SUCCESS;

    Status = CheckUEFIBitMode(&bitMode);
    if (EFI_ERROR(Status)) {
        return Status;
    } else {
        if (bitMode == UEFI_32_BIT_MODE) {
            ROOT_SYSTEM_DESCRIPTION_TABLE      *RSDT = AllocateZeroPool(sizeof(ROOT_SYSTEM_DESCRIPTION_TABLE));
            rsdpAddress = (UINTN)ScanEfiConfigurationTable(SystemTable, &gEfiAcpi20TableGuid);
            if (rsdpAddress != 0x00) {
                rsdpAddress = *(UINTN*)(rsdpAddress + RSDP_RSDT_ADDRESS_BYTE_OFFEST);
                if (rsdpAddress != 0x00) {
                    RSDT->Length = *(UINT32*)(rsdpAddress + RSDT_LENGTH_BYTE_OFFEST);
                    RSDT->Entry = *(UINT32*)(rsdpAddress + RSDT_DEFINITION_BLOCK_BYTE_OFFEST);
                    UINT8 SsdtSignature[SSDT_SIGNATURE_BYTE_LENGTH] = SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE_ASCII;
                    UINTN tableNumber = (RSDT->Length - RSDT_DEFINITION_BLOCK_BYTE_OFFEST) / sizeof(RSDT->Entry);
                    ACPI_RSDT_DATA *AcpiTables = AllocateZeroPool(tableNumber * sizeof(ACPI_RSDT_DATA));
                    for (UINTN tableIndex = 0, RSDT_ACPI_Tables_Address = rsdpAddress + RSDT_DEFINITION_BLOCK_BYTE_OFFEST; \
                        RSDT_ACPI_Tables_Address < rsdpAddress + RSDT->Length; \
                        tableIndex++, RSDT_ACPI_Tables_Address += sizeof(RSDT->Entry)) {
                        UINTN tableAddress = *(UINTN*)RSDT_ACPI_Tables_Address;
                        for (UINT8 i = 0; i < sizeof(AcpiTables[0].Signature); i++) {
                            AcpiTables[tableIndex].Signature[i] = *(UINT8*)((UINTN)tableAddress + i);
                        }
                        AcpiTables[tableIndex].Address = tableAddress;
                        if (CompareMem(AcpiTables[tableIndex].Signature, SsdtSignature, sizeof(SsdtSignature)) == 0) {
                            UINT8 SSDT_OEM_TABLE_ID[SSDT_OEM_TABLE_ID_BYTE_LENGTH] = {0};
                            for (UINT8 i = 0; i < SSDT_OEM_TABLE_ID_BYTE_LENGTH; i++) {
                                SSDT_OEM_TABLE_ID[i] =  *(UINT8*)(AcpiTables[tableIndex].Address + SSDT_OEM_TABLE_ID_BYTE_OFFEST + i);
                            }
                            if (CompareMem(SSDT_OEM_TABLE_ID, TableSignature, sizeof(SSDT_OEM_TABLE_ID)) == 0) {
                                *TableAddress = AcpiTables[tableIndex].Address;
                                break;
                            }
                        }
                    }
                    if (TableAddress != 0x00) {
                        Status = EFI_SUCCESS;
                    } else {
                        Status = EFI_NOT_FOUND;
                    }
                    if (RSDT != NULL) {
                        FreePool(RSDT);
                    }
                    if (AcpiTables != NULL) {
                        FreePool(AcpiTables);
                    }
                }
            }
        } else {
            EXTENDED_SYSTEM_DESCRIPTION_TABLE  *XSDT = AllocateZeroPool(sizeof(EXTENDED_SYSTEM_DESCRIPTION_TABLE));
            rsdpAddress = (UINTN)ScanEfiConfigurationTable(SystemTable, &gEfiAcpi20TableGuid);
            if (rsdpAddress != 0x00) {
                xsdtAddress = *(UINTN*)(rsdpAddress + RSDP_XSDT_ADDRESS_BYTE_OFFEST);
                if (rsdpAddress != 0x00) {
                    XSDT->Length = *(UINT32*)(xsdtAddress + XSDT_LENGTH_BYTE_OFFEST);
                    XSDT->Entry = *(UINT64*)(xsdtAddress + XSDT_DEFINITION_BLOCK_BYTE_OFFEST);
                    UINT8 SsdtSignature[SSDT_SIGNATURE_BYTE_LENGTH] = SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE_ASCII;
                    UINTN tableNumber = (XSDT->Length - XSDT_DEFINITION_BLOCK_BYTE_OFFEST) / sizeof(XSDT->Entry);
                    ACPI_XSDT_DATA *AcpiTables = AllocateZeroPool(tableNumber * sizeof(ACPI_XSDT_DATA));
                    for (UINTN tableIndex = 0, XSDT_ACPI_Tables_Address = xsdtAddress + XSDT_DEFINITION_BLOCK_BYTE_OFFEST; \
                        XSDT_ACPI_Tables_Address < xsdtAddress + XSDT->Length; \
                        tableIndex++, XSDT_ACPI_Tables_Address += sizeof(XSDT->Entry)) {
                        UINTN tableAddress = *(UINTN*)XSDT_ACPI_Tables_Address;
                        for (UINT8 i = 0; i < sizeof(AcpiTables[0].Signature); i++) {
                            AcpiTables[tableIndex].Signature[i] = *(UINT8*)(tableAddress + i);
                        }
                        AcpiTables[tableIndex].Address = tableAddress;
                        if (CompareMem(AcpiTables[tableIndex].Signature, SsdtSignature, sizeof(SsdtSignature)) == 0) {
                            UINT8 SSDT_OEM_TABLE_ID[SSDT_OEM_TABLE_ID_BYTE_LENGTH] = {0};
                            for (UINT8 i = 0; i < SSDT_OEM_TABLE_ID_BYTE_LENGTH; i++) {
                                SSDT_OEM_TABLE_ID[i] =  *(UINT8*)(AcpiTables[tableIndex].Address + SSDT_OEM_TABLE_ID_BYTE_OFFEST + i);
                            }
                            if (CompareMem(SSDT_OEM_TABLE_ID, TableSignature, sizeof(SSDT_OEM_TABLE_ID)) == 0) {
                                *TableAddress = AcpiTables[tableIndex].Address;
                                break;
                            }
                        }
                    }
                    if (TableAddress != 0x00) {
                        Status = EFI_SUCCESS;
                    } else {
                        Status = EFI_NOT_FOUND;
                    }
                    if (XSDT != NULL) {
                        FreePool(XSDT);
                    }
                    if (AcpiTables != NULL) {
                        FreePool(AcpiTables);
                    }
                }
            }
        }
    }
    return Status;
}


/**
  Retrieve the XSDT table.

  This function retrieves the Extended System Description Table (XSDT) based on the current
  UEFI bit mode. It checks the bit mode and, if in 64-bit mode, loads the XSDT table entries
  and metadata into the provided structure.

  @param[in]     SystemTable  Pointer to the EFI system table.
  @param[in,out] XSDT         Pointer to an EXTENDED_SYSTEM_DESCRIPTION_TABLE structure 
                              to store the XSDT data.

  @retval        EFI_SUCCESS  XSDT table found and loaded.
  @retval        EFI_UNSUPPORTED  XSDT retrieval is not supported in the current bit mode.
  @retval        EFI_ERROR    An error occurred while retrieving the XSDT table.

**/
EFI_STATUS
GetXSDT (
  IN     EFI_SYSTEM_TABLE                   *SystemTable,
  IN OUT EXTENDED_SYSTEM_DESCRIPTION_TABLE  *XSDT
  )
{
    UINT8       bitMode     = 0;
    EFI_STATUS  Status      = EFI_SUCCESS;

    Status = CheckUEFIBitMode(&bitMode);
    if (EFI_ERROR(Status)) {
        return Status;
    } else {
        if (bitMode == UEFI_64_BIT_MODE) {
            rsdpAddress = (UINTN)ScanEfiConfigurationTable(SystemTable, &gEfiAcpi20TableGuid);
            if (rsdpAddress != 0x00) {
                xsdtAddress = *(UINTN*)(rsdpAddress + RSDP_XSDT_ADDRESS_BYTE_OFFEST);
                if (rsdpAddress != 0x00) {
                    for (UINT8 i = 0; i < XSDT_SIGNATURE_BYTE_LENGTH; i++) {
                        XSDT->Signature[i] = *(UINT8*)(xsdtAddress + i);
                    }
                    XSDT->Length = *(UINT32*)(xsdtAddress + XSDT_LENGTH_BYTE_OFFEST);
                    XSDT->Revision = *(UINT8*)(xsdtAddress + XSDT_REVISION_BYTE_OFFEST);
                    XSDT->Checksum = *(UINT8*)(xsdtAddress + XSDT_CHECKSUM_BYTE_OFFEST);
                    for (UINT8 i = 0; i < XSDT_OEM_ID_BYTE_LENGTH; i++) {
                        XSDT->OemId[i] = *(UINT8*)(xsdtAddress + XSDT_OEM_ID_BYTE_OFFEST + i);
                    }
                    for (UINT8 i = 0; i < XSDT_OEM_TABLE_ID_BYTE_LENGTH; i++) {
                        XSDT->OemTableId[i] = *(UINT8*)(xsdtAddress + XSDT_OEM_TABLE_ID_BYTE_OFFEST + i);
                    }
                    XSDT->OemRevision = *(UINT32*)(xsdtAddress + XSDT_OEM_REVISION_BYTE_OFFEST);
                    for (UINT8 i = 0; i < XSDT_CREATOR_ID_BYTE_LENGTH; i++) {
                        XSDT->CreatorID[i] = *(UINT8*)(xsdtAddress + XSDT_CREATOR_ID_BYTE_OFFEST + i);
                    }
                    XSDT->CreatorRevision = *(UINT32*)(xsdtAddress + XSDT_CREATOR_REVISION_BYTE_OFFEST);
                    XSDT->Entry = *(UINT64*)(xsdtAddress + XSDT_DEFINITION_BLOCK_BYTE_OFFEST);
                    return EFI_SUCCESS;
                }
            }
        } else {
            return EFI_UNSUPPORTED;
        }
    }
    return Status;
}


/**
  Retrieve the RSDT table.

  This function retrieves the Root System Description Table (RSDT) based on the current
  UEFI bit mode. If in 32-bit mode, it loads the RSDT table entries and metadata 
  into the provided structure.

  @param[in]     SystemTable  Pointer to the EFI system table.
  @param[in,out] RSDT         Pointer to a ROOT_SYSTEM_DESCRIPTION_TABLE structure 
                              to store the RSDT data.

  @retval        EFI_SUCCESS  RSDT table found and loaded.
  @retval        EFI_UNSUPPORTED  RSDT retrieval is not supported in the current bit mode.
  @retval        EFI_ERROR    An error occurred while retrieving the RSDT table.

**/
EFI_STATUS
GetRSDT (
  IN     EFI_SYSTEM_TABLE               *SystemTable,
  IN OUT ROOT_SYSTEM_DESCRIPTION_TABLE  *RSDT
  )
{

    UINT8       bitMode     = 0;
    EFI_STATUS  Status      = EFI_SUCCESS;

    Status = CheckUEFIBitMode(&bitMode);
    if (EFI_ERROR(Status)) {
        return Status;
    } else {
        if (bitMode == UEFI_32_BIT_MODE) {
            rsdpAddress = (UINTN)ScanEfiConfigurationTable(SystemTable, &gEfiAcpi10TableGuid);
            if (rsdpAddress != 0x00) {
                rsdtAddress = *(UINT32*)(rsdpAddress + RSDP_RSDT_ADDRESS_BYTE_OFFEST);
                if (rsdpAddress != 0x00) {
                    for (UINT8 i = 0; i < RSDT_SIGNATURE_BYTE_LENGTH; i++) {
                        RSDT->Signature[i] = *(UINT8*)(rsdtAddress + i);
                    }
                    RSDT->Length = *(UINT32*)(rsdtAddress + RSDT_LENGTH_BYTE_OFFEST);
                    RSDT->Revision = *(UINT8*)(rsdtAddress + RSDT_REVISION_BYTE_OFFEST);
                    RSDT->Checksum = *(UINT8*)(rsdtAddress + RSDT_CHECKSUM_BYTE_OFFEST);
                    for (UINT8 i = 0; i < RSDT_OEM_ID_BYTE_LENGTH; i++) {
                        RSDT->OemId[i] = *(UINT8*)(rsdtAddress + RSDT_OEM_ID_BYTE_OFFEST + i);
                    }
                    for (UINT8 i = 0; i < RSDT_OEM_TABLE_ID_BYTE_LENGTH; i++) {
                        RSDT->OemTableId[i] = *(UINT8*)(rsdtAddress + RSDT_OEM_TABLE_ID_BYTE_OFFEST + i);
                    }
                    RSDT->OemRevision = *(UINT32*)(rsdtAddress + RSDT_OEM_REVISION_BYTE_OFFEST);
                    for (UINT8 i = 0; i < RSDT_CREATOR_ID_BYTE_LENGTH; i++) {
                        RSDT->CreatorID[i] = *(UINT8*)(rsdtAddress + RSDT_CREATOR_ID_BYTE_OFFEST + i);
                    }
                    RSDT->CreatorRevision = *(UINT32*)(rsdtAddress + RSDT_CREATOR_REVISION_BYTE_OFFEST);
                    RSDT->Entry = *(UINT32*)(rsdtAddress + RSDT_DEFINITION_BLOCK_BYTE_OFFEST);
                    return EFI_SUCCESS;
                }
            }
        } else {
            return EFI_UNSUPPORTED;
        }
    }
    return Status;
}


/**
  Retrieve the RSDP structure.

  This function locates and retrieves the Root System Description Pointer (RSDP) based 
  on the current UEFI bit mode and stores it in the provided structure.

  @param[in]     SystemTable  Pointer to the EFI system table.
  @param[in,out] RSDP         Pointer to a ROOT_SYSTEM_DESCRIPTION_POINTER structure 
                              to store the RSDP data.

  @retval        EFI_SUCCESS  RSDP structure found and loaded.
  @retval        EFI_NOT_FOUND The RSDP structure was not found.
  @retval        EFI_ERROR    An error occurred while retrieving the RSDP structure.

**/
EFI_STATUS
GetRSDP (
  IN     EFI_SYSTEM_TABLE                 *SystemTable,
  IN OUT ROOT_SYSTEM_DESCRIPTION_POINTER  *RSDP
  )
{
    UINT8       bitMode       = 0;
    EFI_STATUS  Status        = EFI_SUCCESS;
    EFI_GUID    smbiosGuids[] = {0};

    Status = CheckUEFIBitMode(&bitMode);
    if (EFI_ERROR(Status)) {
        return Status;
    } else {
        if (bitMode == UEFI_32_BIT_MODE) {
            smbiosGuids[0] = gEfiAcpi10TableGuid;
        } else {
            smbiosGuids[0] = gEfiAcpi20TableGuid;
        }
    }
    
    for (UINT8 guidNumber = 0; guidNumber < sizeof(smbiosGuids) / sizeof(EFI_GUID); guidNumber++) {
        rsdpAddress = (UINTN)ScanEfiConfigurationTable(SystemTable, &smbiosGuids[guidNumber]);
        if (rsdpAddress != 0) {
            for (UINT8 i = 0; i < RSDP_SIGNATURE_BYTE_LENGTH; i++) {
                RSDP->Signature[i] = *(UINT8*)(rsdpAddress + RSDP_SIGNATURE_BYTE_OFFEST + i);
            }
            RSDP->Checksum = *(UINT8*)(rsdpAddress + RSDP_CHECKSUM_BYTE_OFFEST);
            for (UINT8 i = 0; i < RSDP_OEMID_BYTE_LENGTH; i++) {
                RSDP->OemId[i] = *(UINT8*)(rsdpAddress + RSDP_OEMID_BYTE_OFFEST + i);
            }
            RSDP->Revision = *(UINT8*)(rsdpAddress + RSDP_REVISION_BYTE_OFFEST);
            RSDP->RsdtAddress = *(UINT32*)(rsdpAddress + RSDP_RSDT_ADDRESS_BYTE_OFFEST);
            // ACPI VERSION IS NOT 1.0
            if (RSDP->Revision >= 0x02) {
                RSDP->Length = *(UINT32*)(rsdpAddress + RSDP_LENGTH_BYTE_OFFEST);
                RSDP->XsdtAddress = *(UINT64*)(rsdpAddress + RSDP_XSDT_ADDRESS_BYTE_OFFEST);
                RSDP->ExtendedChecksum = *(UINT8*)(rsdpAddress + RSDP_EXTENDED_CHECKSUM_BYTE_OFFEST);
                for (UINT8 i = 0; i < RSDP_RESERVED_BYTE_LENGTH; i++) {
                    RSDP->Reserved[i] = *(UINT8*)(rsdpAddress + RSDP_RESERVED_BYTE_OFFEST + i);
                }
            } else {
                RSDP->Length = 0;
                RSDP->XsdtAddress = 0;
                RSDP->ExtendedChecksum = 0;
                for (UINT8 i = 0; i < RSDP_RESERVED_BYTE_LENGTH; i++) {
                    RSDP->Reserved[i] = 0;
                }
            }

            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}


/**
  Check if the ACPI table is supported.

  This function checks if the ACPI table's signature is supported, meaning 
  it is either the Secondary System Description Table (SSDT) or the 
  Differentiated System Description Table (DSDT).

  @param[in]  Table         Pointer to an ACPI table header structure.

  @retval     EFI_SUCCESS   The table is supported.
  @retval     EFI_UNSUPPORTED The table is not supported.
**/
EFI_STATUS
IsSupportedTable (
  IN ACPI_TABLE_HEADER  *Table
  )
{
    if ((Table->Signature != SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) && (Table->Signature != DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE)) {
        return EFI_UNSUPPORTED;
    } 
    return EFI_SUCCESS;
}


/**
  Retrieve an object in the ASL table.

  This function searches for an object with a specified name within the ACPI ASL 
  table based on the provided operation code and stores its information in the 
  provided ASL_OBJECT_INFO structure.

  @param[in]     OperationCode  Operation code used for identifying the object.
  @param[in]     TableAddress   The start address of the ACPI table.
  @param[in]     TableLength    The length of the ACPI table.
  @param[in]     ObjectName     Pointer to the name of the object to search for.
  @param[in]     ObjectNameLength Length of the object name.
  @param[in,out] ObjectInfo     Pointer to an ASL_OBJECT_INFO structure to store 
                              information about the object if found.

  @retval        EFI_SUCCESS    Object found and information stored in ObjectInfo.
  @retval        EFI_NOT_FOUND  The object was not found in the ASL table.
  @retval        EFI_INVALID_PARAMETER ObjectName is NULL or ObjectNameLength is invalid.
  @retval        EFI_UNSUPPORTED OperationCode or object type is unsupported.
**/
EFI_STATUS
GetObjectInAsl (
  IN     UINT16           OperationCode,
  IN     UINTN            TableAddress,
  IN     UINTN            TableLength,
  IN     UINT8            *ObjectName,
  IN     UINTN            ObjectNameLength,
  IN OUT ASL_OBJECT_INFO  *ObjectInfo
  )
{
    EFI_STATUS   Status = EFI_SUCCESS;
    UINT32 SearchName;
    VOID *AslObjectNameBuffer = AllocatePool(ObjectNameLength);

    switch (OperationCode) {
        case AML_NAME_OPERATION_CODE:
            if (ObjectName != NULL) {
                CopyMem(AslObjectNameBuffer, ObjectName, ObjectNameLength);
            } else {
                DEBUG((DEBUG_ERROR, "[AcpiLib - GetObjectInAsl]: ObjectName is NULL\n"));
                return EFI_INVALID_PARAMETER;
            }

            for (UINTN StartAddress = TableAddress; StartAddress < StartAddress + TableLength; StartAddress++) {
                switch (ObjectNameLength) {
                    case 0x04:  // 4 Byte
                        SearchName = *(UINT32*)StartAddress;
                        break;
                    case 0x03:  // 3 Byte
                        SearchName = (*(UINT32*)StartAddress) & 0xFFFFFF;
                        break;
                    case 0x02:  // 2 Byte
                        SearchName = *(UINT16*)StartAddress;
                        break;
                    case 0x01:  // 1 Byte
                        SearchName = *(UINT8*)StartAddress;
                        break;
                    default:
                        SearchName = 0;
                        break;
                }

                if (*(UINT32*)AslObjectNameBuffer == SearchName) {
                    if (OperationCode == *(UINT8*)(StartAddress - sizeof(UINT8))) {
                        ObjectInfo->ObjectAddress = StartAddress - sizeof(UINT8);
                        ObjectInfo->ObjectType = *(UINT8*)(StartAddress - sizeof(UINT8));
                        ObjectInfo->ObjectName = SearchName;
                        ObjectInfo->ObjectDataType = *(UINT8*)(StartAddress + ObjectNameLength);
                        switch (*(UINT8*)(StartAddress + ObjectNameLength)) {
                            case BytePrefix:
                                ObjectInfo->ObjectDataLength = sizeof(UINT8);
                                break;
                            case WordPrefix:
                                ObjectInfo->ObjectDataLength = sizeof(UINT16);
                                break;
                            case DWordPrefix:
                                ObjectInfo->ObjectDataLength = sizeof(UINT32);
                                break;
                            case StringPrefix:
                                UINT8 i = 0;
                                while ((*(UINT8*)StartAddress) != 0x00) {
                                    i++;    // calculate the length of string of name object
                                }
                                ObjectInfo->ObjectDataLength = i;
                                break;
                            case QWordPrefix:
                                ObjectInfo->ObjectDataLength = sizeof(UINT64);
                                break;
                        }
                        ObjectInfo->ObjectDataStartAddress = StartAddress + ObjectNameLength + sizeof(UINT8);
                        break;
                    }
                }
            }

            if (ObjectInfo->ObjectName != 0xFFFF) {
                Status = EFI_SUCCESS;
            } else {
                Status = EFI_NOT_FOUND;
            }

            if (AslObjectNameBuffer != NULL) {
                FreePool(AslObjectNameBuffer);
            }
            break;


        case 0xFFFF:
            DEBUG((DEBUG_INFO, "[AcpiLib - GetObjectInAsl]: only support BYTE operation code, WORD operation code is not supported now.\n", OperationCode));
            Status = EFI_UNSUPPORTED;
            break;


        default:
            DEBUG((DEBUG_ERROR, "[AcpiLib - GetObjectInAsl]: OperationCode(%x)is not supported\n", OperationCode));
            Status = EFI_UNSUPPORTED;
            break;
    }
    return Status;
}


/**
  Set the value of an object in the ASL table.

  This function sets the value of a specified object in the ASL table 
  according to the object's data type and the provided operation code.

  @param[in]     OperationCode  Operation code to specify the type of operation.
  @param[in,out] AslObjectInfo  Pointer to an ASL_OBJECT_INFO structure containing 
                              information about the object to be modified.
  @param[in]     Value          The value to set for the object.

  @retval        EFI_SUCCESS    The object value was set successfully.
  @retval        EFI_UNSUPPORTED Unsupported operation code or data type.
  @retval        EFI_ERROR      An error occurred during the operation.
**/
EFI_STATUS
SetObjectInAsl (
  IN UINT16           OperationCode,
  IN ASL_OBJECT_INFO  *AslObjectInfo,
  IN UINTN            Value
  )
{
    EFI_STATUS   Status = EFI_SUCCESS;
    switch (OperationCode) {
        case AML_NAME_OPERATION_CODE:
            switch (AslObjectInfo->ObjectDataType) {
                case BytePrefix: {
                    UINT8 *ObjectDataAddress = (UINT8*)AslObjectInfo->ObjectDataStartAddress;
                    *ObjectDataAddress = (UINT8)Value;
                    Status = EFI_SUCCESS;
                    }
                    break;
                case WordPrefix: {
                    UINT16 *ObjectDataAddress = (UINT16*)AslObjectInfo->ObjectDataStartAddress;
                    *ObjectDataAddress = (UINT16)Value;
                    Status = EFI_SUCCESS;
                    }
                    break;
                case DWordPrefix: {
                    UINT32 *ObjectDataAddress = (UINT32*)AslObjectInfo->ObjectDataStartAddress;
                    *ObjectDataAddress = (UINT32)Value;
                    Status = EFI_SUCCESS;
                    }
                    break;
                case QWordPrefix: {
                    UINT64 *ObjectDataAddress = (UINT64*)AslObjectInfo->ObjectDataStartAddress;
                    *ObjectDataAddress = (UINT64)Value;
                    Status = EFI_SUCCESS;
                    }
                    break;
                case StringPrefix:
                    Status = EFI_SUCCESS;
                    break;
                default:
                    DEBUG((DEBUG_ERROR, "[AcpiLib - SetObjectInAsl]: AslObjectInfo->ObjectDataLength(%x)is not supported\n", AslObjectInfo->ObjectDataLength));
                    Status = EFI_UNSUPPORTED;
                    break;
            }
            break;

        default:
            DEBUG((DEBUG_ERROR, "[AcpiLib - SetObjectInAsl]: OperationCode(%x)is not supported\n", OperationCode));
            Status = EFI_UNSUPPORTED;
            break;
    }
    return Status;
}


/**
  Update a Name object in the ASL table.

  This function searches for a Name object in the specified ACPI ASL table by name 
  and updates its value if found.

  @param[in] TableAddress     Start address of the ACPI table.
  @param[in] ObjectName       Pointer to the name of the Name object.
  @param[in] ObjectNameLength Length of the object name.
  @param[in] Value            The new value to set for the object.

  @retval    EFI_SUCCESS      The Name object was found and updated.
  @retval    EFI_NOT_FOUND    The Name object was not found.
  @retval    EFI_UNSUPPORTED  The table or object type is unsupported.
  @retval    EFI_ERROR        An error occurred during the update.
**/
EFI_STATUS
UpdateNameObjectInAsl (
  IN UINTN  TableAddress,
  IN UINT8  *ObjectName,
  IN UINTN  ObjectNameLength,
  IN UINTN  Value
  )
{
    EFI_STATUS         Status = EFI_SUCCESS;
    ASL_OBJECT_INFO    *AslObjectInfo = AllocatePool(sizeof(ASL_OBJECT_INFO));
    ACPI_TABLE_HEADER  *AcpiTable = (ACPI_TABLE_HEADER*)TableAddress;
    Status = IsSupportedTable(AcpiTable);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "[AcpiLib - UpdateNameObjectInAsl]: IsSupportedTable - Status = 0x%x(%r)\n", Status, Status));
        return Status;
    }

    Status = GetObjectInAsl(AML_NAME_OPERATION_CODE, TableAddress, AcpiTable->Length, ObjectName, ObjectNameLength, AslObjectInfo);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "[AcpiLib - GetObjectInAsl]: GetObjectInAsl - Status = 0x%x(%r)\n", Status, Status));
        return Status;
    }

    Status = SetObjectInAsl(AML_NAME_OPERATION_CODE, AslObjectInfo, Value);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "[AcpiLib - SetObjectInAsl]: GetObjectInAsl - Status = 0x%x(%r)\n", Status, Status));
        return Status;
    }

    return EFI_SUCCESS;
}