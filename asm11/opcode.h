/************************************************************************************
 *
 ************************************************************************************
 */
#ifndef	opcode_h_
#define	opcode_h_

#include "hash.h"
#include "instruction_set.h"

#define	USE_CALL_MACRO	1
#define	USE_CALL_WORK	"[1027]"

typedef	unsigned short WORD;

struct _OPCODE ;		// ���傱���Ɛ錾.

//	���ߎ��s�֐��̃v���g�^�C�v�^.
typedef int (*EMUFUNC) (int code,struct _OPCODE *tab);
//	���ߋt�A�Z���u���֐��̃v���g�^�C�v�^.
//typedef int (*DISFUNC) (int code,struct _OPCODE *tab);


typedef	struct _OPCODE {
	char *mnemonic;			// �j��
	char *comment;			// �Ӗ�
	int	  pattern;			// �@�B��
//	char *pattern;			// �@�B��
	EMUFUNC emufunc;		// ���ߎ��s�֐�
	int	  data;				// 
} OPCODE;

#define	MEMSIZE		0x800			// �����������[�T�C�Y (WORD)

//OPCODE  code_table[256];		// ���ߕ\(8bit)

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
typedef	struct _CONTEXT {
	WORD	pc;
	WORD	acc;
	char	z;	// zero
	char	c;	// carry
	char	m;	// minus
//
	WORD	pc_bak;	// jump����O��PC.	(�t�A�Z���u�����ɕK�v)
} CONTEXT;

CONTEXT	reg;


#define	EMUFUNC_(x_)	int x_(int code,struct _OPCODE *tab)
#define	DISFUNC_(x_)	int x_(int code,struct _OPCODE *tab)

//	���ߎ��s�֐��̃v���g�^�C�v�錾.
EMUFUNC_( f_MOV );
EMUFUNC_( f_JMP );
EMUFUNC_( f_JNC );
EMUFUNC_( f_ADD );
EMUFUNC_( f_SUB );

EMUFUNC_( f_ORG );
EMUFUNC_( f_EQU );
EMUFUNC_( f_DW  );
EMUFUNC_( f_DOT );
EMUFUNC_( f_END );

EMUFUNC_( f_CALL );
EMUFUNC_( f_RET  );
EMUFUNC_( f_OUT  );

//	xx	�A�h���b�V���O
//	00	Imm8
//	01	Reg8
//	10	[Reg8]
//	11	Imm16

enum {
	XX_IMM8 =0x0000,
	XX_REG8 =0x1000,
	XX_IND8 =0x2000,
	XX_IMM16=0x3000,	//�㑱���� IMM16 ���l������.
	XX_MASK =0x3000,

	IS_REG	=0x10000,
};
//	dd	�f�B�X�v���C�X�����g
//	00	Abs
//	01	fwd
//	10	back
//	11	fwd+carry
enum {
	DD_ABS  =0x0000,
	DD_FWD  =0x0400,
	DD_BACK =0x0800,
	DD_FWDC =0x0C00,
	DD_MASK =0x0C00,
};
//
//	���Z���߂ŁA���ʂ�Acc�ɖ߂����ǂ���.
//
enum {
	OP_TEST	    =0x0100,	//�e�X�g�݂̂�Acc���X�V���Ȃ�.
	OP_TEST_MASK=0x0100,
};

char symbuf[128];
char label[128];
char mnemonic[128];
char operandbuf[128];
char comment[512];

char *operand;

int	save_ptr;	//memory[]�̗L���T�C�Y.
HASH *sym;		//�V���{���\.

#define	ZZ	printf("%s:%d: ZZ\n",__FILE__,__LINE__);

#endif	//opcode_h_
