/** *********************************************************************************
 *	
 ************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include "opcode.h"
#include "gr.h"
#include "led.h"


OPCODE opcode_init_tab[]={
 //�j��,�I�y�����h	,�@�B��	  ,f_emu,d_dis  //z,c,m,���� --------------------
 {"MOV","A,*" 		,MOV_A_imm,f_MOV_A_imm,d_MOV_A_imm},//
 {"MOV","B,*" 		,MOV_B_imm,f_MOV_B_imm,d_MOV_B_imm},//
 {"MOV","C,*" 		,MOV_C_imm,f_MOV_C_imm,d_MOV_C_imm},//
 {"MOV","A,[*]" 	,MOV_A_ind,f_MOV_A_ind,d_MOV_A_ind},//
 {"MOV","B,[*]" 	,MOV_B_ind,f_MOV_B_ind,d_MOV_B_ind},//
 {"MOV","C,[*]" 	,MOV_C_ind,f_MOV_C_ind,d_MOV_C_ind},//
 {"MOV","[*],A" 	,MOV_ind_A,f_MOV_ind_A,d_MOV_ind_A},//
 {"MOV","[*],C" 	,MOV_ind_C,f_MOV_ind_C,d_MOV_ind_C},//
 {"ADD","A,B"       ,ADD_A_B  ,f_ADD_A_B  ,d_ADD_A_B  },//
 {"ADD","C,*"       ,ADD_C_imm,f_ADD_C_imm,d_ADD_C_imm},//
 {"SUB","A,B"       ,SUB_A_B  ,f_SUB_A_B  ,d_SUB_A_B  },//
 {"JMP","*"         ,JMP_imm,f_JMP_imm	  ,d_JMP_imm  },//
 {"JMP","[*]"       ,JMP_ind,f_JMP_ind	  ,d_JMP_ind  },//
 {"JNC","*"     	,JNC_imm,f_JNC_imm	  ,d_JNC_imm  },//
 {"OUT","A"         ,OUT_A  ,f_OUT_A      ,d_OUT_A},//
 //�j��,�Ӗ�					,�@�B��		 ,f_emu,d_dis  //z,c,m,���� --------------------
 { NULL,NULL,0}
};

OPCODE UNDEFINED_OPCODE= {"???","???",0,f_und,d_und};

FILE *ifp;

#define Ropen(name) {ifp=fopen(name,"rb");if(ifp==NULL) \
{ printf("Fatal: can't open file:%s\n",name);exit(1);}  \
}
#define Rclose()  fclose(ifp)

void memdump(int adr,int len);
void VRAM_output(int adrs,int data);
extern	int	led_trace;

/** *********************************************************************************
 *	���߃R�[�h code ���}�b�`����OPCODE��opcode_init_tab[]����T��. 
 ************************************************************************************
 *	����������A����� *table �Ɋۂ��ƃR�s�[����.
 *	������Ȃ�������AUNDEFINED_OPCODE�� *table �Ɋۂ��ƃR�s�[����.
 */
void make_table(OPCODE *table,int code)
{
	OPCODE *s = opcode_init_tab;
	while(s->mnemonic) {
		if(code == s->pattern) {
			*table = *s;
			return;
		}
		s++;
	}
	*table = UNDEFINED_OPCODE;
}

/** *********************************************************************************
 *	opcode_init_tab[]�̏������ƂɁA�������\�� code_table[256] ���\�z����.
 ************************************************************************************
 */
void init_table()
{
	OPCODE *table = code_table;		// �\�z�������\ code_table[256]; �̐擪�A�h���X.
	int code;
	for(code=0;code<32;code++,table++) {
		make_table(table,code);
	}
}

/** *********************************************************************************
 *	16bit�̖��߂��P�t�F�b�`���Ď��s����.
 ************************************************************************************
 */
int	execute_pc()
{
	int rc=0;
	int opcode = memory[ reg.pc++ ];
	OPCODE *table = &code_table[ (opcode >> 11) & 0x1f ];
	reg.pc_bak = reg.pc;

	rc = table->emufunc(opcode,table);	//���ߎ��s.
	if(led_trace) {
		table->disfunc(opcode,table);	//�t�A�Z���u��.
	}
	if(rc) return rc;
	return LED_draw(reg.A,reg.B,reg.C, reg.pc,led_trace);

}

/** *********************************************************************************
 *	
 ************************************************************************************
 */
int	load_binary(char *fname)
{
	Ropen(fname);
	int rc = fread (memory,1,sizeof(memory),ifp);
	Rclose();
	return rc;
}
/** *********************************************************************************
 *	���C�����[�`��.
 ************************************************************************************
 */
int main(int argc,char **argv)
{
	int rc;
	init_table();

	gr_init(SCREEN_W,SCREEN_H,32,0);

	load_binary(argv[1]);

	reg.pc = 0;

	while(1) {
		rc = execute_pc();
		if(rc) break;
	}
	gr_close();
	return 0;
}


/** *********************************************************************************
 *
 ************************************************************************************
 */
