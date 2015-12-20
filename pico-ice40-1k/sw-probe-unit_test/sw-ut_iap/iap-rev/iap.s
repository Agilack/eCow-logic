    .syntax unified
    .arch armv6-m

    .text
    .thumb
    .thumb_func
    .align 2
    .globl    IAP_Handler
    .type    IAP_Handler, %function
IAP_Handler:
    push	{r3, r4, r5, lr}
    subs	r4, r0, #0      /* Copy first param (id) to r4 */
.hnd_cmd1: /* ERAS_DAT0 ? */
    cmp		r4, #16
    bne.n	hnd_cmd2
    ldr		r0, [pc, #76] 	/* (0x1fff1058) */
    bl		fct_ERAS
    b.n		hnd_ret

hnd_cmd2:  /* ERAS_DAT1 ? */
    cmp		r0, #17
    bne.n	hnd_cmd3
    ldr		r0, [pc, #68]	/* (0x1fff105c) */
    bl		fct_ERAS
    b.n		hnd_ret

hnd_cmd3: /* ERAS_SECT ? */
    cmp		r0, #18
    bne.n	hnd_cmd4        /* 0x1fff1028 */
    adds	r0, r1, #0
    bl		fct_ERAS
    b.n		hnd_ret

hnd_cmd4:
    cmp	r0, #19
    bne.n	hnd_cmd5        /* 0x1fff1034 */
    adds	r0, r1, #0
    bl	fct_ERAS_BLCK
    b.n	hnd_ret
hnd_cmd5:
    adds	r5, r0, #0
    subs	r5, #20
    cmp		r5, #1
    bhi.n	hnd_cmd_prog    /* 0x1fff1044 */
    uxtb	r0, r0
    bl		0x1fff1100
    b.n		hnd_ret
hnd_cmd_prog:
    movs	r0, #1
    cmp		r4, #34	/* 0x22 */
    bne.n	hnd_ret         /* 0x1fff1054 */
    adds	r0, r1, #0
    adds	r1, r2, #0
    adds	r2, r3, #0
    bl		0x1fff1164
hnd_ret:
    pop		{r3, r4, r5, pc}
    nop

hnd_const_0: .word 0xfe001003
hnd_const_1: .word 0xff001003

fct_ERAS:
	/* 1) send keys */
	ldr	r3, [pc, #60]	/* Load Flash controller base address  */
	ldr	r2, [pc, #64]	/* Load Key0 value (0x52537175)        */
	str	r2, [r3, #48]	/* Write it to KEYR0                   */
	ldr	r2, [pc, #64]	/* Load Key1 value (0xa91875fc)        */
      	str	r2, [r3, #52]	/* Write it to KEYR1                   */
        /* 2) write something to FACCR */
      	ldr	r1, [r3, #0]    /* Read FACCR                    */
      	movs	r2, #192	/* r2=0xC0                       */
      	lsls	r2, r2, #10     /* shift left r2, now 0x00030000 */
      	orrs	r2, r1          /* set bits                      */
      	str	r2, [r3, #0]    /* Update FACCR                  */
        /* 3) Set Address       */
      	str	r0, [r3, #4]    /* Write FADDR */
        /* 4) Start operation   */
      	movs	r2, #128	/* r2=0x80               */
      	lsls	r2, r2, #20     /* shift, now 0x08000000 */
      	str	r2, [r3, #12]   /* Write FCTRLR          */
      	adds	r2, r3, #0
fct_ERAS_wait:
      	ldr	r3, [r2, #16]   /* Read FSTATR  */
	cmp	r3, #0
      	bge.n	fct_ERAS_wait   /* Wait until command complete ? */
        /* 5) send kays again   */
      	ldr	r3, [pc, #24]	/* Read (again) Flash controller base addr */
      	ldr	r2, [r3, #16]   /* Read FSTATR                         */
      	ldr	r1, [pc, #24]	/* Load Key0 value (0x52537175)        */
      	str	r1, [r3, #48]	/* Write it to KEYR0                   */
      	ldr	r1, [pc, #24]	/* Load Key1 value (0xa91875fc)        */
      	str	r1, [r3, #52]	/* Write it to KEYR1                   */
        /* 6) restore FACCR value */
      	ldr	r0, [r3, #0]    /* Read FACCR   */
      	ldr	r1, [pc, #24]	/* Load mask to reset bits of 2)       */
      	ands	r1, r0          /* Binary and to reset bits            */
      	str	r1, [r3, #0]    /* Update FACCR */
        /* 7) that's all :)     */
      	movs	r0, #3          /* Return 2-lsb of status */
      	ands	r0, r2
      	bx	lr
      	nop

fct_ERAS_FACCR: .word 0x50004100 /* 0x41005000 : FACCR */
fct_ERAS_KEY0:  .word 0x71755253 /* Key 0 */
fct_ERAS_KEY1:  .word 0x75fca918 /* Key 1 */
fct_ERAS_var4:  .word 0xfffffffc /* Mask 0xfffcffff */

fct_ERAS_BLCK: /* @ 0x1fff10b0 */
    ldr		r3, [pc, #60]	/* (0x1fff10f0) */
    ldr		r2, [pc, #64]	/* (0x1fff10f4) */
    str		r2, [r3, #48]	/* 0x30 */
    ldr		r2, [pc, #64]	/* (0x1fff10f8) */
    str		r2, [r3, #52]	/* 0x34 */
    ldr		r1, [r3, #0]
    movs	r2, #192	/* 0xc0 */
    lsls	r2, r2, #10
    orrs	r2, r1
    str		r2, [r3, #0]
    str		r0, [r3, #4]
    movs	r2, #128	/* 0x80 */
    lsls	r2, r2, #21
    str		r2, [r3, #12]
    adds	r2, r3, #0
    ldr		r3, [r2, #16]
    cmp		r3, #0
    bge.n	0x1fff10ce
    ldr		r3, [pc, #24]	/* (0x1fff10f0) */
    ldr		r2, [r3, #16]
    ldr		r1, [pc, #24]	/* (0x1fff10f4) */
    str		r1, [r3, #48]	/* 0x30 */
    ldr		r1, [pc, #24]	/* (0x1fff10f8) */
    str		r1, [r3, #52]	/* 0x34 */
    ldr		r0, [r3, #0]
    ldr		r1, [pc, #24]	/* (0x1fff10fc) */
    ands	r1, r0
    str		r1, [r3, #0]
    movs	r0, #3
    ands	r0, r2
    bx		lr
    nop

fct_ERAS_BLCK_FACCR: .word 0x50004100 /* 0x41005000 : FACCR */
fct_ERAS_BLCK_KEY0:  .word 0x71755253 /* Key 0 */
fct_ERAS_BLCK_KEY1:  .word 0x75fca918 /* Key 1 */
fct_ERAS_BLCK_mask:  .word 0xfffffffc /* Mask 0xfffcffff */

some_fct_1: /* @ 0x1fff1100 */
    adds	r2, r0, #0
    ldr	r3, [pc, #80]	/* (0x1fff1154) */
    ldr	r1, [pc, #80]	/* (0x1fff1158) */
    str	r1, [r3, #48]	/* 0x30 */
    ldr	r1, [pc, #80]	/* (0x1fff115c) */
    str	r1, [r3, #52]	/* 0x34 */
    ldr	r0, [r3, #0]
    movs	r1, #192	/* 0xc0 */
    lsls	r1, r1, #10
    orrs	r1, r0
    str		r1, [r3, #0]
    cmp		r2, #20
    bne.n	0x1fff1122
    movs	r2, #128	/* 0x80 */
    lsls	r2, r2, #22
    str		r2, [r3, #12]
    b.n		0x1fff1130
    movs	r0, #1
    cmp		r2, #21
    bne.n	0x1fff1150
    movs	r2, #128	/* 0x80*/
    lsls	r2, r2, #23
    ldr		r3, [pc, #36]	/* (0x1fff1154) */
    str		r2, [r3, #12]
    ldr		r2, [pc, #32]	/* (0x1fff1154)*/
    ldr		r3, [r2, #16]
    cmp		r3, #0
    bge.n	0x1fff1132
    ldr		r3, [pc, #24]	/* (0x1fff1154) */
    ldr		r2, [r3, #16]
    ldr		r1, [pc, #24]	/* (0x1fff1158) */
    str		r1, [r3, #48]	/* 0x30 */
    ldr		r1, [pc, #24]	/* (0x1fff115c) */
    str		r1, [r3, #52]	/* 0x34 */
    ldr		r0, [r3, #0]
    ldr		r1, [pc, #24]	/* (0x1fff1160) */
    ands	r1, r0
    str		r1, [r3, #0]
    movs	r0, #3
    ands	r0, r2
    bx		lr
    nop

/* @ 1fff1154 */
var1: .word 0x50004100
var2: .word 0x71755253
var3: .word 0x75fca918
var4: .word 0xfffffffc

fct_PROG: /* @ 0x1fff1164 */
    push	{r4, r5, r6, r7, lr}
    mov		r7, sl
    mov		r6, r9
    mov		r5, r8
    push	{r5, r6, r7}
    mov		sl, r0
    mov		r8, r2
    ldr		r3, [pc, #348]	/* Load Flash controller base address (0x1fff12d0) */
    ldr		r2, [r3, #0]    /* FACCR */
    movs	r0, #192	/* 0xc0 */
    ands	r0, r2
    mov		ip, r0
    /* Key0 */
    ldr		r2, [pc, #340]	/* (0x1fff12d4) */
    str		r2, [r3, #48]	/* 0x30 */
    /* Key 1 */
    ldr		r2, [pc, #340]	        /* (0x1fff12d8) */
    str		r2, [r3, #52]	/* 0x34 */

    ldr		r0, [r3, #0]
    ldr		r2, [pc, #340]	/* fffcff03 (0x1fff12dc) */
    ands	r2, r0
    ldr		r0, [pc, #340]	/* (0x1fff12e0) */
    orrs	r2, r0
    str		r2, [r3, #0]
    mov		r0, sl
    str		r0, [r3, #4]
    movs	r3, #3
    ands	r3, r0
    beq.n	0x1fff11d4
    movs	r2, #4
    subs	r3, r2, r3
    beq.n	0x1fff11d8
    mov		r2, r8
    cmp		r2, #0
    beq.n	0x1fff1296
    mov		r9, r8
    adds	r7, r3, #0
    movs	r3, #0
    movs	r5, #0
    ldr		r4, [pc, #288]	/* (0x1fff12d0) */
    movs	r2, #128	/* 0x80         */
    movs	r6, #3
      	ldrb	r3, [r1, r3]
      	str	r3, [r4, #8]
      	str	r2, [r4, #12]
      	ldr	r3, [r4, #16]
      	cmp	r3, #0
      	bge.n	0x1fff11ba
    ldr		r0, [r4, #16]
    ands	r0, r6
    bne.n	0x1fff127e
    adds	r5, #1
    subs	r3, r5, #0
      	cmp	r3, r7
      	beq.n	0x1fff11da
      	cmp	r5, r9
      	bne.n	0x1fff11b4
      	b.n	0x1fff11da
      	movs	r5, #0
      	b.n	0x1fff11da
      	movs	r5, #0
      	mov	r3, r8
      	subs	r6, r3, r5
      	ldr	r3, [pc, #240]	/* (0x1fff12d0) */
      	ldr	r2, [pc, #240]	/* (0x1fff12d4) */
      	str	r2, [r3, #48]	/* 0x30 */
      	ldr	r2, [pc, #240]	/* (0x1fff12d8) */
      	str	r2, [r3, #52]	/* 0x34 */
      	ldr	r2, [r3, #0]
      	movs	r0, #252	/* 0xfc */
      	bics	r2, r0
      	movs	r0, #12
      	orrs	r2, r0
      	str	r2, [r3, #0]
      	mov	r0, sl
      	adds	r2, r5, r0
      	str	r2, [r3, #4]
      	cmp	r6, #3
      	ble.n	0x1fff1238
      	adds	r4, r1, r5
    adds	r2, r3, #0
      	movs	r3, #128	/* 0x80 */
      	mov	r9, r3
      	movs	r7, #3
      	ldrb	r0, [r4, #3]
      	lsls	r0, r0, #24
      	ldrb	r3, [r4, #2]
      	lsls	r3, r3, #16
      	adds	r3, r0, r3
      	ldrb	r0, [r4, #1]
      	lsls	r0, r0, #8
      	adds	r3, r3, r0
      	ldrb	r0, [r4, #0]
      	adds	r3, r3, r0
      	str	r3, [r2, #8]
      	mov	r0, r9
    str	r0, [r2, #12]
      	ldr	r3, [r2, #16]
      	cmp	r3, #0
      	bge.n	0x1fff1222
      	ldr	r0, [r2, #16]
      	ands	r0, r7
      	bne.n	0x1fff127e
      	adds	r5, #4
      	subs	r6, #4
      	adds	r4, #4
      	cmp	r6, #3
      	bgt.n	0x1fff1208
      	ldr	r3, [pc, #148]	/* (0x1fff12d0) */
      	ldr	r2, [pc, #152]	/* (0x1fff12d4) */
      	str	r2, [r3, #48]	/* 0x30 */
      	ldr	r2, [pc, #152]	/* (0x1fff12d8) */
    str	r2, 	[r3, #52]	/* 0x34 */
      	ldr	r0, [r3, #0]
      	ldr	r2, [pc, #148]	/* (0x1fff12dc) */
      	ands	r2, r0
      	ldr	r0, [pc, #148]	/* (0x1fff12e0) */
      	orrs	r2, r0
      	str	r2, [r3, #0]
      	mov	r2, sl
      	adds	r7, r5, r2
      	str	r7, [r3, #4]
      	movs	r0, #0
      	cmp	r6, #0
      	ble.n	0x1fff127e
      	adds	r4, r1, r5
      	adds	r6, r1, r6
      	adds	r5, r6, r5
      	adds	r2, r3, #0
      	movs	r6, #128	/* 0x80 */
      	movs	r7, #3
      	ldrb	r3, [r4, #0]
      	str	r3, [r2, #8]
      	str	r6, [r2, #12]
      	ldr	r3, [r2, #16]
      	cmp	r3, #0
    bge.n	0x1fff126c
      	ldr	r0, [r2, #16]
      	ands	r0, r7
      	bne.n	0x1fff127e
      	adds	r4, #1
      	cmp	r4, r5
      	bne.n	0x1fff1266
     	ldr	r3, [pc, #80]	/* (0x1fff12d0) */
      	ldr	r2, [pc, #80]	/* (0x1fff12d4) */
      	str	r2, [r3, #48]	/* 0x30 */
      	ldr	r2, [pc, #80]	/* (0x1fff12d8) */
      	str	r2, [r3, #52]	/* 0x34 */
      	ldr	r4, [r3, #0]
      	ldr	r2, [pc, #80]	/* (0x1fff12dc) */
      	ands	r2, r4
      	mov	r1, ip
      	orrs	r1, r2
      	str	r1, [r3, #0]
      	b.n	0x1fff12c6
      	ldr	r3, [pc, #56]	/* (0x1fff12d0) */
      	ldr	r0, [pc, #56]	/* (0x1fff12d4) */
      	str	r0, [r3, #48]	/* 0x30 */
      	ldr	r2, [pc, #56]	/* (0x1fff12d8) */
      	str	r2, [r3, #52]	/* 0x34 */
    ldr	r5, [r3, #0]
      	movs	r4, #252	/* 0xfc */
      	bics	r5, r4
      	movs	r4, #12
      	orrs	r4, r5
      	str	r4, [r3, #0]
      	mov	r1, sl
      	str	r1, [r3, #4]
      	str	r0, [r3, #48]	/* 0x30 */
      	str	r2, [r3, #52]	/* 0x34 */
      	ldr	r0, [r3, #0]
      	ldr	r2, [pc, #36]	/* (0x1fff12dc) */
      	ands	r2, r0
      	ldr	r0, [pc, #36]	/* (0x1fff12e0) */
      	orrs	r2, r0
      	str	r2, [r3, #0]
      	str	r1, [r3, #4]
      	movs	r0, #0
      	b.n	0x1fff127e
      	pop	{r2, r3, r4}
      	mov	r8, r2
      	mov	r9, r3
      	mov	sl, r4
      	pop	{r4, r5, r6, r7, pc}

fct_PROG_var1: .word 0x50004100 /* 12d0 */
fct_PROG_var2: .word 0x71755253
fct_PROG_var3: .word 0x75fca918
fct_PROG_var4: .word 0xff03fffc
fct_PROG_var5: .word 0x00040003

some_fct_2: /* @ 0x1fff12e0 */
    push	{r3, r4, r5, lr}
    cmp		r1, #0
    beq.n	0x1fff12fc
    adds	r4, r0, #0
    adds	r5, r0, r1
    movs	r0, #0
    bl		0x1fff1548
    strb	r0, [r4, #0]
    adds	r4, #1
    cmp		r4, r5
    bne.n	0x1fff12ee
    movs	r0, #0
    pop		{r3, r4, r5, pc}
    cmp		r2, #0
    beq.n	0x1fff1312
    adds	r2, r0, r2
    ldrb	r3, [r1, #0]
    strb	r3, [r0, #0]
    adds	r1, #1
    adds	r0, #1
    cmp	r0, r2
    bne.n	0x1fff1306
    movs	r0, #0
    bx	lr
    nop

some_fct_3: /* @ 0x1fff1318 */
    push	{r4, r5, lr}
    sub		sp, #36		/* 0x24 */
    adds	r4, r1, #0
    adds	r5, r2, #0
    movs	r3, #0
    str		r3, [sp, #0]
    str		r3, [sp, #4]
    str		r3, [sp, #8]
    str		r3, [sp, #12]
    str		r3, [sp, #16]
    movs	r3, #255	/* 0xff */
    mov		r2, sp
    strh	r3, [r2, #0]
    movs	r3, #128	/* 0x80 */
    lsls	r3, r3, #1
    cmp		r0, r3
    bne.n	0x1fff1368
    ldr		r4, [pc, #212]	/* (0x1fff1410) */
    add		r0, sp, #20
    ldr		r1, [r4, #0]
    bl		0x1fff1528
    add		r0, sp, #20
    bl		0x1fff1538
    ldr		r0, [pc, #200]	/* (0x1fff1414) */
    bl		0x1fff1538
    ldr		r1, [r4, #4]
    add		r0, sp, #20
    bl		0x1fff1528
    add		r0, sp, #20
    bl		0x1fff1538
    ldr		r0, [pc, #184]	/* (0x1fff1418) */
 	bl	0x1fff1538
      	movs	r3, #0
     	b.n	0x1fff140a
      	movs	r3, #1
      	movs	r2, #2
      	adds	r2, #255	/* 0xff */
      	cmp	r0, r2
      	bne.n	0x1fff140a
    cmp		r1, #0
      	blt.n	0x1fff1390
      	ldr	r3, [pc, #152]	/* (0x1fff1410) */
      	ldr	r3, [r3, #0]
      	cmp	r3, #0
      	bge.n	0x1fff138a
      	movs	r0, #20
 	bl	0x1fff1100
      	movs	r3, #1
      	cmp	r0, #0
      	bne.n	0x1fff140a
      	ldr	r3, [pc, #144]	/* (0x1fff141c) */
      	mov	r2, sp
      	strh	r3, [r2, #0]
      	lsls	r3, r4, #1
      	bmi.n	0x1fff139a
      	ldr	r3, [pc, #132]	/* (0x1fff141c) */
      	mov	r2, sp
      	strh	r3, [r2, #2]
      	lsls	r3, r4, #28
      	bmi.n	0x1fff13b8
      	ldr	r3, [pc, #112]	/* (0x1fff1410) */
    ldr		r3, [r3, #0]
    lsls	r2, r3, #28
    bpl.n	0x1fff13b2
    ldr		r0, [pc, #120]	/* (0x1fff1420) */
    bl		0x1fff1060
    movs	r3, #1
    cmp		r0, #0
    bne.n	0x1fff140a
    ldr		r3, [pc, #104]	/* (0x1fff141c) */
    mov		r2, sp
    strh	r3, [r2, #6]
    lsls	r3, r4, #29
    bmi.n	0x1fff13d6
    ldr		r3, [pc, #80]	/* (0x1fff1410) */
    ldr		r3, [r3, #0]
    lsls	r2, r3, #29
    bpl.n	0x1fff13d0
    ldr		r0, [pc, #92]	/* (0x1fff1424) */
    bl		0x1fff1060
    movs	r3, #1
    cmp		r0, #0
    bne.n	0x1fff140a
    ldr		r3, [pc, #72]	/* (0x1fff141c) */
    mov	r2, sp
    strh	r3, [r2, #4]
    lsls	r3, r4, #30
      	bmi.n	0x1fff13e0
      	ldr	r3, [pc, #64]	/* (0x1fff141c) */
      	mov	r2, sp
      	strh	r3, [r2, #10]
      	lsls	r3, r4, #31
      	bmi.n	0x1fff13ea
      	ldr	r3, [pc, #52]	/* (0x1fff141c) */
      	mov	r2, sp
      	strh	r3, [r2, #8]
      	str	r5, [sp, #12]
      	mvns	r3, r5
      	str	r3, [sp, #16]
      	ldr	r3, [pc, #28]	/* (0x1fff1410) */
      	str	r4, [r3, #0]
      	str	r5, [r3, #4]
      	ldr	r4, [pc, #48]	/* (0x1fff1428) */
      	adds	r0, r4, #0
 	bl	0x1fff1060
      	adds	r0, r4, #0
      	mov	r1, sp
      	movs	r2, #20
 	bl	0x1fff1164
      	movs	r3, #0
      	adds	r0, r3, #0
      	add	sp, #36		/* 0x24 */
      	pop	{r4, r5, pc}

.word 0x382c2000
.word 0x0dfc0000
.word 0x0df80000
.word 0x8a75ffff
.word 0xff001003
.word 0xfe001003
.word 0xfc001003

some_fct_4: /* @ 0x1fff142c */
    ldr		r3, [pc, #88]	/* (0x1fff1488) */
    cmp		r0, r3
    bhi.n	0x1fff1438
    movs	r3, #128	/* 0x80 */
    lsls	r3, r3, #21
    orrs	r0, r3
    ldr		r3, [pc, #80]	/* (0x1fff148c) */
    cmp		r0, r3
    bhi.n	0x1fff144a
    ldr		r3, [pc, #80]	/* (0x1fff1490) */
    ldr		r0, [r3, #0]
    lsrs	r0, r0, #31
    movs	r3, #3
    subs	r0, r3, r0
    b.n		0x1fff1484
    ldr		r2, [pc, #72]	/* (0x1fff1494) */
    adds	r3, r0, r2
    cmp		r3, #255	/* 0xff */
    bhi.n	0x1fff1460
    ldr		r3, [pc, #60]	/* (0x1fff1490) */
    ldr		r0, [r3, #0]
    lsls	r0, r0, #29
    lsrs	r0, r0, #31
    movs	r3, #3
    subs	r0, r3, r0
    b.n		0x1fff1484
    ldr		r3, [pc, #52]	/* (0x1fff1498) */
    cmp		r0, r3
    bhi.n	0x1fff1474
    ldr		r3, [pc, #40]	/* (0x1fff1490) */
    ldr		r0, [r3, #0]
    lsls	r0, r0, #28
    lsrs	r0, r0, #31
    movs	r3, #3
    subs	r0, r3, r0
    b.n		0x1fff1484
    ldr		r2, [pc, #36]	/* (0x1fff149c) */
    adds	r3, r0, r2
    movs	r0, #0
    ldr		r2, [pc, #36]	/* (0x1fff14a0) */
    cmp		r2, r3
    adcs	r0, r0
    movs	r3, #3
    subs	r0, r3, r0
    bx		lr
    nop

/* @ 0x1fff1488 */
.word 0xffff0003
.word 0xffff1001
.word 0x382c2000
.word 0x0200effc
.word 0xffff1003
.word 0xf000e000
.word 0x07ff0000

some_fct_5: /* @ 0x1fff14a4 */
    push	{r4, r5, r6, r7, lr}
    mov		r7, r9
    mov		r6, r8
    push	{r6, r7}
    sub		sp, #20
    adds	r6, r1, #0
    movs	r3, #3
    adds	r7, r0, #0
    bics	r7, r3
    cmp		r1, #0
    beq.n	0x1fff1516
    adds	r4, r7, #0
    movs	r3, #128	/* 0x80 */
    lsls	r3, r3, #11
    mov		r9, r3
    ldr		r3, [pc, #96]	/* (0x1fff1524) */
    mov		r8, r3
    add		r0, sp, #4
    adds	r1, r4, #0
    bl		0x1fff1528
    add		r0, sp, #4
    bl		0x1fff1538
    movs	r0, #58		/* 0x3a */
    bl		0x1fff1558
    subs	r5, r4, #0
    cmp		r5, r9
    bhi.n	0x1fff14e6
    movs	r5, #128	/* 0x80 */
    lsls	r5, r5, #21
    orrs	r5, r4
    adds	r0, r5, #0
    bl		0x1fff142c
    cmp		r0, #2
    bne.n	0x1fff14fa
    add		r0, sp, #4
    movs	r1, #0
    bl		0x1fff1528
    b.n		0x1fff1502
    ldr		r1, [r5, #0]
    add		r0, sp, #4
    bl		0x1fff1528
    add		r0, sp, #4
    bl		0x1fff1538
    mov		r0, r8
    bl		0x1fff1538
    adds	r4, #4
    subs	r3, r4, r7
    cmp		r3, r6
    bcc.n	0x1fff14c6
    movs	r0, #0
    add		sp, #20
    pop		{r2, r3}
    mov		r8, r2
    mov		r9, r3
    pop		{r4, r5, r6, r7, pc}
    nop

/* @ 1fff1524 */
.word 0x0df80000

some_fct_6: /* @ 0x1fff1528 */
    push	{r0}
    ldr		r0, [pc, #8]	/* (0x1fff1534) */
    mov		ip, r0
    pop		{r0}
    bx		ip
    nop

some_fct_10: /* @ 0x1fff1534 */
    lsls	r5, r1, #20
    movs	r0, r0
    push	{r0}
    ldr		r0, [pc, #8]	/* (0x1fff1544) */
    mov		ip, r0
    pop		{r0}
    bx		ip
    nop

var5: .word 0x03e50000 /* @ 1544 */

some_fct_11: /* @ 0x1fff1548 */
    push	{r0}
    ldr		r0, [pc, #8]	/* (0x1fff1554) */
    mov		ip, r0
    pop		{r0}
    bx		ip
    nop

.word 0x03790000

some_fct_12: /* @ 1fff1558 */
    push	{r0}
    ldr		r0, [pc, #8]	/* (0x1fff1564) */
    mov		ip, r0
    pop		{r0}
    bx		ip
    nop

.word 0x03d10000

    .pool
    .size IAP_Handler, . - IAP_Handler
