#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
        EFI_STATUS status;
        EFI_INPUT_KEY key;
        ST = SystemTable;

        status = ST->ConOut->OutputString(ST->ConOut, L"Hello World\r\n");
        if (EFI_ERROR(status))
                return status;

        status = ST->ConIn->Reset(ST->ConIn, FALSE);
        if (EFI_ERROR(status))
                return status;

        while ((status = ST->ConIn->ReadKeyStroke(ST->ConIn, &key)) == EFI_NOT_READY);

        return status;
}
