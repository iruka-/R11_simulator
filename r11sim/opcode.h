/************************************************************************************
 *
 ************************************************************************************
 */
#ifndef	opcode_h_
#define	opcode_h_

#include "../asm11/instruction_set.h"

typedef	unsigned short WORD;

struct _OPCODE ;		// ���傱���Ɛ錾.

//	���ߎ��s�֐��̃v���g�^�C�v�^.
typedef int (*EMUFUNC) (int code,struct _OPCODE *tab);
//	���ߋt�A�Z���u���֐��̃v���g�^�C�v�^.
typedef int (*DISFUNC) (int code,struct _OPCODE *tab);


typedef	struct _OPCODE {
	char *mnemonic;			// �j��
	char *operand;			// �I�y�����h
	int	  pattern;			// �@�B��
	EMUFUNC emufunc;		// ���ߎ��s�֐�
	DISFUNC disfunc;		// ���ߋt�A�Z���u���֐�
} OPCODE;


#define	MEMSIZE		0x800			// �����������[�T�C�Y (WORD)
#define	WORD_MASK	0x7ff			// 11bit

#define	SIGN_MASK	0x400			// sign bit(unuse)
#define	CARRY_MASK	0x800			// carry bit (use)

OPCODE  code_table[32];		// ���ߕ\(5bit)

/** *********************************************************************************
 *	�������[�R���e�L�X�g
 ************************************************************************************
 */
WORD    memory[MEMSIZE];

/** *********************************************************************************
 *	�u�q�`�l�R���e�L�X�g
 ************************************************************************************
 */
//WORD    vram[VRAMSIZE];

/** *********************************************************************************
 *	���W�X�^�R���e�L�X�g
 ************************************************************************************
 */
typedef	struct tag_R16_CONTEXT {
	WORD	pc;
	WORD	acc;
	WORD	A;
	WORD	B;
	WORD	C;
	char	z;	// zero
	char	c;	// carry
	char	m;	// minus
	WORD	pc_bak;	// jump����O��PC.	(�t�A�Z���u�����ɕK�v)
} R16_CONTEXT;

R16_CONTEXT	reg;


#define	EMUFUNC_(x_)	int x_(int code,struct _OPCODE *tab)
#define	DISFUNC_(x_)	int x_(int code,struct _OPCODE *tab)

//	���ߎ��s�֐��̃v���g�^�C�v�錾.
EMUFUNC_( f_MOV_A_imm );
EMUFUNC_( f_MOV_B_imm );
EMUFUNC_( f_MOV_C_imm );
EMUFUNC_( f_MOV_A_ind );
EMUFUNC_( f_MOV_B_ind );
EMUFUNC_( f_MOV_C_ind );
EMUFUNC_( f_MOV_ind_A );
EMUFUNC_( f_MOV_ind_C );
EMUFUNC_( f_ADD_A_B   );
EMUFUNC_( f_ADD_C_imm );
EMUFUNC_( f_SUB_A_B   );
EMUFUNC_( f_JMP_imm   );
EMUFUNC_( f_JMP_ind   );
EMUFUNC_( f_JNC_imm   );
EMUFUNC_( f_OUT_A     );

DISFUNC_( d_MOV_A_imm );
DISFUNC_( d_MOV_B_imm );
DISFUNC_( d_MOV_C_imm );
DISFUNC_( d_MOV_A_ind );
DISFUNC_( d_MOV_B_ind );
DISFUNC_( d_MOV_C_ind );
DISFUNC_( d_MOV_ind_A );
DISFUNC_( d_MOV_ind_C );
DISFUNC_( d_ADD_A_B   );
DISFUNC_( d_ADD_C_imm );
DISFUNC_( d_SUB_A_B   );
DISFUNC_( d_JMP_imm );
DISFUNC_( d_JMP_ind );
DISFUNC_( d_JNC_imm );
DISFUNC_( d_OUT_A );

//	���ߋt�A�Z���u���֐��̃v���g�^�C�v�錾.

EMUFUNC_( f_und );	//����`.
DISFUNC_( d_und );

//
//	���o�͊֐�.
//
void LED_output(int acc , int ea);

#endif	//opcode_h_
