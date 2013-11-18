#include <mini-os/x86/os.h>
#include <mini-os/x86/arch_limits.h>
#include <xen/features.h>

.globl _start_hvm
	
_start_hvm:
	cld
	movq stack_start(%rip),%rsp
        andq $(~(__STACK_SIZE-1)), %rsp
        movq %rsi,%rdi
        call start_kernel_hvm

