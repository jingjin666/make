#include <sys/types.h>
#include <elf.h>
#include "syscall.h"
#include "osx_dl.h"

__asm__(
	".text \n"
	".global osx_dlstart\n"
	".type osx_dlstart,%function\n"
	"osx_dlstart:\n"
	"	mov x29, #0\n"
	"	mov x30, #0\n"
	"	mov x0, sp\n"
	".weak _DYNAMIC\n"
	".hidden _DYNAMIC\n"
	"	adrp x1, _DYNAMIC\n"
	"	add x1, x1, #:lo12:_DYNAMIC\n"
	"	and sp, x0, #-16\n"
	"	b osx_dlstart_c\n"
);

#ifndef GETFUNCSYM
#define GETFUNCSYM(fp, sym, got) do { \
	hidden void sym(void); \
	static void (*static_func_ptr)() = sym; \
	__asm__ __volatile__ ("" : "+m"(static_func_ptr) : : "memory"); \
	*(fp) = static_func_ptr; \
	} while (0)
#endif

hidden void osx_dlstart_c(size_t *sp, size_t *dynv)
{
	size_t i, aux[AUX_CNT], dyn[DYN_CNT];
	size_t *rel, rel_size, base;
	int argc = *sp;
	char **argv = (void *)(sp+1);

	for (i = argc + 1; argv[i]; i++) {
		;
	}

	size_t *auxv = (void *)(argv + i + 1);

	for (i = 0; i < AUX_CNT; i++) {
		aux[i] = 0;
	}
	for (i = 0; auxv[i]; i += 2) {
		if (auxv[i] < AUX_CNT) {
			aux[auxv[i]] = auxv[i+1];
		}
	}

	for (i = 0; i < DYN_CNT; i++) {
		dyn[i] = 0;
	}
	for (i = 0; dynv[i]; i += 2) {
		if (dynv[i] < DYN_CNT) {
			dyn[dynv[i]] = dynv[i+1];
		}
	}

	base = aux[AT_BASE];
	__syscall6(499, base, dynv, 0, 0, 0, __LINE__);

	rel = (void *)(base + dyn[DT_REL]);
	rel_size = dyn[DT_RELSZ];
	for (; rel_size; rel += 2, rel_size -= 2 * sizeof(size_t)) {
		if (!IS_RELATIVE(rel[1])) {
			continue;
		}
		size_t *rel_addr = (void *)(base + rel[0]);
		*rel_addr += base;
	}

	rel = (void *)(base + dyn[DT_RELA]);
	rel_size = dyn[DT_RELASZ];
	for (; rel_size; rel += 3, rel_size -= 3 * sizeof(size_t)) {
		if (!IS_RELATIVE(rel[1])) {
			continue;
		}
		size_t *rel_addr = (void *)(base + rel[0]);
		*rel_addr = base + rel[2];
	}

	rel = (void *)(base + dyn[DT_RELR]);
	rel_size = dyn[DT_RELRSZ];
	size_t *relr_addr = 0;
	for (; rel_size; rel++, rel_size -= sizeof(size_t)) {
		if ((rel[0] & 1) == 0) {
			relr_addr = (void *)(base + rel[0]);
			*relr_addr++ += base;
		} else {
			for (size_t i = 0, bitmap = rel[0]; bitmap >>= 1; i++) {
				if (bitmap & 1) {
					relr_addr[i] += base;
				}
			}
			relr_addr += 8*sizeof(size_t)-1;
		}
	}

	stage2_func __osx_dl_s1;
	GETFUNCSYM(&__osx_dl_s1, osx_dl_s1, base + dyn[DT_PLTGOT]);
	__osx_dl_s1((unsigned char *)base, sp);
}
