#include <string.h>

#define OK 0
#define ERROR -1
#define MAX_LENGTH 32 //正则式最大字符长度
#define MAX_VERTEX_NUM 50

typedef struct ArcCell{
	bool adj;		/*对于无权图，用0或1表示相邻与否*/
	char info;		/*弧的信息指针*/
}ArcCell, AdjMartrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
//定义有向图
typedef struct {
	char ID;
	int vexs[MAX_VERTEX_NUM];
	AdjMartrix arcs;	/*邻接矩阵*/
	int vexnum;		/*图当前的顶点数*/
	int start,end;		/*指示图的开始与结束节点*/ 
	int isEnd[MAX_VERTEX_NUM];	//对于有不止一个终态的图启用这项代替end
}mGraph;

typedef struct {	
	int Table[20][20];
	int States[20];	/*DFA状态集*/
	bool State_isFinal[20];	/*终态标识*/
	int start;		/*指示图的开始节点*/ 
	int statesnum;		/*记录状态的个数*/
}DFA;


//====================================================================//
#define STACK_INIT_SIZE 50	//存储空间初始分配量
#define STACK_INCREMENT 10	//分配增量

typedef struct {
	char *base;		/*栈构造之前和销毁之后，栈底指针为NULL*/
	char *top;
	int stacksize;
}SqStack;

int InitStack(SqStack &S){
	S.base = (char *)malloc(STACK_INIT_SIZE*sizeof(char));
	if(!S.base) exit(ERROR);	//存储分配失败
	S.top = S.base;
	S.stacksize = STACK_INIT_SIZE;
	return OK;
}
int GetTop(SqStack S, char &e){
	if(S.top==S.base) return ERROR;
	e = *(S.top-1);
	return OK;
}
int Push(SqStack &S, char e){
	if(S.top-S.base>= S.stacksize){	//栈满，追加存储空间
		S.base = (char*)realloc(S.base,
			(S.stacksize + STACK_INCREMENT)*sizeof(char));
		if(!S.base) exit(ERROR);	//存储分配失败
		S.top = S.base+S.stacksize;
		S.stacksize +=STACK_INCREMENT;
	}
	*S.top++ =e;
	return OK;
}
int Pop(SqStack &S,char &e){
	if(S.top==S.base) return ERROR;
	e = *--S.top;
	return OK;
}
bool isEmpty(SqStack &S){
	if(S.top==S.base) return true;
	else{
		return false;
	}
}
