/** *********************************************************************************
 *	�I�y�R�[�h�̋t�A�Z���u��
 ************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "opcode.h"


/** *********************************************************************************
//	�f�B�X�e�B�l�[�V�����i�I�y�����h�j�𕶎���ŏo��.
 ************************************************************************************
 */
void gen_dst_string(int code,OPCODE *tab,char *buf)
{
	char *s=tab->operand;
	while(*s) {
		if(*s == '*') {
			s++;
			sprintf(buf,"0x%x",code & WORD_MASK);
			buf += strlen(buf);
		}else{
			*buf++=*s++;
		}
	}
	*buf=0;
}

int disasm(int code,OPCODE *tab)
{
	char code2[40];
	char dst[80];
	code2[0] = 0;
	dst[0] = 0;
	
	gen_dst_string(code,tab,dst);	//JMP�ȊO

	char c='C';
	if(reg.c==0) c = '_';
	
	int pc = ( reg.pc_bak - 1 ) & WORD_MASK;
	printf("%04x: %04x %4s %-5s %-9s :%c A:%03x B:%03x C:%03x\n"
		,pc
		,code
		,code2
		,tab->mnemonic
		,dst

		,c
		,reg.A
		,reg.B
		,reg.C
	);
	return 0;
}

/** *********************************************************************************
 *	
 ************************************************************************************
 */
int d_MOV_A_imm		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_MOV_B_imm		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_MOV_C_imm		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_MOV_A_ind		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_MOV_B_ind		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_MOV_C_ind		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_MOV_ind_A		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_MOV_ind_C		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_ADD_A_B  		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_ADD_C_imm		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_SUB_A_B  		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_JMP_imm		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_JMP_ind		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_JNC_imm		(int code,OPCODE *tab){	return disasm(code,tab);}
int d_OUT_A			(int code,OPCODE *tab){	return disasm(code,tab);}
int d_und			(int code,OPCODE *tab){	return disasm(code,tab);}
/** *********************************************************************************
 *
 ************************************************************************************
 */
