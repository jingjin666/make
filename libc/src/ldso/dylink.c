#include <sys/types.h>
#include <elf.h>
#include "syscall.h"
#include "dl.h"

struct dso {
	unsigned char *base;
	char *name;
	size_t *dynv;
	struct dso *next, *prev;
	char relocated;

	Phdr *phdr;
	int phnum;
	size_t phentsize;
	Sym *syms;
	Symndx *hashtab;
	uint32_t *ghashtab;
	int16_t *versym;
	char *strings;
	struct dso *syms_next, *lazy_next;
	struct dso **deps, *needed_by;
	size_t ndeps_direct;
};

static struct dso rt_ldso;
static struct dso *head, *tail, *syms_tail;

int data1 = 9527;

int *p = &data1;

int get_dl_data(void)
{
	return data1;
}

#define ldso_addr(p, v) ((p)->base + (v))

static void relocate_test(void)
{
	int x = get_dl_data();
	int y = ++(*p);
	__syscall6(499, 4, x, y, 0, 0, 0);
}

struct symdef {
	Sym *sym;
	struct dso *dso;
};

static uint32_t sysv_hash(const char *s0)
{
	const unsigned char *s = (void *)s0;
	uint32_t h = 0;
	while (*s) {
		h = 16*h + *s++;
		h ^= h>>24 & 0xf0;
	}
	return h & 0xfffffff;
}

static uint32_t gnu_hash(const char *s0)
{
	const unsigned char *s = (void *)s0;
	uint32_t h = 5381;
	for (; *s; s++)
		h += h*32 + *s;
	return h;
}

static int dl_strcmp(const char *l, const char *r)
{
	for (; *l == *r && *l; l++, r++) {
		;
	}
	return *(unsigned char *)l - *(unsigned char *)r;
}

#define strcmp(l, r) dl_strcmp(l, r)

static Sym *sysv_lookup(const char *s, uint32_t h, struct dso *dso)
{
	size_t i;
	Sym *syms = dso->syms;
	Symndx *hashtab = dso->hashtab;
	char *strings = dso->strings;
	// __syscall6(499, 14, syms, hashtab, strings, dso->versym, 0);
	for (i = hashtab[2 + h % hashtab[0]]; i; i = hashtab[2 + hashtab[0] + i]) {
		if ((!dso->versym || dso->versym[i] >= 0)
		    && (!dl_strcmp(s, strings+syms[i].st_name))) {
			return syms + i;
		}
	}
	return 0;
}

static Sym *gnu_lookup(uint32_t h1, uint32_t *hashtab, struct dso *dso, const char *s)
{
	uint32_t nbuckets = hashtab[0];
	uint32_t *buckets = hashtab + 4 + hashtab[2] * (sizeof(size_t) / 4);
	uint32_t i = buckets[h1 % nbuckets];

	if (!i) {
		return 0;
	}

	uint32_t *hashval = buckets + nbuckets + (i - hashtab[1]);

	for (h1 |= 1; ; i++) {
		uint32_t h2 = *hashval++;
		if ((h1 == (h2|1)) && (!dso->versym || dso->versym[i] >= 0)
		    && !strcmp(s, dso->strings + dso->syms[i].st_name)) {
			return dso->syms + i;
		}
		if (h2 & 1) {
			break;
		}
	}

	return 0;
}

static Sym *gnu_lookup_filtered(uint32_t h1, uint32_t *hashtab, struct dso *dso, const char *s, uint32_t fofs, size_t fmask)
{
	const size_t *bloomwords = (const void *)(hashtab+4);
	size_t f = bloomwords[fofs & (hashtab[2]-1)];
	if (!(f & fmask)) {
		return 0;
	}

	f >>= (h1 >> hashtab[3]) % (8 * sizeof f);
	if (!(f & 1)) {
		return 0;
	}

	return gnu_lookup(h1, hashtab, dso, s);
}

#define OK_TYPES (1<<STT_NOTYPE | 1<<STT_OBJECT | 1<<STT_FUNC | 1<<STT_COMMON | 1<<STT_TLS)
#define OK_BINDS (1<<STB_GLOBAL | 1<<STB_WEAK | 1<<STB_GNU_UNIQUE)

#ifndef ARCH_SYM_REJECT_UND
#define ARCH_SYM_REJECT_UND(s) 0
#endif

static inline struct symdef find_sym2(struct dso *dso, const char *s, int need_def, int use_deps)
{
	uint32_t h = 0, gh = gnu_hash(s), gho = gh / (8*sizeof(size_t)), *ght;
	size_t ghm = 1ul << gh % (8*sizeof(size_t));
	struct symdef def = {0};
	struct dso **deps = use_deps ? dso->deps : 0;
	for (; dso; dso = use_deps ? *deps++ : dso->syms_next) {
		// __syscall6(499, 11, 0, 0, 0, 0, 0);
		Sym *sym;
		ght = dso->ghashtab;
		if (ght) {
			// __syscall6(499, 12, 0, 0, 0, 0, 0);
			sym = gnu_lookup_filtered(gh, ght, dso, s, gho, ghm);
		} else {
			if (!h) {
				h = sysv_hash(s);
			}
			// __syscall6(499, 13, dso->name, 0, 0, 0, __LINE__);
			sym = sysv_lookup(s, h, dso);
			// __syscall6(499, 255, sym == NULL ? 0 : sym->st_value, 0, 0, 0, __LINE__);
		}

		if (!sym) {
			continue;
		}
		if (!sym->st_shndx)
			if (need_def || (sym->st_info&0xf) == STT_TLS
			    || ARCH_SYM_REJECT_UND(sym)) {
				continue;
			    }
		if (!sym->st_value) {
			if ((sym->st_info&0xf) != STT_TLS) {
				continue;
			}
		}
		if (!(1 << (sym->st_info & 0xf) & OK_TYPES)) {
			continue;
		}
		if (!(1 << (sym->st_info >> 4) & OK_BINDS)) {
			continue;
		}
		def.sym = sym;
		def.dso = dso;
		break;
	}
	return def;
}

static struct symdef find_sym(struct dso *dso, const char *s, int need_def)
{
	return find_sym2(dso, s, need_def, 0);
}

static void relocate(struct dso *ldso, Rel *rel, size_t rel_size)
{
	struct symdef def = {0};
	struct dso *ctx = NULL;
	size_t rel_end = (size_t)(rel + rel_size / sizeof(*rel));
	int skip_relative = 0;

	if (ldso == &rt_ldso) {
		skip_relative = 1;
	}

	for (; (size_t)rel < rel_end; rel++) {
		if (skip_relative && IS_RELATIVE(rel->r_info)) {
			__syscall6(499, 0, 0, 0, 0, 0, __LINE__);
			continue;
		}

		size_t index, type, *where, addend;

		where = (size_t *)ldso_addr(ldso, rel->r_offset);
		index = R_SYM(rel->r_info);
		type = R_TYPE(rel->r_info);
		if (type == R_AARCH64_GLOB_DAT || R_AARCH64_JUMP_SLOT || R_AARCH64_COPY) {
			addend = 0;
		} else {
			addend = *where;
		}

		if (type == R_AARCH64_NONE) {
			continue;
		}

		if (index) {
			Sym *dynsym = ldso->syms + index;
			const char *name = ldso->strings + dynsym->st_name;

			if (type == R_AARCH64_COPY) {
				ctx = head->syms_next;
			} else {
				ctx = head;
			}

			// __syscall6(499, where, rel->r_info, index, dynsym->st_value, 0, 0);
			if (dynsym->st_info >> 4 == STB_LOCAL) {
				def.dso = ldso;
				def.sym = dynsym;
				// __syscall6(499, 8, rel->r_info, index, def.sym->st_value, 0, 0);
			} else {
				// __syscall6(499, 9, rel->r_info, index, dynsym->st_value, 0, 0);
				// __syscall6(499, rel->r_info, name, 0, 0, 0, __LINE__);
				def = find_sym(ctx, name, type == R_AARCH64_JUMP_SLOT);
				// __syscall6(499, 10, rel->r_info, index, def.sym->st_value, 0, __LINE__);
			}

			switch (type) {
			case R_AARCH64_ABS64:
			case R_AARCH64_GLOB_DAT:
			case R_AARCH64_JUMP_SLOT:
				*where = (size_t)ldso_addr(def.dso, def.sym->st_value) + addend;
				// __syscall6(499, where, *where , 0, 0, 0, __LINE__);
				break;
			case R_AARCH64_RELATIVE:
				*where = (size_t)ldso_addr(ldso, addend);
				break;
			default:
				break;
			}
		}
	}
}

static void relocate_a(struct dso *ldso, Rela *rel, size_t rel_size)
{
	struct symdef def = {0};
	struct dso *ctx = NULL;
	size_t rel_end = (size_t)(rel + rel_size / sizeof(*rel));
	int skip_relative = 0;

	if (ldso == &rt_ldso) {
		skip_relative = 1;
	}

	for (; (size_t)rel < rel_end; rel++) {
		if (skip_relative && IS_RELATIVE(rel->r_info)) {
			// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
			continue;
		}

		size_t index, type, *where, addend;

		where = (size_t *)ldso_addr(ldso, rel->r_offset);
		index = R_SYM(rel->r_info);
		type = R_TYPE(rel->r_info);
		addend = rel->r_addend;

		if (type == R_AARCH64_NONE) {
			continue;
		}

		if (index) {
			Sym *dynsym = ldso->syms + index;
			const char *name = ldso->strings + dynsym->st_name;

			if (type == R_AARCH64_COPY) {
				ctx = head->syms_next;
			} else {
				ctx = head;
			}

			// __syscall6(499, where, rel->r_info, index, dynsym->st_value, 0, 0);
			if (dynsym->st_info >> 4 == STB_LOCAL) {
				def.dso = ldso;
				def.sym = dynsym;
				// __syscall6(499, 8, rel->r_info, index, def.sym->st_value, 0, 0);
			} else {
				// __syscall6(499, 9, rel->r_info, index, dynsym->st_value, 0, 0);
				// __syscall6(499, rel->r_info, name, 0, 0, 0, __LINE__);
				def = find_sym(ctx, name, type == R_AARCH64_JUMP_SLOT);
				// __syscall6(499, 10, rel->r_info, index, def.sym->st_value, 0, __LINE__);
			}

			switch (type) {
			case R_AARCH64_ABS64:
			case R_AARCH64_GLOB_DAT:
			case R_AARCH64_JUMP_SLOT:
				*where = (size_t)ldso_addr(def.dso, def.sym->st_value) + addend;
				// __syscall6(499, where, *where , 0, 0, 0, __LINE__);
				break;
			case R_AARCH64_RELATIVE:
				*where = (size_t)ldso_addr(ldso, addend);
				break;
			default:
				break;
			}
		}
	}
}

static void decode_vec(size_t *v, size_t *a, size_t cnt)
{
	size_t i;
	for (i = 0; i < cnt; i++) {
		a[i] = 0;
	}
	for (; v[0]; v += 2) {
		if (v[0] - 1 < cnt - 1) {
			if (v[0] < 8 * sizeof(long)) {
				a[0] |= 1UL<<v[0];
			}
			a[v[0]] = v[1];
		}
	}
}

static int search_vec(size_t *v, size_t *r, size_t key)
{
	for (; v[0] != key; v += 2) {
		if (!v[0]) {
			return 0;
		}
	}
	*r = v[1];
	return 1;
}

static void decode_dyn(struct dso *ldso)
{
	size_t dyn[DYN_CNT];
	decode_vec(ldso->dynv, dyn, DYN_CNT);

	ldso->syms = (Sym *)ldso_addr(ldso, dyn[DT_SYMTAB]);
	ldso->strings = (char *)ldso_addr(ldso, dyn[DT_STRTAB]);
	if (dyn[0]&(1<<DT_HASH)) {
		ldso->hashtab = (Symndx *)ldso_addr(ldso, dyn[DT_HASH]);
	}
	if (search_vec(ldso->dynv, dyn, DT_GNU_HASH)) {
		ldso->ghashtab = (uint32_t *)ldso_addr(ldso, *dyn);
	}
	if (search_vec(ldso->dynv, dyn, DT_VERSYM)) {
		ldso->versym = (int16_t *)ldso_addr(ldso, *dyn);
	}
}

static void dl_relocate(struct dso *ldso)
{
	for (; ldso; ldso = ldso->next) {
		if (ldso->relocated) {
			// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
			continue;
		}

		// __syscall6(499, ldso->name, 0, 0, 0, 0, __LINE__);

		size_t dyn[DYN_CNT];
		decode_vec(ldso->dynv, dyn, DYN_CNT);

		ldso->syms = (Sym *)ldso_addr(ldso, dyn[DT_SYMTAB]);
		ldso->strings = (char *)ldso_addr(ldso, dyn[DT_STRTAB]);
		if (dyn[0]&(1<<DT_HASH)) {
			ldso->hashtab = (Symndx *)ldso_addr(ldso, dyn[DT_HASH]);
		}
		if (search_vec(ldso->dynv, dyn, DT_GNU_HASH)) {
			ldso->ghashtab = (uint32_t *)ldso_addr(ldso, *dyn);
		}
		if (search_vec(ldso->dynv, dyn, DT_VERSYM)) {
			ldso->versym = (int16_t *)ldso_addr(ldso, *dyn);
		}

		// __syscall6(499, 1, dyn[DT_PLTREL], dyn[DT_PLTRELSZ], 0, 0, __LINE__);
		if (dyn[DT_PLTREL] == DT_RELA) {
			relocate_a(ldso, (Rela *)ldso_addr(ldso, dyn[DT_JMPREL]), dyn[DT_PLTRELSZ]);
		} else {
			relocate(ldso, (Rel *)ldso_addr(ldso, dyn[DT_JMPREL]), dyn[DT_PLTRELSZ]);
		}

		// __syscall6(499, 1, dyn[DT_RELSZ], 0, 0, 0, __LINE__);
		relocate(ldso, (Rel *)ldso_addr(ldso, dyn[DT_REL]), dyn[DT_RELSZ]);

		// __syscall6(499, 1, dyn[DT_RELASZ], 0, 0, 0, __LINE__);
		relocate_a(ldso, (Rela *)ldso_addr(ldso, dyn[DT_RELA]), dyn[DT_RELASZ]);

		// __syscall6(499, 1, dyn[DT_RELRSZ], 0, 0, 0, __LINE__);

		ldso->relocated = 1;
	}
}

static size_t ldso_page_size;
#ifndef PAGE_SIZE
#define PAGE_SIZE ldso_page_size
#endif

static struct dso *builtin_deps[2];
static struct dso *const no_deps[1];

static struct dso *dl_load_library(const char *name, struct dso *needed_by)
{
	// __syscall6(499, name, rt_ldso.name, 0, 0, 0, __LINE__);
	if (strcmp(name, rt_ldso.name) == 0) {
		// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
		if (!rt_ldso.prev) {
			// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
			tail->next = &rt_ldso;
			rt_ldso.prev = tail;
			tail = &rt_ldso;
		}
		return &rt_ldso;
	}

	return NULL;
}

static void dl_load_deps(struct dso *ldso)
{
	if (ldso->deps) {
		return;
	}

	for (; ldso; ldso = ldso->next) {
		size_t i, cnt = 0;

		// __syscall6(499, ldso, ldso->dynv, 0, 0, 0, __LINE__);

		if (ldso->deps) {
			break;
		}

		for (i = 0; ldso->dynv[i]; i += 2) {
			if (ldso->dynv[i] == DT_NEEDED) {
				cnt++;
			}
		}

		// __syscall6(499, cnt, 0, 0, 0, 0, __LINE__);

		if (ldso == head && cnt < 2) {
			// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
			ldso->deps = builtin_deps;
		} else {
			// ldso->deps = malloc(sizeof(*ldos->deps));
		}

		cnt = 0;
		if (ldso == head) {
			// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
			for (struct dso *q = ldso->next; q; q = q->next) {
				// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
				ldso->deps[cnt++] = q;
			}
		}

		for (i = 0; ldso->dynv[i]; i += 2) {
			if (ldso->dynv[i] != DT_NEEDED) {
				continue;
			}
			// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
			struct dso *dep = dl_load_library(ldso->strings + ldso->dynv[i+1], ldso);
			if (!dep) {
				// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
				// crash();
			}
			ldso->deps[cnt++] = dep;
		}

		ldso->deps[cnt] = 0;
		ldso->ndeps_direct = cnt;
	}

	// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
	for (struct dso *p = head; p; p = p->next) {
		// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
		if (!p->syms_next && syms_tail != p) {
			// __syscall6(499, 0, 0, 0, 0, 0, __LINE__);
			syms_tail->syms_next = p;
			syms_tail = p;
		}
	}
}

static struct dso main_ldso;
void _dl_s2(size_t *sp, size_t *auxv)
{
	struct dso *ldso = &main_ldso;
	int argc = *sp;
	char **argv = (void *)(sp+1);
	size_t aux[AUX_CNT];

	decode_vec(auxv, aux, AUX_CNT);
	if (aux[AT_PHDR] != (size_t)rt_ldso.phdr) {
		ldso->phdr = (Phdr *)aux[AT_PHDR];
		ldso->phnum = aux[AT_PHNUM];
		ldso->phentsize = aux[AT_PHENT];
		// __syscall6(499, 7, ldso->phdr, ldso->phnum, 0, 0, 0);

		Phdr *phdr = NULL;
		for (int n = 0; n < ldso->phnum; n++) {
			phdr = &ldso->phdr[n];

			if (phdr->p_type == PT_PHDR) {
				ldso->base = (unsigned char *)(aux[AT_PHDR] - phdr->p_vaddr);
				// __syscall6(499, 7, aux[AT_PHDR], phdr->p_vaddr, 0, 0, 0);
			} else if (phdr->p_type == PT_INTERP) {
				rt_ldso.name = (char *)ldso_addr(ldso, phdr->p_vaddr);
				// __syscall6(499, 7, rt_ldso.name, phdr->p_vaddr, 0, 0, 0);
			} else if (phdr->p_type == PT_DYNAMIC) {
				ldso->dynv = (size_t *)ldso_addr(ldso, phdr->p_vaddr);
				// __syscall6(499, 7, ldso->dynv, phdr->p_vaddr, 0, 0, 0);
			} else if (phdr->p_type == PT_TLS) {
				//todo
			}
		}

		ldso->name = argv[0];
	} else {
		// crash():
	}

	decode_dyn(ldso);

	head = tail = syms_tail = ldso;

	rt_ldso.deps = (struct dso **)no_deps;

	dl_load_deps(ldso);

	__syscall6(499, 0, 0, 0, 0, 0, __LINE__);
	dl_relocate(ldso->next);

	__syscall6(499, 0, 0, 0, 0, 0, __LINE__);
	dl_relocate(ldso);

	__syscall6(499, aux[AT_ENTRY], argc, argv - 1, 0, 0, __LINE__);

	__asm__ __volatile__("mov sp, %1 ; br %0" : : "r"((void *)aux[AT_ENTRY]), "r"(argv - 1) : "memory");
}

hidden void _dl_s1(unsigned char *base, size_t *sp)
{
	struct dso *ldso = &rt_ldso;
	size_t *auxv;

	for (auxv = sp + 1 + *sp + 1; *auxv; auxv++) {
		;
	}
	auxv++;

	ldso->base = base;
	Ehdr *ehdr = (Ehdr *)ldso->base;
	ldso->phdr = (Phdr *)ldso_addr(ldso, ehdr->e_phoff);
	ldso->phnum = ehdr->e_phnum;
	ldso->phentsize = ehdr->e_phentsize;
	search_vec(auxv, &ldso_page_size, AT_PAGESZ);

	Phdr *phdr = NULL;
	for (int n = 0; n < ldso->phnum; n++) {
		phdr = &ldso->phdr[n];

		if (phdr->p_type == PT_DYNAMIC) {
			ldso->dynv = (size_t *)ldso_addr(ldso, phdr->p_vaddr);
		}
	}

	head = ldso;

	dl_relocate(ldso);
	relocate_test();

	_dl_s2(sp, auxv);
}
