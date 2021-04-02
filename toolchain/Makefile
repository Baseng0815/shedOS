BINUTILS 	:= binutils-2.36
GCC 		:= gcc-10.2.0

.PHONY: all binutils gcc

all: binutils gcc

binutils: $(BINUTILS)
	mkdir -p build-binutils
	cd build-binutils; \
	    ../$(BINUTILS)/configure --target=$(TARGET) --prefix="$(TOOLCHAIN)" \
	    --with-sysroot --disable-nls --disable-werror; \
	    make $(MAKEOPTS); \
	    make install

$(BINUTILS):
	curl "https://ftp.gnu.org/gnu/binutils/$(BINUTILS).tar.xz" \
	    -o $(BINUTILS).tar.xz
	tar xvf $(BINUTILS).tar.xz

gcc: $(GCC)
	echo "MULTILIB_OPTIONS += mno-red-zone" > $(GCC)/gcc/config/i386/t-x86_64-elf
	echo "MULTILIB_DIRNAMES += no-red-zone" >> $(GCC)/gcc/config/i386/t-x86_64-elf
	sed -i '/x86_64-\*-elf\*/ a tmake_file="$$\{tmake_file\} i386/t-x86_64-elf"' \
	    "$(GCC)/gcc/config.gcc"
	mkdir -p build-gcc
	cd build-gcc; \
	    ../$(GCC)/configure --target=$(TARGET) --prefix="$(TOOLCHAIN)" \
	    --disable-nls --enable-languages=c,c++ --without-headers; \
	    make $(MAKEOPTS) all-gcc; \
	    make $(MAKEOPTS) all-target-libgcc; \
	    make install-gcc; \
	    make install-target-libgcc

$(GCC):
	curl "https://ftp.gnu.org/gnu/gcc/$(GCC)/$(GCC).tar.xz" \
	    -o $(GCC).tar.xz
	tar xvf $(GCC).tar.xz

clean:
	find . ! -name Makefile -exec rm -rf {} \;
