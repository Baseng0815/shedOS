#include <efi.h>
#include <efilib.h>

static void print(CHAR16 *str);
static void println(CHAR16 *str);
static inline void plotpixel_32bpp(int x, int y, uint32_t pixel);

EFI_SYSTEM_TABLE *eST;
EFI_BOOT_SERVICES *eBS;
EFI_RUNTIME_SERVICES *eRT;
EFI_GRAPHICS_OUTPUT_PROTOCOL *eGOP;

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
        status = eBS->LocateProtocol(&gopGuid, NULL, (void**)&eGOP);
        if (EFI_ERROR(status)) {
                println(L"Failed to get gop.");
                return status;
        } else
                println(L"Successfully got gop.");

        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
        UINTN sizeOfInfo, numModes, nativeMode;
        status = eGOP->QueryMode(eGOP, eGOP->Mode == NULL ? 0 : eGOP->Mode->Mode, &sizeOfInfo, &info);
        if (EFI_ERROR(status)) {
                println(L"Failed to get graphics output information.");
                return status;
        } else
                println(L"Successfully got grahics output information.");

        numModes = eGOP->Mode->MaxMode;
        nativeMode = eGOP->Mode->Mode;

        status = eGOP->SetMode(eGOP, nativeMode);
        if (EFI_ERROR(status)) {
                println(L"Failed to set gop mode.");
                return status;
        } else
                println(L"Successfully set gop mode.");

        for (int x = 0; x < 100; x++) {
                for (int y = 0; y < 100; y++)
                        plotpixel_32bpp(x, y, 0xFF00FFFF);
        }

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

static inline void plotpixel_32bpp(int x, int y, uint32_t pixel)
{
        *((uint32_t*)(eGOP->Mode->FrameBufferBase + 4 * eGOP->Mode->Info->PixelsPerScanLine * y + 4 * x)) = pixel;
}
