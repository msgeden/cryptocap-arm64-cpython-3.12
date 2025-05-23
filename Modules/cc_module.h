#ifndef CC_MODULE_H
#define CC_MODULE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#ifdef TARGET_CRYPTO_CAP
typedef enum capPermFlags {
    READ = 1,
    WRITE = 2,
    EXEC = 4,
    TRANS = 8,
} capPermFlagsType;

typedef struct cc_dcap {
    uint64_t perms_base;
    uint32_t offset;
    uint32_t size;
    uint64_t PT;
    uint64_t MAC;
} cc_dcap;

typedef struct ipc_cap_msg{
    uint32_t magic; 
    pid_t pid;       // PID of the writer
    cc_dcap cap;     // Capability describing the data
} ipc_cap_msg;
//#endif	 

// Execute custom instruction (example: 'myinst r0, r1')
static inline void execute_cc_nop(int arg1, int arg2) {
    __asm__ volatile (
        "nop"
    );
}

#if defined(__aarch64__) || defined(__ARM_ARCH_8A__)

static inline void execute_cc_print_cap(cc_dcap cap) {
    uint64_t perms = (cap.perms_base & 0xFFFF000000000000) >> 48;
    uint64_t base = cap.perms_base & 0x0000FFFFFFFFFFFF;
    printf("cap.perms=0x%lx, .base=0x%lx, .offset=%d, .size=%d, .PT=0x%lx, .MAC=0x%lx\n",
           perms, base, cap.offset, cap.size, cap.PT, cap.MAC);
}
static inline void execute_cc_store_cap_from_CR0(cc_dcap* cap) {
    __asm__ volatile (
         "mov x9, %0\n\t"
         ".word 0x02100009\n\t"      // stc cr0, [x9]
         :   
         : "r" (cap)
         : "x9"
    );
}
static inline cc_dcap execute_cc_create_cap(const void* base, size_t offset, size_t size, bool write_flag) {
    cc_dcap cap;
    uint64_t perms = (write_flag) ? WRITE + READ : READ;
    perms = (perms << 48);
    uint64_t perms_base = perms | (uint64_t)base;
    uint64_t offset_size = (((uint64_t)size) << 32) | (uint64_t)offset;
    __asm__ volatile (
        "mov x9, %0\n\t" // mov perms_base to x9
        "mov x10, %1\n\t" // mov offset+size to x10
        ".word 0x0340012a\n\t"  // ccreate cr0, x9, x10  
        : 
        : "r" (perms_base), "r" (offset_size)
        : "x9", "x10"
    );
    execute_cc_store_cap_from_CR0(&cap);
    return cap;
}    
static inline void execute_cc_load_ver_cap_to_CR0(cc_dcap* cap) {
    __asm__ volatile (
         "mov x9, %0\n\t"
         ".word 0x02000009\n\t"      // ldc cr0, [x9]
         :   
         : "r" (cap)
         : "x9"
    );
}
static inline uint8_t* execute_cc_memcpy(void* dst, cc_dcap src, size_t count) {
    if (count == 0)
        return (uint8_t*)dst;
    
    execute_cc_load_ver_cap_to_CR0(&src);

    __asm__ volatile(
        "mov x10, %[dst_ptr]\n\t"
        "mov x11, %[cnt]\n\t"
        "mov x12, #0\n\t"
        "1:\n\t"      
        ".word 0x02200d20\n\t"       // cldg8 x9, [cr0]
        ".word 0x03c00001\n\t"       // cincoffset cr0, #1
        "strb w9, [x10]\n\t"
        "add x10, x10, #1\n\t"
        "add x12, x12, #1\n\t"
        "cmp x12, x11\n\t"
        "b.lt 1b\n\t"
        : 
        : [dst_ptr] "r" (dst), [cnt] "r" (count)
        : "x9", "x10", "x11", "x12", "cc", "memory"
    ); 

    return (uint8_t*)dst;
}
#else
static inline void execute_cc_print_cap(cc_dcap cap) {
    fprintf(stderr, "ARM64 instructions not supported on this platform\n");
    abort();
}
static inline void execute_cc_store_cap_from_CR0(cc_dcap cap) {
    fprintf(stderr, "ARM64 instructions not supported on this platform\n");
    abort();
}
static inline cc_dcap execute_cc_create_cap(const void* base, size_t offset, size_t size, bool write_flag) {
    fprintf(stderr, "ARM64 instructions not supported on this platform\n");
    abort();
}
static inline void execute_cc_load_ver_cap_to_CR0(cc_dcap* cap) {
    fprintf(stderr, "ARM64 instructions not supported on this platform\n");
    abort();
}
static inline uint8_t* execute_cc_memcpy(void* dst, cc_dcap src, size_t count) {
    fprintf(stderr, "ARM64 instructions not supported on this platform\n");
    abort();
}
#endif

#endif