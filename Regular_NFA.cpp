#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NFA.h"
#include "NFA_DFA.cpp"

#define OK 0
#define ERROR -1
#define MAX_LENGTH 32 //正则式最大字符长度

int state_ID = 0;
char subNFA_ID = 'a';
char inputSymbol[MAX_LENGTH];	//用户输入符号集

//函数声明
void Input_andCheck(char str[MAX_LENGTH]);
void Improve_Operator(char str[MAX_LENGTH], char Perfect_Regular[MAX_LENGTH*2]);
int Convert_SuffixExpression(char Perfect_Regular[MAX_LENGTH*2], char mSuffix_exp[MAX_LENGTH*2]);
int Constructor_NFA(char mSuffix_exp[MAX_LENGTH*2], mGraph *NFA);
void Constructor_a(mGraph *mcharNFA,char c);
void Constructor_connect(mGraph *m1_NFA,mGraph *m2_NFA);
void Constructor_or(mGraph *m1_NFA,mGraph *m2_NFA);
void Constructor_closure(mGraph *m1_NFA);
void Output(mGraph *mG);
bool isLetter(char c);

int main(){
	mGraph NFA;
	char mRegular_exp[MAX_LENGTH];
	char Perfect_Regular[MAX_LENGTH*2];
	char mSuffix_exp[MAX_LENGTH*2];	//后缀表达式接收数组
	Input_andCheck(mRegular_exp);
	//strcpy(mRegular_exp, "((a|b))|c");
	//strcpy(mRegular_exp, "ab((a|b)*|c*)d");
	//strcpy(mRegular_exp, "abc((ab)*|(c*|d))efg");
	Improve_Operator(mRegular_exp, Perfect_Regular);		//添加连接符号比如:abc -> a.b.c
	Convert_SuffixExpression(Perfect_Regular, mSuffix_exp);		//将正则式转换为后缀表达式
	Constructor_NFA(mSuffix_exp, &NFA);
	Output(&NFA);

	NFA_to_DFA(&NFA,inputSymbol);
	system("pause");
	return 0;
}

/*输入并检查，检查是否属于正则表达式*/
void Input_andCheck(char str[MAX_LENGTH]){
	bool isRegular;
	do{
		isRegular = true;
		printf("Input Regular_Expression: ");
		scanf("%s", str);
		printf("----The length: %d\n", strlen(str));
		int length = strlen(str);
		int i;
		for(i=0; i<length; i++){
			char a = str[i];
			if(a>='a'&&a<='z'|| a>='A'&&a<='Z'|| a>='0'&&a<='9'|| a=='*'|| a=='|'|| a=='('|| a==')'){}
			else{
				printf("ERROR----> %c\n",a);
				isRegular = false;
			}
		}
		if(isRegular==false){
			printf("Sorry!! your input is NOT Regular_Expression!\n");
			printf("=================================================\n");
		}
	}while(isRegular==false);
	//形成并存储输入符号集合
	int i,j,n=0;
	bool hasExist = false;
	for(i=0; i<strlen(str); i++){
		if(isLetter(str[i])){
			for(j=0;j<strlen(inputSymbol); j++){
				if(str[i] == inputSymbol[j]) hasExist=true;
			}
			if(hasExist == false){
				inputSymbol[n++] = str[i];
			}
			hasExist = false;
		}
	}
	inputSymbol[n] = '\0';
	printf(">>>>>%s \n",inputSymbol);
}

/*完善正则表达式，添加连接符号.*/
void Improve_Operator(char str[MAX_LENGTH], char Perfect_Regular[MAX_LENGTH*2]){
	int i;
	int n=0;
	for(i=0; i<strlen(str); i++){
		char c = str[i];
		Perfect_Regular[n++] = c;
		Perfect_Regular[n] = '\0';
		if(isLetter(c)) {
			c = str[++i];
			if(c=='(' || isLetter(c)){
				Perfect_Regular[n++] = '.';
				Perfect_Regular[n] = '\0';
			}
			i--;
		}else if(c==')' || c=='*'){
			c = str[++i];
			if(isLetter(c)){
				Perfect_Regular[n++] = '.';
				Perfect_Regular[n] = '\0';
			}
			i--;
		}
	}
	printf("Improve_Operator=======%s  %s\n",str,Perfect_Regular);
}

/*将完善后的表达式转换为---后缀表达式--*/
int Convert_SuffixExpression(char Perfect_Regular[MAX_LENGTH*2], char mSuffix_exp[MAX_LENGTH*2]){
	SqStack mOperatorStack;	//运算符栈
	if(InitStack(mOperatorStack)==-1){
		return 0;
	}
	int i,n=0;
	char mChar;
	for(i=0;i<=strlen(Perfect_Regular);i++){
		char c = Perfect_Regular[i];
		if(isLetter(c)==true || c=='*'){
			mSuffix_exp[n++] = c;
			mSuffix_exp[n] = '\0';
		}else if(c=='.' || c=='(' || c=='|'){
			Push(mOperatorStack, c);
		}else if(c==')'){
			mChar = 'a';
			while(mChar !='('){
				Pop(mOperatorStack, mChar);
				if(mChar !='('){
					mSuffix_exp[n++] = mChar;
					mSuffix_exp[n] = '\0';
				}
			}
			
			//括号内全部出栈
		}else if(c=='\0'){
			while(!isEmpty(mOperatorStack)){
				Pop(mOperatorStack, mChar);
				mSuffix_exp[n++] = mChar;
				mSuffix_exp[n] = '\0'; 
			}
		}
	}
	printf("Convert_SuffixExpression=======%s  lenght:%d\n",mSuffix_exp,strlen(mSuffix_exp));
	return 0;
}

int Constructor_NFA(char mSuffix_exp[MAX_LENGTH*2], mGraph *NFA){
	int i,m=0;
	char m1_ID,m2_ID;
	mGraph mSGraph[MAX_LENGTH];
	SqStack mNFAStack;		//字符表达式或NFA栈
	if(InitStack(mNFAStack)==-1){
		return 0;
	}
	for(i=0;i<=strlen(mSuffix_exp);i++){
		char c = mSuffix_exp[i];
		if(isLetter(c)){
			//Push NFA
			Constructor_a(&mSGraph[m],c);
			Push(mNFAStack,mSGraph[m++].ID);
		}else if(c=='.'){
			//Pop 2--> Push
			Pop(mNFAStack, m2_ID);
			Pop(mNFAStack, m1_ID);
			Constructor_connect(&mSGraph[m1_ID-'a'],&mSGraph[m2_ID-'a']);
			Push(mNFAStack,m1_ID);
		}else if(c=='*'){
			//Pop 1--> Push
			Pop(mNFAStack, m1_ID);
			Constructor_closure(&mSGraph[m1_ID-'a']);
			Push(mNFAStack,m1_ID);
		}else if(c=='|'){
			//Pop 2--> Push
			Pop(mNFAStack, m2_ID);
			Pop(mNFAStack, m1_ID);
			Constructor_or(&mSGraph[m1_ID-'a'],&mSGraph[m2_ID-'a']);
			Push(mNFAStack,m1_ID);
		}else if(c=='\0'){
			//Pop
			Pop(mNFAStack, m1_ID);
			*NFA = mSGraph[m1_ID-'a'];
		}
	}
	return 0;
}
/*构造单个字母数字的小NFA*/
void Constructor_a(mGraph *mcharNFA,char c){
	int v1 = mcharNFA->vexnum;
	printf("Constructor_a=======%d\n",mcharNFA->vexnum);
	mcharNFA->vexs[mcharNFA->vexnum++] = state_ID++;
	mcharNFA->vexs[mcharNFA->vexnum] = '\0';  //添加结束符，防止输出混乱

	int v2 = mcharNFA->vexnum;
	mcharNFA->vexs[mcharNFA->vexnum++] = state_ID++;
	mcharNFA->vexs[mcharNFA->vexnum] = '\0';  //添加结束符，防止输出混乱

	mcharNFA->ID = subNFA_ID++;
	mcharNFA->arcs[v1][v2].adj = 1;
	mcharNFA->arcs[v1][v2].info = c;
	if(mcharNFA->start==-1){		//如果开始节点尚未标记，证明第一次构造
		mcharNFA->start = v1;
	}
	mcharNFA->end = v2;
}

void Constructor_connect(mGraph *m1_NFA,mGraph *m2_NFA){
	int i,j;
	int mvex = m1_NFA->vexnum;
	for(i=0;i<m2_NFA->vexnum;i++){	//复制m2_NFA的节点和信息到m1
		m1_NFA->vexs[m1_NFA->vexnum++] = m2_NFA->vexs[i];
		for(j=0;j<m2_NFA->vexnum;j++){
			m1_NFA->arcs[mvex+i][mvex+j].adj = m2_NFA->arcs[i][j].adj;
			m1_NFA->arcs[mvex+i][mvex+j].info = m2_NFA->arcs[i][j].info;
		}
	}
	int m1start = m1_NFA->start;
	int m1end = m1_NFA->end;
	int m2start = m2_NFA->start + mvex;	/*注意加上mvex*/
	int m2end = m2_NFA->end + mvex; /*注意加上mvex*/
	m1_NFA->arcs[m1end][m2start].adj = 1;
	m1_NFA->arcs[m1end][m2start].info = '-';

	m1_NFA->end = m2end;		//修改m1_NFA的end信息
}

void Constructor_or(mGraph *m1_NFA,mGraph *m2_NFA){
	int i,j;
	int mvex = m1_NFA->vexnum;
	for(i=0;i<m2_NFA->vexnum;i++){	//复制m2_NFA的节点和信息到m1
		m1_NFA->vexs[m1_NFA->vexnum++] = m2_NFA->vexs[i];
		for(j=0;j<m2_NFA->vexnum;j++){
			m1_NFA->arcs[mvex+i][mvex+j].adj = m2_NFA->arcs[i][j].adj;
			m1_NFA->arcs[mvex+i][mvex+j].info = m2_NFA->arcs[i][j].info;
		}
	}
	int m1start = m1_NFA->start;
	int m1end = m1_NFA->end;
	int m2start = m2_NFA->start + mvex;
	int m2end = m2_NFA->end + mvex;
	m1_NFA->arcs[m1start][m2start].adj = 1;
	m1_NFA->arcs[m1start][m2start].info = '-';
	m1_NFA->arcs[m2end][m1end].adj = 1;
	m1_NFA->arcs[m2end][m1end].info = '-';
}

void Constructor_closure(mGraph *m1_NFA){
	int v1 = m1_NFA->vexnum;
	m1_NFA->vexs[m1_NFA->vexnum++] = state_ID++;
	m1_NFA->vexs[m1_NFA->vexnum] = '\0';  //添加结束符，防止输出混乱

	int v2 = m1_NFA->vexnum;
	m1_NFA->vexs[m1_NFA->vexnum++] = state_ID++;
	m1_NFA->vexs[m1_NFA->vexnum] = '\0';  //添加结束符，防止输出混乱

	m1_NFA->arcs[v1][m1_NFA->start].adj = 1;	//连接三条线，条件是空值‘-’
	m1_NFA->arcs[v1][m1_NFA->start].info = '-';
	m1_NFA->arcs[m1_NFA->end][m1_NFA->start].adj = 1;
	m1_NFA->arcs[m1_NFA->end][m1_NFA->start].info = '-';
	m1_NFA->arcs[m1_NFA->end][v2].adj = 1;
	m1_NFA->arcs[m1_NFA->end][v2].info = '-';
	m1_NFA->arcs[v1][v2].adj = 1;
	m1_NFA->arcs[v1][v2].info = '-';

	m1_NFA->start = v1;
	m1_NFA->end = v2;
}

/*打印输出NFA顶点和矩阵信息*/
void Output(mGraph *mG){
	int length = mG->vexnum;
	printf("Output length> %d(vexnum) \n",length);
	printf("\n\n\n****************-------------****************|\n");
	int i,j;
	printf(" ");
	for(i=0;i<length;i++){		//打印节点
		printf("  %2d",mG->vexs[i]);
	}
	printf("\n");
	for(i=0;i<length;i++){
		printf("%2d ",mG->vexs[i]);
		for(j=0;j<length;j++){
			printf("%2d  ",mG->arcs[i][j].adj);	//打印连接与否
		}
		printf("\n");
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	printf(" ");
	for(i=0;i<length;i++){
		printf("  %2d",mG->vexs[i]);
	}
	printf("\n");
	for(i=0;i<length;i++){
		printf("%2d ",mG->vexs[i]);
		for(j=0;j<length;j++){
			printf("%2c  ",mG->arcs[i][j].info);	//打印连接信息
		}
		printf("\n");
	}
	printf("start:%d   end:%d\n",mG->vexs[mG->start],mG->vexs[mG->end]);
	printf("****************-------------****************|\n\n");
}

bool isLetter(char c){
	if(c!='*' && c!='|' && c!='.' && c!='(' && c!=')' &&c!='\0') {
		return true;
	}else return false;
}
