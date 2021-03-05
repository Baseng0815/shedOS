#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include <stdbool.h>
#include <stddef.h>

struct framebuffer {
        void        *addr;
        size_t      size;
        uint32_t    width;
        uint32_t    height;
        uint32_t    scanline_width;
        uint32_t    pitch; /* bytes per scanline */
};

#define PSF_MAGIC0 0x36
#define PSF_MAGIC1 0x04

struct psf1_header {
        unsigned char magic[2];
        unsigned char mode;
        unsigned char charsize;
};

struct psf1_font {
        struct psf1_header  *header;
        void                *glyphs;
};

struct bootinfo {
        struct framebuffer  *framebuffer;
        struct psf1_font    *font;
};

EFI_SYSTEM_TABLE        *eST;
EFI_BOOT_SERVICES       *eBS;
EFI_RUNTIME_SERVICES    *eRT;

EFI_STATUS                  efi_main(EFI_HANDLE, EFI_SYSTEM_TABLE*);
static EFI_FILE             *load_file(CHAR16*, EFI_HANDLE);
static EFI_STATUS           get_tables(EFI_SYSTEM_TABLE*);
static struct framebuffer   initialize_gop();
static struct psf1_font     *load_font(CHAR16*, EFI_HANDLE);

static void print(CHAR16 *str);
static void println(CHAR16 *str);
static int  memcmp(const void*, const void*, size_t);

EFI_STATUS efi_main(EFI_HANDLE ImageHandle,
                    EFI_SYSTEM_TABLE *SystemTable)
{
        EFI_STATUS status;
        /* tables (system table, boot services and runtime table) */
        status = get_tables(SystemTable);
        if (EFI_ERROR(status)) {
                println(L"Failed to get all tables.");
                return status;
        } else {
                println(L"Successfully got all tables.");
        }

        /* kernel file */
        EFI_FILE *kernel = load_file(L"boot\\kernel.elf", ImageHandle);
        if (kernel == NULL) {
                println(L"Failed to get kernel file.");
        } else {
                println(L"Successfully got kernel file.");
        }

        /* elf header */
        Elf64_Ehdr header;
        {
                UINTN size = sizeof(header);
                status = kernel->Read(kernel, &size, (void*)&header);
                if (EFI_ERROR(status)) {
                        println(L"Failed to get kernel header.");
                } else {
                        println(L"Successfully got kernel header.");
                }
        }

        println(L"Got kernel header.");

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
        /* kernel entry point
           The UEFI loader uses some weird Microsoft calling convention while we
           use the good ol' x86-64 sysv abi. Because I am too stupid to change
           the calling convention using compiler settings, we just pass kernel
           arguments manually using some inline assembly.
           Not pretty, but it works. */

        /* gop */
        struct framebuffer framebuffer = initialize_gop();
        if (framebuffer.addr == NULL) {
                println(L"Failed to initialize gop and framebuffer.");
                return status;
        } else {
                println(L"Successfully initialized gop and framebuffer.");
        }

        /* font */
        struct psf1_font *font = load_font(L"res\\font.psf", ImageHandle);
        if (font == NULL) {
                println(L"Failed to load font.");
        } else {
                println(L"Successfully loaded font.");
        }

        /* exit boot services and transfer control to kernel */
        struct bootinfo bootinfo = {
                .framebuffer = &framebuffer,
                .font = font
        };

        void (*kernel_start)() = (void(*)())(header.e_entry);
        println(L"Got kernel entry point.");

        asm volatile("leaq %0, %%rdi;"
                     : "+m" (bootinfo));

        println(L"Running kernel...");
        kernel_start();
        println(L"Kernel finished. Exiting now.");

        while (true) {}

        return EFI_SUCCESS;
}

EFI_FILE *load_file(CHAR16 *path,
                    EFI_HANDLE imageHandle)
{
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

        EFI_FILE_PROTOCOL *root = NULL;
        status = fileSystem->OpenVolume(fileSystem, &root);
        if (EFI_ERROR(status)) {
                println(L"Failed to open root volume.");
                return NULL;
        } else {
                println(L"Successfully opened root volume.");
        }

        EFI_FILE_PROTOCOL *loadedFile = NULL;
        status = root->Open(root, &loadedFile, path,
                                 EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

        if (EFI_ERROR(status)) {
                print(L"Failed to open file ");
                println(path);
                return NULL;
        } else {
                print(L"Successfully opened file ");
                println(path);
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

struct framebuffer initialize_gop()
{
        struct framebuffer framebuffer;
        framebuffer.addr = NULL;

        EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
        EFI_STATUS status = eBS->LocateProtocol(&gopGuid, NULL, (void**)&gop);
        if (EFI_ERROR(status)) {
                println(L"Failed to get gop.");
                return framebuffer;
        } else {
                println(L"Successfully got gop.");
        }

        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
        UINTN sizeOfInfo, numModes, nativeMode;
        status = gop->QueryMode(gop, gop->Mode == NULL ? 0 :
                                gop->Mode->Mode, &sizeOfInfo, &info);
        if (EFI_ERROR(status)) {
                println(L"Failed to get graphics output information.");
                return framebuffer;
        } else {
                println(L"Successfully got grahics output information.");
        }

        numModes = gop->Mode->MaxMode;
        nativeMode = gop->Mode->Mode;

        status = gop->SetMode(gop, nativeMode);
        if (EFI_ERROR(status)) {
                println(L"Failed to set gop mode.");
                return framebuffer;
        } else {
                println(L"Successfully set gop mode.");
        }

        framebuffer.width = gop->Mode->Info->HorizontalResolution;
        framebuffer.height = gop->Mode->Info->VerticalResolution;
        framebuffer.scanline_width = gop->Mode->Info->PixelsPerScanLine;
        framebuffer.pitch = 4 * gop->Mode->Info->PixelsPerScanLine;
        framebuffer.addr = (void*)gop->Mode->FrameBufferBase;
        framebuffer.size = gop->Mode->FrameBufferSize;

        return framebuffer;
}

struct psf1_font *load_font(CHAR16 *path,
                            EFI_HANDLE imageHandle)
{
        EFI_FILE *font = load_file(path, imageHandle);
        if (font == NULL) {
                println(L"Failed to load font: file could not be opened.");
                return NULL;
        }

        struct psf1_header *header;
        eBS->AllocatePool(EfiLoaderData, sizeof(struct psf1_header),
                          (void**)&header);
        UINTN size = sizeof(struct psf1_header);
        font->Read(font, &size, header);

        if (header->magic[0] != PSF_MAGIC0 || header->magic[1] != PSF_MAGIC1) {
                println(L"Failed to load font: font magic does not match.");
                return NULL;
        }

        UINTN glyphBufferSize = header->charsize * 256;
        if (header->mode == 1) { /* 512 glyph mode */
                glyphBufferSize = header->charsize * 512;
        }

        void *glyphs;
        {
                font->SetPosition(font, sizeof(struct psf1_header));
                eBS->AllocatePool(EfiLoaderData, glyphBufferSize,
                                  (void**)&glyphs);
                font->Read(font, &glyphBufferSize, glyphs);
        }

        struct psf1_font *psf1_font;
        eBS->AllocatePool(EfiLoaderData, sizeof(struct psf1_font), (void**)&psf1_font);
        psf1_font->header = header;
        psf1_font->glyphs = glyphs;

        return psf1_font;
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
