#define EI_NIDENT          16     /* Size of e_ident[] */

/* NOTE: elf64.h and elf32.h refer EI_NIDENT defined above */

#include "elf64.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Values for Elf_Ehdr::e_type */

#define ET_NONE           0       /* No file type */
#define ET_REL            1       /* Relocatable file */
#define ET_EXEC           2       /* Executable file */
#define ET_DYN            3       /* Shared object file */
#define ET_CORE           4       /* Core file */
#define ET_LOPROC         0xff00  /* Processor-specific */
#define ET_HIPROC         0xffff  /* Processor-specific */

/* Values for Elf_Ehdr::e_machine (most of this were not included in the
 * original SCO document but have been gleaned from elsewhere).
 */

#define EM_NONE            0      /* No machine */
#define EM_M32             1      /* AT&T WE 32100 */
#define EM_SPARC           2      /* SPARC */
#define EM_386             3      /* Intel 80386 */
#define EM_68K             4      /* Motorola 68000 */
#define EM_88K             5      /* Motorola 88000 */
#define EM_486             6      /* Intel 486+ */
#define EM_860             7      /* Intel 80860 */
#define EM_MIPS            8      /* MIPS R3000 Big-Endian */
#define EM_MIPS_RS4_BE     10     /* MIPS R4000 Big-Endian */
#define EM_PARISC          15     /* HPPA */
#define EM_SPARC32PLUS     18     /* Sun's "v8plus" */
#define EM_PPC             20     /* PowerPC */
#define EM_PPC64           21     /* PowerPC64 */
#define EM_ARM             40     /* ARM */
#define EM_ARM64           183    /* AArch64 */
#define EM_SH              42     /* SuperH */
#define EM_SPARCV9         43     /* SPARC v9 64-bit */
#define EM_H8_300          46
#define EM_IA_64           50     /* HP/Intel IA-64 */
#define EM_X86_64          62     /* AMD x86-64 */
#define EM_S390            22     /* IBM S/390 */
#define EM_CRIS            76     /* Axis Communications 32-bit embedded processor */
#define EM_V850            87     /* NEC v850 */
#define EM_M32R            88     /* Renesas M32R */
#define EM_XTENSA          94     /* Tensilica Xtensa */
#define EM_RISCV           243    /* RISC-V */
#define EM_ALPHA           0x9026
#define EM_CYGNUS_V850     0x9080
#define EM_CYGNUS_M32R     0x9041
#define EM_S390_OLD        0xa390
#define EM_FRV             0x5441

/* Values for Elf_Ehdr::e_version */

#define EV_NONE            0      /* Invalid version */
#define EV_CURRENT         1      /* The current version */

/* Table 2. Ehe ELF identifier */

#define EI_MAG0            0      /* File identification */
#define EI_MAG1            1
#define EI_MAG2            2
#define EI_MAG3            3
#define EI_CLASS           4      /* File class */
#define EI_DATA            5      /* Data encoding */
#define EI_VERSION         6      /* File version */
#define EI_PAD             7      /* Start of padding bytes */

/* EI_NIDENT is defined in "Included Files" section */

#define EI_MAGIC_SIZE      4
#define EI_MAGIC           {0x7f, 'E', 'L', 'F'}

/* Table 3. Values for EI_CLASS */

#define ELFCLASSNONE       0      /* Invalid class */
#define ELFCLASS32         1      /* 32-bit objects */
#define ELFCLASS64         2      /* 64-bit objects */

/* Table 4. Values for EI_DATA */

#define ELFDATANONE        0     /* Invalid data encoding */
#define ELFDATA2LSB        1     /* Least significant byte occupying the lowest address */
#define ELFDATA2MSB        2     /* Most significant byte occupying the lowest address */

/* Table 7: Special Section Indexes */

#define SHN_UNDEF          0
#define SHN_LOPROC         0xff00
#define SHN_HIPROC         0xff1f
#define SHN_ABS            0xfff1
#define SHN_COMMON         0xfff2

/* Figure 4-9: Section Types, sh_type */

#define SHT_NULL           0
#define SHT_PROGBITS       1
#define SHT_SYMTAB         2
#define SHT_STRTAB         3
#define SHT_RELA           4
#define SHT_HASH           5
#define SHT_DYNAMIC        6
#define SHT_NOTE           7
#define SHT_NOBITS         8
#define SHT_REL            9
#define SHT_SHLIB          10
#define SHT_DYNSYM         11
#define SHT_LOPROC         0x70000000
#define SHT_HIPROC         0x7fffffff
#define SHT_LOUSER         0x80000000
#define SHT_HIUSER         0xffffffff

/* Figure 4-11: Section Attribute Flags, sh_flags */

#define SHF_WRITE          1
#define SHF_ALLOC          2
#define SHF_EXECINSTR      4
#define SHF_MASKPROC       0xf0000000

/* Figure 4-16: Symbol Binding, ELF_ST_BIND */

#define STB_LOCAL          0
#define STB_GLOBAL         1
#define STB_WEAK           2
#define STB_GNU_UNIQUE     10
#define STB_LOPROC         13
#define STB_HIPROC         15

/* Figure 4-17: Symbol Types, ELF_ST_TYPE */

#define STT_NOTYPE         0
#define STT_OBJECT         1
#define STT_FUNC           2
#define STT_SECTION        3
#define STT_FILE           4
#define STT_COMMON         5
#define STT_TLS            6
#define STT_LOPROC         13
#define STT_HIPROC         15

/* Figure 5-2: Segment Types, p_type */

#define PT_NULL            0
#define PT_LOAD            1
#define PT_DYNAMIC         2
#define PT_INTERP          3
#define PT_NOTE            4
#define PT_SHLIB           5
#define PT_PHDR            6
#define	PT_TLS		   7
#define	PT_NUM		   8
#define PT_LOOS		   0x60000000
#define PT_GNU_EH_FRAME	   0x6474e550
#define PT_GNU_STACK	   0x6474e551
#define PT_GNU_RELRO	   0x6474e552
#define PT_GNU_PROPERTY	   0x6474e553
#define PT_HIOS		   0x6fffffff
#define PT_LOPROC	   0x70000000
#define PT_HIPROC	   0x7fffffff

/* Figure 5-3: Segment Flag Bits, p_flags */

#define PF_X               1          /* Execute */
#define PF_W               2          /* Write */
#define PF_R               4          /* Read */
#define PF_MASKPROC        0xf0000000 /* Unspecified */

/* Figure 5-10: Dynamic Array Tags, d_tag */

#define DT_NULL            0          /* d_un=ignored */
#define DT_NEEDED          1          /* d_un=d_val */
#define DT_PLTRELSZ        2          /* d_un=d_val */
#define DT_PLTGOT          3          /* d_un=d_ptr */
#define DT_HASH            4          /* d_un=d_ptr */
#define DT_STRTAB          5          /* d_un=d_ptr */
#define DT_SYMTAB          6          /* d_un=d_ptr */
#define DT_RELA            7          /* d_un=d_ptr */
#define DT_RELASZ          8          /* d_un=d_val */
#define DT_RELAENT         9          /* d_un=d_val */
#define DT_STRSZ           10         /* d_un=d_val */
#define DT_SYMENT          11         /* d_un=d_val */
#define DT_INIT            12         /* d_un=d_ptr */
#define DT_FINI            13         /* d_un=d_ptr */
#define DT_SONAME          14         /* d_un=d_val */
#define DT_RPATH           15         /* d_un=d_val */
#define DT_SYMBOLIC        16         /* d_un=ignored */
#define DT_REL             17         /* d_un=d_ptr */
#define DT_RELSZ           18         /* d_un=d_val */
#define DT_RELENT          19         /* d_un=d_val */
#define DT_PLTREL          20         /* d_un=d_val */
#define DT_DEBUG           21         /* d_un=d_ptr */
#define DT_TEXTREL         22         /* d_un=ignored */
#define DT_JMPREL          23         /* d_un=d_ptr */
#define DT_BINDNOW         24         /* d_un=ignored */
#define	DT_INIT_ARRAY	25
#define	DT_FINI_ARRAY	26
#define	DT_INIT_ARRAYSZ	27
#define	DT_FINI_ARRAYSZ	28
#define DT_RUNPATH	29
#define DT_FLAGS	30
#define DT_ENCODING	32
#define DT_PREINIT_ARRAY 32
#define DT_PREINIT_ARRAYSZ 33
#define DT_SYMTAB_SHNDX	34
#define DT_RELRSZ	35
#define DT_RELR		36
#define DT_RELRENT	37
#define	DT_NUM		38
#define DT_GNU_HASH        0x6ffffef5
#define DT_VERSYM          0x6ffffff0
#define DT_LOPROC          0x70000000 /* d_un=unspecified */
#define DT_HIPROC          0x7fffffff /* d_un= unspecified */

/* Symbolic values for the entries in the auxiliary table put on the initial stack */

#define AT_NULL   0	/* end of vector */
#define AT_IGNORE 1	/* entry should be ignored */
#define AT_EXECFD 2	/* file descriptor of program */
#define AT_PHDR   3	/* program headers for program */
#define AT_PHENT  4	/* size of program header entry */
#define AT_PHNUM  5	/* number of program headers */
#define AT_PAGESZ 6	/* system page size */
#define AT_BASE   7	/* base address of interpreter */
#define AT_FLAGS  8	/* flags */
#define AT_ENTRY  9	/* entry point of program */
#define AT_NOTELF 10	/* program is not ELF */
#define AT_UID    11	/* real uid */
#define AT_EUID   12	/* effective uid */
#define AT_GID    13	/* real gid */
#define AT_EGID   14	/* effective gid */
#define AT_PLATFORM 15  /* string identifying CPU for optimizations */
#define AT_HWCAP  16    /* arch dependent hints at CPU capabilities */
#define AT_CLKTCK 17	/* frequency at which times() increments */
/* AT_* values 18 through 22 are reserved */
#define AT_SECURE 23   /* secure mode boolean */
#define AT_BASE_PLATFORM 24	/* string identifying real platform, may differ from AT_PLATFORM. */
#define AT_RANDOM 25	/* address of 16 random bytes */
#define AT_HWCAP2 26	/* extension of AT_HWCAP */

#define AT_EXECFN  31	/* filename of program */

#define AT_SYSINFO	32
#define AT_SYSINFO_EHDR	33

#define AT_MINSIGSTKSZ	51	/* stack needed for signal delivery */

#define AT_VECTOR_SIZE_ARCH 2
#define AT_VECTOR_SIZE_BASE 20
#define AT_VECTOR_SIZE (2*(AT_VECTOR_SIZE_ARCH + AT_VECTOR_SIZE_BASE + 1))

#define AUX_CNT 32
#define DYN_CNT 37

#define R_AARCH64_NONE		0
#define R_AARCH64_P32_ABS32	1
#define R_AARCH64_P32_COPY	180
#define R_AARCH64_P32_GLOB_DAT	181
#define R_AARCH64_P32_JUMP_SLOT	182
#define R_AARCH64_P32_RELATIVE	183
#define R_AARCH64_P32_TLS_DTPMOD 184
#define R_AARCH64_P32_TLS_DTPREL 185
#define R_AARCH64_P32_TLS_TPREL	186
#define R_AARCH64_P32_TLSDESC	187
#define R_AARCH64_P32_IRELATIVE	188
#define R_AARCH64_ABS64         257
#define R_AARCH64_ABS32         258
#define R_AARCH64_ABS16		259
#define R_AARCH64_PREL64	260
#define R_AARCH64_PREL32	261
#define R_AARCH64_PREL16	262
#define R_AARCH64_MOVW_UABS_G0	263
#define R_AARCH64_MOVW_UABS_G0_NC 264
#define R_AARCH64_MOVW_UABS_G1	265
#define R_AARCH64_MOVW_UABS_G1_NC 266
#define R_AARCH64_MOVW_UABS_G2	267
#define R_AARCH64_MOVW_UABS_G2_NC 268
#define R_AARCH64_MOVW_UABS_G3	269
#define R_AARCH64_MOVW_SABS_G0	270
#define R_AARCH64_MOVW_SABS_G1	271
#define R_AARCH64_MOVW_SABS_G2	272
#define R_AARCH64_LD_PREL_LO19	273
#define R_AARCH64_ADR_PREL_LO21	274
#define R_AARCH64_ADR_PREL_PG_HI21 275
#define R_AARCH64_ADR_PREL_PG_HI21_NC 276
#define R_AARCH64_ADD_ABS_LO12_NC 277
#define R_AARCH64_LDST8_ABS_LO12_NC 278
#define R_AARCH64_TSTBR14	279
#define R_AARCH64_CONDBR19	280
#define R_AARCH64_JUMP26	282
#define R_AARCH64_CALL26	283
#define R_AARCH64_LDST16_ABS_LO12_NC 284
#define R_AARCH64_LDST32_ABS_LO12_NC 285
#define R_AARCH64_LDST64_ABS_LO12_NC 286
#define R_AARCH64_MOVW_PREL_G0	287
#define R_AARCH64_MOVW_PREL_G0_NC 288
#define R_AARCH64_MOVW_PREL_G1	289
#define R_AARCH64_MOVW_PREL_G1_NC 290
#define R_AARCH64_MOVW_PREL_G2	291
#define R_AARCH64_MOVW_PREL_G2_NC 292
#define R_AARCH64_MOVW_PREL_G3	293
#define R_AARCH64_LDST128_ABS_LO12_NC 299
#define R_AARCH64_MOVW_GOTOFF_G0 300
#define R_AARCH64_MOVW_GOTOFF_G0_NC 301
#define R_AARCH64_MOVW_GOTOFF_G1 302
#define R_AARCH64_MOVW_GOTOFF_G1_NC 303
#define R_AARCH64_MOVW_GOTOFF_G2 304
#define R_AARCH64_MOVW_GOTOFF_G2_NC 305
#define R_AARCH64_MOVW_GOTOFF_G3 306
#define R_AARCH64_GOTREL64	307
#define R_AARCH64_GOTREL32	308
#define R_AARCH64_GOT_LD_PREL19	309
#define R_AARCH64_LD64_GOTOFF_LO15 310
#define R_AARCH64_ADR_GOT_PAGE	311
#define R_AARCH64_LD64_GOT_LO12_NC 312
#define R_AARCH64_LD64_GOTPAGE_LO15 313
#define R_AARCH64_TLSGD_ADR_PREL21 512
#define R_AARCH64_TLSGD_ADR_PAGE21 513
#define R_AARCH64_TLSGD_ADD_LO12_NC 514
#define R_AARCH64_TLSGD_MOVW_G1	515
#define R_AARCH64_TLSGD_MOVW_G0_NC 516
#define R_AARCH64_TLSLD_ADR_PREL21 517
#define R_AARCH64_TLSLD_ADR_PAGE21 518
#define R_AARCH64_TLSLD_ADD_LO12_NC 519
#define R_AARCH64_TLSLD_MOVW_G1	520
#define R_AARCH64_TLSLD_MOVW_G0_NC 521
#define R_AARCH64_TLSLD_LD_PREL19 522
#define R_AARCH64_TLSLD_MOVW_DTPREL_G2 523
#define R_AARCH64_TLSLD_MOVW_DTPREL_G1 524
#define R_AARCH64_TLSLD_MOVW_DTPREL_G1_NC 525
#define R_AARCH64_TLSLD_MOVW_DTPREL_G0 526
#define R_AARCH64_TLSLD_MOVW_DTPREL_G0_NC 527
#define R_AARCH64_TLSLD_ADD_DTPREL_HI12 528
#define R_AARCH64_TLSLD_ADD_DTPREL_LO12 529
#define R_AARCH64_TLSLD_ADD_DTPREL_LO12_NC 530
#define R_AARCH64_TLSLD_LDST8_DTPREL_LO12 531
#define R_AARCH64_TLSLD_LDST8_DTPREL_LO12_NC 532
#define R_AARCH64_TLSLD_LDST16_DTPREL_LO12 533
#define R_AARCH64_TLSLD_LDST16_DTPREL_LO12_NC 534
#define R_AARCH64_TLSLD_LDST32_DTPREL_LO12 535
#define R_AARCH64_TLSLD_LDST32_DTPREL_LO12_NC 536
#define R_AARCH64_TLSLD_LDST64_DTPREL_LO12 537
#define R_AARCH64_TLSLD_LDST64_DTPREL_LO12_NC 538
#define R_AARCH64_TLSIE_MOVW_GOTTPREL_G1 539
#define R_AARCH64_TLSIE_MOVW_GOTTPREL_G0_NC 540
#define R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21 541
#define R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC 542
#define R_AARCH64_TLSIE_LD_GOTTPREL_PREL19 543
#define R_AARCH64_TLSLE_MOVW_TPREL_G2 544
#define R_AARCH64_TLSLE_MOVW_TPREL_G1 545
#define R_AARCH64_TLSLE_MOVW_TPREL_G1_NC 546
#define R_AARCH64_TLSLE_MOVW_TPREL_G0 547
#define R_AARCH64_TLSLE_MOVW_TPREL_G0_NC 548
#define R_AARCH64_TLSLE_ADD_TPREL_HI12 549
#define R_AARCH64_TLSLE_ADD_TPREL_LO12 550
#define R_AARCH64_TLSLE_ADD_TPREL_LO12_NC 551
#define R_AARCH64_TLSLE_LDST8_TPREL_LO12 552
#define R_AARCH64_TLSLE_LDST8_TPREL_LO12_NC 553
#define R_AARCH64_TLSLE_LDST16_TPREL_LO12 554
#define R_AARCH64_TLSLE_LDST16_TPREL_LO12_NC 555
#define R_AARCH64_TLSLE_LDST32_TPREL_LO12 556
#define R_AARCH64_TLSLE_LDST32_TPREL_LO12_NC 557
#define R_AARCH64_TLSLE_LDST64_TPREL_LO12 558
#define R_AARCH64_TLSLE_LDST64_TPREL_LO12_NC 559
#define R_AARCH64_TLSDESC_LD_PREL19 560
#define R_AARCH64_TLSDESC_ADR_PREL21 561
#define R_AARCH64_TLSDESC_ADR_PAGE21 562
#define R_AARCH64_TLSDESC_LD64_LO12 563
#define R_AARCH64_TLSDESC_ADD_LO12 564
#define R_AARCH64_TLSDESC_OFF_G1 565
#define R_AARCH64_TLSDESC_OFF_G0_NC 566
#define R_AARCH64_TLSDESC_LDR	567
#define R_AARCH64_TLSDESC_ADD	568
#define R_AARCH64_TLSDESC_CALL	569
#define R_AARCH64_TLSLE_LDST128_TPREL_LO12 570
#define R_AARCH64_TLSLE_LDST128_TPREL_LO12_NC 571
#define R_AARCH64_TLSLD_LDST128_DTPREL_LO12 572
#define R_AARCH64_TLSLD_LDST128_DTPREL_LO12_NC 573
#define R_AARCH64_COPY         1024
#define R_AARCH64_GLOB_DAT     1025
#define R_AARCH64_JUMP_SLOT    1026
#define R_AARCH64_RELATIVE     1027
#define R_AARCH64_TLS_DTPMOD   1028
#define R_AARCH64_TLS_DTPMOD64 1028
#define R_AARCH64_TLS_DTPREL   1029
#define R_AARCH64_TLS_DTPREL64 1029
#define R_AARCH64_TLS_TPREL    1030
#define R_AARCH64_TLS_TPREL64  1030
#define R_AARCH64_TLSDESC      1031

#define IS_RELATIVE(x) (ELF64_R_TYPE(x) == R_AARCH64_RELATIVE)
