/* 
 ****************************************************************************
 * (C) 2006 - Grzegorz Milos - Cambridge University
 ****************************************************************************
 *
 *        File: console.h
 *      Author: Grzegorz Milos
 *     Changes: 
 *              
 *        Date: Mar 2006
 * 
 * Environment: Xen Minimal OS
 * Description: Console interface.
 *
 * Handles console I/O. Defines printk.
 *
 ****************************************************************************
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
 
#include <mini-os/types.h>
#include <mini-os/wait.h>
#include <mini-os/mm.h>
#include <mini-os/hypervisor.h>
#include <mini-os/events.h>
#include <mini-os/x86/os.h>
#include <mini-os/lib.h>
#include <mini-os/xenbus.h>
#include <xen/io/console.h>


void console_print(const char *data, unsigned int length)
{
//    (void)HYPERVISOR_console_io(CONSOLEIO_write, length, data);
    xencons_ring_send(NULL, data, length);
}

void output_xen(const char *buf, int len)
{
    xencons_ring_send(NULL, buf, len);
}

static char *vidmem = (const char *) 0xb8000;
static int next_line = 0;
static int next_col = 0;

static void framebuffer_scroll()
{
    memmove(vidmem, vidmem + 80 * 2, 24 * 80 * 2);
}

static void output_framebuffer(const char *buf, int len)
{
    int i;
    while (*buf)
    {
        if (*buf == '\n') {
            next_line ++;
            next_col = 0;
            break;
        }
        if (next_col == 80) {
            next_line ++;
            next_col = 0;
        }
        if (next_line == 25) {
            framebuffer_scroll ();
            next_line = 24;
        }
        i = (next_line * 80 + next_col) * 2;
        vidmem[i]= *buf;
        *buf ++;
        vidmem[i+1]= 0x7;
        next_col ++;
    };
}

void print(const char *fmt, va_list args)
{
    static char   buf[1024];
    
    (void)vsnprintf(buf, sizeof(buf), fmt, args);
//    (void)HYPERVISOR_console_io(CONSOLEIO_write, strlen(buf), buf);

//    output_xen(buf, strlen(buf));
    output_framebuffer(buf, strlen(buf));
}

void printk(const char *fmt, ...)
{
    va_list       args;
    va_start(args, fmt);
    print(fmt, args);
    va_end(args);        
}

void xprintk(const char *fmt, ...)
{
    va_list       args;
    va_start(args, fmt);
    print(fmt, args);
    va_end(args);        
}
