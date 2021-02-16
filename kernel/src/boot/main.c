#include <efi.h>
#include <efilib.h>

void print(CHAR16 *str);
void println(CHAR16 *str);

EFI_SYSTEM_TABLE *eST;
EFI_BOOT_SERVICES *eBS;
EFI_RUNTIME_SERVICES *eRT;

EFI_STATUS get_tables(EFI_SYSTEM_TABLE *SystemTable)
{
        eST = SystemTable;
        if (eST != NULL)
                println(L"Successfully got system table.");
        else
        {
                println(L"Failed to get system table.");
                return EFI_LOAD_ERROR;
        }
        eBS = eST->BootServices;
        if (eBS != NULL)
                println(L"Successfully got boot service table.");
        else
        {
                println(L"Failed to get boot service table.");
                return EFI_LOAD_ERROR;
        }
        eRT = eST->RuntimeServices;
        if (eRT != NULL)
                println(L"Successfully got runtime service table.");
        else
        {
                println(L"Failed to get runtime service table.");
                return EFI_LOAD_ERROR;
        }

        return EFI_SUCCESS;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
        EFI_STATUS status;
        status = get_tables(SystemTable);
        if (EFI_ERROR(status)) {
                println(L"Failed to get all tables.");
                return status;
        } else
                println(L"Successfully got all tables.");

        EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
        status = eBS->LocateProtocol(&gopGuid, NULL, (void**)&gop);
        if (EFI_ERROR(status)) {
                println(L"Failed to get GOP.");
                return status;
        } else
                println(L"Successfully got GOP.");

        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
        UINTN sizeOfInfo, numModes, nativeMode;
        status = gop->QueryMode(gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &sizeOfInfo, &info);
        if (EFI_ERROR(status)) {
                println(L"Failed to get graphics output information.");
                return status;
        } else
                println(L"Successfully got grahics output information.");


        EFI_INPUT_KEY key;
        while ((status = eST->ConIn->ReadKeyStroke(eST->ConIn, &key)) == EFI_NOT_READY);

        return EFI_SUCCESS;
}

void print(CHAR16 *str)
{
        eST->ConOut->OutputString(eST->ConOut, str);
}

void println(CHAR16 *str)
{
        print(str);
        print(L"\n\r");
}
