/* Copyright (c) 2013, Ben Noordhuis <info@bnoordhuis.nl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef UV_ATOMIC_OPS_H_
#define UV_ATOMIC_OPS_H_

#include "internal.h"  
#include <atomic.h>

static int cmpxchgi(int* ptr, int oldval, int newval);
static long cmpxchgl(long* ptr, long oldval, long newval);
static void cpu_relax(void);

/* Prefer hand-rolled assembly over the gcc builtins because the latter also
 * issue full memory barriers.
 */
static int cmpxchgi(int* ptr, int oldval, int newval)
{
/**
#if defined(__i386__) || defined(__x86_64__)
  int out;
  __asm__ __volatile__ ("lock; cmpxchg %2, %1;"
                        : "=a" (out), "+m" (*(volatile int*) ptr)
                        : "r" (newval), "0" (oldval)
                        : "memory");
  return out;
#else
  return __sync_val_compare_and_swap(ptr, oldval, newval);
#endif
*/
    return atomic_compare_exchange32((uint32_t volatile * )ptr , oldval , newval) ;
}

static long cmpxchgl(long* ptr, long oldval, long newval)
{
/**
#if defined(__i386__) || defined(__x86_64__)
  long out;
  __asm__ __volatile__ ("lock; cmpxchg %2, %1;"
                        : "=a" (out), "+m" (*(volatile long*) ptr)
                        : "r" (newval), "0" (oldval)
                        : "memory");
  return out;
#else
  return __sync_val_compare_and_swap(ptr, oldval, newval);
#endif
*/
    if(sizeof(long) == 4)
        return (long)atomic_compare_exchange32((uint32_t volatile * )ptr , oldval , newval) ;
    else
        return (long)atomic_compare_exchange64((uint64_t volatile * )ptr , oldval , newval) ;
}

static void cpu_relax(void)
{
/**
#if defined(__i386__) || defined(__x86_64__)
  __asm__ __volatile__ ("rep; nop"); 
#endif
*/
    __asm{rep nop}
}

#endif  /* UV_ATOMIC_OPS_H_ */
