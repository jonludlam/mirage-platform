/******************************************************************************
 * kernel.c
 * 
 * Assorted crap goes here, including the initial C entry point, jumped at
 * from head.S.
 * 
 * Copyright (c) 2002-2003, K A Fraser & R Neugebauer
 * Copyright (c) 2005, Grzegorz Milos, Intel Research Cambridge
 * Copyright (c) 2006, Robert Kaiser, FH Wiesbaden
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 */

#include <mini-os/x86/os.h>
#include <mini-os/hypervisor.h>
#include <mini-os/mm.h>
#include <mini-os/events.h>
#include <mini-os/time.h>
#include <mini-os/types.h>
#include <mini-os/lib.h>
#include <mini-os/sched.h>
#include <mini-os/xenbus.h>
#include <mini-os/xmalloc.h>
#include <fcntl.h>
#include <xen/features.h>
#include <xen/version.h>
#include <log.h>

static void cpuid(uint32_t idx, uint32_t *regs)
{
    asm volatile (
#ifdef __i386__
#define R(x) "%%e"#x"x"
#else
#define R(x) "%%r"#x"x"
#endif
        "push "R(a)"; push "R(b)"; push "R(c)"; push "R(d)"\n\t"
        "test %1,%1 ; jz 1f ; ud2a ; .ascii \"xen\" ; 1: cpuid\n\t"
        "mov %%eax,(%2); mov %%ebx,4(%2)\n\t"
        "mov %%ecx,8(%2); mov %%edx,12(%2)\n\t"
        "pop "R(d)"; pop "R(c)"; pop "R(b)"; pop "R(a)"\n\t"
        : : "a" (idx), "c" (0), "S" (regs) : "memory" );
}


#define WRMSR(msr, val1, val2) \
     __asm __volatile__(\
        "WRMSR"\
        : \
        : "c" (msr), "a" (val1), "d" (val2));

char *hypercall_page;

/* based on xen-unstable/tools/misc/xen-detect.c */
void look_for_xen(){
    uint32_t base, pages, msr, regs[4];
    uint64_t page;
    int version, i;
    char signature[13];
    printk("Looking for Xen hypervisor\n");

    for ( base = 0x40000000; base < 0x40010000; base += 0x100 )
    {
        cpuid(base, regs);

        *(uint32_t *)(signature + 0) = regs[1];
        *(uint32_t *)(signature + 4) = regs[2];
        *(uint32_t *)(signature + 8) = regs[3];
        signature[12] = '\0';

        if ( !strcmp("XenVMMXenVMM", signature) && (regs[0] >= (base + 2)) )
        {
            printk("Detected Xen hypervisor (CPUID = %x)\n", base);
            goto found;
        }
    }
    printk("Xen hypervisor not detected\n");
    return;
found:
    cpuid(base + 1, regs);
    version = regs[0];
    printk("Xen version: %d.%d\n", (uint16_t)(version >> 16), (uint16_t)version);
    cpuid(base + 2, regs);
    pages = regs[0];
    msr = regs[1];
    printk("Number of hypercall pages: %d; MSR = %x\n", pages, msr);

    /* Use the page before the code section for hypercalls */
    hypercall_page = (char*) (0x100000L - 4096L * pages);
    page = (uint64_t) hypercall_page;
    for (i=0; i<pages; i++)
    {
        printk("Allocating memory at %x for hypercalls\n", page);
        WRMSR(msr, (uint32_t) page, (uint32_t) (page >> 32));
        page += 4096;
    };
    printk("Mapped %d hypercall pages at %x\n", pages, hypercall_page);

}

