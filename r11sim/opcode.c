/** *********************************************************************************
 *	�I�y�R�[�h�̉��ߎ��s.
 ************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include "opcode.h"
#include "led.h"
/** *********************************************************************************
 *	Acc���ʂ���A�t���O z,c,m��S���X�V����.
 ************************************************************************************
 */
void flag_update_zcm(int acc)
{
	if(acc & WORD_MASK) reg.z = 0;
	else		        reg.z = 1;

	if(acc & SIGN_MASK) reg.m = 1;
	else		        reg.m = 0;

	if(acc & CARRY_MASK) reg.c = 1;
	else		         reg.c = 0;

}
/** *********************************************************************************
 *	�I�y�R�[�h����͂��āA�����A�h���X(ea) �����߂�.
 ************************************************************************************
 *	�Ƃ����A�����A�h���X�̕]�����ʁi�܂�I�y�����h�j��Ԃ�.
 */
int	opadrs(int code)
{
	return code & WORD_MASK;		// 11bit
}
int	opadrm(int code)
{
	return memory[code & WORD_MASK];// 11bit
}
int	store_wd(int code,int reg)
{
	memory[code & WORD_MASK]=reg & WORD_MASK;	// 11bit
	return reg & WORD_MASK;
}
/** *********************************************************************************
 *	�I�y�R�[�h����͂��āA���������A�h���X(ea) �����߂�.
 ************************************************************************************
 *	����n�� XX �����߂�����Add�����߂��邱�ƂɂȂ��Ă���.
 */
int	jpadrs(int code)
{
	return code & WORD_MASK;		// 11bit
}
/** *********************************************************************************
 *	�������牺�́A���߂̎��s���s�Ȃ�.
 ************************************************************************************
 *	���ߎ��s�̖߂�l�͏�� 0 .
 *	1 �ɂȂ����ꍇ�A�G�~�����[�V�����I���ɂȂ�.
 */
int f_MOV_A_imm (int code,OPCODE *tab){reg.A = opadrs(code);return 0;}
int f_MOV_B_imm (int code,OPCODE *tab){reg.B = opadrs(code);return 0;}
int f_MOV_C_imm (int code,OPCODE *tab){reg.C = opadrs(code);return 0;}
int f_MOV_A_ind (int code,OPCODE *tab){reg.A = opadrm(code);return 0;}
int f_MOV_B_ind (int code,OPCODE *tab){reg.B = opadrm(code);return 0;}
int f_MOV_C_ind (int code,OPCODE *tab){reg.C = opadrm(code);return 0;}
int f_MOV_ind_A (int code,OPCODE *tab){store_wd(code,reg.A);return 0;}
int f_MOV_ind_C (int code,OPCODE *tab){store_wd(code,reg.C);return 0;}


int f_OUT_A     (int code,OPCODE *tab)
{
	printf("%c",reg.A);
	return 0;
}

int f_ADD_A_B (int code,OPCODE *tab)
{
	int acc = reg.A + reg.B;
	flag_update_zcm(acc);
	reg.A = acc & WORD_MASK;
	return 0;
}

int f_ADD_C_imm (int code,OPCODE *tab)
{
	int acc = reg.C + opadrs(code);
	flag_update_zcm(acc);
	reg.C = acc & WORD_MASK;
	return 0;
}

int f_SUB_A_B (int code,OPCODE *tab)
{
	int acc = reg.A - reg.B;
	flag_update_zcm(acc);
	reg.A = acc & WORD_MASK;
	return 0;
}

int f_JMP_imm (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	reg.pc = ea;
	if(reg.pc_bak == (ea+1)) {return 1;}	// == HALT
	return 0;
}

int f_JMP_ind (int code,OPCODE *tab)
{
	int ea = opadrm(code);
	reg.pc = ea;
	return 0;
}

int f_JNC_imm (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	if(	reg.c == 0 ) {
		reg.pc = ea;
	}
	return 0;
}

//	����`����.
int f_und (int code,OPCODE *tab)
{
	return 1;	//����`���߂𓥂ނƏI������.
//	return 0;
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
