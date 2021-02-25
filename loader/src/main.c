#include <efi.h>
#include <efilib.h>

static void print(CHAR16 *str);
static void println(CHAR16 *str);

EFI_SYSTEM_TABLE *eST;
EFI_BOOT_SERVICES *eBS;
EFI_RUNTIME_SERVICES *eRT;
EFI_GRAPHICS_OUTPUT_PROTOCOL *eGOP;

EFI_FILE *load_file(EFI_FILE *directory, CHAR16 *path, EFI_HANDLE imageHandle)
{
        EFI_FILE *loadedFile;
        EFI_LOADED_IMAGE_PROTOCOL *loadedImage;
        eBS->HandleProtocol(imageHandle, &gEfiLoadedImageProtocolGuid, (void**)&loadedImage);
        println(L"Successfully got loaded image protocol.");

        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fileSystem;
        eBS->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)fileSystem);
        println(L"Successfully got loaded file system protocol.");

        if (directory == NULL) {
                EFI_STATUS s = fileSystem->OpenVolume(fileSystem, &directory);
                if (EFI_ERROR(s)) {
                        println(L"Failed to open volume.");
                        return NULL;
                } else
                        println(L"Successfully opened volume.");
        }

        // it's crashing here
        // msg to future self: go fix it
        EFI_STATUS s = directory->Open(directory, &loadedFile, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
        if (EFI_ERROR(s)) {
                println(L"Failed to open kernel file.");
                return NULL;
        } else
                println(L"Successfully opened kernel file.");

        return loadedFile;
}

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

        if (load_file(NULL, L"kernel.elf", ImageHandle) == NULL) {
                println(L"Failed to load kernel.");
        } else
                println(L"Successfully loaded kernel.");

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
