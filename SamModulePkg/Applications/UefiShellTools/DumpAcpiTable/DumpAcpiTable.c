#include <Library/AcpiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CommonLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include "DumpAcpiTable.h"

#define SSDT_WMI_STUDY_SIGNATURE  { 'W', 'm', 'i', 'S', 't', 'u', 'd', 'y'}


/**

  DumpAcpiTableEFI's entry point.

  @param[in] ImageHandle      The firmware allocated handle for the EFI image.
  @param[in] SystemTable      A pointer to the EFI System Table.

  @retval EFI_SUCCESS               DumpAcpiTableEFI executed successfully
  @retval EFI_INVALID_PARAMETER     Parameter is not supported by DumpAcpiTableEFI
  @retval Others                    DumpAcpiTableEFI executed failed

**/
EFI_STATUS
EFIAPI
DumpAcpiTableEFIMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS   Status = EFI_SUCCESS;
    UINTN        SsdtWmiStudyAddress = 0;
    UINT8 SsdtSignature[SSDT_OEM_TABLE_ID_BYTE_LENGTH] = SSDT_WMI_STUDY_SIGNATURE;
    ROOT_SYSTEM_DESCRIPTION_POINTER    *RSDP = AllocatePool(sizeof(ROOT_SYSTEM_DESCRIPTION_POINTER));
    ROOT_SYSTEM_DESCRIPTION_TABLE      *RSDT = AllocatePool(sizeof(ROOT_SYSTEM_DESCRIPTION_TABLE));
    EXTENDED_SYSTEM_DESCRIPTION_TABLE  *XSDT = AllocatePool(sizeof(EXTENDED_SYSTEM_DESCRIPTION_TABLE));

    Status = GetRSDP(SystemTable, RSDP);
    if (EFI_ERROR(Status)) {
        Print(L"[DumpAcpiTableEFIMain]: GetRSDP - Status = %x(%r)\n", Status, Status);
        // return Status;
    } else {
        PrintRSDPInfo(RSDP);
        if (RSDP != NULL) {
            AllocatePool((UINTN)RSDP);
        }
    }

    Status = GetRSDT(SystemTable, RSDT);
    if (EFI_ERROR(Status)) {
        Print(L"[DumpAcpiTableEFIMain]: GetRSDT - Status = %x(%r)\n", Status, Status);
        // return Status;
    } else {
        PrintRSDTInfo(RSDT);
        if (RSDT != NULL) {
            AllocatePool((UINTN)RSDT);
        }
    }

    Status = GetXSDT(SystemTable, XSDT);
    if (EFI_ERROR(Status)) {
        Print(L"[DumpAcpiTableEFIMain]: GetXSDT - Status = %x(%r)\n", Status, Status);
        // return Status;
    } else {
        PrintXSDTInfo(XSDT);
        if (XSDT != NULL) {
            AllocatePool((UINTN)XSDT);
        }
    }

    Status = GetSSDT(SystemTable, SsdtSignature, &SsdtWmiStudyAddress);
    if (EFI_ERROR(Status)) {
        Print(L"[DumpAcpiTableEFIMain]: GetSSDT - Status = %x(%r)\n", Status, Status);
        // return Status;
    } else {
        Print(L"[DumpAcpiTableEFIMain]: SsdtWmiStudyAddress = %x\n", SsdtWmiStudyAddress);
        UINT32 Length = ((SECONDARY_SYSTEM_DESCRIPTION_TABLE*)(SsdtWmiStudyAddress))->Length;
        Print(L"[DumpAcpiTableEFIMain]: Length = %x\n", Length);
        // PrintBytes((UINTN)Length, (UINT8*)SsdtWmiStudyAddress);
    }

    Status = UpdateNameObjectInAsl(SsdtWmiStudyAddress, (UINT8*)"WMIR", AsciiStrLen("WMIR"), 0x12345678);
    if (EFI_ERROR(Status)) {
        Print(L"[DumpAcpiTableEFIMain]: UpdateAslNameObject(WMIR) - Status = 0x%x(%r)\n", Status, Status);
        // return Status;
    }

    return Status;
}
