//
//	RAM�͌㔼 1k WORD �� �擪���� 4 WORD���݂���Ɖ��肵�Ă���.
//
work0	equ	0x400
work1	equ	0x401
work2	equ	0x402
work3	equ	0x403
//	work3��CALL���߂�PC�ۑ��p(RET)�Ŏg�p����.

	org	0
//----------------------------------------------------------------
//
//		atan(x)=x-(1/3)*x^3+(1/5)*x^5-(1/7)*x^7....
//
//	pi=4*(4*atan(1/5) - atan(1/239))
//	  =16 * (1/5) - (16*(1/5)/3 *(1/5)*(1/5) - 4 * (1/239)
//
//	��������->�����̉��ʗ����� �W��500���|���Ă���̂ŁA
//	  =1600 - (1600/75) - (2000/239)
//	���ʂ̐����l(���ʗ����W��500) �� 500 �Ŋ���ƁA3.14�ɂȂ�.
//----------------------------------------------------------------

main:
	mov a,1600		// 4*4*100
//	mov b,5			// �W��500���|���Ă���̂ŁA5�Ŋ��炸���̂܂�.
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
	mov [work0],a	// work0=16*atan(1/5) (�}�N���[�����W�J�͂Q���̂�)


	mov a,2000		// 4*500
	mov b,239
	call div_ab
	mov [work1],c	// work1=4*atan(1/239) == 4*(1/239)

	mov a,[work0]
	mov b,[work1]
	sub a,b
//	������A���W�X�^�Ɏc��. �W��500���|�������Œ菬��.

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


