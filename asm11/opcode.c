/** *********************************************************************************
 *	オペコードの解釈実行.
 ************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "opcode.h"
#include "hash.h"

enum {
	REG_A,
	REG_B,
	REG_C,
};
/** *********************************************************************************
 *
 ************************************************************************************
 */
int	 str_cmpi(char *t,char *s);
int	 set_label(char *label,int val);
void dw(int);
void Error(char *format, ...);
int	 op_exec(char *inst,char *operand);
int	 s_pass;

char *op1,*op2;
char opbuf[256];

#define	SPLIT_MAX 256
char *split_result[SPLIT_MAX];
char  split_string[1024];

//
//	文字列(s)に文字(a)が含まれていれば、その位置を文字数(>=0)で返す.
//	含まれていなければ -1 を返す.
int	str_index(char *s,int a)
{
	int c;
	int idx=0;
	while(1) {
		c = *s;
		if(c==0) return -1;
		if(c==a) return idx;
		s++;idx++;
	}
}
//
//	文字列(s)をカンマ(,)で複数に分解する.
//	-----------------------------------------------
//	分解された個数(最大256個) を戻り値にする.
//		分解後の文字列バッファは split_result[]
int	 split_str(char *s)
{
	strcpy(split_string,s);
	char *p=split_string;
	int cnt=0;
	int c;

	split_result[cnt++]=p;	//最初の位置.
	while(1) {
		c = *p;
		if(c==0) break;
		if(c==',') {
			*p=0;
			split_result[cnt++]=(p+1);	// ','の次の位置.
		}
		p++;
	}
	return cnt;
}

//
//	文字列(s)をカンマ(,)で２つまでに分解する.
//	-----------------------------------------------
//	分解された個数(最大2個) を戻り値にする.
//	分解された文字列は op1とop2になる.
int	 split_str2(char *s)
{
	char *p=opbuf;
	int c;
	if(str_index(s,',')<0) return 1;

	strcpy(opbuf,s);
	op1=opbuf;
	while(1) {
		c = *p;
		if(c==0) break;
		if(c==',') {
			*p=0;op2=&p[1];return 2;
		}
		p++;
	}
	op2=p;
	return 1;
}

//
//	asm命令(op) を cnt個生成する.
//
void op_execN(char *op,char *cnt)
{
	int n=1;
	int i;
	int rc = sscanf(cnt,"%d",&n);
	if(rc==0) {
		Error("OP cnt(%s)",cnt);
	}
	for(i=0;i<n;i++) {
		op_exec(op,"1");
	}
}
/** *********************************************************************************
 *	１文字(c)が１６進数なら、数値に変換する.
 ************************************************************************************
 *	エラーの場合は (-1)
 */
int	is_hex(int c)
{
	if((c>='0')&(c<='9')) return c-'0';
	if((c>='A')&(c<='F')) return c-'A'+10;
	if((c>='a')&(c<='f')) return c-'a'+10;
	return -1;
}
/** *********************************************************************************
 *	文字列(src)を数値、もしくは１６進数値に変換して(*val)に入れる.
 ************************************************************************************
 *	最後に'H'が付いているものだけ１６進数値に変換する.
 *
 *	成功すれば(1) 失敗すれば(0)を返す.
 */
int	is_numhex(char *src,int *val)
{
	int d=0,hex=0;
	int c,x,hexf=0;

	c = *src++;
	d = is_hex(c);hex=d;
	if(d>=10) hexf=1;

	if(d==(-1)) return 0;	// Error
	while(1) {
		c = *src++;
		if(c==0) {
			if(hexf==0) {
				*val = d;	// 10進.
				return 1;	// OK
			}
			return 0;		// Error
		}
		if((c=='h')||(c=='H')) {
			if(*src==0) {
				*val = hex;
				return 1;	// OK
			}
		}
		x = is_hex(c);
		if(x == (-1)) {
			return 0;		// Error
		}
		if(x>=10) hexf=1;
		d=d*10+x;
		hex=hex*16+x;
	}
}
/** *********************************************************************************
 *	オペランドを解析して、実効アドレス(*val) を求める.
 ************************************************************************************
 *	modeはアドレスモードを入れる.
 *	成功すれば真(非ゼロ)を返す.
 */
int eval(char *src,int *val,int *mode)
{
	int rc;
	static char srcbuf[256];
	int	len;
	char *s = srcbuf;
	int c;
//	int rp=0;
	strcpy(srcbuf,src);
	len=strlen(srcbuf);

	*mode = XX_IMM8;

	if(*s=='*') {
		*mode = XX_IND8;
		s++;
	}

	if(*s=='[') {
//		*mode = XX_REG8;
		*mode = XX_IND8;
		if(	srcbuf[len-1]==']') {
			srcbuf[len-1]=0;
		}
		s++;
	}

	c = tolower(*s);
	// $ffff
	if(c=='$') {
		rc = sscanf(s+1,"%x",val);
		return rc;
	}
	// r10h
	if((c=='r') && (is_numhex(s+1,val))){
		*val |= IS_REG;
		rc=1;
		if(	*mode != XX_IND8) {
			*mode  = XX_REG8;
		}
		return rc;
	}
	// 0xaaaa
	if((c=='0') && (s[1]=='x')) {
		rc = sscanf(s+2,"%x",val);
		return rc;
	}
	// 1234 || aaffh
	if(is_numhex(s,val)){
		rc=1;
		return rc;
	}
	//	label
	if( hash_search(sym,s,val) ) {
		if(	*val & IS_REG) {		// r16 などと等価なレジスタラベル.
			if(	*mode == XX_IMM8) {	// IMMのままだったら REGに書き換え.
				*mode  = XX_REG8;
			}
		}//						そうでない場合は定数ラベル.
		return 1;
	}else{
		if(s_pass) {
			Error("Symbol Not Found(%s)",s);	//未定義ラベル.
		}
		return 0;
	}
	return 0;
}
/** *********************************************************************************
 *	オペコードを解析して、実効アドレス(ea) を求める.
 ************************************************************************************
 *	といいつつ、実効アドレスの評価結果（つまりオペランド）を返す.
 */
int	opadrs(int code)
{
	int ea = 0;
	int xx = code & XX_MASK;
	// アドレッシング修飾子(xx)
	switch(xx) {
	 case XX_IMM8 :	ea = code & 0xff;		break;
	 case XX_REG8 :	ea = memory[code & 0xff];break;
	 case XX_IND8 :	ea = memory[code & 0xff];
	 				ea = memory[ea];		 break;
	 case XX_IMM16:	ea = memory[ reg.pc++ ]; break;
	 default:	break;
	}
	return ea;
}
/** *********************************************************************************
 *	こっから下は、命令の実行を行なう.
 ************************************************************************************
 {"JMP","JMP always"  ,"00xxdd00"  ,f_JMP,d_JMP},//-,-,-,無条件に分岐する
 */
int f_JMP (int code,OPCODE *tab)
{
	int val=0,mode,rc;
	rc = eval(operand,&val,&mode);
	if(mode == XX_IND8) {
		code = JMP_ind<<11;		// [imm]
	}
	dw( code|val );
	(void)rc;
	return 0;
}
//	擬似命令.
int f_ORG (int code,OPCODE *tab)
{
	int val=0,mode,rc;
	rc = eval(operand,&val,&mode);
	if(rc>0) {
		reg.pc = val;
		reg.pc_bak = val;
	}
	return 0;
}
int f_EQU (int code,OPCODE *tab)
{
	int val=0,mode,rc;
	rc = eval(operand,&val,&mode);
	if(rc>0) {
//		set_label(label,val);
		hash_insert(sym,label,val,1);
	}
	(void)rc;
	return 0;
}
int f_DW (int code,OPCODE *tab)
{
	int i,val,mode,rc;
	int cnt=split_str(operand);
	for(i=0;i<cnt;i++) {
		rc = eval(split_result[i],&val,&mode);
		if(rc>0) {
			dw(val);
		}else{
			Error("DW value(%s)",split_result[i]);
		}
	}
	return 0;
}
int f_DOT (int code,OPCODE *tab)
{
	int i,val,mode,rc,high=0;
	int cnt=split_str(operand);
	for(i=0;i<cnt;i++) {
		rc = eval(split_result[i],&val,&mode);
		if(rc>0) {
			if(i & 1) {
				dw(high | val);
			}else{
				high=(val<<4);
			}
		}else{
			Error("DW value(%s)",split_result[i]);
		}
	}
	if(cnt & 1) {
		dw(high | 0);
	}
	return 0;
}
int f_END (int code,OPCODE *tab)
{
	return 0;
}
int	isReg(char *name)
{
	if(str_cmpi(name,"A")==0) {return REG_A;}
	if(str_cmpi(name,"B")==0) {return REG_B;}
	if(str_cmpi(name,"C")==0) {return REG_C;}
	return -1;
}
int move_reg_ind(int r1,char *op2)
{
	int code=0;
	int val=0,mode,rc;
	rc = eval(op2,&val,&mode);
	switch(r1) {
		case REG_A:	code=MOV_A_ind;break;
		case REG_B:	code=MOV_B_ind;break;
		case REG_C:	code=MOV_C_ind;break;
		default:Error("Internal Error");break;
	}
	dw( (code<<11) | val);
	(void) rc;
	return 0;
}
int move_reg_imm(int r1,char *op2)
{
	int code=0;
	int val=0,mode,rc;
	rc = eval(op2,&val,&mode);
	switch(r1) {
		case REG_A:	code=MOV_A_imm;break;
		case REG_B:	code=MOV_B_imm;break;
		case REG_C:	code=MOV_C_imm;break;
		default:Error("Internal Error");break;
	}
	dw( (code<<11) | val);
	(void) rc;
	return 0;
}
int move_ind_reg(char *op1,int r2)
{
	int code=0;
	int val=0,mode,rc;
	rc = eval(op1,&val,&mode);
	switch(r2) {
		case REG_A:	code=MOV_ind_A;break;
		case REG_C:	code=MOV_ind_C;break;
		default:Error("Internal Error");break;
	}
	dw( (code<<11) | val);
	(void) rc;
	return 0;
}
int f_MOV (int code,OPCODE *tab)
{
	int n = split_str2(operand);
	if( n!=2 ) Error("Need 2 operand!");
	// 第一引数はA,B,Cのどれか？
	int r1=isReg(op1);
	if(	r1>=0 ) {
		if(op2[0] == '[') {
			return move_reg_ind(r1,op2);
		}else{
			return move_reg_imm(r1,op2);
		}
	}
	int r2=isReg(op2);
	if(	r2>=0 ) {
		if(r2!=REG_B) {
			return move_ind_reg(op1,r2);
		}
	}
	Error("Invalid MOV Operand!");
	return 0;
}
int	add_a_b()
{
	int code=ADD_A_B;
	dw( (code<<11) );
	return 0;
}
int	sub_a_b()
{
	int code=SUB_A_B;
	dw( (code<<11) );
	return 0;
}
int	add_c_imm(char *op2)
{
	int code=ADD_C_imm;
	int val=0,mode,rc;
	rc = eval(op2,&val,&mode);
	dw( (code<<11)|val );
	(void) rc;
	return 0;
}
int f_ADD (int code,OPCODE *tab)
{
	int n=split_str2(operand);
	if(n!=2) Error("Need 2 operand!");

	// 第一引数はA,B,Cのどれか？
	int r1=isReg(op1);
	int r2=isReg(op2);
	if(	r1 == REG_A ) {
		if(	r2 == REG_B ) {
			return add_a_b();
		}
	}
	if(	r1 == REG_C ) {
		return add_c_imm(op2);
	}
	Error("Invalid ADD Operand!");
	return 0;
}
int f_SUB (int code,OPCODE *tab)
{
	int n=split_str2(operand);
	if(n==0) {	//引数無しも許容する.
		return sub_a_b();
	}
	// 第一引数はA,B,Cのどれか？
	int r1=isReg(op1);
	int r2=isReg(op2);
	if(	r1 == REG_A ) {
		if(	r2 == REG_B ) {
			return sub_a_b();
		}
	}
	Error("Invalid SUB Operand!");
	return 0;
}

/*
int f_CP (int code,OPCODE *tab)
{
	int n = split_str2(operand);
	if(n!=2) Error("Need 2 operand!");
//	op2-op1にしているのは Borrowになっているから.
	op_exec("ld" ,op2);		// src
	op_exec("sub",op1);		// dst
	return 0;
}
*/

//	------------------------------
//	CALL adr は２命令に展開される.
//		MOV C,pc+2
//		JMP adr
//	------------------------------
//	subr先頭で、C regを work3にMOVする必要がある.
//	CALLのネストを行う場合はsubr内で、work3をどこかに保存する必要もある.
//
int f_CALL (int code,OPCODE *tab)
{
	int val=0,mode,rc;
	rc = eval(operand,&val,&mode);
	if(mode == XX_IND8) {
		code = JMP_ind<<11;		// [imm]
	}
//	printf("1 code=%x,val=%x\n",code,val);
	char nextpc[64];
	sprintf(nextpc,"C,0x%x",reg.pc+2);
	op_exec("MOV" ,nextpc);		//

//	printf("2 code=%x,val=%x\n",code,val);
	dw( code|val );
	(void)rc;
	return 0;
}
//	------------------------------
//	RET は１命令に展開される.
//		JMP [work3]
//	------------------------------
int f_RET (int code,OPCODE *tab)
{
	op_exec("JMP" ,USE_CALL_WORK);		//
	return 0;
}
int f_OUT (int code,OPCODE *tab)
{
	dw( (OUT_A<<11) );
	return 0;
}
/** *********************************************************************************
 *
 ************************************************************************************
 */
