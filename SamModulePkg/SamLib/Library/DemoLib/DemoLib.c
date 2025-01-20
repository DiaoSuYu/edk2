#include "Include/Library/DemoLib.h"
#include <Library/UefiLib.h>


VOID
DemoLibFunction (
  VOID
  )
{
  Print(L"LibFunction() is called!\n");
}

RETURN_STATUS
EFIAPI
DemoLibConstructor (
  VOID
  )
{
  Print(L"DemoLibConstructor() is called!\n");
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
DemoLibDestructor (
  VOID
  )
{
  Print(L"DemoLibDestructor() is called!\n");
  return EFI_SUCCESS;
}
