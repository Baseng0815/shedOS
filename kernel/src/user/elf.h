/*
   This header was written in accordance with the Linux Programmer's Manual (and
   various other sources which I don't think were up to date). Some values are
   even taken directly from the GNU elf.h file!
   Copyright © 2021 Bastian Engel

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
   */


#ifndef ELF_H
#define ELF_H

#include <stdint.h>

/* e_ident index */
enum {
        EI_MAG0         = 0, /* file identification */
        EI_MAG1         = 1, /* file identification */
        EI_MAG2         = 2, /* file identification */
        EI_MAG3         = 3, /* file identification */
        EI_CLASS        = 4, /* file class */
        EI_DATA         = 5, /* data encoding */
        EI_VERSION      = 6, /* file version */
        EI_OSABI        = 7, /* ABI identification */
        EI_ABIVERSION   = 8, /* ABI version */
        EI_PAD          = 9, /* start of padding bytes */
        EI_NIDENT       = 16 /* size of e_ident */
};

/* ei_class */
enum {
        ELFCLASSNONE    = 0, /* invalid class */
        ELFCLASS32      = 1, /* 32-bit objects */
        ELFCLASS64      = 2 /* 64-bit objects */
};

/* ei_data */
enum {
        ELFDATANONE     = 0, /* invalid data */
        ELFDATA2LSB     = 1, /* least significant byte first */
        ELFDATA2MSB     = 2 /* most significant byte first */
};

/* ei_version */
/* other versions are equal to current */
enum {
        EV_NONE = 0 /* invalid version */
};

/* ei_osabi */
/* we only list a few examples */
enum {
        ELFOSABI_NONE       = 0, /* UNIX SysV */
        ELFOSABI_SYSV       = 0, /* alias */
        ELFOSABI_HPUX       = 1, /* HP-UX */
        ELFOSABI_NETBSD     = 2, /* NetBSD */
        ELFOSABI_LINUX      = 3, /* Linux */
        ELFOSABI_GNU        = 3, /* alias */
        ELFOSABI_SOLARIS    = 6, /* Sun Solaris */
        ELFOSABI_AIX        = 7, /* IBM AIX */
        ELFOSABI_IRIX       = 8, /* SGI Irix */
        ELFOSABI_FREEBSD    = 9, /* FreeBSD */
        ELFOSABI_TRU64      = 10, /* Compax TRU64 UNIX */
        ELFOSABI_MODESTO    = 11, /* Novell Modesto */
        ELFOSABI_OPENBSD    = 12, /* OpenBSD */
        ELFOSABI_ARM_AEABI  = 64, /* ARM EABI */
        ELFOSABI_ARM        = 97, /* ARM */
        ELFOSABI_STANDALONE = 255 /* standalone (embedded) application */
};

/* e_type */
enum {
        ET_NONE     = 0, /* no file type */
        ET_REL      = 1, /* relocatable file */
        ET_EXEC     = 2, /* executable file */
        ET_DYN      = 3, /* shared object file */
        ET_CORE     = 4, /* core file */
        ET_LOOS     = 0xfe00, /* lobound os-specific */
        ET_HIOS     = 0xfeff, /* hibound os-specific */
        ET_LOPROC   = 0xff00, /* lobound processor-specific */
        ET_HIPROC   = 0xffff /* hibound processor-specific */
};

/* e_machine */
enum {
        EM_NONE         = 0, /* no machine */
        EM_M32          = 1, /* AT&T WE 32100 */
        EM_SPARC        = 2, /* SPARC */
        EM_386          = 3, /* Intel 80386 */
        EM_68K          = 4, /* Motorola 68K */
        EM_88K          = 5, /* Motorola 88K */
        EM_IAMCU        = 6, /* Intel MCU */
        EM_860          = 7, /* Intel 80860 */
        EM_MIPS         = 8, /* MIPS I (R3000 big-endian) */
        EM_S370         = 9, /* IBM System/370 */
        EM_MIPS_RS3_LE  = 10, /* MIPS R3000 little-endian */
        EM_PARISC       = 15, /* HPPA */
        EM_VPP500       = 17, /* VPP500 */
        EM_SPARC32PLUS  = 18, /* v8plus */
        EM_960          = 19, /* Intel 80960 */
        EM_PPC          = 20, /* PowerPC */
        EM_PPC64        = 21, /* 64-bit PowerPC */
        EM_S390         = 22, /* IBM System/390 */
        EM_SPU          = 23, /* IBM SPU/SPC */
        EM_V800         = 36, /* NEC V800 */
        EM_FR20         = 37, /* Fujitsu FR20 */
        EM_RH32         = 38, /* TRW RH-32 */
        EM_RCE          = 39, /* Motorola RCE */
        EM_ARM          = 40, /* ARM */
        EM_IA64         = 50, /* Intel 64-bit architecture */
        EM_X86_64       = 62 /* AMD x86-64 architecture */

};

/* ELF header */
typedef struct {
        uint8_t     e_ident[EI_NIDENT];
        uint16_t    e_type; /* object file type */
        uint16_t    e_machine; /* machine architecture */
        uint32_t    e_version; /* object file version */
        uint32_t    e_entry; /* address to which control is transferred */
        uint32_t    e_phoff; /* phdr table file offset */
        uint32_t    e_shoff; /* shdr table file offset */
        uint32_t    e_flags; /* elf flags */
        uint16_t    e_ehsize; /* hdr size in bytes */
        uint16_t    e_phentsize; /* phdr table entry size */
        uint16_t    e_phnum; /* phdr table entry count */
        uint16_t    e_shentsize; /* shdr table entry size */
        uint16_t    e_shnum; /* shdr table entry count */
        uint16_t    e_shstrndx; /* string table index */
} Elf32_Ehdr;

/* ELF header */
typedef struct {
        uint8_t     e_ident[EI_NIDENT];
        uint16_t    e_type; /* object file type */
        uint16_t    e_machine; /* machine architecture */
        uint32_t    e_version; /* object file version */
        uint64_t    e_entry; /* address to which control is transferred */
        uint64_t    e_phoff; /* phdr table file offset */
        uint64_t    e_shoff; /* shdr table file offset */
        uint32_t    e_flags; /* elf flags */
        uint16_t    e_ehsize; /* hdr size in bytes */
        uint16_t    e_phentsize; /* phdr table entry size */
        uint16_t    e_phnum; /* phdr table entry count */
        uint16_t    e_shentsize; /* shdr table entry size */
        uint16_t    e_shnum; /* shdr table entry count */
        uint16_t    e_shstrndx; /* string table index */
} Elf64_Ehdr;

/* shdr special index */
enum {
        SHN_UNDEF	= 0,
        SHN_LORESERVE	= 0xff00,
        SHN_LOPROC	= 0xff00,
        SHN_HIPROC	= 0xff1f,
        SHN_LOOS	= 0xff20,
        SHN_HIOS	= 0xff3f,
        SHN_ABS	        = 0xfff1,
        SHN_COMMON	= 0xfff2,
        SHN_XINDEX	= 0xffff,
        SHN_HIRESERVE	= 0xffff
};

/* shdr type */
enum {
        SHT_NULL            = 0, /* shdr is inactive */
        SHT_PROGBITS	    = 1,
        SHT_SYMTAB          = 2, /* symbol table */
        SHT_STRTAB          = 3, /* string table */
        SHT_RELA            = 4,
        SHT_HASH	    = 5,
        SHT_DYNAMIC	    = 6,
        SHT_NOTE            = 7,
        SHT_NOBITS          = 8,
        SHT_REL	            = 9,
        SHT_SHLIB	    = 10,
        SHT_DYNSYM	    = 11, /* minimum dynamic symbol table */
        SHT_INIT_ARRAY	    = 14,
        SHT_FINI_ARRAY	    = 15,
        SHT_PREINIT_ARRA    = 16,
        SHT_GROUP           = 17,
        SHT_SYMTAB_SHNDX    = 18
};

/* 32-bit section header */
typedef struct {
        uint32_t    sh_name; /* section name (strtab index) */
        uint32_t    sh_type; /* section type */
        uint32_t    sh_flags; /* section flags */
        uint32_t    sh_addr; /* address at which the section should be loaded */
        uint32_t    sh_offset; /* byte offset in file image */
        uint32_t    sh_size; /* section size in bytes */
        uint32_t    sh_link; /* section index of associated section */
        uint32_t    sh_info; /* extra section information */
        uint32_t    sh_addralign; /* required alignment of section */
        uint32_t    sh_entsize; /* entry size for fixed-size sections */
} Elf32_Shdr;

/* 64-bit section header */
typedef struct {
        uint32_t    sh_name; /* section name (strtab index) */
        uint32_t    sh_type; /* section type */
        uint64_t    sh_flags; /* section flags */
        uint64_t    sh_addr; /* address at which the section should be loaded */
        uint64_t    sh_offset; /* byte offset in file image */
        uint64_t    sh_size; /* section size in bytes */
        uint32_t    sh_link; /* section index of associated section */
        uint32_t    sh_info; /* extra section information */
        uint64_t    sh_addralign; /* required alignment of section */
        uint64_t    sh_entsize; /* entry size for fixed-size sections */
} Elf64_Shdr;

/* symbol attribute manipulation */
#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define ELF64_ST_BIND(i) ((i)>>4)
#define ELF64_ST_TYPE(i) ((i)&0xf)
#define ELF64_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

/* symbol type attribute */
enum {
        STT_NOTYPE  = 0, /* not specified */
        STT_OBJECT  = 1, /* associated with a data object */
        STT_FUNC    = 2, /* associated with a function */
        STT_SECTION = 3, /* associated with a section */
        STT_FILE    = 4, /* associated with a source file */
        STT_COMMON  = 5, /* uninitialized common block */
        STT_TLS     = 6, /* thread-local storage  entity */
        STT_LOOS    = 10, /* lobound os-reserved */
        STT_HIOS    = 12, /* hibound os-reserved */
        STT_LOPROC  = 13, /* lobound processor-reserved */
        STT_HIPROC  = 15 /* hibound processor-reserved */
};

/* symbol binding attribute */
enum {
        STB_LOCAL   = 0, /* local symbols not visible outside of file */
        STB_GLOBAL  = 1, /* global symbols are visible to all combined files */
        STB_WEAK    = 2, /* lower precedence than global symbols */
        STB_LOOS    = 10, /* lobound os-reserved */
        STB_HIOS    = 12, /* hibound os-reserved */
        STB_LOPROC  = 13, /* lobound processor-reserved */
        STB_HIPROC  = 15 /* hibound processor-reserved */
};

/* symbol visibility */
enum {
        STV_DEFAULT     = 0, /* as specified by the binding attribute */
        STV_INTERNAL    = 1, /* meaning defined by processor supplements */
        STV_HIDDEN      = 2, /* not visible to other components */
        STV_PROTECTED   = 3 /* visible in other components, not preemptable */
};

/* 32-bit ELF symbol */
typedef struct {
        uint32_t    st_name; /* symbol name (strtab index) */
        uint32_t    st_value; /* value of the symbol */
        uint32_t    st_size; /* associated size */
        uint8_t     st_info; /* type and binding attributes */
        uint8_t     st_other; /* visibility */
        uint16_t    st_shndx; /* index to the related shdr */
} Elf32_Sym;

/* 64-bit ELF symbol */
typedef struct {
        uint32_t    st_name; /* symbol name (strtab index) */
        uint8_t     st_info; /* type and binding attributes */
        uint8_t     st_other; /* visibility */
        uint16_t    st_shndx; /* index to the related shdr */
        uint64_t    st_value; /* value of the symbol */
        uint64_t    st_size; /* associated size */
} Elf64_Sym;

#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((uint8_t)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(uint8_t)(t))

#define ELF64_R_SYM(i) ((i)>>32)
#define ELF64_R_TYPE(i) ((i)&0xffffffffL)
#define ELF64_R_INFO(s,t) (((s)<<32)+((t)&0xffffffffL))

/* relocation entries (connecting symbolic references with definitions) */
typedef struct {
        uint32_t    r_offset; /* where to apply the relocation */
        uint32_t    r_info; /* symbol table index and type of relocation */
} Elf32_Rel;

typedef struct {
        uint64_t    r_offset; /* where to apply the relocation */
        uint64_t    r_info; /* symbol table index and type of relocation */
} Elf64_Rel;

/* relocation entries with addend */
typedef struct {
        uint32_t    r_offset; /* where to apply the relocation */
        uint32_t    r_info; /* symbol table index and type of relocation */
        int32_t     r_addend; /* constant addend */
} Elf32_Rela;

typedef struct {
        uint64_t    r_offset; /* where to apply the relocation */
        uint64_t    r_info; /* symbol table index and type of relocation */
        int64_t     r_addend; /* constant addend */
} Elf64_Rela;

/* segment type */
enum {
        PT_NULL         = 0, /* unused */
        PT_LOAD         = 1, /* loadable segment */
        PT_DYNAMIC      = 2, /* dynamic linking information */
        PT_INTERP       = 3, /* location of path to invoke as interpreter */
        PT_NOTE         = 4, /* location of auxiliary information */
        PT_SHLIB        = 5, /* reserved */
        PT_PHDR         = 6, /* location and size of phdr itself */
        PT_TLS          = 7, /* thread-local storage template */
        PT_LOOS         = 0x60000000, /* lobound os-specific */
        PT_GNU_EH_FRAME = 0x6474e550, /* GCC .eh_frame_hdr segment */
        PT_GNU_STACK    = 0x6474e551, /* stack executability */
        PT_GNU_RELRO    = 0x6474e552, /* read-only after relocation */
        PT_GNU_PROPERTY = 0x6474e553, /* GNU property */
        PT_HIOS         = 0x6fffffff, /* hibound os-specific */
        PT_LOPROC       = 0x70000000, /* lobound processor-specific */
        PT_HIPROC       = 0x7fffffff /* hibound processor-specific */
};

/* segment flags */
enum {
        PF_X        = (1 << 0), /* execute */
        PF_W        = (1 << 1), /* write */
        PF_R        = (1 << 2), /* read */
        PF_MASKOS   = 0x0ff00000, /* unspecified */
        PF_MASKPROC = 0xf0000000 /* unspecified */
};

typedef struct {
        uint32_t p_type; /* segment type */
        uint32_t p_offset; /* offset of first byte in file */
        uint32_t p_vaddr; /* vaddr of first byte */
        uint32_t p_paddr; /* relevant for systems with physical addressing */
        uint32_t p_filesz; /* number of bytes of file image */
        uint32_t p_memsz; /* number of bytes of memory image */
        uint32_t p_flags; /* flags */
        uint32_t p_align; /* memory alignment in powers of 2 */
} Elf32_Phdr;

typedef struct {
        uint32_t p_type; /* segment type */
        uint32_t p_flags; /* flags */
        uint64_t p_offset; /* offset of first byte in file */
        uint64_t p_vaddr; /* vaddr of first byte */
        uint64_t p_paddr; /* relevant for systems with physical addressing */
        uint64_t p_filesz; /* number of bytes of file image */
        uint64_t p_memsz; /* number of bytes of memory image */
        uint64_t p_align; /* memory alignment in powers of 2 */
} Elf64_Phdr;

extern const char *elf_str_machine[];
extern const char *elf_str_os_abi[];
extern const char *elf_str_sh_type[];
extern const char *elf_str_ph_type[];

#endif
