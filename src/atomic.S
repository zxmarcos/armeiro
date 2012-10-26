/* arm atomic operations
 * Marcos Medeiros
 */

#include <asm/asm.h>

/* u32 atomic_xchg(lock_t *ptr, u32 val) */
ENTRY(atomic_xchg)
	swp		r2, r1, [r0]
	mov		r0, r2
	mov		pc, lr

/* u32 atomic_inc(lock_t *ptr) */
ENTRY(atomic_inc)
	swp		r1, r1, [r0]
	add		r1, #1
	swp		r1, r1, [r0]
	mov		r0, r1
	mov		pc, lr

/* u32 atomic_dec(lock_t *ptr) */
ENTRY(atomic_dec)
	swp		r1, r1, [r0]
	sub		r1, #1
	swp		r1, r1, [r0]
	mov		r0, r1
	mov		pc, lr