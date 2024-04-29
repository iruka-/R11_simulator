//
//	RAMは後半 1k WORD の 先頭から 4 WORD存在すると仮定している.
//
work0	equ	0x400
work1	equ	0x401
work2	equ	0x402
work3	equ	0x403
//	work3はCALL命令のPC保存用(RET)で使用する.

	org	0
//----------------------------------------------------------------
//
//		atan(x)=x-(1/3)*x^3+(1/5)*x^5-(1/7)*x^7....
//
//	pi=4*(4*atan(1/5) - atan(1/239))
//	  =16 * (1/5) - (16*(1/5)/3 *(1/5)*(1/5) - 4 * (1/239)
//
//	浮動小数->整数の下駄履きは 係数500を掛けているので、
//	  =1600 - (1600/75) - (2000/239)
//	結果の整数値(下駄履き係数500) を 500 で割ると、3.14になる.
//----------------------------------------------------------------

main:
	mov a,1600		// 4*4*100
//	mov b,5			// 係数500を掛けているので、5で割らずそのまま.
//	call div_ab
//	mov [work0],c	// work0=4*4*(1/5)
	mov [work0],a	// work0=4*4*(1/5)

	mov a,[work0]
	mov b,75		// 3*5*5
	call div_ab
	mov [work1],c	// work1=4*4*(1/5) /3 *(1/5) * (1/5)

	mov a,[work0]
	mov b,[work1]
	sub a,b
	mov [work0],a	// work0=16*atan(1/5) (マクローリン展開は２項のみ)


	mov a,2000		// 4*500
	mov b,239
	call div_ab
	mov [work1],c	// work1=4*atan(1/239) == 4*(1/239)

	mov a,[work0]
	mov b,[work1]
	sub a,b
//	答えはAレジスタに残る. 係数500が掛かった固定小数.

halt:
	jmp halt

//	C=A/B
div_ab:
	mov [work3],c
	mov c,0x7ff
div_loop:
	add	c,1
	sub	a,b
	jnc	div_loop
	ret

	end


