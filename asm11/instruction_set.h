#ifndef	instruction_set_h_
#define	instruction_set_h_

enum {
	MOV_A_imm,
	MOV_B_imm,
	MOV_C_imm,
	JMP_imm,		// MOV PC,imm�Ɠ��`.
	MOV_A_ind,
	MOV_B_ind,
	MOV_C_ind,
	JMP_ind,		// MOV PC,ind�Ɠ��`.

	MOV_ind_A,
	MOV_ind_C,
	JNC_imm,
	ADD_A_B,
	ADD_C_imm,
	SUB_A_B,
	
	OUT_A,			// for deBUG
};	// =11bit���V�t�g���Ă��̂܂�16bit-code�ɂ���.

#endif
