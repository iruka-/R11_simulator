/** *********************************************************************************
 *	オペコードの解釈実行.
 ************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>

#define Extern /* */

#include "opcode.h"
#include "led.h"
/** *********************************************************************************
 *	Acc結果から、フラグ z,c,mを全部更新する.
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
 *	オペコードを解析して、実効アドレス(ea) を求める.
 ************************************************************************************
 *	といいつつ、実効アドレスの評価結果（つまりオペランド）を返す.
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
 *	オペコードを解析して、分岐先実効アドレス(ea) を求める.
 ************************************************************************************
 *	分岐系は XX を解釈した後、ddを解釈することになっている.
 */
int	jpadrs(int code)
{
	return code & WORD_MASK;		// 11bit
}
/** *********************************************************************************
 *	こっから下は、命令の実行を行なう.
 ************************************************************************************
 *	命令実行の戻り値は常に 0 .
 *	1 になった場合、エミュレーション終了になる.
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

//	未定義命令.
int f_und (int code,OPCODE *tab)
{
	return 1;	//未定義命令を踏むと終了する.
//	return 0;
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
