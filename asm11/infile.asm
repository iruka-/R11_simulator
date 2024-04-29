	org	0
//
//	RAMは後半 1k WORD の 先頭から 4 WORD存在すると仮定している.
//
work0	equ	0x400
work1	equ	0x401
work2	equ	0x402
work3	equ	0x403

	mov a,32
	mov b,0x7ff
	mov c,1
	mov	[work3],a
	mov	[work1],c
	mov a,[work0]
	mov b,[work1]
	mov c,[work2]

label1:
	add a,b
	add c,123
	sub a,b
	jmp	label2
	jmp	[work1]
	jnc	label1

label2:
//	subのオペランド省略を許す.
	sub

//	疑似的なCall
	call	subr1

//	HALTが無いので、自己ジャンプで代用する.
label3:
	jmp	label3

subr1:
	mov	[work3],c
	mov a,0x41
	add a,b
	ret

//	out a
	end


