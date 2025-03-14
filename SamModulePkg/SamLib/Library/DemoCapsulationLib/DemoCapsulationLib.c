#include "Library/DemoCapsulationLib.h"

#include <Library/UefiLib.h>


VOID
DemoCapsulationLibFunction (
  VOID
  )
{
  Print(L"DemoCapsulationLibFunction() is called!\n");
}

RETURN_STATUS
EFIAPI
DemoCapsulationLibConstructor (
  VOID
  )
{
  Print(L"DemoCapsulationLibConstructor() is called!\n");
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
DemoCapsulationLibDestructor (
  VOID
  )
{
  Print(L"DemoCapsulationLibDestructor() is called!\n");
  return EFI_SUCCESS;
}
