#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 32 //正则式最大字符长度
#define MAX_STATES_NUM 20
#define MAX_VERTEX_NUM 20	//minDFA状态节点数

char start = 'P';

typedef struct {	
	int GroupID[MAX_STATES_NUM];	/*DFA状态节点分组ID*/
	int DestID[MAX_STATES_NUM];		/*状态转换到目标状态的ID*/
	bool isparted[MAX_STATES_NUM];	/*标记是否已经分组*/
}States_info;

//函数声明
void DFA_minimize(DFA *mDFA, char inputSymbol[MAX_LENGTH]);
void PPconstruct(DFA *mDFA, States_info *mStates_info, char inputSymbol[MAX_LENGTH], int *Gnum);
void Finish_minDFA(DFA *minDFA, DFA *mDFA, States_info mStates_info, char inputSymbol[MAX_LENGTH], int Gnum);
void getDestID(DFA *mDFA, int whichInput, int whichState, int *destID);
void printINFO(DFA minDFA, DFA *mDFA, States_info *mStates_info, char inputSymbol[MAX_LENGTH], int Gnum);
bool Test_Verification(DFA minDFA, char inputSymbol[MAX_LENGTH]);

/*分割法最小化DFA*/
void DFA_minimize(DFA *mDFA, char inputSymbol[MAX_LENGTH]){
	DFA minDFA;
	States_info mStates_info;
	int Gnum = 1;	//分组Group的数量
	bool quantity = false;
	int i;
	for(i=0;i<MAX_STATES_NUM;i++){		//按终态非终态进行初始划分
		if(mDFA->State_isFinal[i] == true){
			mStates_info.GroupID[i] = 0;
		}else{
			mStates_info.GroupID[i] = 1;
			quantity = true;
		}
	}
	if(quantity == true){
		Gnum = 2; //分组Group的数量
	}

	PPconstruct(mDFA, &mStates_info, inputSymbol, &Gnum);	//PP构造完成分组
	Finish_minDFA(&minDFA, mDFA, mStates_info, inputSymbol, Gnum); //利用前面的分组构造minDFA，完成最小化
	printINFO(minDFA, mDFA, &mStates_info, inputSymbol, Gnum);
	while(true){
		Test_Verification(minDFA,inputSymbol);
	}
}


/*对每一组进行PP构造*/
void PPconstruct(DFA *mDFA, States_info *mStates_info, char inputSymbol[MAX_LENGTH], int *Gnum){
	int i,j,k,l;
	for(i=0;i<*Gnum;i++){	//对每个分组进行重新分组
		for(j=0;j<strlen(inputSymbol);j++){		//对每个输入符号a进行审查
			for(k=0;k<mDFA->statesnum;k++){	//确定该组元素的DestID；
				if(mStates_info->GroupID[k] == i){
					getDestID(mDFA, j, k, &mStates_info->DestID[k]);
					mStates_info->isparted[k] = false;
				}
			}//确定该组元素的DestID；

			bool isfirst = true;
			for(k=0;k<mDFA->statesnum;k++){
				if(mStates_info->GroupID[k]==i && mStates_info->isparted[k]==false){	//只对同一组的元素分组
					mStates_info->isparted[k] = true;
					int tempdest = mStates_info->DestID[k];
					if(isfirst == false){ 	//确定GroupID
						mStates_info->GroupID[k] = (*Gnum)++;
					}else{
						isfirst = false;
					}
					for(l=0;l<mDFA->statesnum;l++){
						if(mStates_info->GroupID[l]==i && mStates_info->isparted[l]==false){ //在同一组寻找DestID相同的元素
							if(mStates_info->DestID[l] == tempdest){
								mStates_info->isparted[l] = true;
								mStates_info->GroupID[l] = mStates_info->GroupID[k];
							}
						}
					}//end for
				}
			}


		}//对每个输入符号a进行审查
	}//对每个分组进行重新分组
}

/*利用前面的分组构造minDFA，完成最小化*/
void Finish_minDFA(DFA *minDFA, DFA *mDFA, States_info mStates_info, char inputSymbol[MAX_LENGTH], int Gnum){
	int i,j,k;
	minDFA->statesnum = Gnum;	
	for(i=0;i<Gnum;i++){	//初始化minDFA信息
		minDFA->State_isFinal[i] = false;
		for(j=0;j<strlen(inputSymbol);j++){
			minDFA->Table[i][j] = -1;
		}
	}
	minDFA->start = mStates_info.GroupID[0];
	for(i=0;i<Gnum;i++){
		minDFA->States[i] = i;	//添加节点
		for(j=0;j<mDFA->statesnum;j++){
			if(mStates_info.GroupID[j] == i){
				if(mDFA->State_isFinal[j] == true){	 //保存终态信息
					minDFA->State_isFinal[i] = true;
				}
				for(k=0;k<strlen(inputSymbol);k++){
					int destID = mDFA->Table[j][k];
					if(destID != -1){ 	//不为空时添加信息
						int a = mStates_info.GroupID[destID];
						minDFA->Table[i][k] = a;
					}
				}
				break;
			}
		}
	}
}

void getDestID(DFA *mDFA, int whichInput, int whichState, int *destID){
	*destID = mDFA->Table[whichState][whichInput];
}

void printINFO(DFA minDFA, DFA *mDFA, States_info *mStates_info, char inputSymbol[MAX_LENGTH], int Gnum){
	printf("\nDFA_states & GroupID:-------------------Gnum:%d\n",Gnum);
	int i,j;
	for(i=0;i<mDFA->statesnum;i++){
		if(mDFA->State_isFinal[i] == true){
			printf("%c- ",mDFA->States[i] + start);
		}else
		printf("%c  ",mDFA->States[i] + start);
	}
	printf("\n");
	for(i=0;i<mDFA->statesnum;i++){
		printf("%d  ",mStates_info->GroupID[i]);
	}
	printf("\nminDFA:-------------------Gnum:%d\n",Gnum);
	printf("   ");
	for(i=0;i<strlen(inputSymbol);i++){
		printf("%c  ",inputSymbol[i]);
	}
	printf("\n");
	for(i=0;i<Gnum;i++){
		printf("%d  ",minDFA.States[i]);
		for(j=0;j<strlen(inputSymbol);j++){
			if(minDFA.Table[i][j] == -1){
				printf("   ");
			}else
				printf("%d  ",minDFA.Table[i][j]);	//打印连接信息
		}
		printf("\n");
	}
	printf("\nminDFA->start:%d",minDFA.start);	//打印开始态、终结态信息
	printf("\nminDFA->end: ");
	for(j=0;j<Gnum;j++){
		if(minDFA.State_isFinal[j] == true){
			printf("%d ",j);
		}
	}
}

bool Test_Verification(DFA minDFA, char inputSymbol[MAX_LENGTH]){
	int i,j,k;
	char string[MAX_LENGTH];
	printf("\n===============================================\n");
	printf("input String that'll be verificated: ");
	scanf("%s", string);

	int currentState = minDFA.start;
	for(i=0;i<strlen(string);i++){	//对每个字母进行处理
		char c = string[i];
		for(j=0;j<strlen(inputSymbol);j++){
			if(inputSymbol[j] == c){
				if(minDFA.Table[currentState][j] != -1){
					int a = minDFA.Table[currentState][j];
					currentState = a;
				}
				if(minDFA.State_isFinal[currentState] == true){ //查看currentState是否为终态
					printf("OK, Accepted!!\n");
					return true;
				}
			}
		}
	}
	printf("SORRY, Don't be Accepted!!\n");
	return false;
}
