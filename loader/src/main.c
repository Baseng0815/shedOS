#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include <stdbool.h>
#include <stddef.h>

static void print(CHAR16 *str);
static void println(CHAR16 *str);
static int memcmp(const void*, const void*, size_t);

EFI_SYSTEM_TABLE *eST;
EFI_BOOT_SERVICES *eBS;
EFI_RUNTIME_SERVICES *eRT;
EFI_GRAPHICS_OUTPUT_PROTOCOL *eGOP;

EFI_FILE *load_kernel_file(EFI_FILE *directory,
                           CHAR16 *path,
                           EFI_HANDLE imageHandle)
{
        EFI_FILE *loadedFile;

        EFI_LOADED_IMAGE_PROTOCOL *loadedImage;
        EFI_STATUS status = eBS->HandleProtocol(imageHandle,
                                                &gEfiLoadedImageProtocolGuid,
                                                (void**)&loadedImage);
        if (EFI_ERROR(status)) {
                println(L"Failed to get image protocol.");
                return NULL;
        } else {
                println(L"Successfully got loaded image protocol.");
        }

        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fileSystem;
        status = eBS->HandleProtocol(loadedImage->DeviceHandle,
                                     &gEfiSimpleFileSystemProtocolGuid,
                                     (void**)&fileSystem);
        if (EFI_ERROR(status)) {
                println(L"Failed to get file system protocol.");
                return NULL;
        } else {
                println(L"Successfully got loaded file system protocol.");
        }

        if (directory == NULL) {
                status = fileSystem->OpenVolume(fileSystem, &directory);
                if (EFI_ERROR(status)) {
                        println(L"Failed to open volume.");
                        return NULL;
                } else {
                        println(L"Successfully opened volume.");
                }
        }

        status = directory->Open(directory, &loadedFile, path,
                                 EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

        if (EFI_ERROR(status)) {
                println(L"Failed to open kernel file.");
                return NULL;
        } else {
                println(L"Successfully opened kernel file.");
        }

        return loadedFile;
}

EFI_STATUS get_tables(EFI_SYSTEM_TABLE *SystemTable)
{
        eST = SystemTable;
        if (eST != NULL) {
                println(L"Successfully got system table.");
        } else {
                println(L"Failed to get system table.");
                return EFI_LOAD_ERROR;
        }
        eBS = eST->BootServices;
        if (eBS != NULL) {
                println(L"Successfully got boot service table.");
        } else {
                println(L"Failed to get boot service table.");
                return EFI_LOAD_ERROR;
        }
        eRT = eST->RuntimeServices;
        if (eRT != NULL) {
                println(L"Successfully got runtime service table.");
        } else {
                println(L"Failed to get runtime service table.");
                return EFI_LOAD_ERROR;
        }

        return EFI_SUCCESS;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle,
                    EFI_SYSTEM_TABLE *SystemTable)
{
        EFI_STATUS status;
        /* get tables (system table, boot services and runtime table) */
        status = get_tables(SystemTable);
        if (EFI_ERROR(status)) {
                println(L"Failed to get all tables.");
                return status;
        } else {
                println(L"Successfully got all tables.");
        }

        /* load kernel file */
        EFI_FILE *kernel = load_kernel_file(NULL, L"kernel.elf", ImageHandle);
        if (kernel == NULL) {
                println(L"Failed to get kernel file.");
        } else {
                println(L"Successfully got kernel file.");
        }

        /* get elf header */
        Elf64_Ehdr header;
        {
                UINTN fileInfoSize;
                EFI_FILE_INFO *fileInfo;
                kernel->GetInfo(kernel, &gEfiFileInfoGuid, &fileInfoSize, NULL);
                eBS->AllocatePool(EfiLoaderData, fileInfoSize,
                                  (void**)&fileInfo);

                UINTN size = sizeof(header);
                kernel->Read(kernel, &size, &header);
        }

        /* check if elf is valid and executable */
        if (
                memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
                header.e_ident[EI_CLASS] != ELFCLASS64 ||
                header.e_ident[EI_DATA] != ELFDATA2LSB ||
                header.e_type != ET_EXEC ||
                header.e_machine != EM_X86_64 ||
                header.e_version != EV_CURRENT
           ) {
                /* invalid */
                println(L"Kernel format is bad.");
        } else {
                /* valid */
                println(L"Kernel format is good.");
        }

        /* program headers */
        Elf64_Phdr *phdrs;
        {
                kernel->SetPosition(kernel, header.e_phoff);
                UINTN size = header.e_phnum * header.e_phentsize;
                eBS->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
                kernel->Read(kernel, &size, phdrs);
        }

        for (
                Elf64_Phdr *phdr = phdrs;
                (char*)phdr < (char*)phdrs +
                header.e_phnum * header.e_phentsize;
                phdr = (Elf64_Phdr*)((char*)(phdr + header.e_phentsize))
            ) {
                switch (phdr->p_type) {
                        case PT_LOAD: {
                                int pages = (phdr->p_memsz + 0x1000 - 1) /
                                        0x1000;
                                Elf64_Addr segment = phdr->p_paddr;
                                eBS->AllocatePages(AllocateAddress,
                                                   EfiLoaderData,
                                                   pages, &segment);

                                kernel->SetPosition(kernel, phdr->p_offset);
                                UINTN size = phdr->p_filesz;
                                kernel->Read(kernel, &size, (void*)segment);
                                break;
                        }
                }
        }

        println(L"Kernel loaded.");
        /* kernel entry point */
        int (*kernel_start)() = ((__attribute__((sysv_abi))
                                  int(*)())header.e_entry);

        int res = kernel_start();
        if (res == 133742069) {
                println(L"nice.");
        }

        while (true) {}

        /* gop */
        EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        status = eBS->LocateProtocol(&gopGuid, NULL, (void**)&eGOP);
        if (EFI_ERROR(status)) {
                println(L"Failed to get gop.");
                return status;
        } else {
                println(L"Successfully got gop.");
        }

        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
        UINTN sizeOfInfo, numModes, nativeMode;
        status = eGOP->QueryMode(eGOP, eGOP->Mode == NULL ? 0 :
                                 eGOP->Mode->Mode, &sizeOfInfo, &info);
        if (EFI_ERROR(status)) {
                println(L"Failed to get graphics output information.");
                return status;
        } else {
                println(L"Successfully got grahics output information.");
        }

        numModes = eGOP->Mode->MaxMode;
        nativeMode = eGOP->Mode->Mode;

        status = eGOP->SetMode(eGOP, nativeMode);
        if (EFI_ERROR(status)) {
                println(L"Failed to set gop mode.");
                return status;
        } else {
                println(L"Successfully set gop mode.");
        }

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

int memcmp(const void *pb,
           const void *pa,
           size_t n)
{
        const unsigned char *a = pa, *b = pb;
        for (size_t i = 0; i < n; i++) {
                if (a[i] < b[i]) { return -1; }
                else if (a[i] > b[i]) { return 1; }
        }

        return 0;
}
