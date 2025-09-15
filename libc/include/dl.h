#ifndef _INTERNAL_DL_H
#define _INTERNAL_DL_H

#include <features.h>
#include <elf.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define AUX_CNT 32
#define DYN_CNT 37

#if UINTPTR_MAX == 0xffffffff
typedef Elf32_Ehdr Ehdr;
typedef Elf32_Phdr Phdr;
typedef Elf32_Sym Sym;
typedef Elf32_Rel Rel;
typedef Elf32_Rela Rela;
#define R_TYPE(x) ELF32_R_TYPE(x)
#define R_SYM(x) ELF32_R_SYM(x)
#define R_INFO ELF32_R_INFO
#else
typedef Elf64_Ehdr Ehdr;
typedef Elf64_Phdr Phdr;
typedef Elf64_Sym Sym;
typedef Elf64_Rel Rel;
typedef Elf64_Rela Rela;
#define R_TYPE(x) ELF64_R_TYPE(x)
#define R_SYM(x) ELF64_R_SYM(x)
#define R_INFO ELF64_R_INFO
#endif

#define IS_RELATIVE(x) (R_TYPE(x) == R_AARCH64_RELATIVE)

typedef uint32_t Symndx;

typedef void (*stage2_func)(unsigned char *, size_t *);

#endif
