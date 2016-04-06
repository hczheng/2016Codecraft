
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream> 
using namespace std;

#define INLINE  static __inline
#define PRINT   printf
#define MAX_RECORD  100
#define MAX_LINE_LEN 4000
#define Error( Str )   FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )
//邻接表读图，
//前面这些把图读进邻接表，
typedef struct ListNode *Position;//位置
typedef Position List;//链表
typedef struct Tbl *Table;//邻接表
typedef int ElementType;
typedef int Vertex;//顶点
typedef struct VertexTable *VerTable;//例子4个顶点
#define Infinity 65536 //各个顶点初始化
#define NotAVertex -1
#define nowstart 0//当前起点，初始化为0
#define NotFound -1//折半查找
#define LeftChild(i) (2*(i)+1)//堆排序							
typedef struct StackRecord *Stack;
#define EmptyTOS -1

int read_file(char ** const buff, const unsigned int spec, const char * const filename)
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		PRINT("Fail to open file %s, %s.\n", filename, strerror(errno));
		return 0;
	}
	PRINT("Open file %s OK.\n", filename);

	char line[MAX_LINE_LEN + 2];
	unsigned int cnt = 0;
	while ((cnt < spec) && !feof(fp))
	{
		line[0] = 0;
		fgets(line, MAX_LINE_LEN + 2, fp);
		if (line[0] == 0)   continue;
		buff[cnt] = (char *)malloc(MAX_LINE_LEN + 2);
		strncpy(buff[cnt], line, MAX_LINE_LEN + 2 - 1);
		buff[cnt][4001] = 0;
		cnt++;
	}
	fclose(fp);
	PRINT("There are %d lines in file %s.\n", cnt, filename);
	return cnt;
}

struct StackRecord
{
	int Capacity;
	int TopOfStack;
	ElementType *Array;
};
////创建栈
Stack CreateStack(int MaxElements)
{
	Stack S;
	S = (struct StackRecord*)malloc(sizeof(struct StackRecord));
	if (S == NULL)
		FatalError("Out of space!!!");
	S->Array = (int*)malloc(sizeof(ElementType)*MaxElements);
	if (S->Array == NULL)
		FatalError("Out of space!!!");
	S->Capacity = MaxElements;
	S->TopOfStack = EmptyTOS;
	return S;
}
//出栈
void Pop(Stack S)
{
	if (S->TopOfStack == EmptyTOS)
		Error("Empty Stack");
	else
		S->TopOfStack--;
}
//入栈
void Push(ElementType X, Stack S)
{
	if (S->TopOfStack == S->Capacity - 1)
		Error("Full stack");
	else
		S->Array[++S->TopOfStack] = X;
}
//销毁栈
void DisposeStack(Stack S)
{
	if (S != NULL)
	{
		free(S->Array);
		free(S);
	}
}
ElementType Top(Stack S)
{
	if (S->TopOfStack != EmptyTOS)
	{
		return S->Array[S->TopOfStack];
	}
	else
	{
		Error("Empty Stack");
		return 0;
	}
}


//链表，每个存储了顶点Element，权重Cost，指向下一个的Next；
struct ListNode
{
	ElementType ELement;
	int Cost;
	int Priority;
	int Edge;
	Position Next;
};//0->1,1   Element=1,cost=1;

  //邻接表
struct Tbl
{
	int TableSize;
	List *TheLists;
};

//顶点表
struct VertexTable
{
	int known;
	int Dist;
	Vertex Path;
};
//还是那个邻接表的程序，不超过600个点就直接生成T->TheLists[0]到T->TheLists[599]
Table InitializeTable(int TableSize)
{
	Table T;
	int i;
	T = (struct Tbl*)malloc(sizeof(struct Tbl));
	if (T == NULL)
		FatalError("Out of space!!!");
	T->TableSize = TableSize;
	T->TheLists = (struct ListNode**)malloc(sizeof(List)*T->TableSize);
	for (i = 0;i<T->TableSize;i++)
	{
		T->TheLists[i] = (struct ListNode*)malloc(sizeof(struct ListNode));
		if (T->TheLists[i] == NULL)
			FatalError("Out of space!!!");
		else
			T->TheLists[i]->Next = NULL;
	}
	return T;
}
//Pos是起点位置,Key是终点,Cost是权重，上面讲了
//Insert就是链表的插入
void Insert(int Edge, ElementType Pos, ElementType Key, ElementType Cost, ElementType Priority, Table T)
{
	Position NewCell;
	List L;
	NewCell = (struct ListNode*)malloc(sizeof(struct ListNode));
	if (NewCell == NULL)
		FatalError("Out of space!!!");
	else
	{
		L = T->TheLists[Pos];
		NewCell->Next = L->Next;
		NewCell->Edge = Edge;
		NewCell->ELement = Key;
		NewCell->Cost = Cost;
		NewCell->Priority = Priority;
		L->Next = NewCell;
	}
}
//Start是起点0，VerTable初始化
VerTable InitialWeighted(Vertex Start, VerTable V, int NumVertex)//V为顶点集合，每隔顶点有三种标记
{
	int i;
	V = (struct VertexTable*)malloc(sizeof(struct VertexTable)*NumVertex);
	for (i = 0;i<NumVertex;i++)
	{
		V[i].known = 0;
		V[i].Dist = 0;
		V[i].Path = NotAVertex;
	}
	V[Start].Dist = 0;
	return V;
}

//对分查找
int BinarySearch(const ElementType A[], ElementType X, int N)
{
	int Low, Mid, High;
	Low = 0;High = N - 1;
	while (Low <= High)
	{
		Mid = (Low + High) / 2;
		if (A[Mid]<X)
			Low = Mid + 1;
		else
			if (A[Mid]>X)
				High = Mid - 1;
			else
				return Mid;
	}
	return NotFound;
}

void MakeEmpty(List L)
{
	Position P, Tmp;
	P = L->Next;
	L->Next = NULL;
	while (P != NULL)
	{
		Tmp = P->Next;
		free(P);
		P = Tmp;
	}
}
void Disposable(Table T)
{
	int i;
	for (i = 0;i<T->TableSize;i++)
	{
		MakeEmpty(T->TheLists[i]);
	}
	free(T->TheLists);
	free(T);
}

//自定义查找下一顶点的算法
int find_start(VerTable V, Table T, ElementType demand[], int start_now, int known_p, int end, Stack S, int N)
{
	//传入的参数分别为:顶点表（konwn,dis,path）、优先点集、当前遍历起点
	//返回值为下一个起点索引TempV，以及特征点入栈
	Position tmp;
	int min = Infinity;//普通点最小权值
	int min_sp = Infinity;//优先点最小权值
	int count_sp = 0;//优先点计数（用于判断特征点）
	int count_normal = 0;//普通点计数
	int normal[8] = { 0 };//普通点数组（暂存）
	int special[8] = { 0 };//优先点数组（暂存）
	Vertex TempV = -1;//开始标记
	int flag = 0;//终点标记
				 //TMP = T->TheLists[start_now];
	tmp = T->TheLists[start_now]->Next;
	while (tmp != NULL) //0->3->1->NULL  还有邻接点且未到达过
	{
		if (V[tmp->ELement].known != 1) {
			if (tmp->Priority == 1)//如果该顶点是优先点
			{
				count_sp++;//当前层级优先点数+1
				if (tmp->Cost < min_sp) //当前点权值更小
				{
					if (count_sp > 1)
					{
						special[count_sp - 2] = TempV;
						V[TempV].Dist = 0;
					}
					min_sp = tmp->Cost;//更新min_sp					
					TempV = tmp->ELement;//返回值
					V[TempV].Dist = V[start_now].Dist + tmp->Cost;
				}
				else
				{
					if (count_sp > 1)
					{
						special[count_sp - 2] = tmp->ELement;;
					}
				}
			}
			else if (tmp->ELement == end)//如果该顶点是终点
			{
				flag = 1;//表明这一轮有终点，但暂时不作处理
				V[end].Dist = V[start_now].Dist + tmp->Cost;
				V[end].Path = start_now;
			}
			else //如果该顶点是普通点
			{
				count_normal++;//普通点计数
				normal[count_normal - 1] = tmp->ELement;//普通点暂存
				if (count_sp == 0 && tmp->Cost < min)//(1.有普通点没有终点没有优先点2.有普通点有终点没有优先点)
				{
					min = tmp->Cost;//更新min
					V[TempV].Dist = 0;
					TempV = tmp->ELement;//返回值	
					V[TempV].Dist = V[start_now].Dist + tmp->Cost;
				}
			}
		}
		tmp = tmp->Next;
	}
	///////////*************************LOOK***************************///////////////
	if (count_sp == 0) //假如没有优先点，则把多余的普通点全部入栈
	{
		for (int i = 0;i < count_normal;i++)
		{
			if (TempV != normal[i])
			{
				Push(normal[i], S);Push(start_now, S);//V[normal[i]].Path = start_now;
			}
		}//普通点分支入栈		
	}
	else//当优先点数>=1时
	{
		if (count_sp == 1)//假如刚好只有一个优先点，那么普通点入栈，并赋值path
		{
			for (int i = 0;i < count_normal;i++) { Push(normal[i], S); Push(start_now, S); }//V[normal[i]].Path = start_now;
		}
		else//当优先点数大于1时，普通点先全部入栈，然后多余的优先点入栈
		{
			for (int i = 0;i < count_normal;i++) { Push(normal[i], S); Push(start_now, S); } //V[normal[i]].Path = start_now;
			for (int i = 0;i < count_sp - 1;i++) { Push(special[i], S);Push(start_now, S); }//V[special[i]].Path = start_now;
		}
	}
	if (flag == 1 && known_p == N)//已到终点,且这一层没有优先点了，就要判断是否已经结束（优先点已全部在路径中）
	{
		TempV = 10000; //10000结束标志,到达终点且所有的优先点已在路径中
	}
	if (TempV == -1) { TempV = -start_now; if (start_now == 0) { TempV = -1000; } }//返回停止点（带负号，方便处理）
	if (TempV != 10000 && TempV >= 0) { V[TempV].Path = start_now; }
	return TempV;//返回下一起点索引
}

//显示路径

void PrintPath(Vertex Ver, VerTable V, int C[])
{
	if (V[Ver].Path != NotAVertex)
	{
		PrintPath(V[Ver].Path, V, C);
		printf("->");
	}
	printf("%d", C[Ver]);
}
//边编号
int find_route(int stop, Table T, VerTable V)
{
	Position Tmp;
	int result = -1;
	Tmp = T->TheLists[V[stop].Path]->Next;//
	while (Tmp != NULL)
	{
		if (Tmp->ELement == stop) { result = Tmp->Edge;break; }
		Tmp = Tmp->Next;
	}
	return result;
}
//核心算法，D算法拓展
int DF(VerTable V, Table T, ElementType demand[], Stack S, int N, int start_now, int end, unsigned short *result)
{
	//Vertex Ver, W;//顶点索引
	Position Tmp;
	//int end = 5;//假定一个终点
	int startone=start_now ;//存起点
	int known_p = 0;//记录已到过的优先点
	int flag = 0;//遍历过优先点标记
	int min_dist = 10000;
	int ok = 1;
	for (;;)
	{
		if (start_now == 10000)//当前起点为-100，查找成功
		{
			flag++;//迭代次数
			if (N < 16)
			{
				if (min_dist > V[end].Dist) {
					int tmp = end;
					int count = 0;
					while (tmp != startone)
					{
						result[count] = find_route(tmp, T, V);
						count++;
						ok = count;
						tmp = V[tmp].Path;
					}
					min_dist = V[end].Dist;
				}
				else
					V[end].Dist=min_dist;
				if (S->TopOfStack == -1) break;
				/////////////////////////////////////////////////////////////////////////
				int pass = Top(S);//获得特征点索引 0->1->3					
				Pop(S);//出栈
				start_now = Top(S);//将当前起点改为栈顶元素		
				int stop = V[end].Path;//继续回溯
				while (stop != pass)
				{
					V[stop].known = 0;
					if (BinarySearch(demand, stop, N) >= 0) { known_p--; }
					stop = V[stop].Path;
					//V[stop].Path = NotAVertex;
				}
				Pop(S);//出栈	
				V[start_now].Path = pass;//出栈起点路径信息
				Tmp = T->TheLists[pass]->Next;
				while (Tmp != NULL)
				{
					if (Tmp->ELement == start_now) { V[start_now].Dist = V[pass].Dist + Tmp->Cost; }
					Tmp = Tmp->Next;
				}
				/////////////////////////////////////////////////////////////////////////////
			}
			else
				break;
			if(flag>15) 
				break;
		}		
		if (start_now < 0 && S->TopOfStack == -1) { ok = 0;V[end].Dist = 0;break; }//栈空，则返回无解
		if (start_now <0)//回头
		{
			int stop = -start_now;//停止点
			if (start_now == -1000) { stop = 0; }//0为停止点的情况
			int pass = Top(S);//获得特征点索引 0->1->3					
			Pop(S);//出栈
			printf("%d出栈：\n", pass);
			start_now = Top(S);//将当前起点改为栈顶元素			
			while (stop != pass)
			{
				V[stop].known = 0;
				if (BinarySearch(demand, stop, N) >= 0) { known_p--; }
				printf("%d回撤：\n", stop);
				stop = V[stop].Path;
				//V[stop].Path = NotAVertex;
			}
			Pop(S);//出栈	
			printf("%d出栈：\n", start_now);
			V[start_now].Path = pass;//出栈起点路径信息
			Tmp = T->TheLists[pass]->Next;
			while (Tmp != NULL)
			{
				if (Tmp->ELement == start_now) { V[start_now].Dist = V[pass].Dist + Tmp->Cost; }
				Tmp = Tmp->Next;
			}
		}
		V[start_now].known = 1;//将当前起点标记为已知
		if (BinarySearch(demand, start_now, N) >= 0) { known_p++; }//记录到达的优先点数
		start_now = find_start(V, T, demand, start_now, known_p, end, S, N);//按自定义方法找下一个起点
		printf("当前点：%d\n",start_now);
	}
	return ok;
}
//堆排序
void PercDown(ElementType A[], int i, int N)
{
	int Child;
	ElementType Tmp;
	for (Tmp = A[i];LeftChild(i)<N;i = Child)
	{
		Child = LeftChild(i);
		if (Child != N - 1 && A[Child + 1]>A[Child])
			Child++;
		if (Tmp<A[Child])
			A[i] = A[Child];
		else
			break;
	}
	A[i] = Tmp;
}
void Swap(int *a, int *b)
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}
void Heapsort(ElementType A[], int N)
{
	int i;
	for (i = N / 2;i >= 0;i--)
		PercDown(A, i, N);
	for (i = N - 1;i>0;i--)
	{
		Swap(&A[0], &A[i]);
		PercDown(A, 0, i);
	}
}

void search_route(char *graph[5000], int edge_num, char *condition)//字符串数组(topo)，行数，字符串（demand）
{
	Table T = InitializeTable(600);//顶点
	VerTable V = NULL;
	Stack S = CreateStack(1000);//创建栈（存分支点）
	unsigned short result[100] = { 0 };//示例中的一个解 
	const int n = edge_num;
	int  N, start_now, end, demand[100] = { -1 }, test[261][4] = { 0 };
	char *save_ptr, *line;//存剩余的
	for (int i = 0;i<n;i++)//test初始化
	{
		char *hello = graph[i];
		char *p, *token;
		int j = 0;
		for (p = hello; ; p = NULL, j++)
		{
			token = strtok_s(p, ",", &save_ptr);
			if (NULL == token)   break;
			test[i][j] = atoi(token);
		}
	}
	start_now = atoi(strtok_s(condition, ",", &save_ptr));//起点
	printf("start_now=%d \n", start_now);
	end = atoi(strtok_s(NULL, ",", &save_ptr));//终点
	printf("end=%d \n", end);
	line = strtok_s(NULL, ",", &save_ptr);//优先点字符串
	printf("special_line=%s \n", line);
	char *get, *token;
	save_ptr = NULL;
	int d_count = 0;
	for (get = line; ; get = NULL, d_count++)
	{
		token = strtok_s(get, "|", &save_ptr);
		if (NULL == token)   break;
		demand[d_count] = atoi(token);
		// printf("demand[%d]=%d \n",d_count,demand[d_count]);
	}
	N = d_count;
	int Sort[261 * 2] = { -1 }, Max, *C, p = 0, Stemp, Etemp, *Fdemand;
	for (int i = 0; i < n; i++)
	{
		Sort[2 * i] = test[i][1];
		Sort[2 * i + 1] = test[i][2];
	}//把起点和终点包含的点都算进来
	Heapsort(Sort, n * 2);//对顶点进行排序
	Max = Sort[2 * n - 1];
	C = (int*)malloc(sizeof(int)*(Max + 1));
	if (NULL == C)
	{
		FatalError("Out of space!!!");
	}
	for (int i = 0;i<2 * n;i++)
	{
		if (Sort[i] != Sort[i + 1])
		{
			C[p] = Sort[i];
			p++;
		}
	}
	printf("edge_num=%d \n", n);
	printf("point_num=%d \n", p);
	printf("special_num=%d \n", N);
	Fdemand = (int*)malloc(sizeof(int)*(N));;
	for (int i = 0;i<N;i++)
	{
		Stemp = BinarySearch(C, demand[i], p);
		Fdemand[i] = Stemp;
	}
	Heapsort(Fdemand, N);
	for (int j = 0;j<n;j++)
	{
		Stemp = BinarySearch(C, test[j][1], p);
		Etemp = BinarySearch(C, test[j][2], p);
		if (BinarySearch(Fdemand, Etemp, N) >= 0)
		{
			Insert(test[j][0], Stemp, Etemp, test[j][3], 1, T);//start,stop,cost,List，把3必过点插入
			T->TheLists[Stemp]->ELement++;//有几个优先级的点
		}
		else
		{
			Insert(test[j][0], Stemp, Etemp, test[j][3], 0, T);//start,stop,cost,List，把3必过点插入
			T->TheLists[Stemp]->ELement++;//有几个优先级的点
		}
	}
	start_now = BinarySearch(C, start_now, p);
	end = BinarySearch(C, end, p);
	V = InitialWeighted(0, V, n);
	int route = DF(V, T, Fdemand, S, N, start_now, end, result);//最短路径D算法		
	int tmp = end;
	int result_count = route;//解的个数
	if (route == 0) { printf("NA\n"); }
	else
	{
		if (N>=16) {
	    result_count =0;
		while (tmp != start_now)
		{
			result[result_count] = find_route(tmp, T, V);
			result_count++;
			tmp = V[tmp].Path;
		}
		}
		PrintPath(end, V, C);
		printf(" COST=%4d\n", V[end].Dist);
		for (int i = result_count - 1;i >= 0;i--)
		{
			printf("%d | ", result[i]);
			//record_result(result[i]);
		}
		printf("\n");
	}
	Disposable(T);
	DisposeStack(S);
	free(V);
	free(C);
	free(Fdemand);
	//record_result(result[i]);
}

int main()
{
	char *topo[5000];
	int edge_num;
	char *demand;
	int demand_num;
	edge_num = read_file(topo, 5000, "topo.csv");
	demand_num = read_file(&demand, 1, "demand.csv");
	search_route(topo, edge_num, demand);
	getchar();
	return 0;
}