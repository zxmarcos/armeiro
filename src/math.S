/* arm eabi functions
 * Marcos Medeiros
 */

#include "asm/asm.h"

.section .text
/* __sys_uidiv:
 *   unsigned r = 0;
 *   unsigned q = 0;
 *   for (int i = 31; i >= 0; i--) {
 *       r <<= 1;
 *       r |= ((num >> i) & 1);
 *       if (r >= den) {
 *           r -= den;
 *           q |= (1 << i);
 *       }
 *	}
 *	return q;
 */
ENTRY(__aeabi_uidivmod)
	push	{lr}
	bl		__aeabi_uidiv
	mov 	r0, r1
	pop		{pc}

ENTRY(__aeabi_idivmod)
	push	{lr}
	bl		__aeabi_idiv
	mov 	r0, r1
	pop		{pc}

ENTRY(__aeabi_uidiv)
	push	{r4 - r6}
num	.req	r0 	@ numerador
den	.req	r1 	@ denominador
Q	.req	r3	@ quociente
R	.req	r4	@ resto
i 	.req	r5	@ nosso contralador do loop
S 	.req	r6	@ scratch
	mov 	R, #0
	mov 	Q, #0
	mov		i, #31
1:
	@  r <<= 1
	lsl		R, #1
	@ r |= ((num >> i) & 1);			
	mov		S, num, lsr i 		
	and		S, #1
	orr		R, S
	@ if (r >= den) {
	cmp		R, den
	@ r -= den;
	subge	R, den
	@ q |= (1 << i);
	movge	S, #1
	orrge	Q, S, lsl i
	@ decrementa i
	sub		i, #1
	cmp		i, #0
	@ se for maior ou igual a zero continua o loop
	bge		1b
	mov 	r0, Q
	mov 	r1, R
	pop		{r4 - r6}
	mov		pc, lr
.unreq	Q
.unreq	R
.unreq	S
.unreq	i
.unreq	num
.unreq	den

/* __sys_idiv:
 * This is C code for this functions, a little bit modified from uidiv
 *	int sign = (num & 0x80000000) ^ (den & 0x80000000);
 * 
 *	if (num < 0)
 *       num *= -1;
 *	if (den < 0)
 *      den *= -1;
 *
 *	int r = 0;
 *  int q = 0;
 *  for (int i = 31; i >= 0; i--) {
 *      r <<= 1;
 *      r |= ((num >> i) & 1);
 *      if (r >= den) {
 *          r -= den;
 *          q |= (1 << i);
 *      }
 *	}
 *
 *	if (sign)
 *      q = -q;
 *	return q;
 */
 
ENTRY(__aeabi_idiv)
	push	{r4 - r7}
num	.req	r0 	@ numerador
den	.req	r1 	@ denominador
Q	.req	r3	@ quociente
R	.req	r4	@ resto
i 	.req	r5	@ nosso contralador do loop
S 	.req	r6	@ scratch
Sn 	.req	r7	@ sinal
	@ calcula o sinal:
	@ +- = -
	@ ++ = +
	@ -- = +
	@ int sign = (num & 0x80000000) ^ (den & 0x80000000);
	ldr 	S, =0x80000000
	mov 	i, num
	and		i, S
	mov 	Sn, i
	mov 	i, den
	and		i, S
	eor		Sn, i
	@ coloca o resultado no primeiro bit
	lsr		Sn, #31

	@	if (num < 0)
	@       num *= -1;
	@	if (den < 0)
	@      den *= -1;
	ldr		S, =-1
	cmp		num, #0
	mullt	num, S
	cmp		den, #0
	mullt	den, S

	mov 	R, #0
	mov 	Q, #0
	mov		i, #31
1:
	@  r <<= 1
	lsl		R, #1
	@ r |= ((num >> i) & 1);			
	mov		S, num, lsr i 		
	and		S, #1
	orr		R, S
	@ if (r >= den) {
	cmp		R, den
	@ r -= den;
	subge	R, den
	@ q |= (1 << i);
	movge	S, #1
	orrge	Q, S, lsl i
	@ decrementa i
	sub		i, #1
	cmp		i, #0
	@ se for maior ou igual a zero continua o loop
	bge		1b

	@	if (sign)
	@      q = -q;
	cmp		Sn, #1
	ldreq	S, =-1
	muleq	Q, S

	mov 	r0, Q
	mov 	r1, R
	pop		{r4 - r7}
	mov		pc, lr
.unreq	Q
.unreq	R
.unreq	S
.unreq	Sn
.unreq	i
.unreq	num
.unreq	den
