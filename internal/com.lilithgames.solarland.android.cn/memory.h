#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <link.h>
#include <elf.h>

typedef struct {
    const char* name;
    uintptr_t   base;
    uintptr_t   end;
    size_t      size;
} ModuleData;

typedef struct {
    const char* name;
    ModuleData* out;
} FindModuleCbData;

typedef struct {
    const char* module;
    const char* symbol;
    uint32_t    hash;
    void*       result;
} FindSymbolCbData;

static void* MemCopy(void* dst, const void* src, size_t n)
{
    uint8_t*       d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    while (n--) *d++ = *s++;
    return dst;
}

static int StrCmp(const char* a, const char* b)
{
    while (*a && *a == *b) { ++a; ++b; }
    return (unsigned char)*a - (unsigned char)*b;
}

static const char* StrRChr(const char* s, char c)
{
    const char* last = NULL;
    for (; *s; ++s) if (*s == c) last = s;
    return last;
}

static const char* StrStr(const char* hay, const char* needle)
{
    if (!*needle) return hay;
    for (; *hay; ++hay) {
        const char* h = hay, *n = needle;
        while (*n && *h == *n) { ++h; ++n; }
        if (!*n) return hay;
    }
    return NULL;
}

static void StrCpy(char* dest, const char* src, size_t max_len) {
    size_t i = 0;
    while (src[i] && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static int Atoi(const char* s) {
    int res = 0;
    int sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') {
        res = res * 10 + (*s - '0');
        s++;
    }
    return res * sign;
}

static bool IsPageFault(uintptr_t addr)
{
    unsigned char vec   = 0;
    unsigned long start = addr & ~(unsigned long)(4096 - 1);
    return mincore((void*)start, 4096, &vec) == -1
        || (vec & 1) == 0;
}

static bool IsBadPtr(uintptr_t addr)
{
    return !addr
        || addr >= (uintptr_t)0x7FFFFFFFFFFF
        || IsPageFault(addr);
}

static bool IsBadPtr(void *addr)
{
    return IsBadPtr((uintptr_t)addr);
}

static int GetMemProtect(void* addr)
{
    int fd = open("/proc/self/maps", O_RDONLY);
    if (fd < 0) return -1;

    char      buf[512];
    char      line[256];
    int       line_len = 0;
    uintptr_t target   = (uintptr_t)addr;
    int       result   = -1;
    long      n;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (long i = 0; i < n; ++i) {
            char c = buf[i];
            if (c != '\n' && line_len < (int)sizeof(line) - 1) {
                line[line_len++] = c;
                continue;
            }
            line[line_len] = '\0';
            line_len = 0;

            uintptr_t start = 0, end = 0;
            int pos = 0;

            while (line[pos] && line[pos] != '-') {
                char h = line[pos++];
                start = (start << 4) | (h >= 'a' ? (uintptr_t)(h - 'a' + 10) : (uintptr_t)(h - '0'));
            }
            if (line[pos] == '-') pos++;
            while (line[pos] && line[pos] != ' ') {
                char h = line[pos++];
                end = (end << 4) | (h >= 'a' ? (uintptr_t)(h - 'a' + 10) : (uintptr_t)(h - '0'));
            }

            if (target < start || target >= end) continue;

            if (line[pos] == ' ') pos++;
            int prot = 0;
            if (line[pos]     == 'r') prot |= 0x1;
            if (line[pos + 1] == 'w') prot |= 0x2;
            if (line[pos + 2] == 'x') prot |= 0x4;
            result = prot;
            goto done;
        }
    }

done:
    close(fd);
    return result;
}

static bool ReadMemory(void* addr, void* buf, size_t size)
{
    uintptr_t start = (uintptr_t)addr;
    uintptr_t end   = start + size;

    for (uintptr_t p = start & ~(uintptr_t)(4096 - 1); p < end; p += 4096)
        if (IsPageFault(p)) return false;

    MemCopy(buf, addr, size);
    return true;
}

static bool WriteMemory(void* addr, const void* buf, size_t size)
{
    uintptr_t start = (uintptr_t)addr;
    uintptr_t end   = start + size;

    for (uintptr_t p = start & ~(uintptr_t)(4096 - 1); p < end; p += 4096)
        if (IsPageFault(p)) return false;

    MemCopy(addr, buf, size);
    __builtin___clear_cache((char*)addr, (char*)addr + size);
    return true;
}

static inline uint8_t ReadByte(uintptr_t addr)
{
    uint8_t v = 0;
    ReadMemory((void*)addr, &v, sizeof(v));
    return v;
}

static inline int32_t ReadInt(uintptr_t addr)
{
    int32_t v = 0;
    ReadMemory((void*)addr, &v, sizeof(v));
    return v;
}

static inline uint32_t ReadUInt(uintptr_t addr)
{
    uint32_t v = 0;
    ReadMemory((void*)addr, &v, sizeof(v));
    return v;
}

static inline int64_t ReadInt64(uintptr_t addr)
{
    int64_t v = 0;
    ReadMemory((void*)addr, &v, sizeof(v));
    return v;
}

static inline uint64_t ReadUInt64(uintptr_t addr)
{
    uint64_t v = 0;
    ReadMemory((void*)addr, &v, sizeof(v));
    return v;
}

static inline float ReadFloat(uintptr_t addr)
{
    float v = 0.f;
    ReadMemory((void*)addr, &v, sizeof(v));
    return v;
}

static inline double ReadDouble(uintptr_t addr)
{
    double v = 0.0;
    ReadMemory((void*)addr, &v, sizeof(v));
    return v;
}

static inline uintptr_t ReadPtr(uintptr_t addr)
{
    uintptr_t v = 0;
    ReadMemory((void*)addr, &v, sizeof(v));
    return v;
}

static inline bool WriteByte(uintptr_t addr, uint8_t v)
{
    return WriteMemory((void*)addr, &v, sizeof(v));
}

static inline bool WriteInt(uintptr_t addr, int32_t v)
{
    return WriteMemory((void*)addr, &v, sizeof(v));
}

static inline bool WriteUInt(uintptr_t addr, uint32_t v)
{
    return WriteMemory((void*)addr, &v, sizeof(v));
}

static inline bool WriteInt64(uintptr_t addr, int64_t v)
{
    return WriteMemory((void*)addr, &v, sizeof(v));
}

static inline bool WriteUInt64(uintptr_t addr, uint64_t v)
{
    return WriteMemory((void*)addr, &v, sizeof(v));
}

static inline bool WriteFloat(uintptr_t addr, float v)
{
    return WriteMemory((void*)addr, &v, sizeof(v));
}

static inline bool WriteDouble(uintptr_t addr, double v)
{
    return WriteMemory((void*)addr, &v, sizeof(v));
}

static inline bool WritePtr(uintptr_t addr, uintptr_t v)
{
    return WriteMemory((void*)addr, &v, sizeof(v));
}

static int FindModuleCb(struct dl_phdr_info* info, size_t size, void* data)
{
    FindModuleCbData* d = (FindModuleCbData*)data;
    if (!info->dlpi_name || !*info->dlpi_name) return 0;

    const char* slash   = StrRChr(info->dlpi_name, '/');
    const char* modname = slash ? slash + 1 : info->dlpi_name;
    if (StrCmp(d->name, modname) != 0 && !StrStr(info->dlpi_name, d->name)) return 0;

    uintptr_t lo = (uintptr_t)-1, hi = 0;
    for (int i = 0; i < info->dlpi_phnum; ++i) {
        if (info->dlpi_phdr[i].p_type != PT_LOAD) continue;
        uintptr_t a = info->dlpi_addr + info->dlpi_phdr[i].p_vaddr;
        uintptr_t b = a + info->dlpi_phdr[i].p_memsz;
        if (a < lo) lo = a;
        if (b > hi) hi = b;
    }
    if (hi > lo) { d->out->base = lo; d->out->end = hi; d->out->size = hi - lo; }
    return 1;
}

static void FindModule(const char* name, ModuleData* out)
{
    out->name = name;
    out->base = out->end = out->size = 0;
    FindModuleCbData d = { name, out };
    dl_iterate_phdr(FindModuleCb, &d);
}

static void* FindModuleBase(const char* name)
{
    ModuleData d;
    FindModule(name, &d);
    return (void*)d.base;
}

static void* FindModuleEnd(const char* name)
{
    ModuleData d;
    FindModule(name, &d);
    return (void*)d.end;
}

static size_t FindModuleSize(const char* name)
{
    ModuleData d;
    FindModule(name, &d);
    return d.size;
}

static int FindSymbolCb(struct dl_phdr_info* info, size_t size, void* data)
{
    FindSymbolCbData* d = (FindSymbolCbData*)data;
    if (!info->dlpi_name || !*info->dlpi_name) return 0;

    const char* slash   = StrRChr(info->dlpi_name, '/');
    const char* modname = slash ? slash + 1 : info->dlpi_name;
    if (StrCmp(d->module, modname) != 0 && !StrStr(info->dlpi_name, d->module)) return 0;

    uintptr_t  bias = info->dlpi_addr;
    ElfW(Dyn)* dyn  = NULL;
    for (int i = 0; i < info->dlpi_phnum; ++i)
        if (info->dlpi_phdr[i].p_type == PT_DYNAMIC)
            { dyn = (ElfW(Dyn)*)(bias + info->dlpi_phdr[i].p_vaddr); break; }
    if (!dyn) return 0;

    ElfW(Sym)*  symtab      = NULL;
    const char* strtab      = NULL;
    uint32_t*   hashtab     = NULL;
    uint32_t*   gnu_hashtab = NULL;

    for (ElfW(Dyn)* e = dyn; e->d_tag != DT_NULL; ++e) {
        switch (e->d_tag) {
            case DT_SYMTAB:   symtab      = (ElfW(Sym)*)e->d_un.d_ptr;  break;
            case DT_STRTAB:   strtab      = (const char*)e->d_un.d_ptr; break;
            case DT_HASH:     hashtab     = (uint32_t*)e->d_un.d_ptr;   break;
            case DT_GNU_HASH: gnu_hashtab = (uint32_t*)e->d_un.d_ptr;   break;
            default: break;
        }
    }
    if (!symtab || !strtab) return 0;

    uint32_t h = d->hash;

    if (hashtab) {
        uint32_t nchain = hashtab[1];
        for (uint32_t i = 0; i < nchain; ++i) {
            if (!symtab[i].st_name || !symtab[i].st_value) continue;
            if (StrCmp(&strtab[symtab[i].st_name], d->symbol) == 0)
                { d->result = (void*)(bias + symtab[i].st_value); return 1; }
        }
        return 0;
    }

    if (gnu_hashtab) {
        uint32_t nbuckets    = gnu_hashtab[0];
        uint32_t symoffset   = gnu_hashtab[1];
        uint32_t bloom_size  = gnu_hashtab[2];
        uint32_t bloom_shift = gnu_hashtab[3];

        const ElfW(Addr)* bloom   = (const ElfW(Addr)*)(gnu_hashtab + 4);
        const uint32_t*   buckets = (const uint32_t*)(bloom + bloom_size);
        const uint32_t*   chains  = buckets + nbuckets;

        ElfW(Addr) bword = bloom[(h / (8 * sizeof(ElfW(Addr)))) % bloom_size];
        ElfW(Addr) bmask = ((ElfW(Addr))1 << ( h                % (8 * sizeof(ElfW(Addr))))) |
                           ((ElfW(Addr))1 << ((h >> bloom_shift) % (8 * sizeof(ElfW(Addr)))));
        if ((bword & bmask) != bmask) return 0;

        uint32_t sym = buckets[h % nbuckets];
        if (!sym) return 0;

        for (const uint32_t* c = chains + (sym - symoffset); ; ++sym, ++c) {
            if (((*c ^ h) >> 1) == 0 && symtab[sym].st_value &&
                StrCmp(&strtab[symtab[sym].st_name], d->symbol) == 0)
                { d->result = (void*)(bias + symtab[sym].st_value); return 1; }
            if (*c & 1) break;
        }
    }
    return 0;
}

static void* FindSymbolAddress(const char* module, const char* symbol)
{
    uint32_t h = 5381;
    for (const char* n = symbol; *n; ++n) h = h * 33 + (unsigned char)*n;
    FindSymbolCbData d = { module, symbol, h, NULL };
    dl_iterate_phdr(FindSymbolCb, &d);
    return d.result;
}

static uint32_t A64Bits(uint32_t insn, int hi, int lo)
{
    return (insn >> lo) & ((1u << (hi - lo + 1)) - 1);
}

static int64_t A64Sext(uint64_t val, int bits)
{
    return (int64_t)(val << (64 - bits)) >> (64 - bits);
}

static uint8_t* A64EmitU32(uint8_t* p, uint32_t insn)
{
    MemCopy(p, &insn, 4);
    return p + 4;
}

static uint8_t* A64EmitMov64(uint8_t* p, int rd, uintptr_t val)
{
    p = A64EmitU32(p, 0xD2800000u | ((val         & 0xFFFF) << 5) | (uint32_t)rd);
    p = A64EmitU32(p, 0xF2A00000u | (((val >> 16) & 0xFFFF) << 5) | (uint32_t)rd);
    p = A64EmitU32(p, 0xF2C00000u | (((val >> 32) & 0xFFFF) << 5) | (uint32_t)rd);
    p = A64EmitU32(p, 0xF2E00000u | (((val >> 48) & 0xFFFF) << 5) | (uint32_t)rd);
    return p;
}

static uint8_t* A64EmitAbsJump(uint8_t* p, uintptr_t addr)
{
    p = A64EmitU32(p, 0x58000051u); /* LDR X17, #8 */
    p = A64EmitU32(p, 0xD61F0220u); /* BR  X17     */
    MemCopy(p, &addr, 8);
    return p + 8;
}

static uint8_t* A64Relocate(uint8_t* p, uint32_t insn, uintptr_t orig_pc)
{
    if ((insn & 0x9F000000u) == 0x10000000u) {
        int     rd  = (int)(insn & 0x1Fu);
        int64_t imm = A64Sext((A64Bits(insn, 23, 5) << 2) | A64Bits(insn, 30, 29), 21);
        return A64EmitMov64(p, rd, (uintptr_t)((int64_t)orig_pc + imm));
    }
    if ((insn & 0x9F000000u) == 0x90000000u) {
        int       rd  = (int)(insn & 0x1Fu);
        int64_t   imm = A64Sext((A64Bits(insn, 23, 5) << 2) | A64Bits(insn, 30, 29), 21);
        uintptr_t tgt = (orig_pc & ~(uintptr_t)0xFFFu) + (uintptr_t)(imm << 12);
        return A64EmitMov64(p, rd, tgt);
    }
    if ((insn & 0xFC000000u) == 0x14000000u) {
        int64_t imm = A64Sext(A64Bits(insn, 25, 0), 26) << 2;
        return A64EmitAbsJump(p, (uintptr_t)((int64_t)orig_pc + imm));
    }
    if ((insn & 0xFC000000u) == 0x94000000u) {
        int64_t imm = A64Sext(A64Bits(insn, 25, 0), 26) << 2;
        p = A64EmitMov64(p, 30, orig_pc + 4);
        return A64EmitAbsJump(p, (uintptr_t)((int64_t)orig_pc + imm));
    }
    if ((insn & 0xFF000010u) == 0x54000000u) {
        uint32_t inv = (insn & 0xFu) ^ 1u;
        int64_t  imm = A64Sext(A64Bits(insn, 23, 5), 19) << 2;
        p = A64EmitU32(p, 0x54000000u | (5u << 5) | inv);
        return A64EmitAbsJump(p, (uintptr_t)((int64_t)orig_pc + imm));
    }
    if ((insn & 0x7F000000u) == 0x34000000u) {
        uint32_t sf  = insn >> 31;
        uint32_t rt  = insn & 0x1Fu;
        int64_t  imm = A64Sext(A64Bits(insn, 23, 5), 19) << 2;
        p = A64EmitU32(p, (sf << 31) | 0x35000000u | (5u << 5) | rt);
        return A64EmitAbsJump(p, (uintptr_t)((int64_t)orig_pc + imm));
    }
    if ((insn & 0x7F000000u) == 0x35000000u) {
        uint32_t sf  = insn >> 31;
        uint32_t rt  = insn & 0x1Fu;
        int64_t  imm = A64Sext(A64Bits(insn, 23, 5), 19) << 2;
        p = A64EmitU32(p, (sf << 31) | 0x34000000u | (5u << 5) | rt);
        return A64EmitAbsJump(p, (uintptr_t)((int64_t)orig_pc + imm));
    }
    if ((insn & 0x7F000000u) == 0x36000000u) {
        uint32_t b5  = insn >> 31;
        uint32_t b40 = A64Bits(insn, 23, 19);
        uint32_t rt  = insn & 0x1Fu;
        int64_t  imm = A64Sext(A64Bits(insn, 18, 5), 14) << 2;
        p = A64EmitU32(p, (b5 << 31) | 0x37000000u | (b40 << 19) | (5u << 5) | rt);
        return A64EmitAbsJump(p, (uintptr_t)((int64_t)orig_pc + imm));
    }
    if ((insn & 0x7F000000u) == 0x37000000u) {
        uint32_t b5  = insn >> 31;
        uint32_t b40 = A64Bits(insn, 23, 19);
        uint32_t rt  = insn & 0x1Fu;
        int64_t  imm = A64Sext(A64Bits(insn, 18, 5), 14) << 2;
        p = A64EmitU32(p, (b5 << 31) | 0x36000000u | (b40 << 19) | (5u << 5) | rt);
        return A64EmitAbsJump(p, (uintptr_t)((int64_t)orig_pc + imm));
    }
    if ((insn & 0x3B000000u) == 0x18000000u) {
        uint32_t  opc = insn >> 30;
        uint32_t  rt  = insn & 0x1Fu;
        int64_t   imm = A64Sext(A64Bits(insn, 23, 5), 19) << 2;
        uintptr_t tgt = (uintptr_t)((int64_t)orig_pc + imm);
        p = A64EmitMov64(p, 17, tgt);
        if      (opc == 0u) p = A64EmitU32(p, 0xB9400000u | (17u << 5) | rt); /* LDR  Wt, [X17] */
        else if (opc == 1u) p = A64EmitU32(p, 0xF9400000u | (17u << 5) | rt); /* LDR  Xt, [X17] */
        else if (opc == 2u) p = A64EmitU32(p, 0xB9800000u | (17u << 5) | rt); /* LDRSW Xt,[X17] */
        else                p = A64EmitU32(p, 0xD503201Fu);                    /* NOP            */
        return p;
    }
    return A64EmitU32(p, insn);
}

static bool JumpHook(void* target, void* replace, void** orig)
{
    uint8_t* t = (uint8_t*)mmap(NULL, 256, PROT_READ | PROT_WRITE | PROT_EXEC,
                                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if ((long)t < 0) return false;

    MemCopy(t, target, 16);

    uint8_t*  tp     = t + 16;
    uintptr_t src_pc = (uintptr_t)target;
    for (int i = 0; i < 4; ++i) {
        uint32_t insn;
        MemCopy(&insn, (uint8_t*)target + i * 4, 4);
        tp = A64Relocate(tp, insn, src_pc + (uintptr_t)(i * 4));
    }
    A64EmitAbsJump(tp, src_pc + 16);
    __builtin___clear_cache((char*)t, (char*)t + 256);

    uint8_t stub[16] = {
        0x51, 0x00, 0x00, 0x58,  /* LDR X17, #8 */
        0x20, 0x02, 0x1F, 0xD6,  /* BR  X17     */
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };
    uintptr_t replace_addr = (uintptr_t)replace;
    MemCopy(stub + 8, &replace_addr, 8);

    uintptr_t aligned = (uintptr_t)target & ~(uintptr_t)(4096 - 1);
    if (mprotect((void*)aligned, 4096 * 2, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        munmap(t, 256);
        return false;
    }

    MemCopy(target, stub, 16);
    __builtin___clear_cache((char*)target, (char*)target + 16);

    if (orig) *orig = t + 16;
    return true;
}

static bool JumpUnhook(void* target, void* orig)
{
    uint8_t*  raw     = (uint8_t*)orig - 16;
    uintptr_t aligned = (uintptr_t)target & ~(uintptr_t)(4096 - 1);
    if (mprotect((void*)aligned, 4096 * 2, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
        return false;

    MemCopy(target, raw, 16);
    __builtin___clear_cache((char*)target, (char*)target + 16);
    munmap(raw, 256);
    return true;
}

static bool JumpVirtualHook(void** vtable, int offset, void* replace, void** backup)
{
    void**    slot    = &vtable[offset];
    uintptr_t aligned = (uintptr_t)slot & ~(uintptr_t)(4096 - 1);
    if (mprotect((void*)aligned, 4096 * 2, PROT_READ | PROT_WRITE) != 0)
        return false;

    if (backup) *backup = vtable[offset];
    vtable[offset] = replace;
    return true;
}

static bool JumpVirtualUnhook(void** vtable, int offset, void* orig)
{
    void**    slot    = &vtable[offset];
    uintptr_t aligned = (uintptr_t)slot & ~(uintptr_t)(4096 - 1);
    if (mprotect((void*)aligned, 4096 * 2, PROT_READ | PROT_WRITE) != 0)
        return false;

    vtable[offset] = orig;
    return true;
}

#endif /* MEMORY_H */