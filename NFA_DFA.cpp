#include <stdio.h>
#include <string.h>
#include "DFA_minimize.cpp"

#define MAX_LENGTH 32 //正则式最大字符长度
#define MAX_ELEMENTS 10
#define SUBSET_NUM 10

char subset_ID = 'P';

typedef struct {
	char _ID;	/*子集的ID*/
	int elements[MAX_ELEMENTS];	
	int elenum;
	bool mark;	/*标记*/
}mSubset;


//函数声明
void NFA_to_DFA();
int CheckGroup(mSubset mSubsets[SUBSET_NUM],int number);
void E_closure(mGraph *NFA, int beforeHandle[MAX_ELEMENTS], int *b, int tempSave[MAX_ELEMENTS], int *t);
void findEmptyState(mGraph *NFA,int tempSave[MAX_ELEMENTS], int *t, int x);
void MoveT_a(mGraph *NFA, int beforeHandle[MAX_ELEMENTS],int b, int tempSave[MAX_ELEMENTS],int *t, char c);
int judgeOldSubset(int UArray[MAX_ELEMENTS],int u, mSubset mSubsets[SUBSET_NUM],int number);

/*子集法将NFA转化为DFA*/
void NFA_to_DFA(mGraph *NFA, char inputSymbol[MAX_LENGTH]){
	int m=0;		//记录mSubset数组元素的个数
	mSubset mSubsets[SUBSET_NUM];
	int i,j;
	for(i=0;i<SUBSET_NUM;i++){
		mSubsets[i]._ID = ' ';
		mSubsets[i].elements[MAX_ELEMENTS] = {-1};
		mSubsets[i].elenum = 0;
		mSubsets[i].mark = true;
	}
	
	int beforeHandle[MAX_ELEMENTS], b=0;	
	beforeHandle[b++] = NFA->start;
	int tempSave[MAX_ELEMENTS], t=0;	//记录tempSave数组元素的个数
	E_closure(NFA, beforeHandle, &b, tempSave, &t);

	/*初始化第一个子集*/
	mSubsets[m]._ID = subset_ID++;
	memcpy(mSubsets[m].elements, tempSave, MAX_ELEMENTS*sizeof(int));	//复制数组
	mSubsets[m].elenum = t;
	mSubsets[m++].mark = false;

	DFA mDFA;
	for(i=0;i<20;i++){		//DFA初始化
		mDFA.States[i] = ' ';
		mDFA.State_isFinal[i] = false;
		for(j=0;j<20;j++){
			mDFA.Table[i][j] = {-1};
		}
	}
	int sid=CheckGroup(mSubsets, m);
	//MoveT_a(NFA, mSubsets[sid].elements, mSubsets[sid].elenum, tempSave, &t, inputSymbol[0]);
	//int UArray[MAX_ELEMENTS] = {0,4,7,5,3,2};
	//int u = 6;
	//judgeNewSubset(UArray, u, mSubsets, 1);
	while(sid != -1){
		mSubsets[sid].mark = true;
		for(i=0;i<strlen(inputSymbol);i++){
			printf("\nWhile inputSymbol:%c  \n",inputSymbol[i]);
			t=0;
			tempSave[MAX_ELEMENTS] = {-1};
			int middleArray[MAX_ELEMENTS] = {-1}, ma=0;

			MoveT_a(NFA, mSubsets[sid].elements, mSubsets[sid].elenum, middleArray, &ma, inputSymbol[i]);
			E_closure(NFA, middleArray, &ma, tempSave, &t);
			
			int Oldset_ID = judgeOldSubset(tempSave, t, mSubsets, m);
			if(t>0 && Oldset_ID != -1){
				mDFA.Table[sid][i] = mSubsets[Oldset_ID]._ID - start;	//填入DFA信息
			}
			if(t>0 && judgeOldSubset(tempSave, t, mSubsets, m) == -1){
				mSubsets[m]._ID = subset_ID++;
				mDFA.Table[sid][i] = mSubsets[m]._ID - start;	//填入DFA信息
				memcpy(mSubsets[m].elements, tempSave, MAX_ELEMENTS*sizeof(int));	//复制数组
				mSubsets[m].elenum = t;
				mSubsets[m++].mark = false;
			}
		}
		sid=CheckGroup(mSubsets, m);
	}
	mDFA.statesnum = m;	//记录DFA状态的个数

	printf("\nm:%d  ",m);
	printf("\n@@@@@@@@@@@@ mSubsets @@@@@@@@@@@@@@\n");
	for(i=0;i<m;i++){
		printf("%c \t",mSubsets[i]._ID);
		for(j=0;j<mSubsets[i].elenum;j++){
			printf("%d  ",mSubsets[i].elements[j]);
			if(mSubsets[i].elements[j] == NFA->end){	//记录DFA的终态节点
				mDFA.State_isFinal[i] = true;
			}
		}
		printf("\n");
	}
	printf("\n000000000000000 DFA 00000000000000000\n");
	printf(" \t");
	for(i=0;i<strlen(inputSymbol);i++){ //打印输入符号集
		printf("%c\t",inputSymbol[i]);
	}
	printf("\n");
	for(i=0;i<m;i++){
		printf("%c\t", mSubsets[i]._ID);
		mDFA.States[i] = mSubsets[i]._ID - start;	//记录DFA的状态集
		for(j=0;j<strlen(inputSymbol);j++){
			if(mDFA.Table[i][j] == -1){
				printf(" \t");
			}else
				{printf("%c\t",mDFA.Table[i][j] + start);}
		}
		printf("\n----------------------------\n");
	}


	DFA_minimize(&mDFA, inputSymbol);
}

/*判断得到的子集是否在子集族中已经存在*/
int judgeOldSubset(int UArray[MAX_ELEMENTS],int u, mSubset mSubsets[SUBSET_NUM],int number){
	printf("\n+++++++++++judgeNewSubset+++++++++++++\n");
	int i,j,k,existNum;
	for(i=0;i<number;i++){
		if(u == mSubsets[i].elenum){
			printf("\nmSubsets[i].elenum: %d ",mSubsets[i].elenum);
			existNum = 0;
			for(j=0;j<u;j++){
				int x = UArray[j];
				for(k=0;k<mSubsets[i].elenum;k++){
					if(x==mSubsets[i].elements[k]){
						existNum++;
						break;
					}
				}
			}
			if(existNum == u){
				printf("----false!!!");
				return i;
			}
		}
	}
	printf("true!!!");
	return -1;
}

/*实现Move(T,a)，即集合T中的所有状态经过a弧到达的状态*/
void MoveT_a(mGraph *NFA, int beforeHandle[MAX_ELEMENTS],int b, int tempSave[MAX_ELEMENTS],int *t, char c){
	int i,j;
	for(i=0;i<b;i++){
		for(j=0;j<NFA->vexnum;j++){
			if(NFA->arcs[beforeHandle[i]][j].info == c){
				printf("\nMoveT_a:%d, %c %d  ",NFA->vexs[beforeHandle[i]],c,NFA->vexs[j]);
				tempSave[*t] = j;
				(*t)++;
			}
		}
	}
}

/*构造空闭包集合,即经过任意条E弧能到达的状态集合*/
void E_closure(mGraph *NFA, int beforeHandle[MAX_ELEMENTS], int *b, int tempSave[MAX_ELEMENTS], int *t){
	int i;
	for(i=0;i<*b;i++){
		int x = beforeHandle[i];
		tempSave[(*t)++] = x;
		findEmptyState(NFA, tempSave, t, x);
	}

	int j;
	for(j=0;j<*t;j++){
		printf("E_closure:NFA->vexs--> %d\n",NFA->vexs[tempSave[j]]);
	}
}

/*被E_closure调用，对空闭包集合中的每个元素递归寻找目标状态*/
void findEmptyState(mGraph *NFA,int tempSave[MAX_ELEMENTS], int *t, int x){
	int n;
	bool hasExist;
	for(n=0;n<NFA->vexnum;n++){
		if(NFA->arcs[x][n].info=='-'){
			hasExist == false;
			int j;
			for(j=0;j<*t;j++){
				if(tempSave[j] == n){
					hasExist = true;
				}
			}
			if(hasExist == false){
				findEmptyState(NFA, tempSave, t, n);
				tempSave[*t] = n;
				(*t)++;
			}
		}
	}
}

/*检查子集族C中未被标记的子集，返回子集T在子集族中的位置*/
int CheckGroup(mSubset mSubsets[SUBSET_NUM],int number){
	int i;
	for(i=0;i<number;i++){
		if(mSubsets[i].mark == false){
			//printf("\n%d  ",i);
			return i;
		}
	}
	return -1;
}
