/** *********************************************************************************
 *	R11 アセンブラ.
 ************************************************************************************
 */

/*
ＴｏＤｏ

[O]	-	行分解
	-	ラベル
	-	ニモニック
	-	オペランド
	-	リスト出力
	-	バイナリ出力

注意:
	２パスアセンブラなので、
	-	１パス目はアドレス確定のみ行なう.
	-	２パス目で完全なバイナリーを確定する.
	
	-	つまり、１パス目と２パス目で、命令を落とす処理が変わってはならない
		（アドレスがずれてはならない）
	-	特に、１パス目は未定義ラベル等のためにアドレス不確定となるけれども
		２パス目と同一の振る舞いをしなければならない（生成コードが違っても良いがアドレスがずれるのはだめ）

 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "opcode.h"
#include "hash.h"


OPCODE opcode_init_tab[]={
 //ニモ,意味					,機械語		 ,f_emu,d_dis  //z,c,m,動作 --------------------
 {"MOV","MOVE" 			        ,0   	,f_MOV,0},//-,-,-,
 {"ADD","ADD"                   ,ADD_A_B,f_ADD,0},//?,?,?,加算
 {"SUB","SUBtract"              ,SUB_A_B,f_SUB,0},//?,?,?,減算
 {"JMP","JMP always"            ,JMP_imm,f_JMP,0},//-,-,-,無条件に分岐する
 {"JNC","Jump if Not Carry"     ,JNC_imm,f_JMP,0},//-,-,-,キャリー無(ボロー有)なら分岐
 {"OUT","OUTPUT console"        ,OUT_A  ,f_OUT,0},
 //擬似命令
 {"ORG",".Set Origin"   ,0,f_ORG,0},
 {"EQU",".EQU" 		    ,0,f_EQU,0},
 {"DW" ,".DW" 		    ,0,f_DW ,0},
 {"DOT" ,".DOT" 	    ,0,f_DOT,0},
 {"END",".END" 		    ,0,f_END,0},
 //疑似マクロ.
#if	USE_CALL_MACRO
 {"CALL",".CALL"        ,JMP_imm,f_CALL,0},
 {"RET" ,".RET"         ,JMP_ind,f_RET,0},
#endif
//ニモ,意味				,機械語	   ,f_emu,d_dis  //z,c,m,動作 --------------------
 { NULL,NULL,0}
};

FILE *ifp;
FILE *ofp;
FILE *lfp;

char  *s_infile;	//	ソースファイル名.
int	   lnum;		//	行番号.
char   line[1024];
char  *lptr;
int		s_pass;
int		s_lopt;		// '-l' ?

#define Ropen(name) {ifp=fopen(name,"rb");if(ifp==NULL) \
{ printf("Fatal: can't open file:%s\n",name);exit(1);}  \
}
#define Wopen(name) {ofp=fopen(name,"wb");if(ofp==NULL) \
{ printf("Fatal: can't create file:%s\n",name);exit(1);}  \
}
#define Lopen(name) {lfp=fopen(name,"wb");if(lfp==NULL) \
{ printf("Fatal: can't create file:%s\n",name);exit(1);}  \
}

#define Write(buf,siz)  fwrite(buf,1,siz,ofp)

#define Wclose()  fclose(ofp)
#define Lclose()  fclose(lfp)
#define Rclose()  fclose(ifp)


void dw(int data)
{
	memory[reg.pc++]=data;
	
	if( save_ptr < reg.pc ) {
		save_ptr = reg.pc;
	}
}

void Wsave(void)
{
	Write(memory,save_ptr*2);
}

/** *********************************************************************************
 *	OPCODE構造体の pattern文字列 から data(命令コード)を作る.
 ************************************************************************************
 */
void make_data(OPCODE *t)
{
	t->data = (t->pattern << 11);
#if	0
	char *pat = t->pattern;
	int i,sbit;
	int m=0x8000;
	int code=0;
	for(i=0;i<8;i++,m>>=1) {
		sbit = *pat++;
		if(sbit == '1') {
			code |= m;
		}
	}
	t->data = code;
#endif
}
/** *********************************************************************************
 *
 ************************************************************************************
 */
void opcode_init()
{
	OPCODE *t = opcode_init_tab;
	while(t->mnemonic) {
		make_data(t);
		t++;
	}
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
int	str_cmpi(char *t,char *s)
{
	while(*t) {
		if(tolower(*t)!=tolower(*s)) return 1;
		t++;
		s++;
	}
	if(*s) return 1;
	return 0;
}

/** *********************************************************************************
 *	
 ************************************************************************************
 */
void Error(char *format, ...)
{
	va_list argptr;
	char buffer[256];

	if(s_pass == 0) return;
	
	va_start(argptr, format);
	vsnprintf(buffer, sizeof(buffer), format, argptr);

	fprintf(stderr,"FATAL:%s\n",buffer);
	fprintf(stderr,"%s:%d:%s\n",s_infile,lnum,line);

	fprintf(lfp,"FATAL:%s\n",buffer);
	fprintf(lfp,"%s:%d:%s\n",s_infile,lnum,line);

	va_end(argptr);

//	exit(1);
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
OPCODE *search_code(char *mne)
{
	OPCODE *t = opcode_init_tab;
	while(t->mnemonic) {
		if(str_cmpi(mne,t->mnemonic)==0) return t;
		t++;
	}
	return NULL;
}

//OPCODE UNDEFINED_OPCODE=
// {"???","???","--------",f_und,d_und};
//	xx	アドレッシング
//	00	Imm8
//	01	Reg8
//	10	[Reg8]
//	11	Imm16

//	dd	ディスプレイスメント
//	00	Abs
//	01	fwd
//	10	back
//	11	fwd+carry



/** *********************************************************************************
 *	ファイル(fp)から１行分の文字列を(buf)に読み込む.
 ************************************************************************************
 *	成功すると0
 *	文字数オーバー=1
 *	EOFに達するとEOFを返す.
 */
int getln(char *buf,FILE *fp)
{
	int c;
	int l;
	l=0;
	while(1) {
		c=getc(fp);
		if(c==EOF)  return(EOF);/* EOF */
		if(c==0x0d) continue;
		if(c==0x0a) {
			*buf = 0;	/* EOL */
			return(0);	/* OK  */
		}
		*buf++ = c;l++;
		if(l>=255) {
			*buf = 0;
			return(1);	/* Too long line */
		}
	}
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
int	is_spc(int c)
{
	if(c == '\t') return 1;
	if(c == ' ') return 1;
	return 0;
}
/** *********************************************************************************
 *
 ************************************************************************************
 */
char *spskip(void)
{
	while(*lptr) {
		if(is_spc(*lptr)==0) break;
		lptr++;
	}
	return lptr;
}
/** *********************************************************************************
 *	行バッファから、単語を１つ取得する.
 ************************************************************************************
 */
char *getsym(char *buf)
{
	char *t = buf;
	*t=0;
	spskip();
	while(*lptr) {
		if(is_spc(*lptr)) break;
		*t++ = *lptr++;
	}
	*t=0;
	return buf;
}
int is_comment(char *symbuf)
{
	int c = *symbuf;
	if((c==0)||(c==';')||(c=='#')) return 1;	//コメント.
	if((c=='/')&&(symbuf[1]=='/')) return 1;	//コメント.
	return 0;
}
/** *********************************************************************************
 *	アセンブラ：１行の文字列から、ラベル: ニモニック オペランドに分解する.
 ************************************************************************************
 */
int	asm_pre(char *line,int pass)
{
	int c,l;
//	printf("%d:%s\n",lnum,line);
	getsym(symbuf);
	c = *symbuf;
	if(is_comment(symbuf)) {return 0;}
	(void)c;

	l=strlen(symbuf);
	if((symbuf[l-1] == ':')||(is_spc(*line)==0)) {
		strcpy(label,symbuf);
		if(symbuf[l-1] == ':') {
			label[l-1]=0;			// ':'  を取る.
		}
		getsym(symbuf);
	}
	
	if(is_comment(symbuf)) {return 1;}
	strcpy(mnemonic,symbuf);	//ニモニック取得.
	
	getsym(symbuf);
	if(is_comment(symbuf)) {return 2;}
	strcpy(operandbuf,symbuf);		//オペランド取得.

	return 3;
}

/** *********************************************************************************
 *	
 ************************************************************************************
 */
int	set_label(char *label,int val)
{
	if(label) {
		if(label[0]) {
			if(label[0]!=' ') {
				hash_insert(sym,label,reg.pc,1);
			}
		}
	}
	return 0;
}
void add_spc(char *buf,int spc)
{
	int len = strlen(buf);
	int i = spc - len;
	if( i <= 0) return;

	buf += len;
	while(i>=0) {
		*buf++ = ' ';
		i--;
	}
	*buf = 0;
}
/** *********************************************************************************
 *	
 ************************************************************************************
 */
int	asm_list(int pass)
{
	char hexbuf[256]="";
	char label1[256];
	char *t = hexbuf;
	WORD *d = &memory[reg.pc_bak];
	int  m = reg.pc - reg.pc_bak;
	int	i;

	if(pass == 0) return 0;		//１パスと２パスでアドレスがずれるときは、この行を
								//コメントアウトの状態で２個のリストを出してみる.
	if(m>=12) m=12;
	for(i=0;i<m;i++) {
		sprintf(t,"%04x ",*d++);
		t+=5;
	}
	if(s_lopt) { add_spc(hexbuf,32);}

	strcpy(label1,label);
	if(label1[0]) {
		strcat(label1,":");
	}
	if((label1[0]==0)&&(mnemonic[0]==0)) {
		fprintf(lfp,"%5s %-10s%s\n","",hexbuf,line);
	}else{
		fprintf(lfp,"%04x:%-10s%-10s %-4s %s %s\n"
			,reg.pc_bak,hexbuf,label1,mnemonic,operandbuf,comment);
	}
	return 0;
}

int	op_exec(char *inst,char *oper)
{
	OPCODE *p = search_code(inst);
	if(p==NULL) {
		Error("Mnemonic error(%s)",inst);
	}else{
		operand = oper;
		p->emufunc(p->data,p);	//ニモニックの各処理.
	}
	return 0;
}
/** *********************************************************************************
 *	アセンブラ：１行の処理.
 ************************************************************************************
 */
int	asm_line(char *line,int pass)
{
	int pre = asm_pre(line,pass);

	reg.pc_bak = reg.pc;

	strcpy(comment,lptr);
	if(pre<1) {
		return pre;				//コメントだけの行.
	}
	
	set_label(label,reg.pc);	//ラベル:
	if(mnemonic[0]==0) {
		return 0;				//ラベル:  だけの行.
	}

	op_exec(mnemonic,operandbuf);

	return 0;
}
/** *********************************************************************************
 *	
 ************************************************************************************
 */
void init_line(void)
{
	lptr = line;
		symbuf[0]  =0;
		label[0]   =0;
		mnemonic[0]=0;
		operandbuf[0] =0;
}
/** *********************************************************************************
 *	アセンブラ：１パスの処理.
 ************************************************************************************
 */
int	asm_pass(char *infile,int pass)
{
	int rc;
	lnum = 1;

	s_pass = pass;

	reg.pc=0;

	Ropen(infile);
	while(1) {
		rc = getln(line,ifp);
		if(rc == EOF) break;
		init_line();
		asm_line(line,pass);
		asm_list(pass);

		lnum++;
	}
	Rclose();
	return 0;
}

/** *********************************************************************************
 *	アセンブラ：シンボル表示.
 ************************************************************************************
 */
int printsym_func(char *p,int v)
{
	char *is_reg="";
	if(v & IS_REG) is_reg="R";
	v &= ~IS_REG;
	fprintf(lfp,"%-16s = %s%d\t(0x%x)\n",p,is_reg,v,v);
	return 0;
}
void asm_printsym(void)
{
	fprintf(lfp,"\n\nSYMBOL LIST:\n");
	hash_iterate(sym,printsym_func);
}

/** *********************************************************************************
 *	アセンブラ：メインルーチン
 ************************************************************************************
 */
int	r16_asm(char *infile,char *outfile,char *listfile,int lopt)
{
	int i;
	sym = hash_create(0x1000);
	opcode_init();
	s_infile=infile;
	s_lopt = lopt;
	save_ptr=0;

	Lopen(listfile);
	Wopen(outfile);
	for(i=0;i<3;i++) {		//今のところ適当.
		asm_pass(infile,0);
	}
	asm_pass(infile,1);
	Wsave();
	Wclose();

	asm_printsym();

	Lclose();
	return 0;
}
/** *********************************************************************************
 *
 ************************************************************************************
 */
